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
    ui->label_mask_hint->hide();

    //Share ImageLabel -- so that Operations can access it.
    img = ui->imageLab;
    ui->label_mask_hint->show();

    //initialise camerathread here. PRevent pointer problems with settings.
    op = new Operations;

    readLastWorkingSettings();
    width = op->width;
}

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

    op->frameNum = ui->frameAv->value();
    op->erodeVar = ui->maskEdit->text().toInt();

    qDebug() << " frame:" << op->frameNum << " mask:" << op->erodeVar << " thresh:" << op->limitVal << " interval:" << op->interval_default
             << " email:" << op->emailAlert << op->email_message << op->email_address << op->email_subject << op->email_attach
             << " convert:" << op->convert_images << " delete:" << op->delete_images << " width,height" << op->width << op->height
             << "image dir:" << op->image_dir;

    show_widgets(false);

    op->start();

}


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

    //Statements
    QString warn = " - Warning: Mask value this low detects noise\nfor image size "+size;
    QString small =" - Smallest Noise-Free Mask";
    QString good = " - Good Mask Size for "+size;


    switch(width){
    case 320:
        switch(mask){
        case 0:
        case 1:
        case 2: display = warn; break;
        case 3: display = small; break;
        case 4: display = good; break;
        default: display = ""; break;
        }; break;
    case 640:
        switch(mask){
        case 0:
        case 1:
        case 2:
        case 3: display = warn; break;
        case 4: display = small; break;
        case 5: display = good; break;
        default: display = ""; break;
        }; break;
    case 800:
        switch(mask){
        case 0:
        case 1:
        case 2:
        case 3:
        case 4: display = warn; break;
        case 5: display = small; break;
        case 6: display = good; break;
        default: display = ""; break;
        }; break;
    case 1280:
        switch(mask){
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5: display = warn; break;
        case 6: display = small; break;
        case 7: display = good; break;
        default: display = ""; break;
        }; break;
    }
    ui->label_mask_hint->setText(QString::number(mask)+display);
    ui->label_mask_hint->move(coords);
}

void MainWindow::on_pushButton_settings_clicked()
{
    set = new Settings(this);
    set->exec();

    readLastWorkingSettings();
    size = set->width;

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
        ui->label_mask_hint->hide();

    }
    else
    {
        //Enable widgets
        ui->mask_slide->setEnabled(true); ui->maskEdit->setEnabled(true);
        ui->dial->setEnabled(true); ui->frameAv->setEnabled(true);
        ui->pushButton->hide();
        //Show button
        ui->pushButton->show(); ui->pushButton_stop->hide(); ui->pushButton_settings->setEnabled(true);
        ui->label_mask_hint->show();
    }
}

//Read and write last saved values
void MainWindow::writeSettings(bool new_ones){
    if(!new_ones)
    {
        QSettings settings("fcam");
        settings.beginGroup("main");

        settings.setValue("mask_slider", ui->mask_slide->value());

        settings.endGroup();
    }
    else if(new_ones)
    {
        qDebug() << "writing new settings";

        QSettings settings("fcam");
        settings.beginGroup("main");
        settings.setValue("width", 320);
        settings.setValue("height", 240);

        settings.setValue("mask_slider", 5);
        settings.setValue("interval_value", 2000);
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


void MainWindow::readLastWorkingSettings()
{
    QSettings settings("fcam");
    settings.beginGroup("main");
    bool exists = !settings.value("width").isNull();

    if (exists)
    {
        QSettings settings("fcam");
        settings.beginGroup("main");

        //ui->frameAv->display();
        int slide = settings.value("mask_slider").toInt();
        size = settings.value("size_label").toString();

        ui->mask_slide->setValue(slide);
        on_mask_slide_sliderMoved(slide);

        op->limitVal = settings.value("whitepix_value").toInt();
        op->interval_default = settings.value("interval_value").toInt();

        op->emailAlert = settings.value("email").toBool();
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
