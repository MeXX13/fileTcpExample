#include "server.h"
#include "socketthread.h"

server::server(QHostAddress host, quint16 port, QObject *parent)   : QTcpServer(parent),
    interfaceWindow(parent), host(host),   port(port)
{

}

void server::start()
{
    if ( this->listen(host, port) )
        qDebug() << "Server started at " << host.toString() << ":" << port;
    else
        qDebug() << "Can't start server";
}

void server::incomingConnection(qintptr handle)
{
    Client *client = new Client(handle);
    qDebug() << "incomingConnection = " << handle;
    socketThread *thread = new socketThread(handle);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    clients.append(client);
    thread->start();
}
