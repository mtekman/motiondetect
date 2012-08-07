#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QLabel>
#include <QSettings>
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

    int width; //here for switch statements in moveSLider()
    QString size;

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
    void restoreInterface();

    void on_pushButton_clicked();

    void on_mask_slide_sliderMoved(int position);

    void on_pushButton_settings_clicked();

    void on_pushButton_stop_clicked();

    void show_widgets(bool show);

private:
    Ui::MainWindow *ui;
    void readLastWorkingSettings();
    void writeSettings(bool new_ones=false);

};

#endif // MAINWINDOW_H
