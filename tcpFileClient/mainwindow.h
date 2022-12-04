#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QList>
#include <QString>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    void on_sendPushButton_clicked();

    void on_msgPushButton_clicked();

public slots:
    void bytesWritten(qint64 bytes);
    void readyRead();

private:
    Ui::MainWindow *ui;
    QTcpSocket *socket;

    struct Message
    {
        QString message;
        bool isMyMsg;
    };

    QList<Message> *message_history;

    void sendMessage();
    void readData();

    void updateChatField();
};
#endif // MAINWINDOW_H
