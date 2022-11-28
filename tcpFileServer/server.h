#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QObject>
#include "client.h"

class server : public QTcpServer
{
    friend class socketThread;
    Q_OBJECT
public:
    explicit server(QHostAddress host = QHostAddress::Any,
                    quint16 port      = 5555,
                    QObject *parent   = nullptr);
    //~server();
    static QList<Client*> clients; //список пользователей

public slots:
    void start();

protected:
    void incomingConnection(qintptr handle) Q_DECL_OVERRIDE;

private:
    QObject *interfaceWindow;
    QHostAddress host;
    quint16      port;
};

#endif // SERVER_H
