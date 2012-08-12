#include "operations.h"
#include "emailthread.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <vector>
#include <string.h>

#include <FCam/Image.h>
#include <QPicture>
#include <QLabel>
#include <QDateTime>
#include "CImg.h"
#include "functions.cpp"

using namespace cimg_library;
using namespace std;
//
int norm = 100; //custom normalisation amount
QString current_save_image; //filename of last movement image -- shared for emailing

CImg<unsigned char> reference; //Last reference

// static class variables for saving images with movement
int image_count = 0;
QString save_dir;

void Operations::run(){
    //DEBUG
    /*cout<< "Range=(" << interval_min << "," << interval_max << "),Mod="<< interval_mod<< ",White="<< limitVal
        << ",Size=("<< width <<","<< height << "),Mask=" << erodeVar <<",Convert="<< convert_images <<",Delete="<< delete_images
        << "\nDir=" << image_dir.toUtf8().data() << "\nEmail:" << emailAlert << " " << email_address.toUtf8().data() << " "
        << email_message.toUtf8().data() << " " << email_subject.toUtf8().data() << " "<< email_attach << endl;*/

    //Print Difference between dates
    cout << cyan_col << "Current Date Time:\t" << QDateTime::currentDateTime().toString().toUtf8().data()
         << "\nRecord Until Time:\t" << time.toString().toUtf8().data() << stop_col << endl;

    cout << "CameraThread: "<< yellow_col << "Started" << stop_col << endl;

    if(lensClosed()) {
        willStop = true;
        alert("Make sure lens cap is open");
    }
    else{
        //TimeLapse
        if(timelapse)
        {
            if(time.secsTo(QDateTime::currentDateTime())<0)
            {
                takeSingleWellExposedPhoto();
            }
            else {
                if(convert_images) convertMovie(delete_images);
            }
        }
        else  //Regular Operations
        {
            if(echo_to_log) echoLog("\n# Recording from "+QDate::currentDate().toString().toStdString()+" #\n");
            initial();
            save_dir = image_dir;
            updateReferenceImage(10);
            QThread::sleep(2);
            checkMovement(interval_max*1000, interval_min*1000,interval_mod, limitVal);
            finishAndClose();
        }
    }

    cout << "CameraThread: " << yellow_col << "Exited" << stop_col << endl;

}

Operations::Operations(bool timelap){
    timelapse = timelap;
    initial();
}

void Operations::initial(){
    willStop = false;
    // Check compatiblity
    errorCheck();
    // Set the shot parameters
    stream1.exposure = 80000;
    stream1.gain = 1.0f;
    // Request an image size and allocate storage

    // Enable the histogram unit -- NECESSARY to autoexpose image!
    stream1.histogram.enabled = true;
    stream1.histogram.region = FCam::Rect(0, 0, width, height);

    stream1.image = FCam::Image(width, height, FCam::UYVY);
}

void Operations::finishAndClose(){
    // Order the sensor to stop the pipeline and discard any frames still in it.
    sensor1.stop();
    /*    cout << "Final exposure: " << (frame1.exposure()/1000.f) << "f ms. Final gain: " << frame1.gain();
      cout << "\nFinal color temperature: " << frame1.whiteBalance() << "K" << endl;*/
    // Check that the pipeline is empty
    assert(sensor1.framesPending() == 0);
    assert(sensor1.shotsPending() == 0);
    //If stop signal given, thread terminates here.
    if(!willStop)
    {   //Convert images to movie, bool = delete images
        if(convert_images) convertToMP4(save_dir,delete_images);
    }
}

//Repeats...
Operations::~Operations(){
    // Order the sensor to stop the pipeline and discard any frames still in it.
    sensor1.stop();
        cout << "Final exposure: " << (frame1.exposure()/1000.f) << "f ms. Final gain: " << frame1.gain();
      cout << "\nFinal color temperature: " << frame1.whiteBalance() << "K" << endl;
    // Check that the pipeline is empty
    assert(sensor1.framesPending() == 0);
    assert(sensor1.shotsPending() == 0);
}


