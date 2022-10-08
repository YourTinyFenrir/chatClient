#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QDateTime"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    socket = new QTcpSocket(this);

    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::slotReadyRead);
    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);

    blockSize = 0;
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_buttonConnect_clicked()
{
    socket->connectToHost("127.0.0.1", 5151);
}

void MainWindow::sendToServer(QString msg)
{
    data.clear();
    QDataStream output(&data, QIODevice::WriteOnly);
    output.setVersion(QDataStream::Qt_6_3);

    output << quint16(0) << ui->mailingList->currentText().toULongLong() << msg; // Выделяем место под размер сообщения в начале блока данных
    output.device()->seek(0); // Переместиться в начало блока данных
    output << quint16(data.size() - sizeof(quint16)); // Записать размер сообщения

    socket->write(data);
    ui->messageText->clear();
}

void MainWindow::slotReadyRead()
{
    QDataStream input(socket);
    input.setVersion(QDataStream::Qt_6_3);

    if (input.status() == QDataStream::Ok) {

        while (true) {

            if (blockSize == 0) { // Проверка наличия размера блока данных

                if (socket->bytesAvailable() < 2) {
                    qDebug() << "Block size < 2";
                    break;
                }

                input >> blockSize; // Запись размера блока данных из потока в переменную
                qDebug() << "Block size = " << blockSize;
            }

            if (socket->bytesAvailable() < blockSize) { // Проверка, имеются ли в потоке данные заявленного размера
                qDebug() << "Data isn't full";
                break;
            }

            qint64 senderDesc;
            bool typeInfo;
            QString msg;

            input >> senderDesc;
            input >> typeInfo;
            input >> msg;

            blockSize = 0;

            if (!typeInfo) { // Передано сообщение
                ui->textBrowser->append(" (" + QDateTime::currentDateTime().toString() + ")      " + QString::number(senderDesc) + " - " + msg);
            }
            else { // Пополнение списка адресатов
                ui->mailingList->addItem(msg);
            }

        }

    }
    else {
        ui->textBrowser->append("Ошибка чтения");
    }

}


void MainWindow::on_buttonSend_clicked(bool checked)
{
    sendToServer(ui->messageText->text());
}

