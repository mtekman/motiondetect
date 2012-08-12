#include "commandline.h"
#include <QString>

char red[] =  "\033[0;31m\033[1m";
char cyan[] = "\033[0;36m";
char yellow[] = "\033[0;33m";
char green[] = "\033[0;32m";
char stop[] = "\033[0m";

CommandLine::CommandLine(QStringList &arguments){
    args = arguments;
    args.removeFirst(); //gets rid of app name from list for faster searching

    //Check version and help tags first
    checkVersionOrHelp();
    // mask
    checkMask();
    //interval range
    checkRange();
    //modifier
    checkModifier();
    //whitepixel count
    checkWhitepix();
    //size
    checkSize();
    //convert/delete
    checkConvertDelete();
    //save image directory
    checkImageDir();
    //Email
    checkEmail();
    //Time
    checkTime();
    // TimeLapse
    secretDate();    //<--- secret method not shown in help. Put below checkTime to override time, convert and delete variable;
    // Output File
    checkLog();
    // Silent Mode
    checkSilent();

    //Echo out commands not understood
    unknowns(); //Halt here for unknown variables

}
CommandLine::~CommandLine(){}

//Reusing variables is bad, but fun.
void CommandLine::secretDate(){  // --teenage-diplomacy <DateTime> <Convert> <Delete>
    int d_index=0; timelapse = false;
    if ( ( d_index=args.indexOf("--teenage-diplomacy"))!=-1){  //Two words unlikely to be typed next to each other
        //Date will be yyyy:mm:dd:HH:MM:SS
        QStringList date_time = args.at(d_index+1).split(":");

        //Booleans: Delete and Convert. 1= true, -1= false;
        int in_convert = args.at(d_index+2).toInt();
        int in_del = args.at(d_index+3).toInt();

        if( in_convert==0 || in_del==0 || date_time.length()!=6){
            cerr << red << "Nice try." << stop << endl;
            exit(1);
        }

        //Parse Bools:
        convert = (in_convert==1); del = (in_del==1);

        //Parse Date:
        int year = date_time.at(0).toInt(), month = date_time.at(1).toInt(), day = date_time.at(2).toInt();
        int hour = date_time.at(3).toInt(), minute = date_time.at(4).toInt(), second = date_time.at(5).toInt();

        time = new QDateTime( QDate(year,month,day), QTime(hour,minute,second) );

        //Finish
        timelapse = true;
        args.removeAt(d_index+3); args.removeAt(d_index+2);
        args.removeAt(d_index+1); args.removeAt(d_index);
    }
}

void CommandLine::checkSilent(){
    int s_index=0; silent=false; //default
    if(  (s_index=args.indexOf("-q"))!=-1 || (s_index=args.indexOf("--quiet"))!=-1)
    {
        silent = true;
        args.removeAt(s_index);
        cout << yellow << "Log mode is enabled too" << stop << endl;
        log = true;
    }
}

void CommandLine::checkTime(){
    int t_index = 0;
    int secs=0, mins=1, hour=0, days=0; //1 min default

    if ( (t_index=args.indexOf("-t"))!=-1 || (t_index=args.indexOf("--time"))!=-1 )
    {
        QStringList time_list = args.at(t_index+1).split(":");

        switch(time_list.length()){
        case 1:{
            //Single number specified -- Seconds.
            secs = time_list.at(0).toInt();
            mins = 0; //override default
            if(secs==0)
            {
                cerr << red << "Not valid seconds" << stop << endl;
                exit(1);
            }
            break;
        }
        case 2:{
            //Two numbers -- mm:ss
            mins = time_list.at(0).toInt();
            secs = time_list.at(1).toInt();
            break;
        }
        case 3:{
            //hh:mm:ss
            hour = time_list.at(0).toInt();
            mins = time_list.at(1).toInt();
            secs = time_list.at(2).toInt();
            break;
        }
        case 4:{
            //dd:hh:mm:ss
            days = time_list.at(0).toInt();
            hour = time_list.at(1).toInt();
            mins = time_list.at(2).toInt();
            secs = time_list.at(3).toInt();
            break;
        }
        default:{
            cerr << red
                 << "Too few or too many variables given for time argument" <<
                    stop << endl;
            exit(1); break; //<- hah!
        }
        }

        if( days==0 && hour==0 && mins==0 && secs<10){
            cerr << red <<
                    "Invalid time specified. Please consult --help for compatible time formats."
                 << stop << endl;
        }
        args.removeAt(t_index+1); args.removeAt(t_index);
    }
    //Set datetime -- append current;
    time = new QDateTime(QDateTime::currentDateTime()
                         .addSecs( (60*60*hour)+(60*mins)+secs)
                         .addDays(days));
}


