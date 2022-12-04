#include "socketthread.h"

#include <QDataStream>
#include <QFile>
#include "savedclients.h"

socketThread::socketThread(qintptr descriptor, QObject *parent)
    : QThread(parent),
      m_socketDescriptor(descriptor),
      m_blockSize(0)
{
}


socketThread::~socketThread()
{
    delete m_socket;
}


void socketThread::run()
{
    m_socket = new QTcpSocket;
    m_socket->setSocketDescriptor(m_socketDescriptor);

    connect(m_socket, &QTcpSocket::readyRead,    this, &socketThread::onReadyRead,    Qt::DirectConnection);
    connect(m_socket, &QTcpSocket::disconnected, this, &socketThread::onDisconnected, Qt::DirectConnection);

//    SavedClients::getInstance().mClients.push_back(m_socket);

    foreach(socketThread *st, SavedClients::getInstance().mClients)
    {
        if (this->m_socket == st->m_socket)
            continue;

        connect(this, &socketThread::sendMessagesSignal, st, &socketThread::sendMessagesSlot);
        connect(st, &socketThread::sendMessagesSignal, this, &socketThread::sendMessagesSlot);
    }

    SavedClients::getInstance().mClients.push_back(this);   // добваляем клиента в вектор
    exec();
}

void socketThread::sendMessagesSlot(QString msg, socketThread *st)
{
    if (this == st)
        return;

    if (this->m_socket)
         this->m_socket->write(msg.toUtf8());
}


void socketThread::onReadyRead()
{    
    QByteArray data = m_socket->readAll();
    qDebug() << m_socketDescriptor << " Data in: " << data;

    QString dataAsString = QString::fromStdString(data.toStdString());

    if (dataAsString.left(5) == "[msg]")
    {
        QString message = dataAsString.mid(5, -1);
        qDebug() << "Message: " << message;

        emit sendMessagesSignal(message, this);

        qDebug() << "Success!";
//        sendMessages(message, m_socket);
//        fromSocket->write(("[from server] " + msg).toUtf8());
    }
    else
    {
        QDataStream in(m_socket);
        in.setVersion(QDataStream::Qt_5_1);

        if (m_blockSize == 0)
        {
            if (m_socket->bytesAvailable() < (qint64)sizeof(quint32))
                return;
            in >> m_blockSize;
        }
        if (m_socket->bytesAvailable() < m_blockSize)
            return;

        QString fileName;
        in >> fileName;

        QByteArray line = m_socket->readAll();

        QString filePath = "C:\\Users\\Joel\\Desktop";
        QFile target(filePath + "\\" + fileName);


        if (!target.open(QIODevice::WriteOnly)) {
            qDebug() << "Can't open file for written";
            return;
        }

        target.write(line);
        target.close();
    }

    //emit onFinishRecieved();
    m_socket->disconnectFromHost();

    SavedClients::getInstance().mClients.removeOne(this);
//    mClients.removeOne(m_socket);
}


void socketThread::onDisconnected()
{
    m_socket->close();
    quit();
}

//void socketThread::sendMessages(QString msg, QTcpSocket *fromSocket)
//{
//    foreach(QTcpSocket *socket, SavedClients::getInstance().mClients)
//    {
//        if (socket == fromSocket)
//            continue;

//        emit sendMessagesSignal(msg);
//    }
//}
