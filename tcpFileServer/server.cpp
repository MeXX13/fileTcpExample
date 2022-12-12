#include "server.h"
#include "socketthread.h"

server::server(QHostAddress host, quint16 port, QObject *parent)   : QTcpServer(parent),
    interfaceWindow(parent), host(host),   port(port)
{
    // Do nothing
}

void server::start()
{
    // запуск сервера
    if ( this->listen(host, port) )
        qDebug() << "Server started at " << host.toString() << ":" << port;
    else
        qDebug() << "Can't start server";
}

void server::getMessagesSlot(QString msg, socketThread *fromSocketThread)
{
    emit sendMessagesToClient(msg, fromSocketThread->m_socket);
//    foreach(socketThread *socketThread, mSocketThreads)
//    {
//        if (socketThread != fromSocketThread)
//            emit sendMessagesToClient(msg);
//    }
}

void server::incomingConnection(qintptr handle)
{
    qDebug() << "incomingConnection = " << handle;
    socketThread *thread = new socketThread(handle);

    connect(thread, &QThread::finished, thread, &QThread::deleteLater);

    connect(thread, &socketThread::sendMessagesToServerSignal, this, &server::getMessagesSlot, Qt::AutoConnection);
    connect(this, &server::sendMessagesToClient, thread, &socketThread::receiveMessagesFromServerSlot, Qt::AutoConnection);

    mSocketThreads.push_back(thread);
    thread->start();

}
