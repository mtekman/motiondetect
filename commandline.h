#ifndef COMMANDLINE_H
#define COMMANDLINE_H

#include <QString>
#include <QStringList>
#include <iostream>

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

    bool show;
    bool convert, del;

    QString dir;

    bool email,attach;
    QString address, message, subject;

private:
  void checkVersionOrHelp();
  void checkMask();
  void checkRange();
  void checkModifier();
  void checkWhitepix();
  void checkSize();
  void isVisible();
  void checkConvertDelete();
  void checkImageDir();
  void checkEmail();
  void unknowns();

};

#endif // COMMANDLINE_H
