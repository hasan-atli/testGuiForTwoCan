// Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
// Copyright (C) 2012 Laszlo Papp <lpapp@kde.org>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "console.h"
#include "settingsdialog.h"
#include "torkTypes.h"

#include <QLabel>
#include <QMessageBox>
#include <QTimer>
#include <QThread>
#include <QDebug>
#include <QDateTime>
#include <QDebug>
#include <QSerialPortInfo>

#include <chrono>


static constexpr std::chrono::seconds kWriteTimeout = std::chrono::seconds{5};

//! [0]
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::MainWindow),
//! [0]
    m_status(new QLabel),
    m_console(new Console),
    m_settings(new SettingsDialog(this)),
    m_timer(new QTimer(this)),
//! [1]
    m_serial(new QSerialPort(this))

{
//! [1]
    m_ui->setupUi(this);
    m_console->setEnabled(false);
    //setCentralWidget(m_console);
    m_ui->verticalLayout_rx->addWidget(m_console);

    m_ui->actionConnect->setEnabled(true);
    m_ui->actionDisconnect->setEnabled(false);
    m_ui->actionQuit->setEnabled(true);
    m_ui->actionConfigure->setEnabled(true);

    m_ui->statusBar->addWidget(m_status);


    initActionsConnections();

    connect(m_serial, &QSerialPort::errorOccurred, this, &MainWindow::handleError);
    connect(m_timer, &QTimer::timeout, this, &MainWindow::handleWriteTimeout);
    m_timer->setSingleShot(true);

//! [2]
    connect(m_serial, &QSerialPort::readyRead, this, &MainWindow::readData);
    connect(m_serial, &QSerialPort::bytesWritten, this, &MainWindow::handleBytesWritten);
//! [2]
    connect(m_console, &Console::getData, this, &MainWindow::writeData);    //her karakter girildiginde direk gönderen slot-signal

    QPalette p = palette();
    p.setColor(QPalette::Base, QColor::fromRgb(84, 84, 84));
    p.setColor(QPalette::Text, Qt::green);
    m_ui->lineEdit_Tx->setPalette(p);

#ifndef DBG
    m_ui->groupBox->setVisible(false);
    // MainWindow'un boyutunu ayarla

#endif

    m_ui->canA_Cnfg_0->setToolTip("FRAME_FORMAT");
    m_ui->canA_Cnfg_1->setToolTip("NOMINAL_BITRATE");
    m_ui->canA_Cnfg_2->setToolTip("DATA_BITRATE");
    m_ui->canA_Cnfg_3->setToolTip("STD_FLTER_NUM ");
    m_ui->canA_Cnfg_4->setToolTip("1st_STD_FILTER_TYPE");
    m_ui->canA_Cnfg_5->setToolTip("1st_STD_FILTER_CONFIG");
    m_ui->canA_Cnfg_6->setToolTip("2st_STD_FILTER_TYPE	");
    m_ui->canA_Cnfg_7->setToolTip("2st_STD_FILTER_CONFIG");
    m_ui->canA_Cnfg_8->setToolTip("3st_STD_FILTER_TYPE");
    m_ui->canA_Cnfg_9->setToolTip("3st_STD_FILTER_CONFIG");
    m_ui->canA_Cnfg_10->setToolTip("EXT_FLTER_NUM");
    m_ui->canA_Cnfg_11->setToolTip("EXT_1st_FILTER_TYPE	");
    m_ui->canA_Cnfg_12->setToolTip("EXT_1st_FILTER_CONFIG");
    m_ui->canA_Cnfg_13->setToolTip("EXT_2st_FILTER_TYPE");
    m_ui->canA_Cnfg_14->setToolTip("EXT_2st_FILTER_CONFIG");
    m_ui->canA_Cnfg_15->setToolTip("EXT_3st_FILTER_TYPE");
    m_ui->canA_Cnfg_16->setToolTip("EXT_3st_FILTER_CONFIG");
    m_ui->canA_Cnfg_17->setToolTip("CRC_CAN_A_L");
    m_ui->canA_Cnfg_18->setToolTip("CRC_CAN_A_H");


    m_ui->canA_Fltr_0->setToolTip("CAN_A_1st_Std_FilterID1");
    m_ui->canA_Fltr_1->setToolTip("CAN_A_1st_Std_FilterID2");
    m_ui->canA_Fltr_2->setToolTip("CAN_A_2st_Std_FilterID1");
    m_ui->canA_Fltr_3->setToolTip("CAN_A_2st_Std_FilterID2");
    m_ui->canA_Fltr_4->setToolTip("CAN_A_3st_Std_FilterID1");
    m_ui->canA_Fltr_5->setToolTip("CAN_A_3st_Std_FilterID2");

    m_ui->canA_Fltr_6->setToolTip("CAN_A_1st_Ext_FilterID1");
    m_ui->canA_Fltr_7->setToolTip("CAN_A_1st_Ext_FilterID2");
    m_ui->canA_Fltr_8->setToolTip("CAN_A_2st_Ext_FilterID1");
    m_ui->canA_Fltr_9->setToolTip("CAN_A_2st_Ext_FilterID2");
    m_ui->canA_Fltr_10->setToolTip("CAN_A_3st_Ext_FilterID1");
    m_ui->canA_Fltr_11->setToolTip("CAN_A_3st_Ext_FilterID2");

//    m_ui->canA_Fltr_12->setToolTip("CAN_A_CRC_FilterID_L ");
//    m_ui->canA_Fltr_13->setToolTip("CAN_A_CRC_FilterID_H");



    m_ui->route1_0->setToolTip("ROUTE_ONE_ENABLE");
    m_ui->route1_1->setToolTip("1st_METHOD_TO_BE_IF_RECEIVED_STD_ID_MSG_IN_ONE_ROUTE");
    m_ui->route1_2->setToolTip("1st_ADD_VARIABLE_RECEIVED_STD_ID_ONE_ROUTE ");
    m_ui->route1_3->setToolTip("1st_METHOD_TO_BE_IF_RECEIVED_EXT_ID_MSG_IN_ONE_ROUTE");
    m_ui->route1_4->setToolTip("1st_ADD_VARIABLE_RECEIVED_EXT_ID_ONE_ROUTE");

    m_ui->route1_5->setToolTip("2st_METHOD_TO_BE_IF_RECEIVED_STD_ID_MSG_IN_ONE_ROUTE");
    m_ui->route1_6->setToolTip("2st_ADD_VARIABLE_RECEIVED_STD_ID_ONE_ROUTE");
    m_ui->route1_7->setToolTip("2st_METHOD_TO_BE_IF_RECEIVED_EXT_ID_MSG_IN_ONE_ROUTE");
    m_ui->route1_8->setToolTip("2st_ADD_VARIABLE_RECEIVED_EXT_ID_ONE_ROUTE");
    m_ui->route1_9->setToolTip("CRC_Route1_L");
    m_ui->route1_10->setToolTip("CRC_Route1_H");

    m_ui->canA_Cnfg_17->setMaximum(255);
    m_ui->canA_Cnfg_18->setMaximum(255);
    m_ui->canB_Cnfg_17->setMaximum(255);
    m_ui->canB_Cnfg_18->setMaximum(255);
 //   m_ui->canA_Fltr_12->setMaximum(INT_MAX);
 //   m_ui->canA_Fltr_13->setMaximum(INT_MAX);
 //   m_ui->canB_Fltr_12->setMaximum(INT_MAX);
 //   m_ui->canB_Fltr_13->setMaximum(INT_MAX);
    m_ui->route1_9->setMaximum(255);
    m_ui->route1_10->setMaximum(255);
    m_ui->route2_9->setMaximum(255);
    m_ui->route2_10->setMaximum(255);


    m_ui->canA_Fltr_0->setMaximum(INT_MAX);
    m_ui->canA_Fltr_1->setMaximum(INT_MAX);
    m_ui->canA_Fltr_2->setMaximum(INT_MAX);
    m_ui->canA_Fltr_3->setMaximum(INT_MAX);
    m_ui->canA_Fltr_4->setMaximum(INT_MAX);
    m_ui->canA_Fltr_5->setMaximum(INT_MAX);
    m_ui->canA_Fltr_6->setMaximum(INT_MAX);
    m_ui->canA_Fltr_7->setMaximum(INT_MAX);
    m_ui->canA_Fltr_8->setMaximum(INT_MAX);
    m_ui->canA_Fltr_9->setMaximum(INT_MAX);
    m_ui->canA_Fltr_10->setMaximum(INT_MAX);
    m_ui->canA_Fltr_11->setMaximum(INT_MAX);

    m_ui->canB_Fltr_0->setMaximum(INT_MAX);
    m_ui->canB_Fltr_1->setMaximum(INT_MAX);
    m_ui->canB_Fltr_2->setMaximum(INT_MAX);
    m_ui->canB_Fltr_3->setMaximum(INT_MAX);
    m_ui->canB_Fltr_4->setMaximum(INT_MAX);
    m_ui->canB_Fltr_5->setMaximum(INT_MAX);
    m_ui->canB_Fltr_6->setMaximum(INT_MAX);
    m_ui->canB_Fltr_7->setMaximum(INT_MAX);
    m_ui->canB_Fltr_8->setMaximum(INT_MAX);
    m_ui->canB_Fltr_9->setMaximum(INT_MAX);
    m_ui->canB_Fltr_10->setMaximum(INT_MAX);
    m_ui->canB_Fltr_11->setMaximum(INT_MAX);

    u32LEdit_CanA_Crc_H = new QUInt32LineEdit(this);
    u32LEdit_CanA_Crc_L = new QUInt32LineEdit(this);
    u32LEdit_CanB_Crc_H = new QUInt32LineEdit(this);
    u32LEdit_CanB_Crc_L = new QUInt32LineEdit(this);

    u32LEdit_CanA_Crc_H->setValue(0);
    u32LEdit_CanA_Crc_L->setValue(0);
    u32LEdit_CanB_Crc_H->setValue(0);
    u32LEdit_CanB_Crc_L->setValue(0);

    u32LEdit_CanA_Crc_L->setToolTip("CAN A CRC L ");
    u32LEdit_CanA_Crc_L->setGeometry(1090, 265, 90, 40);

    u32LEdit_CanA_Crc_H->setToolTip("CAN A CRC H");
    u32LEdit_CanA_Crc_H->setGeometry(1200, 265, 90, 40);

    u32LEdit_CanB_Crc_L->setToolTip("CAN B CRC L ");
    u32LEdit_CanB_Crc_L->setGeometry(1090, 395, 90, 40);

    u32LEdit_CanB_Crc_H->setToolTip("CAN B CRC H ");
    u32LEdit_CanB_Crc_H->setGeometry(1200, 395, 90, 40);


//! [3]
}
//! [3]

