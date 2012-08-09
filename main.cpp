#include "mainwindow.h"
#include <QtGui/QApplication>
#include "cmdline.cpp"


int main(int argc, char *argv[])
{
    // -q --quiet --size 320 240 -s 160 140 -m 4 --mask 3 -r 30 2 --range 4 1 --email tetris11@gmail.com "howdy" "Doody" true -e blank@grog.com "Truly" "motion" false -d /home/user --directory /home/ -m 0.9 --modifier 0.1 -w 60 --whitepix 80 --convert

    QApplication app(argc, argv);

    args = QApplication::arguments();

    if(args.length()!=1) //1 is app name
    {
        //Check version and help tags first
        checkVersionOrHelp();

        //interval range
        int min,max; checkRange(min,max);

        //modifier
        float mod = checkModifier();

        //whitepixel count
        int white = checkWhitepix();

        //size
        int width,height; checkSize(width, height);

        //hidden mode (background)
        bool hide = checkQuiet();

        //convert/delete
        bool convert, del; checkConvertDelete(convert,del);

        //save image directory
        QString dir = checkImageDir();

        //Email
        bool email,attach; QString address, message, subject;
        checkEmail(email,address,message,subject,attach);

        cout << "range=(" << min << "," << max << ") mod="<< mod<< " white="<< white << " size=("<< width <<","<< height <<") hide=" << hide
            <<" convert="<< convert <<" delete="<< del << " dir=" << dir.toUtf8().data()
            << "\nEmail:" << email << address.toUtf8().data() << message.toUtf8().data() << subject.toUtf8().data() << attach << endl;

    }

    exit(1);

    std::cout << "Application launched" << std::endl;
    MainWindow mainWindow;

    //if(!hide){
        mainWindow.setOrientation(MainWindow::ScreenOrientationLockLandscape);
        mainWindow.showExpanded();
    //}

    std::cout << "Application Exited - " << app.exec() << std::endl;
}

