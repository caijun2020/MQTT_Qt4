#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTimer>
#include <QSettings>
#include "qmqtt.h"
#include "FileLog.h"
#include <stdint.h>


namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private slots:
    void on_pbConnectToHost_clicked();

    void on_pcSubscribeTopic_clicked();

    void on_pbUnsubscribeTopic_clicked();

    void on_pbClearRecv_clicked();

    void on_pbClearPublish_clicked();

    void on_pbPublishTopic_clicked();

    void slot_mqtt_recvived(const QMQTT::Message& msg);

    void updateUI();
    void autoPublishData();
    void updateConnectionStatus();
    void on_checkBox_autoSend_clicked(bool checked);

    void on_pushButton_reset_clicked();

private:
    Ui::Widget *ui;
    QMQTT::Client* m_client;
    QTimer* m_timer1s;
    QTimer* m_autoSendTimer;   // Timer used to auto send data

    QSettings *currentSetting;  // Store current setting with ini file
    QString widgetFontType; // Store the font type of widget
    int widgetFontSize;     // Store the font size of widget

    FileLog *logFile;   // Log File
    QString logPath;    // Log Path

    QString m_serverIP;
    int m_serverPort;
    QString m_clientID;
    QString m_userName;
    QString m_password;
    int m_keepAliveSecond;

    uint32_t txPacketCnt;
    uint32_t rxPacketCnt;
    uint32_t txTotalBytesSize;
    uint32_t rxTotalBytesSize;

    // Widget close action
    void closeAction();

    void initWidgetFont();  // Init the Font type and size of the widget
    void initWidgetStyle(); // Init Icon of the widget

    void loadSettingFromIniFile();  // Load setting from ini file

    // Update Log to file
    void updateLogData(QString logStr);

    // Reset pub/sub count
    void resetPubSubCnt();

    QString get_cfg_str(const QString& str);
};

#endif // WIDGET_H
