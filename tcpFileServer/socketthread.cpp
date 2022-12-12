#include "socketthread.h"

#include <QDataStream>
#include <QFile>

socketThread::socketThread(qintptr descriptor, QObject *parent)
    : QThread(parent),
      m_socketDescriptor(descriptor),
      m_blockSize(0)
{
    // nothing
}


socketThread::~socketThread()
{
    delete m_socket;
}

void socketThread::run()
{
    m_socket = new QTcpSocket();
    m_socket->setSocketDescriptor(m_socketDescriptor);

//    connect(m_socket, &QTcpSocket::readyRead,    this, &socketThread::onReadyRead,    Qt::DirectConnection);
//    connect(m_socket, &QTcpSocket::disconnected, this, &socketThread::onDisconnected, Qt::DirectConnection);

    connect(m_socket, &QTcpSocket::readyRead,    this, &socketThread::onReadyRead,    Qt::AutoConnection);
    connect(m_socket, &QTcpSocket::disconnected, this, &socketThread::onDisconnected, Qt::AutoConnection);

    qDebug() << "client connected" << m_socketDescriptor;

    exec();
}

void socketThread::onReadyRead()
{    
    QByteArray data = m_socket->readAll();
    qDebug() << m_socketDescriptor << " Data in: " << data;

    QString dataAsString = QString::fromStdString(data.toStdString());
    if (dataAsString.left(5) == "[msg]")
    {
        // отправка сообщения
        QString message = dataAsString.mid(5, -1);
        qDebug() << "Message: " << message;

        emit sendMessagesToServerSignal(message, this);
        qDebug() << "Success!";
    }
    else
    {
        // логика отправки файла
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

        //emit onFinishRecieved();
        m_socket->disconnectFromHost();
    }
}

void socketThread::onDisconnected()
{
    m_socket->close();
    quit();
}

void socketThread::receiveMessagesFromServerSlot(QString msg, QTcpSocket *socket)
{
    if (socket == m_socket) // на самого себя не пересылаем
        return;

    if (m_socket->state() == QAbstractSocket::ConnectedState)
    {
        m_socket->write(msg.toUtf8());
        m_socket->waitForBytesWritten(3000);
    }
}
