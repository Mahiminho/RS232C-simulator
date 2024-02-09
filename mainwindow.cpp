#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QStringList parity_options = {"EvenParity", "OddParity"};
    ui->comboBoxParity->addItems(parity_options);
    ui->comboBoxParity->setItemData(0, QSerialPort::EvenParity);
    ui->comboBoxParity->setItemData(1, QSerialPort::OddParity);
    isPortActive = false;

     connect(&serialPort, &QSerialPort::readyRead, this, &MainWindow::readyToRead);

     QRegExp regExp2("^[1-2]|10$");
     QRegExpValidator *validator2 = new QRegExpValidator(regExp2, ui->lineEditStopBits);
     ui->lineEditStopBits->setValidator(validator2);

     QRegExp regExp3("^(1[0-6]|[1-9])$");
     QRegExpValidator *validator3 = new QRegExpValidator(regExp3, ui->lineEditDataBits);
     ui->lineEditDataBits->setValidator(validator3);

     QRegExp regExp4("^(?:[1-9]|[1-9][0-9])$");
     QRegExpValidator *validator4 = new QRegExpValidator(regExp4, ui->lineEditWordDelay);
     ui->lineEditWordDelay->setValidator(validator4);
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::on_activateButton_clicked()
{
    if (isPortActive)
    {
        serialPort.close();
        isPortActive = false;
        ui->activateButton->setText("Activate");

        ui->comboBoxParity->setEnabled(true);
        ui->lineEditStopBits->setEnabled(true);
        ui->lineEditDataBits->setEnabled(true);
        ui->lineEditWordDelay->setEnabled(true);
        ui->comboBoxCOMport->setEnabled(true);
        ui->refreshButton->setEnabled(true);
    }
    else
    {
        QString port_name = ui->comboBoxCOMport->currentText();
        if (port_name.isEmpty())
            return;
        QSerialPortInfo port(port_name);

        serialPort.setPortName(port.portName());
        serialPort.setParity(ui->comboBoxCOMport->currentData().value<QSerialPort::Parity>());
        serialPort.setStopBits(QVariant(ui->lineEditStopBits->text().toInt()).value<QSerialPort::StopBits>());
        serialPort.setDataBits(QVariant(ui->lineEditDataBits->text().toInt()).value<QSerialPort::DataBits>());
        isPortActive = serialPort.open(QIODevice::ReadWrite);

        if (isPortActive)
        {
            ui->activateButton->setText("Deactivate");
            ui->comboBoxParity->setEnabled(false);
            ui->lineEditStopBits->setEnabled(false);
            ui->lineEditDataBits->setEnabled(false);
            ui->lineEditWordDelay->setEnabled(false);
            ui->comboBoxCOMport->setEnabled(false);
            ui->refreshButton->setEnabled(false);
        }
    }
}



void MainWindow::on_sendMessageButton_clicked()
{
    if (isPortActive)
    {
        QString text = ui->lineEditSend->text();
        QTextCodec *codec = QTextCodec::codecForName("CP866");
        serialPort.write(codec->fromUnicode(text));
        m_charToSendIndex = 0;


        // Data
        QString result = "";
        QByteArray byteArray = codec->fromUnicode(text);
        QString binaryString;
        int count;
        QString parity;
        int byte;

        // Data from stop bits and word delay
        int stopbits;
        QString stop;

        int delaybits;
        QString delay;

        QString bit = "1";

        QString paritytext = ui->comboBoxParity->currentText();
        for (int i = 0; i < text.length(); i++) {
            // Bit code of symbol
            byte = byteArray.at(i);
            binaryString = QString("%1").arg(QString::number(byte, 2), 8, '0').right(8);

            // Definition parity bit (from ComboBox)
            count = binaryString.count(QChar('1'));
            if (paritytext == "EvenParity") {
                if (count % 2 == 0)
                    parity = "0";
                else
                    parity = "1";
            } else {
                if (count % 2 == 0)
                    parity = "1";
                else
                    parity = "0";
            }

            // Definition count of stop bits and word delay
            stopbits = ui->lineEditStopBits->text().toInt();
            stop = bit.repeated(stopbits);
            delaybits = ui->lineEditWordDelay->text().toInt();
            delay = bit.repeated(delaybits);

            result += "0" + binaryString + parity + stop + delay;
        }



        result.chop(1);
        ui->labelSendBits->setText(result);
    }
}



