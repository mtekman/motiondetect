#include "mainwindow.h"

#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
    std::cout << "Application launched" << std::endl;

    QApplication app(argc, argv);

//    QApplication::arguments();


    MainWindow mainWindow;
    mainWindow.setOrientation(MainWindow::ScreenOrientationLockLandscape);
    mainWindow.showExpanded();

    return app.exec();

    std::cout << "Application Exited" << std::endl;
}
