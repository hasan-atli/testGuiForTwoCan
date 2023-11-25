// Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
// Copyright (C) 2012 Laszlo Papp <lpapp@kde.org>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "mainwindow.h"

#include <QApplication>
#include <QSpinBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    #ifndef DBG
    //w.setFixedSize(1525, 900); // Genişlik: 850, Yükseklik: 600
    #endif

    w.showMaximized();
    w.show();
    return a.exec();
}
