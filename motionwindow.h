#ifndef MOTIONWINDOW_H
#define MOTIONWINDOW_H

#include <QtGui/QMainWindow>
#include <QSettings>
#include <iostream>
#include "settings.h"
#include "operations.h"
#include "commandline.h"

namespace Ui {
class MotionWindow;
}

class MotionWindow : public QMainWindow
{
    Q_OBJECT
public:
    Settings *set;
    Operations *op;

    int width; //here for switch statements in moveSLider()
    QString size;

    enum ScreenOrientation {
        ScreenOrientationLockPortrait,
        ScreenOrientationLockLandscape,
        ScreenOrientationAuto
    };

    //commands needs to be a pointer, since cannot initialise references
    explicit MotionWindow(QWidget *parent = 0, CommandLine *commands = 0);
    virtual ~MotionWindow();

    // Note that this will only have an effect on Symbian and Fremantle.
    void setOrientation(ScreenOrientation orientation);

    void showExpanded();

public slots: //why public? So main.cpp can use them via cmdline
    void restoreInterface();

    void on_pushButton_clicked();

    void on_mask_slide_sliderMoved(int position);

    void on_pushButton_settings_clicked();

    void on_pushButton_stop_clicked();

    void closeAndExit();
    void newImage(const FCam::Image &image);


private slots:
    void show_widgets(bool show);


    void on_checkBox_show_image_clicked(bool checked);

private:
    Ui::MotionWindow *ui;
    void readLastWorkingSettings();
    void writeSettings(bool new_ones=false);
    void closeEvent(QCloseEvent *event);

};


#endif // MOTIONWINDOW_H
