#ifndef OPERATIONS_H
#define OPERATIONS_H

#include <QThread>
#include <FCam/Frame.h>
#include <FCam/N900.h>

//multiple inherit
class Operations : public QThread
{

public:
    int interval_default;
    int width, height;

    int frameNum;
    int limitVal;
    int erodeVar;

    bool emailAlert;
    QString email_message;
    QString email_address;
    QString email_subject;
    bool email_attach;

    bool convert_images;
    bool delete_images;

    QString image_dir;

    //double triggerFract
    //Time is in microseconds *10^-6
    Operations(int w=320, int h=240, int exp=1000000, float gain=1.0f,
               bool histogram=true);
    void finishAndClose(bool quickly=false);
    //No destructor

protected:
    void run();

private:
    void errorCheck();
    void getStaticBGImage(int stableframenum = 10); //50ms
    //For Debugging:
    void updateReferenceImage();
    void checkMovement(int interval=2000, int limit=23000);
    void record(int frame_num, int interval=0);
};
#endif // OPERATIONS_H
