#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QObject>
#include <QVector>
#include "client.h"

class server : public QTcpServer
{
    friend class socketThread;
    Q_OBJECT
public:
    explicit server(QHostAddress host = QHostAddress::Any,
                    quint16 port      = 5555,
                    QObject *parent   = nullptr);
    QTcpSocket *socket;

public slots:
    void start();
    void incomingConnection(qintptr handle) Q_DECL_OVERRIDE;
    void slotReadyRead();

private:
    QVector <QTcpSocket*> Sockets;
    QByteArray Data;
    void SendToClient(QString str);
    QObject *interfaceWindow;
    QHostAddress host;
    quint16      port;
};

#endif // SERVER_H
