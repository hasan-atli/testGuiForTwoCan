QT += widgets serialport
requires(qtConfig(combobox))

TARGET = terminal
TEMPLATE = app

SOURCES += \
    QUInt32LineEdit.cpp \
    main.cpp \
    mainwindow.cpp \
    settingsdialog.cpp \
    console.cpp

HEADERS += \
    QUInt32LineEdit.h \
    ca_serial.h \
    canMsg.h \
    mainwindow.h \
    settingsdialog.h \
    console.h \
    torkTypes.h

FORMS += \
    mainwindow.ui \
    settingsdialog.ui

RESOURCES += \
    terminal.qrc

target.path = $$[QT_INSTALL_EXAMPLES]/serialport/terminal
INSTALLS += target
