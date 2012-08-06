#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <operations.h>
#include <QLabel>
#include "settings.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    Settings *set;
    Operations *op;

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

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
