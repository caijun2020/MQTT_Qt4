#include "MqttClientWidget.h"
#include <QApplication>
#include <QTextCodec>
#include <QObject>

#define USE_SYS_QTEXTCODEC

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

#ifdef USE_SYS_QTEXTCODEC
    QTextCodec *codec = QTextCodec::codecForName("UTF8");
    QTextCodec::setCodecForTr(codec);

#if 1
    QTextCodec::setCodecForLocale(codec);
    QTextCodec::setCodecForCStrings(codec);
#else
    QTextCodec::setCodecForLocale(QTextCodec::codecForLocale());
    QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());
#endif

#endif

    MqttClientWidget w;
    //w.setWindowTitle(QObject::tr("MQTT Client"));
    w.show();

    return a.exec();
}
