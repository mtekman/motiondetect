#include "chooseop.h"
#include "ui_chooseop.h"
#include "motionwindow.h"
#include "timelapsewind.h"
#include "about.h"

chooseop::chooseop(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::chooseop)
{
#ifdef Q_WS_MAEMO_5
    this->setAttribute(Qt::WA_Maemo5StackedWindow);
    this->setWindowFlags(Qt::Window);
#endif
    ui->setupUi(this);
    //Null pointers
    mw =0; tlw =0;
}

chooseop::~chooseop()
{
    //Remove dangling pointers
    if(0!=mw) delete mw;
    if(0!=tlw) delete tlw;
    delete ui;
}

void chooseop::on_button_choose_watchdog_clicked()
{
    if(0!=tlw) {delete tlw; tlw = 0;}

    if(0==mw){
        mw = new MotionWindow(this); //No args given because not CommandLine.
        mw->showMaximized();
    }
    else if(0!=mw){ //already initialised;
        mw->showMaximized();
    }
}

void chooseop::on_button_choose_timelapse_clicked()
{
    if(0!=mw) {delete mw; mw = 0;}

    if(0==tlw){
        tlw = new TimeLapseWind(this);
        tlw->showMaximized();
    }
    else if(0!=tlw){ //already initalised
        tlw->showMaximized();
    }
}

void chooseop::on_button_choose_connect_clicked()
{
    ui->button_choose_connect->setText("Does Nothing For Now");
}

void chooseop::on_actionAbout_triggered()
{
    (new About)->exec();
}
