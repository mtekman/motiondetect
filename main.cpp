#include "mainwindow.h"
#include "timelapsewindow.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QStringList arguments = QApplication::arguments();

    //Enables passing Images as signals
    qRegisterMetaType<FCam::Image>("FCam::Image");

    MainWindow *mw; bool show = true;

    if(arguments.length()!=1) //1 is app name
    {
        CommandLine *cmd = new CommandLine(arguments);
        mw = new MainWindow(cmd);
        show = false;
        //
        if(cmd->silent) close(STDOUT_FILENO);
        delete cmd;
    }
    else{
        mw = new MainWindow;
    }

    if(show){
        mw->setOrientation(MainWindow::ScreenOrientationLockLandscape);
        mw->showExpanded();
    }

    /* //TimeLapse Window
    TimeLapseWindow tlw;
    tlw.showMaximized();*/

    return app.exec();
}

