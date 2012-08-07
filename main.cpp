#include "mainwindow.h"

#include <QtGui/QApplication>
#include <QFile>
#include <QTextStream>
#include <QDebug>


//This is taken from
//http://www.developer.nokia.com/Community/Wiki/How_to_redirect_qDebug_output_to_a_file
void handle(QtMsgType type, const char *msg)
{
    QString txt;
    switch (type) {
    case QtDebugMsg:
        txt = QString("Debug: %1").arg(msg);
        break;

    case QtWarningMsg:
        txt = QString("Warning: %1").arg(msg);
    break;
    case QtCriticalMsg:
        txt = QString("Critical: %1").arg(msg);
    break;
    case QtFatalMsg:
        txt = QString("Fatal: %1").arg(msg);
        abort();
    }

    QFile outFile("/home/user/.config/motion_detect.log");
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts(&outFile);
    QTextStream cons(stdin);
    //cons << txt << endl;
    ts << txt << endl;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

//    QApplication::arguments();

    //Custom Handler to output qDebug() to a Log.

    //qInstallMsgHandler(handle);




    MainWindow mainWindow;
    mainWindow.setOrientation(MainWindow::ScreenOrientationLockLandscape);
    mainWindow.showExpanded();

    return app.exec();
}
