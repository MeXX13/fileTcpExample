#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>

class Client
{
friend class server;
friend class socketThread;
public:
    Client();
    Client(qintptr handle)
    {
        _sok->bind(handle);
    }

private:
    QTcpSocket *_sok;
};

#endif // CLIENT_H
