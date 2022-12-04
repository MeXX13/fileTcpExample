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

//    connect(this, SIGNAL(sendMessagesSignal), this, SLOT(sendMessages));
}

//void server::sendMessages(QString msg, QTcpSocket *fromSocket)
//{
//    foreach(QTcpSocket *socket, SavedClients::getInstance().mClients)
//    {
//        if (socket == fromSocket)
//            continue;
//        socket->write(msg.toUtf8());
//    }
//}

void server::incomingConnection(qintptr handle)
{
    qDebug() << "incomingConnection = " << handle;
    socketThread *thread = new socketThread(handle);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);

    thread->start();
}
