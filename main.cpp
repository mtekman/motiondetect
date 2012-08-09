#include "mainwindow.h"
#include <QtGui/QApplication>


int main(int argc, char *argv[])
{
    // -q --quiet --size 320 240 -s 160 140 -m 4 --mask 3 -r 30 2 --range 4 1 --email tetris11@gmail.com "howdy" "Doody" true -e blank@grog.com "Truly" "motion" false -d /home/user --directory /home/ -m 0.9 --modifier 0.1 -w 60 --whitepix 80 --convert

    QApplication app(argc, argv);

    QStringList arguments = QApplication::arguments();

    MainWindow *mw; bool show = true;

    if(arguments.length()!=1) //1 is app name
    {
        CommandLine *cmd = new CommandLine(arguments);
        mw = new MainWindow(cmd);
        show = false;
        delete cmd;
    }
    else{
        mw = new MainWindow;
    }

    std::cout << "Application launched" << std::endl;

    if(show){
        mw->setOrientation(MainWindow::ScreenOrientationLockLandscape);
        mw->showExpanded();
    }


    return app.exec();
    std::cout << "Application Exited - " << std::endl;

}