MainWindow::~MainWindow()
{
    delete m_settings;
    delete m_ui;
}

//! [4]
void MainWindow::openSerialPort()
{
    const SettingsDialog::Settings p = m_settings->settings();
    m_serial->setPortName(p.name);
    m_serial->setBaudRate(p.baudRate);
    m_serial->setDataBits(p.dataBits);
    m_serial->setParity(p.parity);
    m_serial->setStopBits(p.stopBits);
    m_serial->setFlowControl(p.flowControl);
    if (m_serial->open(QIODevice::ReadWrite)) {
        m_console->setEnabled(true);
        m_console->setLocalEchoEnabled(p.localEchoEnabled);
        m_ui->actionConnect->setEnabled(false);
        m_ui->actionDisconnect->setEnabled(true);
        m_ui->actionConfigure->setEnabled(false);
        showStatusMessage(tr("Connected to %1 : %2, %3, %4, %5, %6")
                          .arg(p.name, p.stringBaudRate, p.stringDataBits,
                               p.stringParity, p.stringStopBits, p.stringFlowControl));
        m_ui->label_serialStatus->setText("baglantı kuruldu");
    } else {
        QMessageBox::critical(this, tr("Error"), m_serial->errorString());

        showStatusMessage(tr("Open error"));
        m_ui->label_serialStatus->setText("baglantı kurulamadı HATA!!");
    }
}
//! [4]

