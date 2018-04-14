/*******************************************************************************
 * Copyright [2018] <青岛艾普智能仪器有限公司>
 * All rights reserved.
 *
 * version:     0.1
 * author:      zhaonanlin
 * brief:       一代IO板调试工具
*******************************************************************************/
#include "appioctrl.h"

AppIoctrl::AppIoctrl(QWidget *parent) : QMainWindow(parent)
{
    initUI();
    initSkin();
    initTitle();
    initTimer();
}

AppIoctrl::~AppIoctrl()
{
}

void AppIoctrl::initUI()
{
    QStringList com;
#ifndef __linux__
    QString path = "HKEY_LOCAL_MACHINE\\HARDWARE\\DEVICEMAP\\SERIALCOMM";
    QSettings *settings = new QSettings(path, QSettings::NativeFormat);
    QStringList key = settings->allKeys();
    HKEY hKey;
    int ret = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("HARDWARE\\DEVICEMAP\\SERIALCOMM"),
                             0, KEY_READ, &hKey);
    if (ret != 0) {
        qDebug() << "Cannot open regedit!";
    } else {
        for (int i=0; i < key.size(); i++) {
            wchar_t name[256];
            DWORD ikey = sizeof(name);
            char kvalue[256];
            DWORD t = sizeof(kvalue);
            DWORD type;
            QString tmpName;
            ret = ::RegEnumValue(hKey, i, LPWSTR(name), &ikey, 0, &type,
                                 reinterpret_cast<BYTE*>(kvalue), &t);
            if (ret == 0) {
                for (int j = 0; j < static_cast<int>(t); j++) {
                    if (kvalue[j] != 0x00) {
                        tmpName.append(kvalue[j]);
                    }
                }
                com << tmpName;
            }
        }
        RegCloseKey(hKey);
    }
#else
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        com << info.portName();
    }
#endif
    com.sort();
    for (int i=0; i < com.size(); i++) {
        if (com.at(i).size() > 4) {
            com.move(i, com.size()-1);
        }
    }
    coms = new QComboBox(this);
    coms->setView(new QListView);
    coms->setMinimumSize(97, 40);
    coms->addItems(com);

    QPushButton *pbtn = new QPushButton(tr("打开串口"), this);
    pbtn->setMinimumSize(97, 44);
    connect(pbtn, SIGNAL(clicked(bool)), this, SLOT(initPort()));

    QHBoxLayout *com_layout = new QHBoxLayout;
    com_layout->addWidget(new QLabel(tr("串口名称:"), this));
    com_layout->addWidget(coms);
    com_layout->addWidget(pbtn);
    com_layout->addStretch();

    QGridLayout *btn_layout = new QGridLayout;

    for (int i=0; i < 16; i++) {
        btns.append(new QToolButton(this));
        btns.at(i)->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        btns.at(i)->setCheckable(true);
        btns.at(i)->setText(tr("Y%1").arg((i >= 8) ? (i%8+10) : i));
        btns.at(i)->setStyleSheet("border-image:url(:/icon_circle_5.png)");
        btn_layout->addWidget(btns.at(i), i/8, i%8);
        connect(btns.at(i), SIGNAL(clicked(bool)), this, SLOT(readButton()));
    }

    QGridLayout *led_layout = new QGridLayout;

    for (int i=0; i < 16; i++) {
        leds.append(new QLabel(this));
        leds.at(i)->setText(tr("X%1").arg((i >= 8) ? (i%8+10) : i));
        leds.at(i)->setAlignment(Qt::AlignCenter);
        leds.at(i)->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        leds.at(i)->setStyleSheet("border-image:url(:/icon_circle_5.png)");
        led_layout->addWidget(leds.at(i), i/8, i%8);
    }

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addLayout(com_layout);
    layout->addLayout(btn_layout);
    layout->addLayout(led_layout);
    layout->addWidget(new QLabel(tr("一代IO板调试工具 by link"), this));

    QFrame *frame = new QFrame(this);
    frame->setLayout(layout);
    this->setCentralWidget(frame);
    this->resize(800, 600);
}

void AppIoctrl::initSkin()
{
    QFile file;
    QString qss;
    file.setFileName(":/qss_black.css");
    file.open(QFile::ReadOnly);
    qss = QLatin1String(file.readAll());
    qApp->setStyleSheet(qss);
}

void AppIoctrl::initPort()
{
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if (btn->text() == tr("打开串口")) {
        com = new QSerialPort(coms->currentText(), this);
        if (com->open(QIODevice::ReadWrite)) {
            com->setBaudRate(9600);
            com->setParity(QSerialPort::NoParity);
            com->setDataBits(QSerialPort::Data8);
            com->setStopBits(QSerialPort::OneStop);
            com->setFlowControl(QSerialPort::NoFlowControl);
            com->setDataTerminalReady(true);
            com->setRequestToSend(false);
            btn->setText(tr("关闭串口"));
            timer->start(50);
        }
    } else {
        btn->setText(tr("打开串口"));
        com->close();
        timer->stop();
    }
}

void AppIoctrl::initTitle()
{
    char s_month[5];
    static const char month_names[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
    int month, day, year;

    sscanf((__DATE__), "%s %d %d", s_month, &day, &year);
    month = (strstr(month_names, s_month)-month_names)/3+1;

    QDate dt;
    dt.setDate(year, month, day);
    static const QTime tt = QTime::fromString(__TIME__, "hh:mm:ss");

    QDateTime t(dt, tt);
    QString verNumb = QString("V-0.1.%1").arg(t.toString("yyMMdd-hhmm"));

    this->setWindowTitle(tr("一代IO板调试工具%1").arg(verNumb));
}

void AppIoctrl::initTimer()
{
    com = NULL;
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(thread()));
}

void AppIoctrl::readButton()
{
    if (com == NULL)
        return;
    if (!com->isOpen())
        return;
    QToolButton *btn = qobject_cast<QToolButton*>(sender());
    if (btn->isChecked()) {
        btn->setStyleSheet("border-image:url(:/icon_circle_6.png)");
    } else {
        btn->setStyleSheet("border-image:url(:/icon_circle_5.png)");
    }
    quint16 hex = 0x00;
    for (int i=0; i < btns.size(); i++) {
        if (btns.at(i)->isChecked())
            hex += quint16(0x0001 << i);
    }
    quint8 crc = 0x00;
    QByteArray msg;
    QDataStream out(&msg, QIODevice::ReadWrite);
    out << quint8(0x7B) << quint8(0x00) << quint8(0xF2)
        << quint8(hex/256) << quint8(hex%256)
        << quint8(crc) << quint8(0x7D);
    out.device()->seek(1);
    out << quint8(msg.size());
    out.device()->seek(msg.size()-2);
    for (int i=1; i < msg.size()-2; i++)
        crc += quint8(msg.at(i));
    out << quint8(crc);
    com->write(msg);
}

bool AppIoctrl::thread()
{
    if (com == NULL || !com->isOpen())
        return false;
    if (com->bytesAvailable() > 7) {
        QByteArray msg = com->readAll();
        io_hex = quint16(msg.at(4)*256) + quint8(msg.at(3));
        for (int i=0; i < 16; i++) {
            if (io_hex & (0x0001 << i))
                leds.at(i)->setStyleSheet("border-image:url(:/icon_circle_6.png)");
            else
                leds.at(i)->setStyleSheet("border-image:url(:/icon_circle_5.png)");
        }
        return true;
    }
    com->write(QByteArray::fromHex("7B06F100F77D"));  // 读取IO板状态
    return true;
}
