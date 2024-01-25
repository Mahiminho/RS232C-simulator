#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTextCodec>
#include <QByteArray>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_activateButton_clicked();

    void on_sendMessageButton_clicked();

    void on_sendPackageButton_clicked();

    void on_refreshButton_clicked();

    void readyToRead();

private:
    Ui::MainWindow *ui;

    bool isPortActive;
    QSerialPort serialPort;
    int m_charToSendIndex;
};

#endif // MAINWINDOW_H
