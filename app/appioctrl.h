/*******************************************************************************
 * Copyright [2018] <青岛艾普智能仪器有限公司>
 * All rights reserved.
 *
 * version:     0.1
 * author:      zhaonanlin
 * brief:       一代IO板调试工具
*******************************************************************************/
#ifndef APPIOCTRL_H
#define APPIOCTRL_H

#include <QFile>
#include <QTimer>
#include <QDebug>
#include <QLabel>
#include <QObject>
#include <QWidget>
#include <QLayout>
#include <QDateTime>
#include <QListView>
#include <QComboBox>
#include <QSettings>
#include <QDataStream>
#include <QToolButton>
#include <QPushButton>
#include <QMainWindow>
#include <QApplication>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

#ifndef __linux__
#include <qt_windows.h>
#endif

class AppIoctrl : public QMainWindow
{
    Q_OBJECT

public:
    explicit AppIoctrl(QWidget *parent = 0);
    ~AppIoctrl();

private slots:
    void initUI();
    void initSkin();
    void initPort();
    void initTitle();
    void initTimer();
    void readButton();
    bool thread();
private:
    QList<QToolButton*> btns;
    QList<QLabel*> leds;
    QSerialPort *com;
    QComboBox *coms;
    QTimer *timer;
    quint16 io_hex;
};

#endif // APPIOCTRL_H