//! [5]
void MainWindow::closeSerialPort()
{
    if (m_serial->isOpen())
        m_serial->close();
    m_console->setEnabled(false);
    m_ui->actionConnect->setEnabled(true);
    m_ui->actionDisconnect->setEnabled(false);
    m_ui->actionConfigure->setEnabled(true);
    showStatusMessage(tr("Disconnected"));
    m_ui->label_serialStatus->setText("baglantı kapatıldı");
}
//! [5]

void MainWindow::about()
{
    QMessageBox::about(this, tr("About Serial Terminal"),
                       tr("The <b>Serial Terminal</b> example demonstrates how to "
                          "use the Qt Serial Port module in modern GUI applications "
                          "using Qt, with a menu bar, toolbars, and a status bar."));
}

//! [6]
void MainWindow::writeData(const QByteArray &data)
{
    const qint64 written = m_serial->write(data);
    if (written == data.size()) {
        m_bytesToWrite += written;
        m_timer->start(kWriteTimeout);
    } else {
        const QString error = tr("Failed to write all data to port %1.\n"
                                 "Error: %2").arg(m_serial->portName(),
                                                  m_serial->errorString());
        showWriteError(error);
    }
}
//! [6]

//! [7]
void MainWindow::readData()
{
    const QByteArray data = m_serial->readAll();
    m_console->putData(data);
    qDebug() << data;

}
//! [7]

