// Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
// Copyright (C) 2012 Laszlo Papp <lpapp@kde.org>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <stdint.h>
#include "QUInt32LineEdit.h"


#define DBG    //eger tanımlı ise debug aciktir

#define SIZE_OF_CAN_VALUE_SETTINGS    17
#define SIZE_OF_CAN_FILTER_IDS        12
#define SIZE_OF_ROUTE_VALUE_SETTINGS  9

QT_BEGIN_NAMESPACE

class QLabel;
class QTimer;

namespace Ui {
class MainWindow;
}

QT_END_NAMESPACE

class Console;
class SettingsDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void openSerialPort();
    void closeSerialPort();
    void about();
    void writeData(const QByteArray &data);
    void readData();

    void handleError(QSerialPort::SerialPortError error);
    void handleBytesWritten(qint64 bytes);
    void handleWriteTimeout();

    void on_serialTxBtn_2_clicked();

    void on_testButton_clicked();

    void on_testButton_3_clicked();


    void on_Btn_canAcnfg_clicked();

    void on_Btn_canBcnfg_clicked();

    void on_Btn_canA_fltr_clicked();

    void on_Btn_canB_fltr_clicked();

    void on_btnRoute1_clicked();

    void on_btnRoute2_clicked();

    void on_pushButton_clicked();

private:
    void initActionsConnections();

private:
    void showStatusMessage(const QString &message);
    void showWriteError(const QString &message);

    uint16_t Calculate_Crc_16(unsigned char* data, int length);
    uint32_t Calculate_Crc_32(uint32_t* data, int length);


    Ui::MainWindow *m_ui = nullptr;
    QLabel *m_status = nullptr;
    Console *m_console = nullptr;
    SettingsDialog *m_settings = nullptr;
    qint64 m_bytesToWrite = 0;
    QTimer *m_timer = nullptr;
    QSerialPort *m_serial = nullptr;

    QUInt32LineEdit *u32LEdit_CanA_Crc_H = new QUInt32LineEdit(this);
    QUInt32LineEdit *u32LEdit_CanA_Crc_L = new QUInt32LineEdit(this);
    QUInt32LineEdit *u32LEdit_CanB_Crc_H = new QUInt32LineEdit(this);
    QUInt32LineEdit *u32LEdit_CanB_Crc_L = new QUInt32LineEdit(this);

public:

};

#endif // MAINWINDOW_H
