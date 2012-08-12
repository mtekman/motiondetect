#include "timelapsewindow.h"
#include "ui_timelapsewindow.h"
#include <iostream>
#define APPID "timelapse"

// Very useful guide here:
//http://wiki.maemo.org/Documentation/Maemo_5_Developer_Guide/Using_Generic_Platform_Components/Alarm_Framework#Adding_Alarm_Event_to_Queue

TimeLapseWindow::TimeLapseWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TimeLapseWindow)
{
    ui->setupUi(this);

    //Update Positions
    ui->label_hour->move(ui->dial_Hour->pos());
    ui->label_minute->move(ui->dial_Minute->pos());

    cookie_t tree = addAlarmdJob();
    tree = addAlarmdJob();
    //fetchAlarmdJob(tree);
    fetchMultipleJobs();
}

TimeLapseWindow::~TimeLapseWindow()
{
    delete ui;
}

void TimeLapseWindow::on_dial_Hour_sliderMoved(int position)
{
    int time = ((float)(position)/(float)(100))*24;
    ui->label_hour->setText(QString::number(time));
}

void TimeLapseWindow::on_dial_Minute_sliderMoved(int position)
{
    int time = ((float)(position)/(float)(100))*60;
    ui->label_minute->setText(QString::number(time));
}

void TimeLapseWindow::on_pushButton_set_cron_clicked()
{
    //--- NOPE!QDate date = ui->calendarWidget->selectedDate();
    //QTime *time = new QTime(ui->label_hour->text().toInt(),
    //                       ui->label_minute->text().toInt(),
    //                       0);
}

/* MAYBE USE ALARMD (in-built) INSTEAD OF CRON
void TimeLapseWindow::addToCron() //http://www.thesitewizard.com/general/set-cron-job.shtml
{
    //minute(0-59) hour(0-23) day(1-31) month(1-12) dayofweek(0-7=sun) command
    // Examples:    45 10 * * * = 10:45 every day
    //              0 * * * * = every hour
*/    //              0 */2 * * * = every 2 hours     0 */8 * * * = every 8 hours
/*  //              0 8 1,20 * * = 8:00 on the 1st and 20th of every month


    char day_of_week = -1; // Nope. Do I use this? Probably not.

    //Date
    char month = 8; //August
    char day = 13; //13 August
    //Time
    char hour = 12; // midday
    char minute = 30; // 12:30 midday

}*/


/*void TimeLapseWindow::deleteAlarmdJob(){

}*/

void TimeLapseWindow::fetchMultipleJobs(){

    cookie_t *list = 0;
    cookie_t cookie = 0;
    alarm_event_t *eve = 0;

    if( (list = alarmd_event_query(0,0, 0,0, APPID)) == 0 )
    {
        std::cout << "query failed\n" << std::endl;
        goto cleanup;
    }

    for( int i = 0; (cookie = list[i]) != 0; ++i )
    {
        alarm_event_delete(eve);

        if( (eve = alarmd_event_get(cookie)) == 0 )
        {
            std::cout << "unable to get details for cookie=" << (long)cookie << std::endl;
            continue;
        }

        std::cout << "cookie: " << cookie << ctime(&eve->trigger) << std::endl;
    }

cleanup:
    free(list);
    alarm_event_delete(eve);
}



void TimeLapseWindow::fetchAlarmdJob(cookie_t cookie){
    std::cout << "fetching!" << std::endl;
    alarm_event_t *eve = 0;
    alarm_action_t *act = 0;
    time_t tmo = time(0);

    if( (eve = alarmd_event_get(cookie)) == 0 )
    {
        std::cout << "unable to get details for cookie=" << cookie << std::endl;
        goto cleanup;
    }
    std::cout << "cookie" << (long)cookie << std::endl;

    tmo -= eve->trigger;
    std::cout << "appid =" << alarm_event_get_alarm_appid(eve) << std::endl;
    std::cout << "trigger =" << (long)tmo << " " << ctime(&eve->trigger) << std::endl;;
    std::cout << "message = " << alarm_event_get_message(eve) << std::endl;

    for( int i = 0; (act = alarm_event_get_action(eve, i)) != 0; ++i )
    {
        std::cout << "action%d.label = " << i << " " << alarm_action_get_label(act)<< std::endl;
    }

cleanup:

    /* Free all dynamic memory associated with the
       * alarm event */
    alarm_event_delete(eve);
}

cookie_t TimeLapseWindow::addAlarmdJob(){
    cookie_t cookie = 0;
    alarm_event_t *eve = 0;
    alarm_action_t *act = 0;

    /* Create alarm event structure, set application
       * identifier and dialog message */
    eve = alarm_event_create();
    alarm_event_set_alarm_appid(eve, APPID);

    /* Use absolute time triggering, show dialog
       * ten seconds from now */
    eve->alarm_time = time(0) + 10;

    /* Add command*/
    act = alarm_event_add_actions(eve,1);
    act->flags |= ALARM_ACTION_WHEN_TRIGGERED;
    act->flags |= ALARM_ACTION_TYPE_EXEC;
    act->exec_command = "phone-control --notify Hello!;";

    /* Send the alarm to alarmd */
    cookie = alarmd_event_add(eve);

    std::cout << cookie << std::endl;

    /* Free all dynamic memory associated with the*/
    //alarm_event_delete(eve); //<-- Invalid pointer exception
    return cookie;
}
