#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "QTcpSocket"

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
    void on_buttonConnect_clicked();

    void on_buttonSend_clicked(bool checked);

private:
    Ui::MainWindow *ui;
    QTcpSocket* socket;
    QByteArray data;
    void sendToServer(QString);
    qint16 blockSize;

public slots:
    void slotReadyRead();
};
#endif // MAINWINDOW_H
