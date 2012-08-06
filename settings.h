#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>

namespace Ui {
class Settings;
}

class Settings : public QDialog
{
    Q_OBJECT

public:
    unsigned short width, height;

    bool email;
    bool email_attach;
    QString email_address, email_subject, email_message;

    QString image_dir;

    bool convert_video;
    bool delete_images;

    unsigned int interval, whitepixel;

    explicit Settings(QWidget *parent = 0);
    ~Settings();

protected:
    void closeEvent(QCloseEvent * event);


private slots:
    void on_dial_interval_sliderMoved(int position);

    void on_dial_whitepixel_sliderMoved(int position);

    void on_pushButton_reset_clicked();

    void on_checkBox_email_clicked(bool checked);

    void on_pushButton_browse_clicked();


    void on_pushButton_save_clicked();

    void on_dial_size_sliderMoved(int position);

private:
    bool saved;
    Ui::Settings *ui;
    void readSettings();
    void writeSettings();
};

#endif // SETTINGS_H
