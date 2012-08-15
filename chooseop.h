#ifndef CHOOSEOP_H
#define CHOOSEOP_H

#include <QMainWindow>
#include "motionwindow.h"
#include "timelapsewind.h"

namespace Ui {
class chooseop;
}

class chooseop : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit chooseop(QWidget *parent = 0);
    ~chooseop();
    
private slots:
    void on_button_choose_watchdog_clicked();

    void on_button_choose_connect_clicked();

    void on_button_choose_timelapse_clicked();

    void on_actionAbout_triggered();

private:
    MotionWindow *mw;
    TimeLapseWind *tlw;
    Ui::chooseop *ui;
};

#endif // CHOOSEOP_H