//! [8]
void MainWindow::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        QMessageBox::critical(this, tr("Critical Error"), m_serial->errorString());
        closeSerialPort();
    }
}
//! [8]

//! [9]
void MainWindow::handleBytesWritten(qint64 bytes)
{
    m_bytesToWrite -= bytes;
    if (m_bytesToWrite == 0)
        m_timer->stop();
}
//! [9]

void MainWindow::handleWriteTimeout()
{
    const QString error = tr("Write operation timed out for port %1.\n"
                             "Error: %2").arg(m_serial->portName(),
                                              m_serial->errorString());
    showWriteError(error);
}

void MainWindow::initActionsConnections()
{
    connect(m_ui->actionConnect, &QAction::triggered, this, &MainWindow::openSerialPort);
    connect(m_ui->actionDisconnect, &QAction::triggered, this, &MainWindow::closeSerialPort);
    connect(m_ui->actionQuit, &QAction::triggered, this, &MainWindow::close);
    connect(m_ui->actionConfigure, &QAction::triggered, m_settings, &SettingsDialog::show);
    connect(m_ui->actionClear, &QAction::triggered, m_console, &Console::clear);
    connect(m_ui->actionAbout, &QAction::triggered, this, &MainWindow::about);
    connect(m_ui->actionAboutQt, &QAction::triggered, qApp, &QApplication::aboutQt);
}

void MainWindow::showStatusMessage(const QString &message)
{
    m_status->setText(message);
}

void MainWindow::showWriteError(const QString &message)
{
    QMessageBox::warning(this, tr("Warning"), message);
}


void MainWindow::on_serialTxBtn_2_clicked()
{
    if(m_serial->isOpen() == false)
    {
        m_ui->lineEdit_Tx->clear();
        m_ui->label_serialStatus->setText("bağlantı yok");
        return;
    }

    m_ui->label_serialStatus->setText("veri gönderildi");
    QString bufData = m_ui->lineEdit_Tx->text();
    QByteArray data =bufData.toUtf8();
    writeData(data);
    m_ui->lineEdit_Tx->clear();
}



void MainWindow::on_testButton_clicked()
{

}


