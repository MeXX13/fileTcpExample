#ifndef SOCKETTHREAD_H
#define SOCKETTHREAD_H

#include <QThread>
#include <QObject>
#include <QTcpSocket>
#include <QTime>

class socketThread : public QThread
{
    Q_OBJECT
public:
    explicit socketThread(qintptr descriptor, QObject *parent = 0);
    ~socketThread();

protected:
    void run() Q_DECL_OVERRIDE;

signals:
    void onFinishRecieved();

private slots:
    void onReadyRead();
    void onDisconnected();

public:
    QTcpSocket *m_socket;

private:
    qintptr     m_socketDescriptor;
    qint32      m_blockSize = 0;

public slots:
    void receiveMessagesFromServerSlot(QString msg, QTcpSocket *socket);

signals:
    void sendMessagesToServerSignal(QString msg, socketThread *fromSocketThread);

};


#endif // SOCKETTHREAD_H
