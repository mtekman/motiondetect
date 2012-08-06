#ifndef EMAILTHREAD_H
#define EMAILTHREAD_H

#include <QThread>
#include <QProcess>

class EmailThread : public QThread
{
public:
    EmailThread(const QString &address, const QString &subject, const QString &message, bool attach, const QString &attach_name);
    ~EmailThread();

private:
    QProcess *qp;

};

#endif // EMAILTHREAD_H