void MainWindow::on_testButton_3_clicked()
{

}




void MainWindow::on_Btn_canAcnfg_clicked()
{
    qDebug() << "btn canAcnfg";


    // calculate CRC
    uint8_t bufferCRC[SIZE_OF_CAN_VALUE_SETTINGS];

    bufferCRC[0]  = m_ui->canA_Cnfg_0->value();
    bufferCRC[1]  = m_ui->canA_Cnfg_1->value();
    bufferCRC[2]  = m_ui->canA_Cnfg_2->value();
    bufferCRC[3]  = m_ui->canA_Cnfg_3->value();
    bufferCRC[4]  = m_ui->canA_Cnfg_4->value();
    bufferCRC[5]  = m_ui->canA_Cnfg_5->value();
    bufferCRC[6]  = m_ui->canA_Cnfg_6->value();
    bufferCRC[7]  = m_ui->canA_Cnfg_7->value();
    bufferCRC[8]  = m_ui->canA_Cnfg_8->value();
    bufferCRC[9]  = m_ui->canA_Cnfg_9->value();
    bufferCRC[10] = m_ui->canA_Cnfg_10->value();
    bufferCRC[11] = m_ui->canA_Cnfg_11->value();
    bufferCRC[12] = m_ui->canA_Cnfg_12->value();
    bufferCRC[13] = m_ui->canA_Cnfg_13->value();
    bufferCRC[14] = m_ui->canA_Cnfg_14->value();
    bufferCRC[15] = m_ui->canA_Cnfg_15->value();
    bufferCRC[16] = m_ui->canA_Cnfg_16->value();

    torkUInt16_VAL calculated_CRC;
    calculated_CRC.Val = Calculate_Crc_16(bufferCRC, sizeof(bufferCRC));

    m_ui->canA_Cnfg_17->setValue(calculated_CRC.Byte[0]);      //crc low
    m_ui->canA_Cnfg_18->setValue(calculated_CRC.Byte[1]);      //crc high

    // send Msg
    QByteArray bufCanFrame;

    bufCanFrame.append("$");
    bufCanFrame.append("C");
    bufCanFrame.append("A");
    bufCanFrame.append("N");
    bufCanFrame.append("A");
    bufCanFrame.append("V");
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->canA_Cnfg_0->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->canA_Cnfg_1->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->canA_Cnfg_2->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->canA_Cnfg_3->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->canA_Cnfg_4->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->canA_Cnfg_5->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->canA_Cnfg_6->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->canA_Cnfg_7->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->canA_Cnfg_8->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->canA_Cnfg_9->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->canA_Cnfg_10->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->canA_Cnfg_11->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->canA_Cnfg_12->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->canA_Cnfg_13->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->canA_Cnfg_14->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->canA_Cnfg_15->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->canA_Cnfg_16->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->canA_Cnfg_17->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->canA_Cnfg_18->value()));
    bufCanFrame.append("*");
    //send
    writeData(bufCanFrame);
}





