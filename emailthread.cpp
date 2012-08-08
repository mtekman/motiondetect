#include "emailthread.h"
#include <QStringList>
#include <iostream>

EmailThread::EmailThread(const QString &address, const QString &subject, const QString &message, bool attach, const QString &attach_name)
{
    QString email = "echo '"+message+"' | mailcmd "+
            ((attach)?("-a "+attach_name+" "):(" "))
            +"-s '"+subject+"' "+address;

    std::cout << "Emailed: " << email.toUtf8().data() << std::endl;
    QStringList args;
    args << "-c" << email;
    qp = new QProcess;
    qp->start("/bin/sh", args);
    qp->waitForFinished();
}

EmailThread::~EmailThread()
{
    qp->terminate();
    std::cout << "email sent.";
}

//TODO: Timestamp for image
