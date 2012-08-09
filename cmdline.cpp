#include <QString>
#include <QStringList>
#include <iostream>

using namespace std;

QStringList args;

void checkEmail(bool &email, QString &address, QString &message, QString &subject, bool &attach)
{
    int email_index = 0;
    if( (email_index=args.indexOf("-e"))!=-1 ||
        (email_index=args.indexOf("--email"))!=-1 ){
        email = true;

        int ind= 1;
        //count args until next flag
        while( !((args.at(email_index+ind)).at(0)=='-') ){
            ind ++;
        }
        if (ind!=4){
            cerr << "email format is: -e <address> <message> <subject> <true/false>" << endl;
            exit(1);
        }
        address = args.at(email_index+1);
        if (!address.contains("@")){
            cerr << "Not a valid address" << endl;
            exit(1);
        }
        message = args.at(email_index+2);
        subject = args.at(email_index+3);
        attach = (args.at(email_index+4).toLower().at(0))=='y';
    }
    else{ //flag not given
        email = false;
        address = "blank@blank.com";
        message = "Hello";
        subject = "Motion Detected";
        attach = false;
    }
}

QString checkImageDir()
{
    int i_i =0;
    QString dir = "/home/user/MyDocs/DCIM/MISC/"; //default
    if ( (i_i=args.indexOf("-i"))!=-1 || (i_i=args.indexOf("--images"))!=-1) {
        dir = args.at(i_i+1);
        if(dir.at(0)=='/'){
            cerr << "Please use absolute paths" << endl;
            exit(1);
        }
    }
    return dir;
}

void checkConvertDelete(bool &convert, bool &del)
{
    if(del=(args.contains("--delete")) || (args.contains("-d")) ){
        if (!(convert= (args.contains("--convert")) || (args.contains("-c")) ))
        {
            cerr << "Why delete images you wont convert? Delete images after you convert them first." << endl;
            exit(1);
        }
    }
}

bool checkQuiet(){
    return (args.contains("--quiet") || args.contains("-q"));
}

void checkSize(int &width, int &height)
{
    int s_i = 0;
    if ( (s_i=args.indexOf("-s"))!=-1 || (s_i=args.indexOf("--size"))!=-1){
        width= args.at(s_i+1).toInt();
        switch(width){
        case 320:  height=240; break;
        case 640:  height=480; break;
        case 800:  height=600; break;
        case 1280: height=960; break;
        default:
            cerr << "Valid widths are : 320 640 800 1280" << endl;
            exit(1);
        }// height can be ignoredd
    }
    else{ //size not given
        width = 320; height = 240;
    }
}

int checkWhitepix()
{
    int w_index = 0, white = 100; //default
    if((w_index =args.indexOf("--whitepix"))!=-1 || (w_index =args.indexOf("-w"))!=-1 )
    {
        white = args.at(w_index+1).toInt();
        if(white==0){
            cerr << "White needs to be a valid integer" << endl;
            exit(1);
        }
    }
    return white;
}


void checkRange(int &min, int&max)
{
    int r_index = 0;
    if( (r_index=args.indexOf("-r"))!=-1 ||
            (r_index=args.indexOf("--range"))!=-1){
        //Check two values given
        QString val1 = args.at(r_index+1);
        QString val2 = args.at(r_index+2);

        if(val1.at(0)=='-' || val2.at(0)=='-'){
            cerr << "Usage: -r <min> <max>, in seconds" << endl;
            exit(1);
        }
        min = val1.toInt();
        max = val2.toInt();
        if(min > max){
            cerr << "min " << min <<" is greater than max " << max << "\nPlease fix." << endl;
            exit(1);
        }
    }
    else{ //not given
        min = 1; max= 10;
    }
}

float checkModifier(){
    float mod = 0.16; //default
    int m_index=0;
    if( (m_index=args.indexOf("-m"))!=-1 ||
            (m_index=args.indexOf("--modifier"))!=-1){
        mod = args.at(m_index+1).toFloat();
        if(mod==0 || mod<0 || mod>1){
            cerr << mod << " is not a valid float value. Using default 0.16" << endl;
            mod = 0.16;
        }
    }
    return mod;
}

void checkVersionOrHelp(){
    if(args.contains("-v") || args.contains("--version"))
    {
        cout << "Version 0.9.2" << endl;
        exit(1);
    }
    if(args.contains("-h") || args.contains("--help"))
    {
        cout << "usage: motiondetect [OPTION...]\n"
              "\t-h, --help\t\tPrint this help\n"
              "\t-v, --version\t\tDisplay version\n"
              "\t-q, --quiet\t\tRun app invisibly\n"
              "\t-i, --images DIRECTORY \tWhere images are saved. Default:/home/user/MyDocs/DCIM/MISC/\n"
              "\t-c, --convert\t\tConverts movement images to an mpg file in te image directory\n"
              "\t-d, --delete\t\tDeletes images on app exit. Useful only if convert flag specified too\n"
              "\t-s, --size WIDTH HEIGHT\tValid sizes are: 320 240, 640 480, 800 600, 1280 960\n"
              "\t-w, --whitepix INT\t\tThreshold for detecting white pixels in eroded images. Default is 100.\n"
              "\t-r, --range MIN MAX\t\tIntervals in seconds that camera varies between.\n"
              "\t-m, --modifier FLOAT\tDetermines adaptiveness of camera interval. 0.1 v.responsive --> 1.0 constant.\n"
              "\t-e, --email ADDRESS MESSAGE SUBJECT [Y/N]\tEmail upon movement. Y attaches the last image to email. N Doesnt.\n"
        << endl;
        exit(1);
    }
}
