#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "operations.h"

#include <QtCore/QCoreApplication>
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
    //Share ImageLabel -- so that Operations can access it.
    img = ui->imageLab;
    //Automatic -- remove later
    /*ui->maskEdit->setText("10");
    on_pushButton_clicked();*/
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
    op = new Operations(set->width, set->height);

    op->frameNum = ui->frameAv->value();
    op->erodeVar = ui->maskEdit->text().toInt();
    op->limitVal = set->whitepixel;
    op->interval_default = set->interval;

    op->emailAlert = set->email;
    op->email_message = set->email_message;
    op->email_address = set->email_address;
    op->email_subject = set->email_subject;
    op->email_attach = set->email_attach;

    op->convert_images = set->convert_video;
    op->delete_images = set->delete_images;

    op->width = set->width;
    op->height = set->height;

    op->image_dir = set->image_dir;

    qDebug() << " frame:" << op->frameNum << " mask:" << op->erodeVar << " thresh:" << op->limitVal << " interval:" << op->interval_default
             << " email:" << op->emailAlert << op->email_message << op->email_address << op->email_subject << op->email_attach
             << " convert:" << op->convert_images << " delete:" << op->delete_images << " width,height" << op->width << op->height
             << "image dir:" << op->image_dir;

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
}

void MainWindow::on_pushButton_settings_clicked()
{
    set = new Settings(this);
    set->exec();
//    this->show();
}