//private methods:
void Operations::errorCheck() {
    // Make sure FCam is running properly by looking for DriverError
    FCam::Event e;
    while (FCam::getNextEvent(&e, FCam::Event::Error)) {
        cout << "Error: " << e.description << endl;
        if (e.data == FCam::Event::DriverMissingError) {
            cout << "fcam-drivers missing. Please install, then reboot." << endl;
            exit(1);
        }
        if (e.data == FCam::Event::DriverLockedError) {
            cout << "Another FCam is running" << endl;
            exit(1);
        }
    }
}

/** Gets a static unchanging frame taken at interval(milliseconds), and exits
  when scene is static for stableframenum number of frames.
  **/
void Operations::defineGoodExposure(int stableframenum)
{
    int count = 0;

    stream1.frameTime = 0;
    // We will stream until the exposure stabilizes
    int stableCount = 0;    // # of consecutive frames with stable exposure
    float exposure;         // total exposure for the current frame (exposure time * gain)
    float lastExposure = 0; // total exposure for the previous frame

    char disp[8] = {'|','|','/','/','-','-','\\','\\'};      //Progress Spinner

    //----- Stage1 ---- Grab a static base background image ----------------
    do {
        //Grab frame
        sensor1.stream(stream1);   //apply params
        frame1 = sensor1.getFrame(); //grab first frame
        assert(frame1.shot().id == stream1.id); // check source matches.
        // Call the autoexposure algorithm. It will update stream
        // using this frame's histogram.
        autoExpose(&stream1, frame1);
        // Call the auto white-balance algorithm. It will similarly
        // update the white balance using the histogram.
        autoWhiteBalance(&stream1, frame1);

        exposure = frame1.exposure() * frame1.gain();

        // Increment stableCount if the exposure is within 5% of the
        // previous one
        if (fabs(exposure - lastExposure) < 0.02f * lastExposure)
        {
            stableCount++;
        } else {
            // Reset stableCount to less than zero to account for latency through the pipeline
            // Otherwise, we might assume we're stable before we see the results of the latest
            // updates.
            stableCount = -sensor1.shotsPending();
        }

        // Update lastExposure
        lastExposure = exposure;
        count++;
        cout << '\r' << disp[count%8] << flush;
    } while (stableCount < stableframenum); // Terminate when stable for 10 frames
    //Constant background image obtained, now make greyscale

}

void Operations::updateReferenceImage(int frames){

    stream1.frameTime = 0; // fast as possible (min exposure)
    sensor1.stream(stream1);   //apply params

    //Re-expose for changes in brightness (sunshine,etc)
    defineGoodExposure(frames);

    frame1 = sensor1.getFrame(); //grab first frame
    assert(frame1.shot().id == stream1.id); // check source matches.

    //Convert from FCAM to CIMG add channels to accumulator
    const FCam::Image &image = frame1.image();
    emit newImage(image);//emit newImage(image);

    CImg<unsigned char> img(width, height,1);
    convertImage(image, img);

    reference = img/norm;
    cout << "\rUpdated reference image." << endl;
}

