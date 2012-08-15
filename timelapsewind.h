#ifndef TIMELAPSEWIND_H
#define TIMELAPSEWIND_H

#include <QMainWindow>
#include <QDate>

namespace Ui {
class TimeLapseWind;
}

class TimeLapseWind : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit TimeLapseWind(QWidget *parent = 0);
    ~TimeLapseWind();
    
private slots:
    void on_dial_Hour_sliderMoved(int position);
    void on_dial_Minute_sliderMoved(int position);
    void on_dial_Second_sliderMoved(int position);

    void on_calendarWidget_activated(const QDate &date);

    void on_pushButton_set_cron_clicked();
    void on_pushButton_delete_cron_clicked();
    void on_pushButton_delete_cron_2_clicked();

    void on_commandLinkButton_clicked();

    void on_dial_interval_days_sliderMoved(int position);
    void on_dial_interval_hours_sliderMoved(int position);
    void on_dial_interval_minute_sliderMoved(int position);
    void on_dial_interval_second_sliderMoved(int position);

    void on_checkBox_never_clicked(bool checked);

    void on_dial_size_sliderMoved(int position);
    void on_dial_fps_sliderMoved(int position);

private:
    Ui::TimeLapseWind *ui;
    void updateJobList();
    void setDefaults();
    int calculateDayDiff();

};

#endif // TIMELAPSEWIND_H
