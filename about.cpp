#include "about.h"
#include "ui_about.h"
#include <iostream>
#include <QDesktopServices>
#include <QUrl>
#include <QMovie>

QMovie *dog_logo=0;

About::About(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::About)

{
#ifdef Q_WS_MAEMO_5
    this->setAttribute(Qt::WA_DeleteOnClose);
#endif
    ui->setupUi(this);
    //
    dog_logo = new QMovie(":/img/images/dog.gif");
    dog_logo->setScaledSize(QSize(200,100));
    ui->label_movie->setMovie(dog_logo);
    dog_logo->start();
}

About::~About()
{
    std::cout << "About Deleted" << std::endl;
    delete ui;
}

void About::on_pushButton_donate_clicked()
{
    QDesktopServices::openUrl(QUrl("http://www.google.com"));
    this->close();
}
