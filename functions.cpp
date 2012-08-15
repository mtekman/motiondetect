#include <QProcess>
#include "CImg.h"
#include <FCam/FCam.h>
#include <iostream>
#include <string.h>


using namespace cimg_library;

char red_col[] = "\033[0;31m\033[1m";
char cyan_col[] = "\033[0;36m";
char yellow_col[] = "\033[0;33m";
char green_col[] = "\033[0;32m";
char stop_col[] = "\033[0m";

//Shared QProcess
QProcess *qp;

//convert address FCam to CImg
void convertImage(const FCam::Image &image,CImg<unsigned char> &img)
{
    unsigned char * imgBuffer = img.data();  //down cast 2bytes(uint) to 1byte (uchar)
    for (unsigned int y=0;y<image.height();y++){
        for (unsigned int x=0;x<image.width();x++){
            imgBuffer[0] = (*(image(x,y)+1));
            imgBuffer++;
        }
    }
}

QString terminalAction(QString command, bool getoutput=false)
{
    //TODO: Show output

    QString result = "";
    QStringList args;
    args << "-c" << command;
    qp = new QProcess;
    qp->start("/bin/sh", args);
    qp->waitForFinished();
    if(getoutput)
    {
        result = QString(qp->readAll());
    }
    qp->close();
    qp->terminate();
    return result;
}

//replace with phone function in main app
void alert(QString text, bool echo=true){
    if (echo) std::cout << red_col << text.toUtf8().data() << stop_col << std::endl;
    QString notif = QString("dbus-send --type=method_call --dest=org.freedesktop.Notifications /org/freedesktop/Notifications org.freedesktop.Notifications.SystemNoteInfoprint string:%1"+text+"%1").arg(QChar(0x22));
    terminalAction(notif);
}

void clearImages(QString &dir)
{
    QString clear = "rm "+dir+"*.jpg";
    terminalAction(clear);

    std::cout << "Images cleared from " << dir.toUtf8().data() << std::endl;
}


void convertToMP4(QString &dir, bool clear, QString videoname="movement", int fps=25)
{
    alert("Converting to MP4 -- please wait");

    if(!dir.endsWith('/'))  dir.append('/');
    if(!videoname.endsWith(".mpg")) videoname.append(".mpg");
    bool timelapse = videoname.contains("timelapse");

    std::cout << "Converting " << dir.toUtf8().data() << "*.jpg to " << (dir+videoname).toUtf8().data() << std::endl;
    if(timelapse) std::cout << "Timelapse" << std::endl;

    QString fileformat = (!timelapse)?"motiondetect-%05d.jpg":"timelapse-%06d.jpg";
    QString convert = "ffmpeg -i "+dir+fileformat+" -y "+dir+videoname+" -r "+QString::number(fps); //y forces overwrite, r must come at end to specify OUTPUT framerate
    terminalAction(convert, true);

    std::cout << "Finished converting." << std::endl;

    if(clear) clearImages(dir);

    alert("Finished!", false);
}

//Taken from FCamera
bool lensClosed() {
    FILE * ff = fopen("/sys/devices/platform/gpio-switch/cam_shutter/state", "r");
    char state = fgetc(ff);
    fclose(ff);
    return state == 'c'; //o = open, c = close
}

void echoLog(std::string text){
    FILE *ff = fopen("/home/user/.config/motion_detect.log", "a+");
    fprintf(ff, text.c_str());
    fclose(ff);
}

void killAllOccurencesOfApp(){
    QStringList pids = terminalAction("ps aux | grep teenage\\-dipl | awk '{print $1}'", true).split('\n');
    QString kill = "echo 'kill ";
    for (char i=0; i< pids.length(); i++)
    {
        kill.append(pids.at(i)+" ");
    }
    kill.append("' | root");
    std::cout << "Command: " << kill.toUtf8().data() << std::endl;

    terminalAction(kill);
}
