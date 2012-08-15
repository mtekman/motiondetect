#include "MotionWindow.h"
#include "ui_MotionWindow.h"

#include <QtCore/QCoreApplication>
#include <QCloseEvent>

int image_label_height = 100;

MotionWindow::MotionWindow(QWidget *parent, CommandLine *commands)
    : QMainWindow(parent), ui(new Ui::MotionWindow)
{
#ifdef Q_WS_MAEMO_5
    this->setAttribute(Qt::WA_Maemo5StackedWindow);
#endif
    ui->setupUi(this);

    //initialise camerathread here. Prevents pointer problems with settings.
    op = new Operations;
    set = 0; //null pointer for settings

    if(commands == 0){ //Not assigned
        //For repeated use with newImage Slot
        image_label_height = ui->imageLab->height();

        connect(op, SIGNAL(finished()), this, SLOT(restoreInterface()));  //returns interfacr to normal on close

        readLastWorkingSettings();
//        on_mask_slide_sliderMoved(ui->mask_slide->value());
        op->timelapse = false;
        op->echo_to_log = false;

        width = op->width;
        ui->pushButton_stop->hide();
        ui->label_mask_hint->hide();

    }
    else{ //Perform commandLineOps
        connect(op,SIGNAL(finished()), this, SLOT(closeAndExit()));

        op->timelapse = commands->timelapse;

        op->limitVal = commands->white;
        op->interval_max = commands->max;
        op->interval_min = commands->min;
        op->interval_mod = commands->mod;

        op->echo_to_log = commands->log;

        op->emailAlert = commands->email;
        op->email_message = commands->message;
        op->email_address = commands->address;
        op->email_subject = commands->subject;
        op->email_attach = commands->attach;

        op->convert_images = commands->convert;
        op->delete_images = commands->del;

        op->width = commands->width;
        op->height = commands->height;

        op->image_dir = commands->dir;

        op->erodeVar = commands->mask;

        op->time.setDate(commands->time->date());
        op->time.setTime(commands->time->time());

        op->initial();
        op->start();

        delete commands;

    }
}

MotionWindow::~MotionWindow()
{
    std::cout << "MotionWindow Deleted" << std::endl;
    if(0!=set) delete set; //Remove dangling pointers
    delete ui;
}

void MotionWindow::closeAndExit(){
    this->close();
    delete ui;
    exit(1);
}

void MotionWindow::setOrientation(ScreenOrientation orientation)
{
#if defined(Q_OS_SYMBIAN)
    // If the version of Qt on the device is < 4.7.2, that attribute won't work
    if (orientation != ScreenOrientationAuto) {
        const QStringList v = QString::fromAscii(qVersion()).split(QLatin1Char('.'));
        if (v.count() == 3 && (v.at(0).toInt() << 16 | v.at(1).toInt() << 8 | v.at(2).toInt()) < 0x040702) {
            qWarning("Screen orientation locking only supported with Qt 4.7.2 and above");
            return;
        }
    }
#endif // Q_OS_SYMBIAN

    Qt::WidgetAttribute attribute;
    switch (orientation) {
#if QT_VERSION < 0x040702
    // Qt < 4.7.2 does not yet have the Qt::WA_*Orientation attributes
    case ScreenOrientationLockPortrait:
        attribute = static_cast<Qt::WidgetAttribute>(128);
        break;
    case ScreenOrientationLockLandscape:
        attribute = static_cast<Qt::WidgetAttribute>(129);
        break;
    default:
    case ScreenOrientationAuto:
        attribute = static_cast<Qt::WidgetAttribute>(130);
        break;
#else // QT_VERSION < 0x040702
    case ScreenOrientationLockPortrait:
        attribute = Qt::WA_LockPortraitOrientation;
        break;
    case ScreenOrientationLockLandscape:
        attribute = Qt::WA_LockLandscapeOrientation;
        break;
    default:
    case ScreenOrientationAuto:
        attribute = Qt::WA_AutoOrientation;
        break;
#endif // QT_VERSION < 0x040702
    };
    setAttribute(attribute, true);
}

void MotionWindow::showExpanded()
{
#if defined(Q_OS_SYMBIAN) || defined(Q_WS_SIMULATOR)
    showFullScreen();
#elif defined(Q_WS_MAEMO_5)
    showMaximized();
#else
    show();
#endif
}

void MotionWindow::on_pushButton_clicked()
{
    readLastWorkingSettings();
    show_widgets(false);
    op->erodeVar = ui->maskEdit->text().toInt();
    op->start();
}

void MotionWindow::on_mask_slide_sliderMoved(int position)
{
    ui->label_mask_hint->show();

    int mask = (0.2*exp( 0.059915*position) )+3;
    ui->maskEdit->setText(QString::number(mask));

    //Roaming QLabel with size hints
    int height = ui->mask_slide->rect().height();

    int slider_pos = height * ((float)(position)/(float)100);

    QPoint coords(80, ui->mask_slide->pos().y()+ (height - slider_pos ) - 80);
    QString display(" ");

    //Statements
    QString warn = " - Warning: Mask value this low detects noise\nfor image size "+size;
    QString small =" - Smallest Noise-Free Mask";
    QString good = " - Good Mask Size for "+size;

    switch(width){
    case 320:
        switch(mask){case 0:case 1:case 2: display = warn; break; case 3: display = small; break; case 4: display = good; break;default: display = ""; break;}; break;
    case 640:
        switch(mask){case 0:case 1:case 2:case 3: display = warn; break; case 4: display = small; break; case 5: display = good; break;default: display = ""; break;}; break;
    case 800:
        switch(mask){case 0:case 1:case 2:case 3:case 4: display = warn; break;case 5: display = small; break;case 6: display = good; break;default: display = ""; break;}; break;
    case 1280:
        switch(mask){case 0:case 1:case 2:case 3:case 4:case 5: display = warn; break;case 6: display = small; break;case 7: display = good; break;default: display = ""; break;}; break;
    }
    ui->label_mask_hint->setText(QString::number(mask)+display);
    ui->label_mask_hint->move(coords);
}

