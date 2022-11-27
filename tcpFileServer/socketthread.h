#ifndef SOCKETTHREAD_H
#define SOCKETTHREAD_H

#include <QThread>
#include <QObject>
#include <QTcpSocket>

class socketThread : public QThread
{
    Q_OBJECT
public:
    socketThread(qintptr descriptor, QObject *parent = 0);
    ~socketThread();

protected:
    void run() Q_DECL_OVERRIDE;

signals:
    void onFinishRecieved();

private slots:
    void onReadyRead();
    void onDisconnected();

private:
    qintptr     m_socketDescriptor;
    QTcpSocket *m_socket;
    qint32      m_blockSize = 0;
};


#endif // SOCKETTHREAD_H
