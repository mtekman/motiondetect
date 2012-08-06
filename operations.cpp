#include "operations.h"
#include "emailthread.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <vector>
#include <string.h>
#include <QDebug>

#include <FCam/N900.h>
#include <FCam/Image.h>
#include <QPicture>
#include <QLabel>
#include "CImg.h"
#include "functions.cpp"

using namespace cimg_library;

namespace Plat = FCam::N900;

//
int norm = 100; //custom normalisation amount
QString current_save_image; //filename of last movement image -- shared for emailing


//Camera Variables
Plat::Sensor sensor1;      //Image Sensor
FCam::Shot stream1;              //Shot
FCam::Frame frame1;              //Frame

CImg<unsigned char> average; //Last average
QLabel *imglabel;

// static class variables for saving images with movement
int image_count = 0;
QString save_dir;



void Operations::run(){
    qDebug() << "Starting";
    save_dir = image_dir;

    qDebug() << "Started";
    getStaticBGImage();
    qDebug() << "Got static";
    updateReferenceImage();
    checkMovement(interval_default,limitVal);
    finishAndClose();
    qDebug() << "Finished.";
}


Operations::Operations(int w, int h, int exp, float gain, bool histogram){   
    width = w; height = h; //update static class vars
    // Check compatiblity
    errorCheck();
    // Set the shot parameters
    stream1.exposure = exp;
    stream1.gain = gain;
    // Request an image size and allocate storage
    stream1.image = FCam::Image(width, height, FCam::UYVY);
    // Enable the histogram unit
    stream1.histogram.enabled = histogram;
    stream1.histogram.region = FCam::Rect(0, 0, width, height);

    if(lensClosed()) //Do something.

}

void Operations::finishAndClose(bool quickly){
    // Order the sensor to stop the pipeline and discard any frames still in it.
    sensor1.stop();
    qDebug() << "Final exposure: " << (frame1.exposure()/1000.f) << "f ms. Final gain: " << frame1.gain();
    qDebug() << "Final color temperature: " << frame1.whiteBalance() << "K";
   // Check that the pipeline is empty
    assert(sensor1.framesPending() == 0);
    assert(sensor1.shotsPending() == 0);

    //Emergency exit -- not very safe method for quitting thread.
    if(quickly) this->quit();

    //Convert images to movie, bool = delete images
    if(convert_images) convertToMP4(save_dir,delete_images);

}


//private methods:
void Operations::errorCheck() {
    // Make sure FCam is running properly by looking for DriverError
    FCam::Event e;
    while (FCam::getNextEvent(&e, FCam::Event::Error)) {
        qDebug() << QString("Error: %1").arg(e.description.c_str());
        if (e.data == FCam::Event::DriverMissingError) {
            qDebug() << "example1: FCam can't find its driver. Did you install "
                   "fcam-drivers on your platform, and reboot the device "
                   "after installation?";
            exit(1);
        }
        if (e.data == FCam::Event::DriverLockedError) {
            qDebug() << "example1: Another FCam program appears to be running "
                   "already. Only one can run at a time.";
            exit(1);
        }
    }
}

/** Gets a static unchanging frame taken at interval(milliseconds), and exits
  when scene is static for stableframenum number of frames.
  **/
void Operations::getStaticBGImage(int stableframenum)
{
    int count = 0;
    stream1.frameTime = 0;
    // We will stream until the exposure stabilizes
    int stableCount = 0;    // # of consecutive frames with stable exposure
    float exposure;         // total exposure for the current frame (exposure time * gain)
    float lastExposure = 0; // total exposure for the previous frame
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
        // Increment frame counter
        //qDebug() << "Frame: " <<
        count++;
    } while (stableCount < stableframenum); // Terminate when stable for 10 frames
    //Constant background image obtained, now make greyscale
}

void Operations::updateReferenceImage(){

    stream1.frameTime = 0; // fast as possible (min exposure)
    sensor1.stream(stream1);   //apply params

    frame1 = sensor1.getFrame(); //grab first frame
    assert(frame1.shot().id == stream1.id); // check source matches.

    //Convert from FCAM to CIMG add channels to accumulator
    const FCam::Image &image = frame1.image();
    CImg<unsigned char> img(width, height,1);
    convertImage(image, img);

    average = img/norm;
    qDebug() << "Updated reference image";
}

/** Subtracts normalized frames from an average frame **/
void Operations::checkMovement(int interval, int limit)
{
    EmailThread *em;

    int count = 40;

    stream1.frameTime = 0;
    sensor1.stream(stream1);

    qDebug() << "White Pixel Threshold is " << limit;

    unsigned long current_interval = interval;

    do{
        QThread::msleep(current_interval);

        //1. Grab a valid frame
        frame1 = sensor1.getFrame();
        assert(frame1.shot().id == stream1.id);

        //Convert from FCam Image to CIMG with 1 depth channel
        const FCam::Image &image = frame1.image();  //Image from sensor
        CImg<unsigned char> img(width, height,1);   //Blank CImg
        convertImage(image, img);                   //Populate CImg Reference

        //2. Confine colorspace to (norm=100) 25 discrete values
        img = img/norm;

        //3. Get subtracted difference between current image and average
        CImg<unsigned char> sub = (img-average).normalize();    //Normalize to {0,1}

        //4. Count number of white pixels
        int totalN = 0; cimg_forXYZC(sub,x,y,z,c) if(sub(x,y,z,c) > 0) totalN++;

        //5. Perform Open/Close and recount
        sub = sub.erode(erodeVar).dilate(erodeVar);
        int totalE = 0; cimg_forXYZC(sub,x,y,z,c) if(sub(x,y,z,c) > 0) totalE++;

        qDebug() << "Totals: Normal - " << totalN << ", Noise Removal -" << totalE << " count=" << count;


        //6. Check for movement -- if so: get new average image
        if (totalE > limit)
        {
            qDebug() << "GOTCHA!";
            record(10,20);             //record 10 images in quick succession.
            updateReferenceImage();

            //Send Email that logs movement
            if(emailAlert){
                em = new EmailThread(email_address,email_subject, email_message, email_attach, current_save_image);
                qDebug() << "Emailed";
            }

            //reset continuous movement counter
            current_interval = interval;      //Back to normal tick speed
            qDebug() << "Current interval:" << current_interval;
        }
    }while(count-- > 0);
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
        std::string file = current_save_image.toUtf8().constData();

        qDebug() << "Record:" << current_save_image;

        const FCam::Image &image = frame1.image();
        FCam::saveJPEG(image,file); //saves using FCAM instead of CImg -- faster.

        image_count++;
    }
    while (count ++<frame_num);
}