void MainWindow::on_Btn_canBcnfg_clicked()
{
    qDebug() << "btn canBcnfg";

    // calculate CRC
    uint8_t bufferCRC[SIZE_OF_CAN_VALUE_SETTINGS];

    bufferCRC[0]  = m_ui->canB_Cnfg_0->value();
    bufferCRC[1]  = m_ui->canB_Cnfg_1->value();
    bufferCRC[2]  = m_ui->canB_Cnfg_2->value();
    bufferCRC[3]  = m_ui->canB_Cnfg_3->value();
    bufferCRC[4]  = m_ui->canB_Cnfg_4->value();
    bufferCRC[5]  = m_ui->canB_Cnfg_5->value();
    bufferCRC[6]  = m_ui->canB_Cnfg_6->value();
    bufferCRC[7]  = m_ui->canB_Cnfg_7->value();
    bufferCRC[8]  = m_ui->canB_Cnfg_8->value();
    bufferCRC[9]  = m_ui->canB_Cnfg_9->value();
    bufferCRC[10] = m_ui->canB_Cnfg_10->value();
    bufferCRC[11] = m_ui->canB_Cnfg_11->value();
    bufferCRC[12] = m_ui->canB_Cnfg_12->value();
    bufferCRC[13] = m_ui->canB_Cnfg_13->value();
    bufferCRC[14] = m_ui->canB_Cnfg_14->value();
    bufferCRC[15] = m_ui->canB_Cnfg_15->value();
    bufferCRC[16] = m_ui->canB_Cnfg_16->value();

    torkUInt16_VAL calculated_CRC;
    calculated_CRC.Val = Calculate_Crc_16(bufferCRC, sizeof(bufferCRC));

    m_ui->canB_Cnfg_17->setValue(calculated_CRC.Byte[0]);      //crc low
    m_ui->canB_Cnfg_18->setValue(calculated_CRC.Byte[1]);      //crc high

    // send Msg
    QByteArray bufCanFrame;

    bufCanFrame.append("$");
    bufCanFrame.append("C");
    bufCanFrame.append("A");
    bufCanFrame.append("N");
    bufCanFrame.append("B");
    bufCanFrame.append("V");
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->canB_Cnfg_0->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->canB_Cnfg_1->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->canB_Cnfg_2->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->canB_Cnfg_3->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->canB_Cnfg_4->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->canB_Cnfg_5->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->canB_Cnfg_6->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->canB_Cnfg_7->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->canB_Cnfg_8->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->canB_Cnfg_9->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->canB_Cnfg_10->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->canB_Cnfg_11->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->canB_Cnfg_12->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->canB_Cnfg_13->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->canB_Cnfg_14->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->canB_Cnfg_15->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->canB_Cnfg_16->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->canB_Cnfg_17->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->canB_Cnfg_18->value()));
    bufCanFrame.append("*");


    //send
    writeData(bufCanFrame);
}


void MainWindow::on_Btn_canA_fltr_clicked()
{
    qDebug() << "canA_fltr_clicked()";

    // verilerin CRC' sini hesapla
    uint32_t bufferCRC[SIZE_OF_CAN_FILTER_IDS];

    bufferCRC[0] = m_ui->canA_Fltr_0->value();
    bufferCRC[1] = m_ui->canA_Fltr_1->value();
    bufferCRC[2] = m_ui->canA_Fltr_2->value();
    bufferCRC[3] = m_ui->canA_Fltr_3->value();
    bufferCRC[4] = m_ui->canA_Fltr_4->value();
    bufferCRC[5] = m_ui->canA_Fltr_5->value();
    bufferCRC[6] = m_ui->canA_Fltr_6->value();
    bufferCRC[7] = m_ui->canA_Fltr_7->value();
    bufferCRC[8] = m_ui->canA_Fltr_8->value();
    bufferCRC[9] = m_ui->canA_Fltr_9->value();
    bufferCRC[10] = m_ui->canA_Fltr_10->value();
    bufferCRC[11] = m_ui->canA_Fltr_11->value();

    uint32_t CRC = Calculate_Crc_32(bufferCRC, sizeof(bufferCRC)/4);
    u32LEdit_CanA_Crc_H->setValue(CRC);
    u32LEdit_CanA_Crc_L->setValue(CRC);

    // send Msg
    QByteArray bufCanFrame;

    bufCanFrame.append("$");
    bufCanFrame.append("F");
    bufCanFrame.append("L");
    bufCanFrame.append("T");
    bufCanFrame.append("R");
    bufCanFrame.append("A");
    bufCanFrame.append("I");
    bufCanFrame.append("D");
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->canA_Fltr_0->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->canA_Fltr_1->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->canA_Fltr_2->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->canA_Fltr_3->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->canA_Fltr_4->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->canA_Fltr_5->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->canA_Fltr_6->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->canA_Fltr_7->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->canA_Fltr_8->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->canA_Fltr_9->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->canA_Fltr_10->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->canA_Fltr_11->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(u32LEdit_CanA_Crc_L->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(u32LEdit_CanA_Crc_H->value()));
    bufCanFrame.append("*");


    //send
    writeData(bufCanFrame);

}


