
#include "CImg.h"
#include <FCam/FCam.h>
#include <QProcess>
#include <iostream>

using namespace cimg_library;

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

QString terminalAction(QString command, bool getoutput=false, bool show_output=false)
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
    if (echo) std::cout << text.toUtf8().data() << std::endl;
    QString notif = QString("dbus-send --type=method_call --dest=org.freedesktop.Notifications /org/freedesktop/Notifications org.freedesktop.Notifications.SystemNoteInfoprint string:%1"+text+"%1").arg(QChar(0x22));
    terminalAction(notif);
}

void clearImages(QString &dir)
{
    QString clear = "rm "+dir+"*.jpg";
    terminalAction(clear);

    std::cout << "Images cleared from " << dir.toUtf8().data() << std::endl;
}


void convertToMP4(QString &dir, bool clear, QString videoname="movement")
{
    alert("Converting to MP4 -- please wait");

    if(!dir.endsWith('/'))  dir.append('/');
    if(!videoname.endsWith(".mpg")) videoname.append(".mpg");

    std::cout << "Converting " << dir.toUtf8().data() << "*.jpg to " << (dir+videoname).toUtf8().data() << std::endl;

    QString convert = "ffmpeg -i "+dir+"%05d.jpg -y "+dir+videoname; //y forces overwrite
    terminalAction(convert, true);

    std::cout << "Finished converting." << std::endl;

    if(clear) clearImages(dir);

    alert("Finished!", false);
}

//Taken from FCamera
bool lensClosed() {
    FILE * ff = fopen("/sys/devices/platform/gpio-switch/cam_shutter/state", "r");
    char state = fgetc(ff);   // file contains either "open" or "closed"
    fclose(ff);
    return state == 'c';
}
