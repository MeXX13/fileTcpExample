#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QObject>
//#include "savedclients.h"

class server : public QTcpServer
{
    Q_OBJECT
public:
    explicit server(QHostAddress host = QHostAddress::Any,
                    quint16 port      = 5555,
                    QObject *parent   = nullptr);
    //~server();

public slots:
    void start();
//    void sendMessages(QString msg, QTcpSocket *fromSocket);

protected:
    void incomingConnection(qintptr handle) Q_DECL_OVERRIDE;

private:
    QObject *interfaceWindow;
    QHostAddress host;
    quint16      port;

//signals:
//    void sendMessagesSignal(QString msg, QTcpSocket *fromSocket);
};

#endif // SERVER_H