void MainWindow::on_Btn_canB_fltr_clicked()
{
    qDebug() << "canB_fltr_clicked()";

    // verilerin CRC' sini hesapla
    uint32_t bufferCRC[SIZE_OF_CAN_FILTER_IDS];

    bufferCRC[0] = m_ui->canB_Fltr_0->value();
    bufferCRC[1] = m_ui->canB_Fltr_1->value();
    bufferCRC[2] = m_ui->canB_Fltr_2->value();
    bufferCRC[3] = m_ui->canB_Fltr_3->value();
    bufferCRC[4] = m_ui->canB_Fltr_4->value();
    bufferCRC[5] = m_ui->canB_Fltr_5->value();
    bufferCRC[6] = m_ui->canB_Fltr_6->value();
    bufferCRC[7] = m_ui->canB_Fltr_7->value();
    bufferCRC[8] = m_ui->canB_Fltr_8->value();
    bufferCRC[9] = m_ui->canB_Fltr_9->value();
    bufferCRC[10] = m_ui->canB_Fltr_10->value();
    bufferCRC[11] = m_ui->canB_Fltr_11->value();

    uint32_t CRC = Calculate_Crc_32(bufferCRC, sizeof(bufferCRC)/4);
    u32LEdit_CanB_Crc_H->setValue(CRC);
    u32LEdit_CanB_Crc_L->setValue(CRC);

    //send msg
    QByteArray bufCanFrame;

    bufCanFrame.append("$");
    bufCanFrame.append("F");
    bufCanFrame.append("L");
    bufCanFrame.append("T");
    bufCanFrame.append("R");
    bufCanFrame.append("B");
    bufCanFrame.append("I");
    bufCanFrame.append("D");
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->canB_Fltr_0->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->canB_Fltr_1->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->canB_Fltr_2->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->canB_Fltr_3->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->canB_Fltr_4->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->canB_Fltr_5->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->canB_Fltr_6->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->canB_Fltr_7->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->canB_Fltr_8->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->canB_Fltr_9->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->canB_Fltr_10->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->canB_Fltr_11->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(u32LEdit_CanB_Crc_L->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(u32LEdit_CanB_Crc_H->value()));
    bufCanFrame.append("*");


    //send
    writeData(bufCanFrame);

}


void MainWindow::on_btnRoute1_clicked()
{
    qDebug() << "btnRoute1_clicked()";

    // calculate CRC
    uint8_t bufferCRC[SIZE_OF_ROUTE_VALUE_SETTINGS];

    bufferCRC[0]  = m_ui->route1_0->value();
    bufferCRC[1]  = m_ui->route1_1->value();
    bufferCRC[2]  = m_ui->route1_2->value();
    bufferCRC[3]  = m_ui->route1_3->value();
    bufferCRC[4]  = m_ui->route1_4->value();
    bufferCRC[5]  = m_ui->route1_5->value();
    bufferCRC[6]  = m_ui->route1_6->value();
    bufferCRC[7]  = m_ui->route1_7->value();
    bufferCRC[8]  = m_ui->route1_8->value();

    torkUInt16_VAL calculated_CRC;
    calculated_CRC.Val = Calculate_Crc_16(bufferCRC, sizeof(bufferCRC));

    m_ui->route1_9->setValue(calculated_CRC.Byte[0]);      //crc low
    m_ui->route1_10->setValue(calculated_CRC.Byte[1]);      //crc high

    // send Msg
    QByteArray bufCanFrame;

    bufCanFrame.append("$");
    bufCanFrame.append("R");
    bufCanFrame.append("O");
    bufCanFrame.append("N");
    bufCanFrame.append("E");
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->route1_0->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->route1_1->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->route1_2->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->route1_3->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->route1_4->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->route1_5->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->route1_6->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->route1_7->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->route1_8->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->route1_9->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->route1_10->value()));
    bufCanFrame.append("*");


    //send
    writeData(bufCanFrame);

}