void CommandLine::checkLog(){
    int c_index = 0;
    if((c_index =args.indexOf("--log"))!=-1 || (c_index =args.indexOf("-l"))!=-1 )
    {
        log = true;
        args.removeAt(c_index);
    }
}


void CommandLine::checkMask(){
    int m_index = 0; mask = 5; //default
    if( ((m_index=args.indexOf("--mask"))!=-1) || ((m_index=args.indexOf("-m"))!=-1) )
    {
        mask = args.at(m_index+1).toInt();
        if(mask < 2){
            cerr << red << "Invalid Mask Size" << stop << endl;
            exit(1);
        }
        args.removeAt(m_index+1); args.removeAt(m_index);
    }
}


void CommandLine::checkEmail()
{
    //Defaults:
    email = false;
    address = "blank@blank.com";
    message = "Hello";
    subject = "Motion Detected";
    attach = false;

    int email_index = 0;
    if( (email_index=args.indexOf("-e"))!=-1 ||
            (email_index=args.indexOf("--email"))!=-1 ){

        email = true;

        // Count args until next flag
        int index = email_index;
        while( (++index < args.length()-1) && (args.at(index).at(0)!='-')){/*count*/}

        //cout << "Args #:" << (index - email_index) << endl;
        if ((index - email_index)!=4){
            cerr << red
                 << "email format is: -e <address> <message> <subject> <Y/N>"
                 << stop << endl;
            exit(1);
        }
        address = args.at(email_index+1);
        if (!address.contains("@")){
            cerr << red << "Not a valid address" << stop << endl;
            exit(1);
        }
        message = args.at(email_index+2);
        subject = args.at(email_index+3);
        attach = (args.at(email_index+4).toLower().at(0))=='y';

        //
        args.removeAt(email_index+4); args.removeAt(email_index+3);
        args.removeAt(email_index+2); args.removeAt(email_index+1);
        args.removeAt(email_index);
    }
}

void CommandLine::checkImageDir()
{
    int i_i =0;
    dir = "/home/user/MyDocs/DCIM/MISC/"; //default
    if ( (i_i=args.indexOf("-i"))!=-1 || (i_i=args.indexOf("--images"))!=-1) {
        dir = args.at(i_i+1);
        if(dir.at(0)!='/'){
            cerr << red << "Please use absolute paths" << stop << endl;
            exit(1);
        }
        args.removeAt(i_i+1); args.removeAt(i_i);
    }
}

void CommandLine::checkConvertDelete()
{
    del = convert = false; //default

    int d_i = 0, c_i = 0;
    if( (d_i=args.indexOf("--delete"))!=-1 || (d_i=args.indexOf("-d"))!=-1 ){
        del = true; args.removeAt(d_i);
    }
    if ( (c_i=args.indexOf("--convert"))!=-1 || (c_i=args.indexOf("-c"))!=-1 ){
        convert = true; args.removeAt(c_i);
    }
    if(del && !convert)
    {
        cerr << red << "Delete images after you convert them first.\n"
                "Otherwise the app saves frames for nothing." << stop << endl;
        exit(1);
    }

}

void CommandLine::checkSize()
{
    width =320; height = 240; //defaults
    int s_i = 0;
    if ( (s_i=args.indexOf("-s"))!=-1 || (s_i=args.indexOf("--size"))!=-1){
        width= args.at(s_i+1).toInt();
        //check height exists too ( though not needed)
        if( s_i+2 >= args.length() ||  args.at(s_i+2).at(0)=='-' ){
            cerr << red << "usage: -s width height" << stop << endl;
            exit(1);
        }

        switch(width){
        case 320:  height=240; break;
        case 640:  height=480; break;
        case 800:  height=600; break;
        case 1280: height=960; break;
        default:
            cerr << red << "Valid widths are : 320 640 800 1280" << stop << endl;
            exit(1);
        }// height can be ignored
        args.removeAt(s_i+2);args.removeAt(s_i+1);args.removeAt(s_i);
    }
}

