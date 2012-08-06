#include "noticebox.h"
#include "ui_noticebox.h"

noticebox::noticebox(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::noticebox)
{
    ui->setupUi(this);
}

noticebox::~noticebox()
{
    delete ui;
}

void noticebox::on_pushButton_save_clicked()
{
    save = true;
    this->accept();
}

void noticebox::on_pushButton_discard_clicked()
{
    save = false;
    this->accept();
}