void MainWindow::on_btnRoute2_clicked()
{
    qDebug() << "btnRoute2_clicked()";

    // calculate CRC
    uint8_t bufferCRC[SIZE_OF_ROUTE_VALUE_SETTINGS];

    bufferCRC[0]  = m_ui->route2_0->value();
    bufferCRC[1]  = m_ui->route2_1->value();
    bufferCRC[2]  = m_ui->route2_2->value();
    bufferCRC[3]  = m_ui->route2_3->value();
    bufferCRC[4]  = m_ui->route2_4->value();
    bufferCRC[5]  = m_ui->route2_5->value();
    bufferCRC[6]  = m_ui->route2_6->value();
    bufferCRC[7]  = m_ui->route2_7->value();
    bufferCRC[8]  = m_ui->route2_8->value();


    torkUInt16_VAL calculated_CRC;
    calculated_CRC.Val = Calculate_Crc_16(bufferCRC, sizeof(bufferCRC));

    m_ui->route2_9->setValue(calculated_CRC.Byte[0]);      //crc low
    m_ui->route2_10->setValue(calculated_CRC.Byte[1]);      //crc high

    // send Msg
    QByteArray bufCanFrame;

    bufCanFrame.append("$");
    bufCanFrame.append("R");
    bufCanFrame.append("T");
    bufCanFrame.append("W");
    bufCanFrame.append("O");
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->route2_0->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->route2_1->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->route2_2->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->route2_3->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->route2_4->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->route2_5->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->route2_6->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->route2_7->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->route2_8->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->route2_9->value()));
    bufCanFrame.append(",");
    bufCanFrame.append(QByteArray::number(m_ui->route2_10->value()));
    bufCanFrame.append("*");


    //send
    writeData(bufCanFrame);

}


uint16_t MainWindow::Calculate_Crc_16(unsigned char* data, int length)
{
    unsigned short crc = 0xFFFF;                   // Başlangıç değeri (16 bit)
    unsigned short polynomial = 0x1021;            // CRC polinomu (16 bit)

    for (int i = 0; i < length; i++)
    {
        crc ^= (data[i] << 8);                     // Veriyi CRC ile XOR'la

        for (int j = 0; j < 8; j++)
        {
            if (crc & 0x8000)
            {
                crc = (crc << 1) ^ polynomial;     // CRC'yi kaydır ve polinom ile XOR'la
            }
            else
            {
                crc <<= 1;                         // CRC'yi sadece kaydır
            }
        }

        crc &= 0xFFFF;                             // CRC'nin 16 biti sınırla
    }
    return crc;
}


uint32_t MainWindow::Calculate_Crc_32(uint32_t* data, int length)
{
    uint32_t crc = 0xFFFFFFFF;

    for (int i = 0; i < length; i++)
    {
        crc ^= data[i];
        for (int j = 0; j < 32; j++)
        {
            if (crc & 1)
            {
                crc = (crc >> 1) ^ 0xEDB88320;
            }
            else
            {
                crc >>= 1;
            }
        }
    }
    return crc ^ 0xFFFFFFFF;
}


void MainWindow::on_pushButton_clicked()
{
    QList<QSerialPortInfo> serialPortInfos = QSerialPortInfo::availablePorts();

    qDebug() << "Available ports:";
    for (const QSerialPortInfo &serialPortInfo : serialPortInfos) {
        qDebug() << "Port: " << serialPortInfo.portName();
        qDebug() << "Description: " << serialPortInfo.description();
        qDebug() << "Manufacturer: " << serialPortInfo.manufacturer();
        qDebug() << "Serial number: " << serialPortInfo.serialNumber();
        qDebug() << "System location: " << serialPortInfo.systemLocation();
        qDebug() << "Vendor ID: " << serialPortInfo.vendorIdentifier();
        qDebug() << "Product ID: " << serialPortInfo.productIdentifier();
    }
}

