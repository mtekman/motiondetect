#include "timelapsewind.h"
#include "ui_timelapsewind.h"
#include "alarmd_ops.cpp"

QTime now;
QString default_text = "Cookie:\tDetails:";

TimeLapseWind::TimeLapseWind(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TimeLapseWind)
{
#ifdef Q_WS_MAEMO_5
    this->setAttribute(Qt::WA_Maemo5StackedWindow);
    this->setWindowFlags(Qt::Window);
//    this->setAttribute(Qt::WA_DeleteOnClose);
#endif
    ui->setupUi(this);
    setDefaults();
    //
    ui->label_interval_day->setText("0");
    ui->label_interval_hour->setText("0");
    ui->label_interval_minute->setText("30");

    ui->label_cron->setText(default_text);
    updateJobList();
}

TimeLapseWind::~TimeLapseWind()
{
    std::cout << "TimeLapseWindow Deleted!" << std::endl;
    delete ui;
}

void TimeLapseWind::on_pushButton_set_cron_clicked()
{
    int tab_index = ui->tabWidget->currentIndex();
    bool never = ui->checkBox_never->isChecked();

    /*Work is only performed if Repeat Until Tab is active.
    If not, it is activated and the Repeat Never checkbox is ticked*/
    if(tab_index==1){
        //Final Date Data
        QDate job_date = ui->calendarWidget->selectedDate();

        QTime job_time(ui->label_hour->text().toInt(),
                       ui->label_minute->text().toInt(),
                       ui->label_second->text().toInt());

        QDateTime final_date(job_date, job_time);

        //Error Check
        if (final_date.operator <(QDateTime::currentDateTime()))
        {
            ui->label_cron->setText("Please select date\nand time later than current!");
            ui->listWidget_cronjob->hide();
        }

        //Perform Operation
        else {
            ui->listWidget_cronjob->show();
            ui->label_cron->setText(default_text);

            //Create Command -- Final_Date -> String
            QString date = "/opt/motiondetect/bin/motiondetect --teenage-diplomacy "+
                    QString::number(final_date.date().year()).rightJustified(4,'0')+":"+
                    QString::number(final_date.date().month()).rightJustified(2,'0')+":"+
                    QString::number(final_date.date().day()).rightJustified(2,'0')+":"+

                    QString::number(final_date.time().hour()).rightJustified(2,'0')+":"+
                    QString::number(final_date.time().minute()).rightJustified(2,'0')+":"+
                    QString::number(final_date.time().second()).rightJustified(2,'0')+" ";
            //Flags
            int convert_flag = (ui->checkBox_convert->isChecked())?1:-1;
            int delete_flag = (ui->checkBox_delete->isChecked())?1:-1;
            if(never) convert_flag = delete_flag = -1;
            date.append(QString::number(convert_flag)+" "+QString::number(delete_flag));

            std::cout << "Query = " << date.toUtf8().data() << std::endl;

            char * command = date.toUtf8().data();
            //char * command = "phone-control --notify Hellow";

            if(never){
                //--QDate to time_t
                uint formatted_date = final_date.toTime_t();
                addAlarmdJob(command,formatted_date);
            }
            else if(!never)
            {
                /*  Step Interval Data
                --  Generate dates at x,y,z steps from each other, starting
                    with current Date.*/
                int days = ui->label_interval_day->text().toInt();
                int hours = ui->label_interval_hour->text().toInt();
                int minutes = ui->label_interval_minute->text().toInt();

                //Error Check -- If true, would add infinite amount.
                if( !(days == 0 && hours==0 && minutes==0) ){

                    QDateTime iterator = QDateTime::currentDateTime();

                    while( (iterator = iterator.addDays(days)
                            .addSecs((minutes*60)+(hours*3600))
                            ).operator <(final_date) )
                    {
                        //Iterator to time_t
                        uint formatted_date = iterator.toTime_t();
                        addAlarmdJob(command,formatted_date);
                    }
                    //Iterate ONCE more, so that commandline knows to convert to video
                    //when job is given at a later date than final_date.
                    uint formatted_date = iterator.toTime_t();
                    addAlarmdJob(command,formatted_date);
                }
            }
        }
    }
    else if(tab_index==0){ //Until:
        ui->tabWidget->setCurrentIndex(1); //Repeat
        ui->checkBox_never->setChecked(true);
        on_checkBox_never_clicked(true);
    }
    //Update window
    if(ui->listWidget_cronjob->isVisible()) updateJobList();

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
    ui->label_cron->setText(default_text+'\t'+QString::number(num_jobs)+" Jobs");
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

int TimeLapseWind::calculateDayDiff(){
    return -ui->calendarWidget->selectedDate().daysTo(QDate::currentDate());
}


//Repeat Every:
void TimeLapseWind::on_dial_interval_days_sliderMoved(int position)
{
    //Difference between the set date and current
    int day_diff = calculateDayDiff();
    int time = ((float)(position)/(float)(100))*day_diff;

    ui->label_interval_day->setText(QString::number(time));
}

void TimeLapseWind::on_dial_interval_hours_sliderMoved(int position)
{
    int hour_scale = 24;

    //If future date is just a few hours away, then limit hour dial
    if(calculateDayDiff()==0){
        hour_scale = ui->label_hour->text().toInt() - now.hour();
    }

    int time = ((float)(position)/(float)(100))*hour_scale;

    ui->label_interval_hour->setText(QString::number(time));

}

void TimeLapseWind::on_dial_interval_minute_sliderMoved(int position)
{
    int minute_scale = 60;
    int time = ((float)(position)/(float)(100))*minute_scale;

    ui->label_interval_minute->setText(QString::number(time));
}

//Until:
void TimeLapseWind::on_dial_Hour_sliderMoved(int position)
{
    int time = ((float)(position)/(float)(100))*24;
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


//Other:
void TimeLapseWind::setDefaults(){
    //Default values
    now = QTime::currentTime();
    ui->label_hour->setText(QString::number(now.hour()));
    ui->label_minute->setText(QString::number(now.minute()));
    ui->label_second->setText(QString::number(now.second()));
}

void TimeLapseWind::on_pushButton_delete_cron_2_clicked()
{
    int length = ui->listWidget_cronjob->count();

    for(int i=0; i<length; i++){
        QString item = ui->listWidget_cronjob->item(i)->text();
        std::cout << "Deleting " << item.toUtf8().data() << std::endl;
        int cookie = item.split('\t').at(0).toInt();
        deleteAlarmd((cookie_t)(cookie));
    }
    updateJobList();
}

void TimeLapseWind::on_checkBox_never_clicked(bool checked)
{
    if(checked)
    {
        ui->dial_interval_days->setDisabled(true);
        ui->dial_interval_hours->setDisabled(true);
        ui->dial_interval_minute->setDisabled(true);
        ui->label_interval_day->setDisabled(true);
        ui->label_interval_hour->setDisabled(true);
        ui->label_interval_minute->setDisabled(true);
        //If it never repeats, it never needs to convert;
        ui->checkBox_convert->setDisabled(true); ui->checkBox_delete->setDisabled(true);
    }
    else{
        ui->dial_interval_days->setEnabled(true);
        ui->dial_interval_hours->setEnabled(true);
        ui->dial_interval_minute->setEnabled(true);
        ui->label_interval_day->setEnabled(true);
        ui->label_interval_hour->setEnabled(true);
        ui->label_interval_minute->setEnabled(true);
        ui->checkBox_convert->setEnabled(true); ui->checkBox_delete->setEnabled(true);
    }
}
