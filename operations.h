#ifndef OPERATIONS_H
#define OPERATIONS_H

#include <QThread>
#include <FCam/Frame.h>
#include <FCam/N900.h>
#include <QDateTime>

//multiple inherit
class Operations : public QThread
{
    Q_OBJECT
public:
    int interval_min, interval_max;
    float interval_mod;

    int width, height;

    int limitVal;
    int erodeVar;

    bool echo_to_log;

    bool emailAlert;
    QString email_message;
    QString email_address;
    QString email_subject;
    bool email_attach;

    bool convert_images;
    bool delete_images;

    bool timelapse;

    QString image_dir;

    QDateTime time;


    Operations(bool timelap = false);
    ~Operations();
    void finishAndClose();
    void initial();

    //TimeLapse commands
    void takeSingleWellExposedPhoto();
    void convertMovie(bool delet);
    static void deathToAlarmdJobs();


public slots:
    void stop(){
        willStop = true;
        sensor1.stopStreaming();
    }

signals:
    //new movement Image has been taken
    void newImage(const FCam::Image &);

protected:
    void run();

private:
    bool willStop; //bool for stopping safely
    //Camera Variables
    FCam::N900::Sensor sensor1;      //Image Sensor
    FCam::Shot stream1;              //Shot
    FCam::Frame frame1;              //Frame

    void errorCheck();
    void updateReferenceImage(int frames=4);
    void checkMovement(int max=2000, int min=1000, float mod=0.8, int limit=200);
    void record(int frame_num, int interval_secs=0);
    void defineGoodExposure(int stableframenum = 10);
};
#endif // OPERATIONS_H