void MotionWindow::on_pushButton_settings_clicked()
{
    if(0==set){
        set = new Settings(this);
        set->exec();
    }
    else{ //already an instance open
        set->showMaximized();
    }

    readLastWorkingSettings();
    width = set->width;
    size = set->size;
}

void MotionWindow::on_pushButton_stop_clicked()
{
    op->stop();
    restoreInterface();
}


void MotionWindow::show_widgets(bool show)
{
    if(!show)
    {
        ui->imageLab->show();
        //Disable widgets
        ui->mask_slide->setDisabled(true); ui->maskEdit->setDisabled(true); ui->label_3->setDisabled(true);
        //Hide button
        ui->pushButton->hide(); ui->pushButton_stop->show(); ui->pushButton_settings->setDisabled(true);
        ui->label_mask_hint->hide();

    }
    else
    {
        ui->imageLab->hide();
        //Enable widgets
        ui->mask_slide->setEnabled(true); ui->maskEdit->setEnabled(true); ui->label_3->setEnabled(true);
        //ui->pushButton->hide();
        //Show button
        ui->pushButton->show(); ui->pushButton_stop->hide(); ui->pushButton_settings->setEnabled(true);
        ui->label_mask_hint->show();
    }
}

//Read and write last saved values
void MotionWindow::writeSettings(bool new_ones){
    if(!new_ones)
    {
        QSettings settings("fcam");
        settings.beginGroup("main");

        settings.endGroup();
    }
    else if(new_ones)
    {
        std::cout << "writing new settings" << std::endl;

        QSettings settings("fcam");
        settings.beginGroup("main");

        settings.setValue("time", QTime(0,1));

        settings.setValue("width", 320);
        settings.setValue("height", 240);

        settings.setValue("interval_max", 10);
        settings.setValue("interval_min", 1);
        settings.setValue("interval_modifier",0.16);

        settings.setValue("whitepix_value", 100);

        settings.setValue("email_bool",false);
        settings.setValue("attach", false);
        settings.setValue("email_address",  "blank@blanketyblank.blank");
        settings.setValue("email_subject", "blank");
        settings.setValue("email_message", "blank");

        settings.setValue("image_dir", "/home/user/MyDocs/DCIM/MISC/");
        settings.setValue("convert_video", true);
        settings.setValue("delete_images", true);

        settings.endGroup();

        //Now execute
        readLastWorkingSettings();
    }
}


void MotionWindow::readLastWorkingSettings()
{
    QSettings settings("fcam");
    settings.beginGroup("main");
    bool exists = !settings.value("width").isNull();

    if (exists)
    {
        QSettings settings("fcam");
        settings.beginGroup("main");

        size = settings.value("size_label").toString();

        QTime tim = settings.value("time").toTime();
        op->time = QDateTime(QDateTime::currentDateTime().addSecs( (60*60*tim.hour())+(60*tim.minute())+tim.second() ) );

        op->limitVal = settings.value("whitepix_value").toInt();
        op->interval_max = settings.value("interval_max").toInt();
        op->interval_min = settings.value("interval_min").toInt();
        op->interval_mod = settings.value("interval_modifier").toFloat();

        op->emailAlert = settings.value("email_bool").toBool();
        op->email_message = settings.value("email_message").toString();
        op->email_address = settings.value("email_address").toString();
        op->email_subject = settings.value("email_subject").toString();
        op->email_attach = settings.value("attach").toBool();

        op->convert_images = settings.value("convert_video").toBool();
        op->delete_images = settings.value("delete_images").toBool();

        op->width = settings.value("width").toInt();
        op->height = settings.value("height").toInt();

        op->image_dir = settings.value("image_dir").toString();

        settings.endGroup();


    }
    else if(!exists)
    {
        writeSettings(true);
    }

}

void MotionWindow::restoreInterface(){
    show_widgets(true);
}

void MotionWindow::newImage(const FCam::Image &image){
    //cout << "Got image" << endl;
    QImage thumbQ(image(0,0), image.width()/2, image.height()/2, image.bytesPerRow()*2, QImage::Format_RGB32);

    QPixmap qp(QPixmap::fromImage(thumbQ));
    ui->imageLab->setPixmap(qp.scaledToHeight(image_label_height) );
}

void MotionWindow::on_checkBox_show_image_clicked(bool checked)
{
    if(checked) {
        connect(op, SIGNAL(newImage(const FCam::Image &)), this, SLOT(newImage(const FCam::Image &) ) );
        ui->imageLab->show();
    }
    else {
        disconnect(op, SIGNAL(newImage(const FCam::Image &)), this, SLOT(newImage(const FCam::Image &) ) );
        ui->imageLab->hide();
    }
}

void MotionWindow::closeEvent(QCloseEvent *event){
    event->ignore();
    op->stop();

    ui->label_mask_hint->show();
    ui->label_mask_hint->setText("Stopping Camera, Please Wait!");
    ui->label_mask_hint->raise();
    ui->label_mask_hint->move(ui->pushButton->pos());

    while(op->isRunning()){/*Wait*/}
    event->accept();
}
