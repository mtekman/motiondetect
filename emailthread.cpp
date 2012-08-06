#include "emailthread.h"
#include <QStringList>
#include <QProcess>
#include <QDebug>

EmailThread::EmailThread(const QString &address, const QString &subject, const QString &message, bool attach, const QString &attach_name)
{
    QString email = "echo '"+message+"' | mailcmd "+
            ((attach)?("-a "+attach_name+" "):(" "))
            +"-s "+subject+" "+address;

    QStringList args;
    args << "-c" << email;
    qp = new QProcess;
    qp->start("/bin/sh", args);
    qp->waitForFinished();
}

EmailThread::~EmailThread()
{
    qp->terminate();
    qDebug() << "email sent.";
}
