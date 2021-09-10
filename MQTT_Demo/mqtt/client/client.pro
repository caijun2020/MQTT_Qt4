TEMPLATE = app
TARGET = qmqtt_client

QT += core gui network

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets
    QT += mqtt
} else {
    DEFINES += QMQTT_EXAMPLE
    INCLUDEPATH += ../../src/mqtt
    LIBS += -L../../src/mqtt/release -lqmqtt
}

target.path = $$[QT_INSTALL_EXAMPLES]/mqtt/client
INSTALLS += target

SOURCES += \
        FileLog.cpp \
        main.cpp \
        widget.cpp

HEADERS += \
        FileLog.h \
        QtBaseType.h \
        widget.h

FORMS += \
        widget.ui

CONFIG += mobility
MOBILITY = 


RESOURCES +=

RC_FILE = icon.rc
