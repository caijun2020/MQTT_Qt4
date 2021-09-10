#include "widget.h"
#include "ui_widget.h"
#include <QMessageBox>
#include <QFile>
#include <QDateTime>
#include "QtBaseType.h"


Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget),
    widgetFontType("Arial"),
    widgetFontSize(16),
    logFile(new FileLog),
    logPath("./Log/"),
    m_serverIP("127.0.0.1"),
    m_serverPort(1883),
    m_clientID("mqtt_client001"),
    m_userName(""),
    m_password(""),
    m_keepAliveSecond(120)
{
    ui->setupUi(this);

    // Default setting file
    currentSetting = new QSettings("config.ini", QSettings::IniFormat);

    // Load Settings from ini file
    loadSettingFromIniFile();

    // Init Widget Style
    initWidgetStyle();

    // Init Widget Font type and size
    initWidgetFont();

    m_client = new QMQTT::Client(QHostAddress::LocalHost, 1883, this);
    connect(m_client, SIGNAL(received(QMQTT::Message)), this, SLOT(slot_mqtt_recvived(QMQTT::Message)));

    m_timer1s = new QTimer(this);
    m_timer1s->start(1000);
    connect(m_timer1s, SIGNAL(timeout()), this, SLOT(updateUI()));

    m_autoSendTimer = new QTimer(this);
    connect(m_autoSendTimer, SIGNAL(timeout()), this, SLOT(autoPublishData()));

    // Reset count
    resetPubSubCnt();
}

Widget::~Widget()
{
    if(m_client->isConnectedToHost())
    {
        for(int i = 0; i != ui->cbxSubscribedTopic->count(); i++)
        {
            m_client->unsubscribe(ui->cbxSubscribedTopic->itemText(i));
        }
        //m_client->disconnected();
        m_client->disconnectFromHost();
    }

    delete ui;
    delete currentSetting;
    delete m_client;
    delete logFile;
    delete m_timer1s;
    delete m_autoSendTimer;
}

void Widget::initWidgetFont()
{
}

void Widget::initWidgetStyle()
{
    ui->cbxQosSubscribe->addItem("0");
    ui->cbxQosSubscribe->addItem("1");
    ui->cbxQosSubscribe->addItem("2");

    ui->cbxQosPublish->addItem("0");
    ui->cbxQosPublish->addItem("1");
    ui->cbxQosPublish->addItem("2");

    ui->spinBox_autoSendInterval->setRange(1, 1000000);
    ui->spinBox_autoSendInterval->setValue(1000);

    // Update Status Color
    ui->label_status->setStyleSheet(BG_COLOR_RED);
    ui->label_status->setText("");
}

void Widget::loadSettingFromIniFile()
{
    // Load Font type and size
    currentSetting->beginGroup("SystemSetting");

    if(currentSetting->contains("FontType"))
    {
        widgetFontType = currentSetting->value("FontType").toString();
    }
    else
    {
        // Init the default value
        currentSetting->setValue("FontType", widgetFontType);
    }

    if(currentSetting->contains("FontSize"))
    {
        widgetFontSize = currentSetting->value("FontSize").toInt();
    }
    else
    {
        // Init the default value
        currentSetting->setValue("FontSize", widgetFontSize);
    }

    if(currentSetting->contains("LogPath"))
    {
        // Load Log Path
        logPath = currentSetting->value("LogPath").toString();
    }
    else
    {
        // Init the default value
        currentSetting->setValue("LogPath", logPath);
    }
    // Init Log File Path
    logFile->setLogPath(logPath);

    currentSetting->endGroup();


    currentSetting->beginGroup("NetworkSetting");
    if(currentSetting->contains("ServerIP"))
    {
        // Load Server IP
        m_serverIP = currentSetting->value("ServerIP").toString();
    }
    else
    {
        // Init the default value
        currentSetting->setValue("ServerIP", m_serverIP);
    }
    ui->leHostaddr->setText(m_serverIP);

    if(currentSetting->contains("Port"))
    {
        // Load Server Port
        m_serverPort = currentSetting->value("Port").toInt();
    }
    else
    {
        // Init the default value
        currentSetting->setValue("Port", m_serverPort);
    }
    ui->lePort->setText(QString::number(m_serverPort));

    if(currentSetting->contains("ClientID"))
    {
        // Load Client ID
        m_clientID = currentSetting->value("ClientID").toString();
    }
    else
    {
        // Init the default value
        currentSetting->setValue("ClientID", m_clientID);
    }
    ui->leClientId->setText(m_clientID);


    if(currentSetting->contains("UserName"))
    {
        // Load User Name
        m_userName = currentSetting->value("UserName").toString();
    }
    else
    {
        // Init the default value
        currentSetting->setValue("UserName", m_userName);
    }
    ui->leUsernam->setText(m_userName);

    if(currentSetting->contains("Password"))
    {
        // Load Password
        m_password = currentSetting->value("Password").toString();
    }
    else
    {
        // Init the default value
        currentSetting->setValue("Password", m_password);
    }
    ui->lePassword->setText(m_password);

    if(currentSetting->contains("KeepAliveSecond"))
    {
        // Load keep alive second
        m_keepAliveSecond = currentSetting->value("KeepAliveSecond").toInt();
    }
    else
    {
        // Init the default value
        currentSetting->setValue("KeepAliveSecond", m_keepAliveSecond);
    }
    ui->leKeepAlive->setText(QString::number(m_keepAliveSecond));

    currentSetting->endGroup();
}

