#include "settings.h"
#include "ui_settings.h"
#include <QFileDialog>
#include <QSettings>
#include <QTabWidget>
#include <iostream>

Settings::Settings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Settings)
{
#ifdef Q_WS_MAEMO_5
    this->setAttribute(Qt::WA_Maemo5StackedWindow);
    this->setWindowFlags(Qt::Window);
#endif
    ui->setupUi(this);
    readSettings();
    saved = false;
}

Settings::~Settings()
{
    std::cout << "Settings Deleted" << std::endl;
    delete ui;
}

//override
void Settings::closeEvent(QCloseEvent *event){
    event->ignore();
    writeSettings();
    event->accept();
}

void Settings::on_dial_whitepixel_sliderMoved(int position)
{
    ui->lineEdit_whitepixel->setText(QString::number(position*5));
}

void Settings::on_pushButton_reset_clicked()
{
    //General
    ui->dial_whitepixel->setValue(40);
    on_dial_whitepixel_sliderMoved(40);
    //ui->lineEdit_whitepixel->setText("60");

    ui->dial_max->setValue(65);
    on_dial_max_sliderMoved(65); // 10 seconds
    //ui->lineEdit_max->setText("2");

    ui->dial_min->setValue(10);
    on_dial_min_sliderMoved(10);
    //ui->lineEdit_min->setText("1");

    ui->horizontalSlider_modifier->setValue(16);
    on_horizontalSlider_modifier_sliderMoved(16);
    //ui->lineEdit_modifier->setText("0.16");

    ui->timeEdit->setTime(QTime(0,1)); //60 seconds

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


    if(!dir.endsWith('/')) dir.append('/');

    ui->lineEdit_image_dir->setText(dir);
}

void Settings::on_dial_size_sliderMoved(int position)
{
    // Valid UYVY Sizes:  (320, 240), (640, 480), (800, 600), (1280, 960), (2560, 1920);
    int size_index = position/25;
    int height = 240;

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

    settings.setValue("time", ui->timeEdit->time());

    settings.setValue("whitepixel_slider", ui->dial_whitepixel->value());
    settings.setValue("whitepix_value", ui->lineEdit_whitepixel->text().toUInt());

    settings.setValue("size_slider", ui->dial_size->value());
    settings.setValue("size_label", ui->label_size->text());

    settings.setValue("email_bool",ui->checkBox_email->isChecked());
    settings.setValue("attach",ui->checkBox_attach->isChecked());
    settings.setValue("email_address", ui->lineEdit_Address->text());
    settings.setValue("email_subject", ui->lineEdit_Subject->text());
    settings.setValue("email_message", ui->plainTextEdit_message->toPlainText());

    settings.setValue("image_dir", ui->lineEdit_image_dir->text());
    settings.setValue("convert_video", ui->checkBox_convertImages->isChecked());
    settings.setValue("delete_images", ui->checkBox_deleteImages_after_convert->isChecked());

    size = ui->label_size->text();

    QStringList sizeL = ui->label_size->text().split("x");
    settings.setValue("width", sizeL.first().toInt());
    settings.setValue("height", sizeL.last().toInt());

    settings.setValue("tab_number", ui->tabWidget->currentIndex());

    settings.setValue("interval_max", ui->lineEdit_max->text().toInt());
    settings.setValue("interval_min", ui->lineEdit_min->text().toInt());

    settings.setValue("dial_max", ui->dial_max->value());
    settings.setValue("dial_min", ui->dial_min->value());

    settings.setValue("slide_modifier", ui->horizontalSlider_modifier->value());
    settings.setValue("interval_modifier", ui->lineEdit_modifier->text().toFloat());

    settings.endGroup();


    std::cout << "Settings.cpp: settings saved" << std::endl;
}


void Settings::readSettings(){
    QSettings settings("fcam");
    settings.beginGroup("main");

    QTime tim = settings.value("time").toTime();
    ui->timeEdit->setTime(tim);

    int mx = settings.value("dial_max").toInt();
    ui->dial_max->setValue(mx);
    on_dial_max_sliderMoved(mx);

    int mn = settings.value("dial_min").toInt();
    ui->dial_min->setValue(mn);
    on_dial_min_sliderMoved(mn);

    int mod = settings.value("slide_modifier").toInt();
    ui->horizontalSlider_modifier->setValue(mod);
    on_horizontalSlider_modifier_sliderMoved(mod);


    int whiteslide = settings.value("whitepixel_slider").toInt();
    ui->dial_whitepixel->setValue(whiteslide);
    ui->lineEdit_whitepixel->setText(settings.value("whitepix_value").toString());

    int sizeslide = settings.value("size_slider").toInt();
    ui->dial_size->setValue(sizeslide);
    ui->label_size->setText(settings.value("size_label").toString());

    bool em = settings.value("email_bool").toBool();
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

//Sliders update each other
int max=2, min=1;

void Settings::on_dial_max_sliderMoved(int position)
{
    min = ui->lineEdit_min->text().toInt();
    max = int((float)(  ( (-position*position)/(position-134))*0.2+1));

    ui->lineEdit_max->setText(QString::number(max));

    if(max <= min && (max-1)>=1) {
        ui->dial_min->setValue(position-1);
        on_dial_min_sliderMoved(position-1);
        //ui->lineEdit_min->setText(QString::number(calculateDial(max-1,false)) );
    }
}

void Settings::on_dial_min_sliderMoved(int position)
{
    max = ui->lineEdit_max->text().toInt();
    min = int((float)(  ( (-position*position)/(position-134))*0.2+1));

    ui->lineEdit_min->setText(QString::number(min));

    if(min > max) {
        ui->dial_max->setValue(position+1);//ui->dial_min->value()+1);
        on_dial_max_sliderMoved(position+1);
        //ui->lineEdit_max->setText(QString::number(calculateDial(min+1,false)));
    }
}

void Settings::on_horizontalSlider_modifier_sliderMoved(int position)
{
    // Max becomes default if the fraction to divide or multiply = 1
    float mod = ((float)(position) )/ ( (float)(100));

    if(position == 0) {
        mod = 0.01;
        ui->lineEdit_modifier->setText("0.01");
    }
    else if(position >= 99) {
         ui->dial_min->setDisabled(true);
         ui->lineEdit_min->setDisabled(true);
    }
    else{
        ui->dial_min->setEnabled(true);
        ui->lineEdit_min->setEnabled(true);
    }

    ui->lineEdit_modifier->setText(QString::number(mod));

}

void Settings::on_timeEdit_editingFinished()
{
    QTime temp = ui->timeEdit->time();
    if(temp.hour() == 0 && temp.minute()==0 && temp.second()<10)
    {
        temp.setHMS(0,0,10,0); //minimum time of 10 seconds
        ui->timeEdit->setTime(temp);
    }
}
