#include "mainwindow.h"
#include "ui_mainwindow.h"
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
    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::readyRead);
    socket->connectToHost(ipAddr, portNumber);
    socket->waitForConnected(3000);
    if  (socket->state() != QTcpSocket::ConnectedState)
    {
          QMessageBox::critical(this, "Connection error", "Connection hasn't been established");
          delete socket;
          return;
    }

    ui->sendPushButton->setEnabled(true);
    ui->sendText->setEnabled(true);
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
    ui->ipLineEdit->setEnabled(true);
    ui->portLineEdit->setEnabled(true);
}


void MainWindow::on_sendText_clicked()
{
    QString text;
    text = ui->text->toPlainText();
    QByteArray qba = "[msg]";
    qba.append(text.toLocal8Bit());

    socket->write(qba);

    ui->chatWindow->append(text);
}

void MainWindow::readyRead()
{
    QByteArray qba = socket->readAll();

    ui->chatWindow->append(qba);
}

