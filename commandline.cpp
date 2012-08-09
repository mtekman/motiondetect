#include "commandline.h"

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

    //Echo out commands not understood
    unknowns(); //Halt here for unknowns variables

//    cout << "range=(" << min << "," << max << ") mod="<< mod<< " white="<< white << " size=("<< width <<","<< height
//         <<") Show=" << show << " Mask=" << mask
//         <<" convert="<< convert <<" delete="<< del << " dir=" << dir.toUtf8().data()
//        << "\nEmail:" << email << " " << address.toUtf8().data() << " " << message.toUtf8().data() << " "<<subject.toUtf8().data() << " "<<attach << endl;

}

CommandLine::~CommandLine(){}

void CommandLine::checkMask(){
    int m_index = 0; mask = 5; //default
    if( ((m_index=args.indexOf("--mask"))!=-1) || ((m_index=args.indexOf("-m"))!=-1) )
    {
        mask = args.at(m_index+1).toInt();
        if(mask < 2){
            cerr << "Invalid Mask Size" << endl;
            exit(1);
        }
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

        //count args until next flag
        int index = email_index;
        while( (++index < args.length()-1) && (args.at(index).at(0)!='-')){/*count*/}

        cout << "Args #:" << (index - email_index) << endl;
        if ((index - email_index)!=4){
            cerr << "email format is: -e <address> <message> <subject> <Y/N>" << endl;
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
            cerr << "Please use absolute paths" << endl;
            exit(1);
        }
        args.removeAt(i_i+1); args.removeAt(i_i);
    }
}

void CommandLine::checkConvertDelete()
{
    del = convert = false; //default
    if(del=  ((args.contains("--delete")) || (args.contains("-d")) ) ){
        if (convert= !( (args.contains("--convert")) || (args.contains("-c")) ) )
        {
            cerr << "Why delete images you wont convert? Delete images after you convert them first." << endl;
            exit(1);
        }
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
            cerr << "usage: -s width height" << endl;
            exit(1);
        }

        switch(width){
        case 320:  height=240; break;
        case 640:  height=480; break;
        case 800:  height=600; break;
        case 1280: height=960; break;
        default:
            cerr << "Valid widths are : 320 640 800 1280" << endl;
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
            cerr << "White needs to be a valid integer" << endl;
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
            cerr << "Usage: -r <min> <max>, in seconds" << endl;
            exit(1);
        }
        min = val1.toInt();
        max = val2.toInt();
        if(min == 0 || max == 0){
            cerr << "Invalid range " << endl;
            exit(1);
        }
        if(min > max){
            cerr << "min " << min <<" is greater than max " << max << "\nPlease fix." << endl;
            exit(1);
        }
        //remove from args if used
        args.removeAt(r_index+2); args.removeAt(r_index+1); args.removeAt(r_index);
    }
}

void CommandLine::checkModifier(){
    mod = 0.16; //default
    int m_index=0;
    if( (m_index=args.indexOf("-m"))!=-1 ||
            (m_index=args.indexOf("--modifier"))!=-1){
        mod = args.at(m_index+1).toFloat();
        if(mod==0 || mod<0 || mod>1){
            cerr << mod << " is not a valid float value. Using default 0.16" << endl;
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
        cout << "usage: motiondetect [OPTION...]\n"
              "  -h, --help\t\t* Print this help\n"
              "  -v, --version\t\t* Display version\n"
              "  -m, --mask INT\t\t* Size of mask in square pixels\n"
              "  -i, --images DIRECTORY \t* Where images are saved.\n\t\t\t\t  Default:/home/user/MyDocs/DCIM/MISC/\n"
              "  -c, --convert\t\t* Converts movement images to an\n\t\t\t\t  MPG file in te image directory\n"
              "  -d, --delete\t\t* Deletes images on app exit. \n\t\t\t\t  Useful only if convert flag specified too\n"
              "  -s, --size WIDTH HEIGHT\t* Valid sizes are: 320 240,\n\t\t\t\t  640 480, 800 600, 1280 960\n"
              "  -w, --whitepix INT\t* Threshold for detecting\n\t\t\t  white pixels in eroded images. Default is 100.\n"
              "  -r, --range MIN MAX\t* Intervals in seconds that\n\t\t\t  camera varies between.\n"
              "  -m, --modifier FLOAT\t* Determines adaptiveness of\n\t\t\t  camera interval. 0.1 v.responsive --> 1.0 constant.\n"
              "  -e, --email ADDRESS MESSAGE SUBJECT [Y/N]\n\t* Email upon movement. Y attaches the last image to email.\n"
        << endl;
        exit(1);
    }
}

void CommandLine::unknowns(){
    if(args.length()!=0){
        cerr << "Could not parse: ";
        for(int i=0; i< args.length(); i++){
            cerr << args.at(i).toUtf8().data() << " ";
        }
        cerr << endl;
        cerr << "Please try --help for more info " << endl;
        exit(1);
    }
}