QString Widget::get_cfg_str(const QString &str)
{
    std::string rawstr = str.trimmed().toStdString();
    int index = rawstr.find("=") + 1;

    if(index > 0)
    {
        std::string cfgstr = rawstr.substr(index, rawstr.size() - 1);
        return QString(cfgstr.c_str());
    }

    return NULL;
}

void Widget::updateLogData(QString logStr)
{
    QDateTime time = QDateTime::currentDateTime();
    QString timeStr = time.toString("[yyyy-MM-dd hh:mm:ss:zzz] ");

    // Add time stamp
    logStr.prepend(timeStr);

    logFile->addLogToFile(logStr);
}

void Widget::on_pbConnectToHost_clicked()
{
    if(m_client->isConnectedToHost() == false)
    {
        connect(m_client, SIGNAL(connected()), this, SLOT(updateConnectionStatus()));

        m_client->setHostName(ui->leHostaddr->text());
        m_client->setPort(ui->lePort->text().toInt());
        m_client->setClientId(ui->leClientId->text());
        m_client->setUsername(ui->leUsernam->text());
        m_client->setPassword(QByteArray(ui->lePassword->text().toStdString().c_str()));
        //m_client->setVersion(QMQTT::V3_1_1);
        if(ui->leKeepAlive->text().size() != 0)
        {
            m_client->setKeepAlive(ui->leKeepAlive->text().toInt());
        }
        m_client->cleanSession();
        m_client->connectToHost();
    }
    else
    {
        for(int i = 0; i != ui->cbxSubscribedTopic->count(); i++)
        {
            m_client->unsubscribe(ui->cbxSubscribedTopic->itemText(i));
        }
        m_client->disconnectFromHost();
    }

    updateConnectionStatus();
}

void Widget::on_pcSubscribeTopic_clicked()
{
    if(m_client->isConnectedToHost())
    {
        m_client->subscribe(ui->leSubscribeTopic->text(), ui->cbxQosSubscribe->currentText().toUInt());

        QString topic = ui->leSubscribeTopic->text();

        if(ui->cbxSubscribedTopic->findText(topic) < 0)
        {
            ui->cbxSubscribedTopic->addItem(topic);
        }
    }
}

void Widget::on_pbUnsubscribeTopic_clicked()
{
    if(m_client->isConnectedToHost())
    {
        m_client->unsubscribe(ui->cbxSubscribedTopic->currentText());
        ui->cbxSubscribedTopic->removeItem(ui->cbxSubscribedTopic->currentIndex());
    }
}

void Widget::on_pbClearRecv_clicked()
{
    ui->teRecvData->clear();
}

void Widget::on_pbClearPublish_clicked()
{
    ui->tePublicData->clear();
}

void Widget::on_pbPublishTopic_clicked()
{
    if(m_client->isConnectedToHost())
    {
        QMQTT::Message msg(0, ui->lePublicTopic->text(), QByteArray(ui->tePublicData->toPlainText().toStdString().c_str()), ui->cbxQosPublish->currentIndex());
        m_client->publish(msg);
        txPacketCnt++;
        txTotalBytesSize += QByteArray(ui->tePublicData->toPlainText().toStdString().c_str()).length();
    }
}

void Widget::slot_mqtt_recvived(const QMQTT::Message &msg)
{
    ui->teRecvData->append("【主题】：" + msg.topic());
    ui->teRecvData->append("【数据】：" + QString(msg.payload()));
    rxPacketCnt++;
    rxTotalBytesSize += msg.payload().length();
}

void Widget::updateUI()
{
    static uint32_t lastRxPacketCnt = 0;
    static uint32_t lastTxPacketCnt = 0;

    updateConnectionStatus();

    ui->label_rxPacketCnt->setText(QString::number(rxPacketCnt));
    ui->label_txPacketCnt->setText(QString::number(txPacketCnt));
    ui->label_rxPacketCntAvg->setText(QString::number(rxPacketCnt - lastRxPacketCnt));
    ui->label_txPacketCntAvg->setText(QString::number(txPacketCnt - lastTxPacketCnt));
    ui->label_rxBytesCnt->setText(QString::number(rxTotalBytesSize));
    ui->label_txBytesCnt->setText(QString::number(txTotalBytesSize));

    lastRxPacketCnt = rxPacketCnt;
    lastTxPacketCnt = txPacketCnt;
}

void Widget::autoPublishData()
{
    on_pbPublishTopic_clicked();
}

void Widget::updateConnectionStatus()
{
    if(m_client->isConnectedToHost())
    {
        ui->pbConnectToHost->setText("断开连接");

        // Update Status Color
        ui->label_status->setStyleSheet(BG_COLOR_GREEN);
    }
    else
    {
        ui->pbConnectToHost->setText("连接");

        // Update Status Color
        ui->label_status->setStyleSheet(BG_COLOR_RED);

        ui->cbxSubscribedTopic->clear();
    }
}

void Widget::on_checkBox_autoSend_clicked(bool checked)
{
    if(checked)
    {
        m_autoSendTimer->start(ui->spinBox_autoSendInterval->value());
    }
    else
    {
        m_autoSendTimer->stop();
    }
}

void Widget::on_pushButton_reset_clicked()
{
    resetPubSubCnt();
}

void Widget::resetPubSubCnt()
{
    txPacketCnt = 0;
    rxPacketCnt = 0;
    txTotalBytesSize = 0;
    rxTotalBytesSize = 0;
}
