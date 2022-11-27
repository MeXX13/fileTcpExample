#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QRegExpValidator>
#include <QMessageBox>
#include <QHostAddress>
#include <QFileDialog>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QString ipRange = "(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])";
    QRegExp ipRegex ("^" + ipRange
                     + "\\." + ipRange
                     + "\\." + ipRange
                     + "\\." + ipRange + "$");
    QRegExpValidator *ipValidator = new QRegExpValidator(ipRegex, this);
    ui->ipLineEdit->setValidator(ipValidator);
}

MainWindow::~MainWindow()
{
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
    socket->connectToHost(ipAddr, portNumber);
    socket->waitForConnected(3000);
    if  (socket->state() != QTcpSocket::ConnectedState)
    {
          QMessageBox::critical(this, "Connection error", "Connection hasn't been established");
          delete socket;
          return;
    }

    ui->sendPushButton->setEnabled(true);
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
}

