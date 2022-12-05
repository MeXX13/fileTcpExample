#include "socketthread.h"
#include <QDataStream>
#include <QFile>

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

    exec();
}


void socketThread::onReadyRead()
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

    QString filePath = "O:\\";
    QFile target(filePath + "\\" + fileName);


    if (!target.open(QIODevice::WriteOnly)) {
        qDebug() << "Can't open file for written";
        return;
    }
    target.write(line);

    target.close();

    //emit onFinishRecieved();
    m_socket->disconnectFromHost();
}


void socketThread::onDisconnected()
{
    m_socket->close();
    quit();
}
