#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QMessageBox>
#include <QHostAddress>
#include <QFileDialog>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QString ipRange = "(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])";
    QRegularExpression ipRegex ("^" + ipRange
                     + "\\." + ipRange
                     + "\\." + ipRange
                     + "\\." + ipRange + "$");
    QRegularExpressionValidator *ipValidator = new QRegularExpressionValidator(ipRegex, this);
    ui->ipLineEdit->setValidator(ipValidator);

    message_history = new QList<Message>();
}

MainWindow::~MainWindow()
{
    delete message_history;
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    QString port = ui->portLineEdit->text();
    bool portFlag = false;
    quint32 portNumber = port.toLong(&portFlag);
    if ((portNumber<1024)||(portNumber>65535))
    {
        portFlag = false;
    }
    if (!portFlag)
    {
        QMessageBox::critical(this, "Parameters error", "Wrong connection parameters");
        return;
    }

    QString ipStringAddr = ui->ipLineEdit->text();
    QHostAddress ipAddr(ipStringAddr);

    socket = new QTcpSocket(this);

    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);

    connect(socket, &QTcpSocket::bytesWritten, this, &MainWindow::bytesWritten);
    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::readyRead);

//    connect(socket, SIGNAL(bytesWritten(qint64)), this, SLOT(bytesWritten(qint64)));
//    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));

    socket->connectToHost(ipAddr, portNumber);
    socket->waitForConnected(3000);
    if  (socket->state() != QTcpSocket::ConnectedState)
    {
          QMessageBox::critical(this, "Connection error", "Connection hasn't been established");
          delete socket;
          return;
    }

    ui->sendPushButton->setEnabled(true);
    ui->msgPushButton->setEnabled(true);
    ui->chatSenderField->setEnabled(true);

    ui->ipLineEdit->setEnabled(false);
    ui->portLineEdit->setEnabled(false);
}



void MainWindow::on_sendPushButton_clicked()
{

    QString fileStr = QFileDialog::getOpenFileName(this, "Select transfering file");

    QFile file(fileStr);
    if (!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::critical(this, "File error", "File hasn't been opened");
    }

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_1);

    QString fileName = file.fileName();
    fileName = fileName.section("/", -1);
    out << (quint32)0 << fileName;

    block.append(file.readAll());
    file.close();

    out.device()->seek(0);
    out << (quint32)(block.size() - sizeof(quint32));

    qint64 curPosition = 0;
    while (curPosition< block.size())
    {
        qint64 curPiece = socket->write(block);

        curPosition += curPiece;
    }

    ui->sendPushButton->setEnabled(false);
    ui->msgPushButton->setEnabled(false);
    ui->chatSenderField->setEnabled(false);

    ui->ipLineEdit->setEnabled(true);
    ui->portLineEdit->setEnabled(true);
}


void MainWindow::on_msgPushButton_clicked()
{
    sendMessage();
}

void MainWindow::bytesWritten(qint64 bytes)
{
    qDebug() << bytes << "bytes written...";
}

void MainWindow::readyRead()
{
    QByteArray data = socket->readAll();
    qDebug() << "[SOCKET] Data in: " << data;

    //    while (socket->canReadLine())
    //    {
    //        QString data = QString(socket->readLine());

    //        Message msg;
    //        msg.message = data;
    //        msg.isMyMsg = false;

    //        message_history->append(msg);
    //    }


    //    updateChatField();
}

void MainWindow::sendMessage()
{
    QString text = ui->chatSenderField->toPlainText();
    text = text + "\n";

    Message msg;
    msg.message = text;
    msg.isMyMsg = true;

    message_history->append(msg);
    updateChatField();

    socket->write(("[msg]" + text).toUtf8());
}

// получаем сообщение с сервера
//void MainWindow::readData()
//{
//    QTcpSocket* conn = qobject_cast<QTcpSocket*>(sender());
//    while (conn->canReadLine())
//    {
//        QString data = QString(conn->readLine());

//        Message msg;
//        msg.message = data;
//        msg.isMyMsg = false;

//        message_history->append(msg);
//    }


//    updateChatField();
//}

void MainWindow::updateChatField()
{
    QString messages = "";
    foreach(Message msg, *message_history)
    {
        if (!msg.isMyMsg)
            messages += "\t";
        messages += msg.message;
    }

    ui->chatField->clear();
    ui->chatField->setPlainText(messages);
}


