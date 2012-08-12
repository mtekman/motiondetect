#ifndef TIMELAPSEWINDOW_H
#define TIMELAPSEWINDOW_H

#include <QDialog>
#include <alarmd/libalarm.h>

namespace Ui {
class TimeLapseWindow;
}

class TimeLapseWindow : public QDialog
{
    Q_OBJECT
    
public:
    explicit TimeLapseWindow(QWidget *parent = 0);
    ~TimeLapseWindow();
    
private slots:
    void on_dial_Hour_sliderMoved(int position);

    void on_dial_Minute_sliderMoved(int position);

    void on_pushButton_set_cron_clicked();

private:
    Ui::TimeLapseWindow *ui;
    cookie_t addAlarmdJob();
    void fetchAlarmdJob(cookie_t cookie);
    void fetchMultipleJobs();
};

#endif // TIMELAPSEWINDOW_H
