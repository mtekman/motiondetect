#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtCore/QCoreApplication>
#include <QSettings>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
#ifdef Q_WS_MAEMO_5
    this->setAttribute(Qt::WA_Maemo5StackedWindow);
    this->setWindowFlags(Qt::Window);
#endif
    ui->setupUi(this);
    ui->frameAv->display(1);
    ui->pushButton_stop->hide();

    //Share ImageLabel -- so that Operations can access it.
    img = ui->imageLab;

   //Load Settings --- doesnt work
    if(!QSettings("Motion","Detect").contains("image_dir") )
    {
        readLastWorkingSettings(false);
    }
}

bool settings_pressed = false;

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setOrientation(ScreenOrientation orientation)
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

void MainWindow::showExpanded()
{
#if defined(Q_OS_SYMBIAN) || defined(Q_WS_SIMULATOR)
    showFullScreen();
#elif defined(Q_WS_MAEMO_5)
    showMaximized();
#else
    show();
#endif
}

void MainWindow::on_pushButton_clicked()
{
    readLastWorkingSettings();

    op = new Operations(width, height);

    //connect(ui->pushButton_stop, SIGNAL(clicked()), op, SLOT(stop()));

    op->limitVal = limitVal;
    op->interval_default = interval_default;

    op->emailAlert = emailAlert;
    op->email_message = email_message;
    op->email_address = email_address;
    op->email_subject = email_subject;
    op->email_attach = email_attach;

    op->convert_images = convert_images;
    op->delete_images = delete_images;

    op->image_dir = image_dir;

    op->frameNum = ui->frameAv->value();
    op->erodeVar = ui->maskEdit->text().toInt();

    qDebug() << " frame:" << op->frameNum << " mask:" << op->erodeVar << " thresh:" << op->limitVal << " interval:" << op->interval_default
             << " email:" << op->emailAlert << op->email_message << op->email_address << op->email_subject << op->email_attach
             << " convert:" << op->convert_images << " delete:" << op->delete_images << " width,height" << op->width << op->height
             << "image dir:" << op->image_dir;

    show_widgets(false);

    qDebug() << "YOU";
    op->start();
    qDebug() << "NRW";

    //writeSettings();
}

//TODO REVERT BACK TO HAVINFG NO DUPLICATES


void MainWindow::on_dial_actionTriggered()
{
    int val = ui->dial->value()/4;
    if(val!=0) ui->frameAv->display(val);
    else ui->frameAv->display(1);
}

void MainWindow::on_mask_slide_sliderMoved(int position)
{
    int mask = (0.2*exp( 0.059915*position) )+3;
    ui->maskEdit->setText(QString::number(mask));

    //Roaming QLabel with size hints

    int height = ui->mask_slide->rect().height();

    int slider_pos = height * ((float)(position)/(float)100);

    QPoint coords(200, ui->mask_slide->pos().y()+ (height - slider_pos ) );
    QString display(" ");

    switch(width){
    case 320:
        switch(mask){
        case 0:
        case 1:
        case 2: display = " - Warning: Mask value this low detects noise\nfor this image size"; break;
        case 3: display = " - Smallest Noise-Free Mask"; break;
        case 4: display = " - Good Mask Size for this image size"; break;
        default: display = ""; break;
        }; break;
    case 640:
        switch(mask){
        case 0:
        case 1:
        case 2:
        case 3: display = " - Warning: Mask value this low detects noise\nfor this image size"; break;
        case 4: display = " - Smallest Noise-Free Mask"; break;
        case 5: display = " - Good Mask Size for this image size"; break;
        default: display = ""; break;
        }; break;
    case 800:
        switch(mask){
        case 0:
        case 1:
        case 2:
        case 3:
        case 4: display = " - Warning: Mask value this low detects noise\nfor this image size"; break;
        case 5: display = " - Smallest Noise-Free Mask"; break;
        case 6: display = " - Good Mask Size for this image size"; break;
        default: display = ""; break;
        }; break;
    }
    ui->label_mask_hint->setText(QString::number(mask)+display);
    ui->label_mask_hint->move(coords);
}

void MainWindow::on_pushButton_settings_clicked()
{
    settings_pressed = true;
    set = new Settings(this);
    set->exec();
    writeSettings();
    //    this->show();
}

void MainWindow::on_pushButton_stop_clicked()
{
    op->stop();
    show_widgets(true);

}


void MainWindow::show_widgets(bool show)
{
    if(!show)
    {
        qDebug() << "finished signal";
        //Disable widgets
        ui->mask_slide->setDisabled(true); ui->maskEdit->setDisabled(true);
        ui->dial->setDisabled(true); ui->frameAv->setDisabled(true);
        //Hide button
        ui->pushButton->hide(); ui->pushButton_stop->show(); ui->pushButton_settings->setDisabled(true);

    }
    else
    {
        //Enable widgets
        ui->mask_slide->setEnabled(true); ui->maskEdit->setEnabled(true);
        ui->dial->setEnabled(true); ui->frameAv->setEnabled(true);
        ui->pushButton->hide();
        //Show button
        ui->pushButton->show(); ui->pushButton_stop->hide(); ui->pushButton_settings->setEnabled(true);
    }
}

//Read and write last saved values
void MainWindow::writeSettings(bool new_ones){
    if(!new_ones)
    {
        QSettings settings("Motion","Detect");
        settings.beginGroup("main");
        settings.setValue("width", set->width);
        settings.setValue("height", set->height);

        settings.setValue("mask_slider", ui->mask_slide->value());
        settings.setValue("interval", set->interval);
        settings.setValue("whitepix_value", set->whitepixel);

        settings.setValue("email_bool", set->email);
        settings.setValue("attach", set->email_attach);
        settings.setValue("email_address", set->email_address);
        settings.setValue("email_subject", set->email_subject);
        settings.setValue("email_message", set->email_address);

        settings.setValue("image_dir", set->image_dir);
        settings.setValue("convert_video", set->convert_video);
        settings.setValue("delete_images", set->delete_images);

        settings.endGroup();
    }
    else if(new_ones)
    {
        qDebug() << "writing new settings";

        QSettings settings("Motion","Detect");
        settings.beginGroup("main");
        settings.setValue("width", 320);
        settings.setValue("height", 240);

        settings.setValue("mask_slider", 5);
        settings.setValue("interval", 2000);
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

        readLastWorkingSettings();

    }
}


void MainWindow::readLastWorkingSettings(bool exists)
{
    qDebug() << "exists: " << exists;
    if (exists)
    {
        qDebug() << "setting values for Op";

        QSettings settings("Motion","Detect");
        settings.beginGroup("main");

        //ui->frameAv->display();
        int slide = settings.value("mask_slider").toInt();
        ui->mask_slide->setValue(slide);
        on_mask_slide_sliderMoved(slide);

        limitVal = settings.value("whitepix_value").toInt();
        interval_default = settings.value("interval").toInt();

        emailAlert = settings.value("email").toBool();
        email_message = settings.value("email_message").toString();
        email_address = settings.value("email_address").toString();
        email_subject = settings.value("email_subject").toString();
        email_attach = settings.value("attach").toBool();

        convert_images = settings.value("convert_images").toBool();
        delete_images = settings.value("delete_images").toBool();

        width = settings.value("width").toInt();
        height = settings.value("height").toInt();

        image_dir = settings.value("image_dir").toString();

        settings.endGroup();

        qDebug() << "finished setting values for op";


    }
    else if(!exists)
    {
        //If settings dont exist, then write new ones
        writeSettings(true);
    }

}