void CommandLine::checkWhitepix()
{
    int w_index = 0;
    white = 100; //default
    if((w_index =args.indexOf("--whitepix"))!=-1 || (w_index =args.indexOf("-w"))!=-1 )
    {
        white = args.at(w_index+1).toInt();

        if(white==0 || white > 5000){
            cerr << red << "White needs to be a valid integer" << stop << endl;
            exit(1);
        }
        args.removeAt(w_index+1);args.removeAt(w_index);
    }
}


void CommandLine::checkRange()
{
    min = 1; max= 10;       //defaults
    int r_index = 0;
    if( (r_index=args.indexOf("-r"))!=-1 ||
            (r_index=args.indexOf("--range"))!=-1){
        //Check two values given
        QString val1 = args.at(r_index+1);
        QString val2 = args.at(r_index+2);

        if(val1.at(0)=='-' || val2.at(0)=='-'){
            cerr << cyan << "Usage: -r <min> <max>, in seconds" << endl;
            exit(1);
        }
        min = val1.toInt();
        max = val2.toInt();
        if(min == 0 || max == 0){
            cerr << "Invalid range " << endl;
            exit(1);
        }
        if(min > max){
            cerr << cyan << "min " << min <<" is greater than max " << max
                 << "\nPlease fix." << stop << endl;
            exit(1);
        }
        //remove from args if used
        args.removeAt(r_index+2); args.removeAt(r_index+1); args.removeAt(r_index);
    }
}

void CommandLine::checkModifier(){
    mod = 0.16; //default
    int m_index=0;
    if( (m_index=args.indexOf("-a"))!=-1 ||
            (m_index=args.indexOf("--adapt"))!=-1){
        mod = args.at(m_index+1).toFloat();
        if(mod==0 || mod<0 || mod>1){
            cerr << yellow << mod << stop
                 << red << " is not a valid float value. Using default 0.16" << stop << endl;
            mod = 0.16;
        }
        //remove from args if used
        args.removeAt(m_index+1); args.removeAt(m_index);
    }
}

void CommandLine::checkVersionOrHelp(){
    if(args.contains("-v") || args.contains("--version"))
    {
        cout << "Version 0.9.2" << endl;
        exit(1);
    }
    if(args.contains("-h") || args.contains("--help"))
    {
        cout << cyan << "usage: motiondetect [OPTION...]\n" << stop
        << green<< "  -h, --help " << stop << yellow << "   * Print this help\n"
        << green << "  -v, --version" <<  yellow << "   * Display version\n"
        << green << "  -m, --mask INT" <<  yellow << "   * Size of mask in square pixels\n"
        << green << "  -q, --quiet" <<  yellow << "   * Silent mode. Messages recorded to log file\n"
        << green << "  -i, --images DIRECTORY" <<  yellow << "  * Where images are saved. Default:~/MyDocs/DCIM/MISC/\n"
        << green << "  -c, --convert" <<  yellow << "   * Convert all images in image directory  to an MPG file\n"
        << green << "  -d, --delete" <<  yellow << "   * Delete images on exit. Convert flag must be specified too\n"
        << green << "  -s, --size WIDTH HEIGHT" <<  yellow << "   * Valid sizes: 320 240, 640 480, 800 600, 1280 960\n"
        << green << "  -w, --whitepix INT" <<  yellow << "   * White pixel count threshold. Default: 100\n"
        << green << "  -r, --range MIN MAX" <<  yellow << "   * Intervals (secs) to restrict camera. Default: 1 10\n"
        << green << "  -a, --adapt FLOAT" <<  yellow << "   * Adaptiveness of interval. Default: 0.16 (v.responsive)\n"
        << green << "  -t, --time secs OR " <<  yellow << "mm:ss "<<green<<"OR"<<yellow<<" hh:mm:ss"<<green<<" OR"<<yellow <<" dd:hh:mm:ss\n"
        << green << "  -l, --log" <<  yellow << "   * Output to log file: ~/.config/motion_detect.log\n"
        << green << "  -e, --email ADDRESS MESSAGE SUBJECT [Y/N]" <<  yellow << "\n\t* Email upon movement. Y attaches the last image to email.\n"
        << stop << endl;
        exit(1);
    }
}

void CommandLine::unknowns(){
    if(args.length()!=0){
        cerr << red << "Could not parse: ";
        for(int i=0; i< args.length(); i++){
            cerr << args.at(i).toUtf8().data() << " ";
        }
        cerr << stop << endl;
        cerr << cyan << "Please try --help for more info " << stop << endl;
        exit(1);
    }
}
