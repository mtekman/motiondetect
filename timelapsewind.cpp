#include "timelapsewind.h"
#include "ui_timelapsewind.h"
#include "alarmd_ops.cpp"

QTime now;

TimeLapseWind::TimeLapseWind(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TimeLapseWind)
{
    ui->setupUi(this);
    setDefaults();
    //
    ui->label_interval_day->setText("0");
    ui->label_interval_hour->setText("0");
    ui->label_interval_minute->setText("30");

    ui->label_cron->setText("Cookie:\tDetails:");
    updateJobList();
}

void TimeLapseWind::setDefaults(){
    //Default values
    now = QTime::currentTime();
    ui->label_hour->setText(QString::number(now.hour()));
    ui->label_minute->setText(QString::number(now.minute()));
    ui->label_second->setText(QString::number(now.second()));
}


TimeLapseWind::~TimeLapseWind()
{
    delete ui;
}

void TimeLapseWind::on_pushButton_set_cron_clicked()
{
    QDate job_date = ui->calendarWidget->selectedDate();

    QTime job_time(ui->label_hour->text().toInt(),
                   ui->label_minute->text().toInt(),
                   ui->label_second->text().toInt());

    QDateTime dt(job_date, job_time);

    if (dt.operator <(QDateTime::currentDateTime()))
    {
        ui->label_cron->setText("Please select date\nand time later than current!");
        ui->listWidget_cronjob->hide();ui->commandLinkButton->hide();
    }
    else
    {
        ui->listWidget_cronjob->show(); ui->commandLinkButton->show();
        ui->label_cron->setText("Cookie:\tDetails:");
        uint formatted_date = dt.toTime_t();

        std::string command = "Hello";
        addAlarmdJob(command,formatted_date);
    }

    //Update window
    updateJobList();

}
void TimeLapseWind::on_calendarWidget_activated(const QDate &date)
{
    QDate today = QDate::currentDate();
    if (date.operator <(today) )
    {
        ui->calendarWidget->setSelectedDate(today);
    }
}

void TimeLapseWind::updateJobList(){
    fetchMultipleJobs();
    ui->listWidget_cronjob->clear();
    ui->listWidget_cronjob->addItems(cookie_info);

}

void TimeLapseWind::on_pushButton_delete_cron_clicked()
{
    int length = ui->listWidget_cronjob->selectedItems().size();

    if(length!=0) {
        for(int i=0; i<length; i++){
            QString item = ui->listWidget_cronjob->selectedItems().at(i)->text();
            int cookie = item.split('\t').at(0).toInt();
            deleteAlarmd((cookie_t)(cookie));
        }
    }
    updateJobList();
}

void TimeLapseWind::on_commandLinkButton_clicked()
{
    updateJobList();
    setDefaults();
}


//Repeat Every:
void TimeLapseWind::on_dial_interval_days_sliderMoved(int position)
{
    //Difference between the set date and current
    int day_diff = -ui->calendarWidget->selectedDate().daysTo(QDate::currentDate());

    int time = ((float)(position)/(float)(100))*day_diff;

    ui->label_interval_day->setText(QString::number(time));
}

void TimeLapseWind::on_dial_interval_hours_sliderMoved(int position)
{
    int hour_scale = 24;

    //If future date is just a few hours away, then limit hour dial
    if(ui->label_interval_day->text().toInt()==0){
        hour_scale = ui->label_hour->text().toInt();
    }

    int time = ((float)(position)/(float)(100))*hour_scale;

    ui->label_interval_hour->setText(QString::number(time));

}

void TimeLapseWind::on_dial_interval_minute_sliderMoved(int position)
{
    int minute_scale = 60;

    //If future date is just a few minutes away, then limit minute dial
    if(ui->label_interval_day->text().toInt()==0 && ui->label_interval_minute->text().toInt()==0){
        minute_scale = ui->label_minute->text().toInt();
    }

    int time = ((float)(position)/(float)(100))*minute_scale;

    ui->label_interval_minute->setText(QString::number(time));
}

//Until:
void TimeLapseWind::on_dial_Hour_sliderMoved(int position)
{
    //Difference between the set date and current
    int day_diff = -ui->calendarWidget->selectedDate().daysTo(QDate::currentDate());

    int time = ((float)(position)/(float)(100))*24;
    if(day_diff==0) time = now.hour();

    ui->label_hour->setText(QString::number(time));
}

void TimeLapseWind::on_dial_Minute_sliderMoved(int position)
{
    int time = ((float)(position)/(float)(100))*60;
    ui->label_minute->setText(QString::number(time));
}


void TimeLapseWind::on_dial_Second_sliderMoved(int position)
{
    int time = ((float)(position)/(float)(100))*60;
    ui->label_second->setText(QString::number(time));
}
