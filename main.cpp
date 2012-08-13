#include "chooseop.h"
#include "motionwindow.h"
#include "timelapsewind.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QStringList arguments = QApplication::arguments();

    //Enables passing Images as signals
    qRegisterMetaType<FCam::Image>("FCam::Image");

    int length = arguments.length();

    //ChooseOp window is launched by default
    if(length==1){ //1 is app name
        chooseop *cp = new chooseop;
        cp->showMaximized();
    }
    else if(length>1)  //Otherwise commandline ops are performed.
    {
        CommandLine *cmd = new CommandLine(arguments);
        if(cmd->silent) close(STDOUT_FILENO);
        //
        new MotionWindow(0,cmd); //Starts quiet
        //
        delete cmd;
    }

    return app.exec();
}

