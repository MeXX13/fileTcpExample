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
    socket = new QTcpSocket;
    socket->setSocketDescriptor(handle);
    connect(socket, &QTcpSocket::readyRead, this, &server::slotReadyRead);
    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
    socketThread *thread = new socketThread(handle);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);

    Sockets.push_back(socket);
    thread->start();
    qDebug() << "client connected" << handle;
}

void server::slotReadyRead()
{
    socket = (QTcpSocket*)sender();
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_6_2);
    if(in.status() == QDataStream::Ok)
    {
        qDebug() << "read...";
        QString str;
        in >> str;
        qDebug() << str;
        SendToClient(str);
    }
    else
    {
        qDebug() << "DataStream error";
    }
}

void server::SendToClient(QString str)
{
    Data.clear();
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_2);
    out << str;

    for(int i = 0; i < Sockets.length(); i++)
    {
        Sockets[i]->write(Data);
    }
}
