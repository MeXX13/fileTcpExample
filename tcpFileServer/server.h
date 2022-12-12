#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QObject>
#include <QTime>

#include "socketthread.h"

class server : public QTcpServer
{
    Q_OBJECT
public:
    explicit server(QHostAddress host = QHostAddress::Any,
                    quint16 port      = 5555,
                    QObject *parent   = nullptr);

private:
    QObject *interfaceWindow;
    QHostAddress host;
    quint16      port;

    QVector<socketThread *> mSocketThreads;

//    QByteArray Data;
    quint16 nextBlockSize;

    void SendToClient(QString str);

public slots:
    void incomingConnection(qintptr handle) Q_DECL_OVERRIDE;
    void start();

    void getMessagesSlot(QString msg, socketThread *fromSocketThread);

signals:
    void sendMessagesToClient(QString msg);
};

#endif // SERVER_H
