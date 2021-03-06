#include <alarmd/libalarm.h>
#include <iostream>
#include <QStringList>

#define APPID "timelapse"

QStringList cookie_info;
int num_jobs = 0;

// Very useful guide here:
//http://wiki.maemo.org/Documentation/Maemo_5_Developer_Guide/Using_Generic_Platform_Components/Alarm_Framework#Adding_Alarm_Event_to_Queue

//True - deleted, False - unable to.
bool deleteAlarmd(cookie_t cookie)
{
    return alarmd_event_del(cookie) != -1;
}

int fetchMultipleJobs(){
    cookie_t *list = 0;
    cookie_t cookie = 0;
    alarm_event_t *eve = 0;

    cookie_info.clear();
    int i=0;

    if( (list = alarmd_event_query(0,0, 0,0, APPID)) == 0 )
    {
        std::cout << "Could not fetch" << std::endl;
        goto cleanup;
    }
    for(i = 0; (cookie = list[i]) != 0; ++i )
    {
        alarm_event_delete(eve); //clear current details
        if( (eve = alarmd_event_get(cookie)) == 0 )
        {
            std::cout << "Details for cookie " << (long)cookie << " could not be acessed" << std::endl;
            continue;
        }
        //Add results to lists;
        cookie_info.append(QString::number( (long)(cookie) )+'\t'+  QString(ctime(&eve->trigger)));
    } num_jobs = i;

cleanup:
    free(list);
    alarm_event_delete(eve);
}

cookie_t addAlarmdJob(QString command, uint secs){
    cookie_t cookie = 0;
    alarm_event_t *eve = 0;
    alarm_action_t *act = 0;

    /* Create alarm event structure, set application
       * identifier and dialog message */
    eve = alarm_event_create();
    alarm_event_set_alarm_appid(eve, APPID);

    /* Use absolute time triggering, show dialog
       * ten seconds from now */
    eve->alarm_time = secs; //time(0) + secs;

    /* Add command*/
    act = alarm_event_add_actions(eve,1);
    act->flags |= (ALARM_ACTION_WHEN_TRIGGERED | ALARM_ACTION_TYPE_EXEC);

    QString action = "/bin/ash -c '"+command+"'";

//    std::cout << "ACTIONNY! " << action.toUtf8().data() << std::endl;
    alarm_action_set_exec_command(act, action.toUtf8().data());

    /* Send the alarm to alarmd */
    cookie = alarmd_event_add(eve);

    /* Free all dynamic memory associated with the*/
    //alarm_event_delete(eve); //<-- Invalid pointer exception
    return cookie;
}
