#include "mainwindow.h"
#include <QtGui/QApplication>
#include "QDebugStream.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QStringList arguments = QApplication::arguments();

    QDebugStream(std::cout, "/home/user/MyDocs/DCIM/MISC/log.txt");

    //Enables passing Images as signals
    qRegisterMetaType<FCam::Image>("FCam::Image");


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

    if(show){
        mw->setOrientation(MainWindow::ScreenOrientationLockLandscape);
        mw->showExpanded();
    }



    return app.exec();
}