void MainWindow::on_sendPackageButton_clicked()
{
    if (isPortActive)
    {
        QString text = ui->lineEditSend->text();
        if (!text.isEmpty())
        {
            int len = text.length();
            if (m_charToSendIndex >= len)
                m_charToSendIndex = 0;
            QChar ch = text.at(m_charToSendIndex);
            QTextCodec *codec = QTextCodec::codecForName("CP866");
            serialPort.write(codec->fromUnicode(ch));



            // Data
            QString result = "";
            QByteArray byteArray = codec->fromUnicode(text);
            QString binaryString;
            int count;
            QString parity;
            int byte;

            // Data from stop bits and word delay
            int stopbits;
            QString stop;

            int delaybits;
            QString delay;

            QString bit = "1";

            QString paritytext = ui->comboBoxParity->currentText();

            // Bit code of symbol
            byte = byteArray.at(m_charToSendIndex);
            binaryString = QString("%1").arg(QString::number(byte, 2), 8, '0').right(8);

            // Definition parity bit (from ComboBox)
            count = binaryString.count(QChar('1'));
            if (paritytext == "EvenParity") {
                if (count % 2 == 0)
                    parity = "0";
                else
                    parity = "1";
            } else {
                if (count % 2 == 0)
                    parity = "1";
                else
                    parity = "0";
            }

            // Definition count of stop bits and word delay
            stopbits = ui->lineEditStopBits->text().toInt();
            stop = bit.repeated(stopbits);
            delaybits = ui->lineEditWordDelay->text().toInt();
            delay = bit.repeated(delaybits);

            result += "0" + binaryString + parity + stop;

            ui->labelSendBits->setText(result);



            if (m_charToSendIndex < (len - 1))
                m_charToSendIndex++;
            else
                m_charToSendIndex = 0;
        }
    }
}



void MainWindow::on_refreshButton_clicked()
{
    ui->comboBoxCOMport->clear();
    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    for (QSerialPortInfo &port : ports)
        ui->comboBoxCOMport->addItem(port.portName());
}



void MainWindow::readyToRead()
{
    QTextCodec *codec = QTextCodec::codecForName("CP866");
    QString text = QString(codec->toUnicode(serialPort.readAll()));
    ui->labelRecieved->setText(text);

    // Data
    QString result = "";
    QByteArray byteArray = codec->fromUnicode(text);
    QString binaryString;
    int count;
    QString parity;
    int byte;

    // Data from stop bits and word delay
    int stopbits;
    QString stop;

    int delaybits;
    QString delay;

    QString bit = "1";

    QString paritytext = ui->comboBoxParity->currentText();
    for (int i = 0; i < text.length(); i++) {
        // Bit code of symbol
        byte = byteArray.at(i);
        binaryString = QString("%1").arg(QString::number(byte, 2), 8, '0').right(8);

        // Definition parity bit (from ComboBox)
        count = binaryString.count(QChar('1'));
        if (paritytext == "EvenParity") {
            if (count % 2 == 0)
                parity = "0";
            else
                parity = "1";
        } else {
            if (count % 2 == 0)
                parity = "1";
            else
                parity = "0";
        }

        // Definition count of stop bits and word delay
        stopbits = ui->lineEditStopBits->text().toInt();
        stop = bit.repeated(stopbits);
        delaybits = ui->lineEditWordDelay->text().toInt();
        delay = bit.repeated(delaybits);

        result += "0" + binaryString + parity + stop + delay;
    }

    result.chop(1);
    ui->labelGetBits->setText(result);
}
