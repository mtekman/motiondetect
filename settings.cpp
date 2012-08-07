#include "settings.h"
#include "ui_settings.h"
#include <QFileDialog>
#include <QDebug>
#include <QSettings>
#include <QTabWidget>

Settings::Settings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Settings)
{
#ifdef Q_WS_MAEMO_5
    this->setAttribute(Qt::WA_Maemo5StackedWindow);
    this->setWindowFlags(Qt::Window);
//    this->setAttribute(Qt::WA_DeleteOnClose);
#endif
    ui->setupUi(this);
    readSettings();
//    on_pushButton_reset_clicked();
    saved = false;
}

Settings::~Settings()
{
    delete ui;
}

//override
void Settings::closeEvent(QCloseEvent *event){
    event->ignore();
    writeSettings();
    event->accept();
}

void Settings::on_dial_interval_sliderMoved(int position)
{
    ui->lineEdit_interval->setText(QString::number( 500+(position)*60) );
}

void Settings::on_dial_whitepixel_sliderMoved(int position)
{
    ui->lineEdit_whitepixel->setText(QString::number(position*2));
}

void Settings::on_pushButton_reset_clicked()
{
    //General
    ui->lineEdit_whitepixel->setText("60");
    ui->lineEdit_interval->setText("2000");

    //Email
    ui->checkBox_attach->setChecked(false);
    ui->lineEdit_Address->setText("blank@blanketyblank.blank");
    ui->lineEdit_Subject->setText("Motion Detected");
    ui->plainTextEdit_message->setPlainText("Howdy");

    ui->checkBox_email->setChecked(false);
    on_checkBox_email_clicked(false);

    //Images
    ui->checkBox_convertImages->setChecked(true);
    ui->checkBox_deleteImages_after_convert->setChecked(false);

    ui->lineEdit_image_dir->setText("/home/user/MyDocs/DCIM/MISC/");
    ui->dial_size->setSliderPosition(1); // 320x240
    on_dial_size_sliderMoved(1);
}

void Settings::on_checkBox_email_clicked(bool checked)
{
    if(!checked)
    {
        ui->label->hide();ui->label_2->hide();ui->label_3->hide();
        ui->lineEdit_Address->hide(); ui->lineEdit_Subject->hide(); ui->plainTextEdit_message->hide();
        ui->checkBox_attach->hide();
    }
    else if(checked){
        ui->label->show();ui->label_2->show();ui->label_3->show();
        ui->lineEdit_Address->show(); ui->lineEdit_Subject->show(); ui->plainTextEdit_message->show();
        ui->checkBox_attach->show();
    }
}

void Settings::on_pushButton_browse_clicked()
{
    QString openpath = ui->lineEdit_image_dir->text();
    if(openpath=="."){openpath="/home/user/MyDocs/DCIM/MISC";}
    QString path = QFileDialog::getExistingDirectory(this, tr("Folder to save images to"),openpath); // tr("Pictures (*.jpg *.png *.gif);;Video (*.avi *.mpg *.mp4 *.mpeg)"));
    QString dir = QFileInfo(path).absoluteFilePath();
    ui->lineEdit_image_dir->setText(dir);

}

void Settings::on_dial_size_sliderMoved(int position)
{
    // Valid UYVY Sizes:  (320, 240), (640, 480), (800, 600), (1280, 960), (2560, 1920);
    int size_index = position/25;
    int height;

    switch(size_index){
    case 0: width=320; height=240; break;
    case 1: width=640; height=480; break;
    case 2: width=800; height = 600; break;
    case 3: width=1280; height = 960; break;
    //case 4: width=2560; height = 1920; break; <--- Too slow. Dropped.
    }

    ui->label_size->setText(QString::number(width)+" x "+QString::number(height));
}


//Read and write last saved values
void Settings::writeSettings(){
    QSettings settings("fcam");
    settings.beginGroup("main");
    settings.setValue("interval_slider", ui->dial_interval->value());
    settings.setValue("interval_value", ui->lineEdit_interval->text().toUInt());

    settings.setValue("whitepixel_slider", ui->dial_whitepixel->value());
    settings.setValue("whitepix_value", ui->lineEdit_whitepixel->text().toUInt());

    settings.setValue("size_slider", ui->dial_size->value());
    settings.setValue("size_label", ui->label_size->text());

    settings.setValue("email_bool",ui->checkBox_email->isChecked());
    settings.setValue("attach",ui->checkBox_attach->isChecked());
    settings.setValue("email_address", ui->lineEdit_Address->text());
    settings.setValue("email_subject", ui->lineEdit_Subject->text());
    settings.setValue("email_message", ui->plainTextEdit_message->toPlainText());

    settings.setValue("image_dir", ui->lineEdit_Address->text());
    settings.setValue("convert_video", ui->checkBox_convertImages->isChecked());
    settings.setValue("delete_images", ui->checkBox_deleteImages_after_convert->isChecked());

    QStringList size = ui->label_size->text().split("x");
    settings.setValue("width", size.first().toInt());
    settings.setValue("height", size.last().toInt());

    settings.setValue("tab_number", ui->tabWidget->currentIndex());

    settings.endGroup();


    qDebug() << "Settings.cpp: settings saved";
}


void Settings::readSettings(){
    QSettings settings("fcam");
    settings.beginGroup("main");

    int intslide = settings.value("interval_slider").toInt();
    ui->dial_interval->setValue(intslide);
    qDebug() << "IJNTERVAL: " << settings.value("interval_value").toString();
    QString inter = settings.value("interval_value").toString();
    ui->lineEdit_interval->setText(inter);

    int whiteslide = settings.value("whitepixel_slider").toInt();
    ui->dial_whitepixel->setValue(whiteslide);
    ui->lineEdit_whitepixel->setText(settings.value("whitepix_value").toString());

    int sizeslide = settings.value("size_slider").toInt();
    ui->dial_size->setValue(sizeslide);
    ui->label_size->setText(settings.value("size_label").toString());

    bool em = settings.value("email_bool").toBool();
    qDebug() << "EMAIL: " << em;
    ui->checkBox_email->setChecked(em);
    on_checkBox_email_clicked(em);

    ui->checkBox_attach->setChecked(settings.value("attach").toBool());
    ui->lineEdit_Address->setText(settings.value("email_address").toString());
    ui->lineEdit_Subject->setText(settings.value("email_subject").toString());
    ui->plainTextEdit_message->setPlainText(settings.value("email_message").toString());

    ui->lineEdit_image_dir->setText(settings.value("image_dir").toString());
    ui->checkBox_convertImages->setChecked(settings.value("convert_video").toBool());
    ui->checkBox_deleteImages_after_convert->setChecked(settings.value("delete_images").toBool());

    ui->tabWidget->setCurrentIndex(settings.value("tab_number").toInt() );

    settings.endGroup();

}
