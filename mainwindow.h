#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QLabel>
#include "settings.h"
#include "operations.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    Settings *set;
    Operations *op;

    //Duplicate variables in Operations.... sigh
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
/////////////////////

    enum ScreenOrientation {
        ScreenOrientationLockPortrait,
        ScreenOrientationLockLandscape,
        ScreenOrientationAuto
    };

    explicit MainWindow(QWidget *parent = 0);
    virtual ~MainWindow();

    QLabel *img;

    // Note that this will only have an effect on Symbian and Fremantle.
    void setOrientation(ScreenOrientation orientation);

    void showExpanded();

private slots:
    void on_pushButton_clicked();
    void on_dial_actionTriggered();

    void on_mask_slide_sliderMoved(int position);

    void on_pushButton_settings_clicked();

    void on_pushButton_stop_clicked();

private:
    Ui::MainWindow *ui;
    void readLastWorkingSettings(bool exists=true);
    void writeSettings(bool new_ones=false);
};

#endif // MAINWINDOW_H
