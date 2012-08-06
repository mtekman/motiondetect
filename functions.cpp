
#include "CImg.h"
#include <FCam/FCam.h>
#include <QProcess>

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

void terminalAction(QString command, bool show_output=false)
{
    //TODO: Show output
    QStringList args;
    args << "-c" << command;
    qp = new QProcess;
    qp->start("/bin/sh", args);
    qp->waitForFinished();
    qp->terminate();
}

void clearImages(QString &dir)
{
    QString clear = "rm "+dir+"*.jpg";
    terminalAction(clear);

    qDebug() << "Images cleared from " << dir;
}


void convertToMP4(QString &dir, bool clear, QString videoname="movement")
{
    if(!dir.endsWith('/'))  dir.append('/');
    if(!videoname.endsWith(".mpg")) videoname.append(".mpg");

    qDebug() << "Converting " << dir << "*.jpg to " << dir+videoname;

    QString convert = "ffmpeg -i "+dir+"%05d.jpg -y "+dir+videoname; //y forces overwrite
    terminalAction(convert, true);

    qDebug() << "finished converting.";

    if(clear) clearImages(dir);
}

//Taken from FCamera
bool lensClosed() {
    FILE * ff = fopen("/sys/devices/platform/gpio-switch/cam_shutter/state", "r");
    char state = fgetc(ff);   // file contains either "open" or "closed"
    fclose(file);
    return state == 'c';
}