/** Subtracts normalized frames from an reference frame **/
void Operations::checkMovement(int max, int min, float mod, int limit)
{
    EmailThread *em;                //Shared

    stream1.frameTime = 0;
    sensor1.stream(stream1);

    unsigned int current_interval = max/2; //halved so that it can grow
    unsigned int consec_no_movement = 0;

    cout << "White Pixel Threshold is " << limit << endl;

    do{
        //1. Grab a valid frame
        frame1 = sensor1.getFrame();
        assert(frame1.shot().id == stream1.id);

        //Convert from FCam Image to CIMG with 1 depth channel
        const FCam::Image &image = frame1.image();  //Image from sensor
        CImg<unsigned char> img(width, height,1);   //Blank CImg
        convertImage(image, img);                   //Populate CImg Reference

        //2. Confine colorspace to (norm=100) 25 discrete values
        img = img/norm;

        //3. Get subtracted difference between current image and reference
        CImg<unsigned char> sub = (img-reference).normalize();    //Normalize to {0,1}

        /*4. Count number of white pixels       -- Can skip this step for speed.
        cout << "Normal - ";
        int totalN = 0; cimg_forXYZC(sub,x,y,z,c) if(sub(x,y,z,c) > 0) totalN++;
        cout << totalN << "  ";*/

        //5. Perform Open/Close and recount
        sub = sub.erode(erodeVar).dilate(erodeVar);
        cout << "After Filter - ";
        int totalE = 0; cimg_forXYZC(sub,x,y,z,c) if(sub(x,y,z,c) > 0) totalE++;
        cout << totalE;

        cout << "  Interval:" << current_interval;
        cout << ", Seconds Left: " << QDateTime::currentDateTime().secsTo(time) << endl;


        //6. Check for movement -- if so: get new reference image
        if (totalE > limit)
        {
            std::string movement = "---Movement at time : " + QTime::currentTime().toString().toStdString();
            cout << red_col << movement << stop_col << endl;

            //7. Record logic:
            float scale = ((float)(current_interval)/(float)(max));

            int record_interval = (int)(scale*400)+100;  //max=500, min = 100
            int record_frames = (int)(10- (scale*8)); //longest stint=10, shortest = 2;

            record(record_frames,record_interval);

            //8. Update reference
            updateReferenceImage();
            alert("Movement!", false);

            //Send Email &&|| Log that logs movement
            if(emailAlert){
                em = new EmailThread(email_address,email_subject, email_message, email_attach, current_save_image);
            }
            if(echo_to_log){
                echoLog(movement+'\n');
            }


            // Move logic -- increase response per movement
            int new_int = (int)(current_interval*mod);
            current_interval = (new_int>min)?new_int:min;

            consec_no_movement = 0;
        }
        else {
            if(consec_no_movement ++> 8)
            {
                int new_int = (int)(current_interval/mod);
                current_interval = (new_int>max)?max:new_int;

                consec_no_movement = 0; // reset counter
            }
        }

        // Sleep here, so that Email thread has time to finish.
        // Reduces overhead
        QThread::msleep(current_interval);

    }while( (QDateTime::currentDateTime().secsTo(time)>0) && !willStop);
}

void Operations::record(int frame_num, int interval)
{
    int count = 0;

    do
    {
        QThread::msleep(interval);
        frame1 = sensor1.getFrame();
        assert(frame1.shot().id == stream1.id);

        //int to string converison
        QString num = QString("%1").arg(image_count, 5, 10, QChar('0'));   //00001.jpg, 00002.jpg, etc.
        // to Filename
        current_save_image = save_dir+num+".jpg";
        string file = current_save_image.toUtf8().constData();

        cout << "\rRecording :" << count << " " << file << flush;
        //cout << "Record:" << file << endl;

        const FCam::Image &image = frame1.image();
        FCam::saveJPEG(image,file); //saves using FCAM instead of CImg -- faster.

        image_count++;
    }
    while (count ++<frame_num && !willStop);

    cout << "  done!" << endl;

}

//TimeLapse Operations
void Operations::takeSingleWellExposedPhoto(){
    defineGoodExposure(10);

    frame1 = sensor1.getFrame();
    assert(frame1.shot().id == stream1.id);

    // to Filename
    QString save_ = "/home/user/MyDocs/DCIM/MISC/timelapse_"+QString::number(QDateTime::currentDateTime().toTime_t())+".jpg";

    const FCam::Image &image = frame1.image();
    FCam::saveJPEG(image,save_.toStdString());
    cout << "Got Photo" << endl;
}

void Operations::convertMovie(bool delet){
    QString dir = "/home/user/MyDocs/DCIM/MISC/";
    convertToMP4(dir, delet,
                 "timelapse__"+QString::number(QDateTime::currentDateTime().toTime_t()) );
}
