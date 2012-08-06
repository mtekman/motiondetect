#ifndef noticebox_H
#define noticebox_H

#include <QDialog>

namespace Ui {
class noticebox;
}

class noticebox : public QDialog
{
    Q_OBJECT

public:
    bool save;
    explicit noticebox(QWidget *parent = 0);
    ~noticebox();

private slots:
    void on_pushButton_save_clicked();

    void on_pushButton_discard_clicked();

private:
    Ui::noticebox *ui;
};

#endif // noticebox_H
