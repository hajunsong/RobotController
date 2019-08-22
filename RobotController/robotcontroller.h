#ifndef CONTROLLERMAIN_H
#define CONTROLLERMAIN_H

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>
#include <native/task.h>
#include <native/timer.h>
#include <rtdk.h>

#include <iostream>
#include <vector>

#include <QTimer>
#include <QObject>
#include <QDebug>
#include <QThread>

#include "TcpServer/tcpserver.h"
#include "DataControl/datacontrol.h"
#include "Dynamixel/H42_20_S300_RA.h"
#include "Dynamixel/XH_540_Series.h"

class RobotController : public QObject{
    Q_OBJECT

    enum Status{ServoOn = 0, Initialize, Wait, JointMove, CartesianMove};
    enum Motion{JogMotion = 0, JointMotion, CartesianJogMotion, CartesianMotion};
    enum Mode{PositionMode = 0, VelocityMode, TorqueMode};
    enum Module{SEA=1, FAR, JS_R8};
    enum Comm{RS485=1, RS232, EtherCAT};

public:
    explicit RobotController(QObject *parent = nullptr);
    ~RobotController();
    void start();

private:
    void robot_RT();
    static void robot_run(void *arg);

    NRMKHelper::TcpServer *tcpServer;
    DataControl *dataControl;

    RT_TASK robot_task;
    bool robot_thread_run;

    QTimer *dxlTimer;
    SEA::DxlControl *Module_SEA;
    FAR::DxlControl *Module_FAR;
    void moduleInitSEA();
    void moduleInitFAR();
    bool module_init;

public slots:
    void dxlTimeout();

public slots:
};

#endif
