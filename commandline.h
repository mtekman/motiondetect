#ifndef COMMANDLINE_H
#define COMMANDLINE_H

#include <QString>
#include <QStringList>
#include <iostream>
#include <QDateTime>

using namespace std;

class CommandLine
{
public:
    CommandLine(QStringList &arguments);
    ~CommandLine();

    QStringList args;

    int min,max, mask;

    float mod;
    int white;

    int width,height;
    bool convert, del;

    QString dir;

    bool email,attach;
    QString address, message, subject;

    char *c_file;

    QDateTime *time;


private:
  void checkVersionOrHelp();
  void checkMask();
  void checkRange();
  void checkModifier();
  void checkWhitepix();
  void checkSize();
  void checkConvertDelete();
  void checkImageDir();
  void checkEmail();
  void checkToFile();
  void checkTime();
  void unknowns();
};

#endif // COMMANDLINE_H
