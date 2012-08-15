#include "timelapsewind.h"
#include "ui_timelapsewind.h"
#include "alarmd_ops.cpp"
#include "operations.h"

QTime now;
QString default_text = "Cookie:\tDetails:";

//Kill_Switch for running kill command on all jobs -- EMERGENCY ONLY.
//--How? Press on DeleteAll, then Delete One, then SetJob in that order
char kill_switch = 0;

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
    ui->label_interval_second->setText("0");

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
    if(kill_switch == 2){
        Operations::deathToAlarmdJobs();
    }
    kill_switch=0;

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
            ui->tabWidget->setCurrentIndex(1);
        }

        //Perform Operation
        else {
            ui->listWidget_cronjob->show();
            ui->label_cron->setText(default_text);

            //Create Command -- Final_Date -> String
            QString commander = "/opt/motiondetect/bin/motiondetect --teenage-diplomacy "+
                    QString::number(final_date.date().year()).rightJustified(4,'0')+":"+
                    QString::number(final_date.date().month()).rightJustified(2,'0')+":"+
                    QString::number(final_date.date().day()).rightJustified(2,'0')+":"+

                    QString::number(final_date.time().hour()).rightJustified(2,'0')+":"+
                    QString::number(final_date.time().minute()).rightJustified(2,'0')+":"+
                    QString::number(final_date.time().second()).rightJustified(2,'0')+" ";
            //Flags
            int convert_flag = (ui->checkBox_convert->isChecked())?1:-1;
            int delete_flag = (ui->checkBox_delete->isChecked())?1:-1;
            int width = ui->label_size->text().split(" x ").at(0).toInt();
            int fps = ui->label_fps->text().split(" ").at(0).toInt();
            if(never) convert_flag = delete_flag = -1;

            commander.append(QString::number(convert_flag)+" "+QString::number(delete_flag)+" "+
                        QString::number(width)+" "+QString::number(fps));
            std::cout << "Query = " << commander.toUtf8().data() << std::endl;

            if(never){
                //--QDate to time_t
                uint formatted_date = final_date.toTime_t();
                addAlarmdJob(commander,formatted_date);
            }
            else if(!never)
            {
                /*  Step Interval Data
                --  Generate dates at x,y,z steps from each other, starting
                    with current Date.*/
                int days = ui->label_interval_day->text().toInt();
                int hours = ui->label_interval_hour->text().toInt();
                int minutes = ui->label_interval_minute->text().toInt();
                int seconds = ui->label_interval_second->text().toInt();

                //Error Check -- If true, would add infinite amount.
                if( !(days == 0 && hours==0 && minutes==0 && seconds==0) ){

                    QDateTime iterator = QDateTime::currentDateTime();

                    while( (iterator = iterator.addDays(days)
                            .addSecs(seconds+(minutes*60)+(hours*3600))
                            ).operator <(final_date) )
                    {
                        //Iterator to time_t
                        uint formatted_date = iterator.toTime_t();
                        addAlarmdJob(commander,formatted_date);
                    }
                    //Iterate ONCE more, so that commandline knows to convert to video
                    //when job is given at a later date than final_date.
                    uint formatted_date = (iterator
                            .addDays(days)
                            .addSecs(seconds+(minutes*60)+(hours*3600))).toTime_t();
                    addAlarmdJob(commander,formatted_date);
                }
            }
        }
    }
    else if(tab_index==0){ //Until:
        ui->tabWidget->setCurrentIndex(1); //Repeat
        ui->checkBox_never->setChecked(true);
        on_checkBox_never_clicked(true);
    }
    else if(2== tab_index){
        ui->tabWidget->setCurrentIndex(1);
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
    (kill_switch == 1)?kill_switch++:kill_switch=0;

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
int time_var = 0; //share vars

void TimeLapseWind::on_dial_interval_days_sliderMoved(int position)
{
    //Difference between the set date and current
    int day_diff = calculateDayDiff();
    time_var = ((float)(position)/(float)(100))*day_diff;
    ui->label_interval_day->setText(QString::number(time_var));
}

void TimeLapseWind::on_dial_interval_hours_sliderMoved(int position)
{
    //If future date is just a few hours away, then limit hour dial
    int hour_scale = 24;
    if(calculateDayDiff()==0){
        hour_scale = ui->label_hour->text().toInt() - now.hour();
    }

    time_var = ((float)(position)/(float)(100))*hour_scale;
    ui->label_interval_hour->setText(QString::number(time_var));

}

void TimeLapseWind::on_dial_interval_minute_sliderMoved(int position)
{
    time_var = ((float)(position)/(float)(100))*60;
    ui->label_interval_minute->setText(QString::number(time_var));
}

void TimeLapseWind::on_dial_interval_second_sliderMoved(int position)
{
    time_var =  ((float)(position)/(float)(100))*60;

    //Minimum of 20 seconds.
    if(ui->label_interval_day->text().toInt()==0 &&
            ui->label_interval_hour->text().toInt()==0 &&
            ui->label_interval_minute->text().toInt()==0 && time_var<30)
    {
        time_var = 20;
    }

    ui->label_interval_second->setText(QString::number(time_var));
}

//Until:
void TimeLapseWind::on_dial_Hour_sliderMoved(int position)
{
    time_var = ((float)(position)/(float)(100))*24;
    ui->label_hour->setText(QString::number(time_var));
}

void TimeLapseWind::on_dial_Minute_sliderMoved(int position)
{
    time_var = ((float)(position)/(float)(100))*60;
    ui->label_minute->setText(QString::number(time_var));
}


void TimeLapseWind::on_dial_Second_sliderMoved(int position)
{
    time_var = ((float)(position)/(float)(100))*60;
    ui->label_second->setText(QString::number(time_var));
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
    (kill_switch == 0)?kill_switch++:kill_switch=0;

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
        ui->dial_interval_second->setDisabled(true);
        ui->label_interval_day->setDisabled(true);
        ui->label_interval_hour->setDisabled(true);
        ui->label_interval_minute->setDisabled(true);
        ui->label_interval_second->setDisabled(true);
        //If it never repeats, it never needs to convert;
        ui->checkBox_convert->setDisabled(true); ui->checkBox_delete->setDisabled(true);
    }
    else
    {
        ui->dial_interval_days->setEnabled(true);
        ui->dial_interval_hours->setEnabled(true);
        ui->dial_interval_minute->setEnabled(true);
        ui->dial_interval_second->setEnabled(true);
        ui->label_interval_day->setEnabled(true);
        ui->label_interval_hour->setEnabled(true);
        ui->label_interval_minute->setEnabled(true);
        ui->label_interval_second->setEnabled(true);
        ui->checkBox_convert->setEnabled(true); ui->checkBox_delete->setEnabled(true);
    }
}

void TimeLapseWind::on_dial_size_sliderMoved(int position)
{
    int size_index = position/25;
    int height = 240, width = 320;

    switch(size_index){
    case 0: width=320; height=240; break;
    case 1: width=640; height=480; break;
    case 2: width=800; height = 600; break;
    case 3: width=1280; height = 960; break;
    }
    ui->label_size->setText(QString::number(width)+" x "+QString::number(height));
}

void TimeLapseWind::on_dial_fps_sliderMoved(int position)
{
    int valid_ranges = position/15;
    int fps = 25;

    switch(valid_ranges){
    case 0: fps=1; break;
    case 1: fps=5;break;
    case 2: fps=10; break;
    case 3: fps=15; break;
    case 4: fps=20; break;
    case 5: fps=25; break;
    case 6: fps=30; break;
    default: fps=25; break;
    }
    ui->label_fps->setText(QString::number(fps)+" fps");
}
