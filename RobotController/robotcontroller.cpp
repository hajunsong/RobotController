#include "robotcontroller.h"

RobotController::RobotController(QObject *parent) : QObject(parent){
    dataControl = new DataControl();
    tcpServer = new NRMKHelper::TcpServer(dataControl);
    module_init = false;
    dxlTimer = new QTimer();
    connect(dxlTimer, SIGNAL(timeout()), this, SLOT(dxlTimeout()));
    dxlTimer->setInterval(1000);

    Module_SEA = new SEA::DxlControl();
    Module_FAR = new FAR::DxlControl();

    data_index = 0;
}

RobotController::~RobotController(){
    qDebug() << "finished program\n";

    delete tcpServer;
    delete dataControl;

    if (robot_thread_run){
        rt_task_suspend(&robot_task);
        QThread::msleep(100);
        rt_task_delete(&robot_task);
        QThread::msleep(100);
        qDebug() << "Robot Control RT Task Stop";
    }
    else{
        qDebug() << "Robot Control RT Thread not running...";
    }

    delete dxlTimer;

    delete Module_SEA;
    for(uint8_t i = 0; i < dataControl->numJoint; i++){
        Module_FAR->dxl_deinit(i);
    }
    delete Module_FAR;
}

void RobotController::start()
{
    qDebug() << "Start RobotController";

    tcpServer->setting("192.168.137.100", 5050);
    if (tcpServer->startServer(SOCK_TCP, tcpServer->getPort())){
        qDebug() << "Control server started at IP : " << tcpServer->getIP() << ", Port : " << tcpServer->getPort();
    }

    qDebug() << "Waiting for Control Tool to connect...";
    tcpServer->waitForConnection(0);

    dxlTimer->start();
//    robot_RT();
}

void RobotController::dxlTimeout(){
//    int32_t present_position = 0;
//    int32_t present_position[6] = {0,};
    switch(dataControl->ClientToServerInitParam.module){
    case Module::SEA:
        if (!module_init){
            dxlTimer->stop();
            Module_SEA->init();
            moduleInitSEA();
        }
        break;
    case Module::FAR:
        if (!module_init){
            dxlTimer->stop();
            Module_FAR->init();
            moduleInitFAR();
        }
        break;
    case Module::JS_R8:
        break;
    default:
        break;
    }
}

void RobotController::robot_RT(){
    rt_print_auto_init(1);
    /* Avoids memory swapping for this program */
    mlockall(MCL_CURRENT|MCL_FUTURE);

    int ret = rt_task_create(&robot_task, "Robot Controll Task", 0, 50, T_FPU|T_CPU(2));
    if (ret < 0){
        qDebug() << "Failed to create Robot Control Task : " << strerror(-ret);
    }
    else{
        qDebug() << "Robot Control RT Task Start";
        dataControl->opMode = Status::Wait;
        rt_task_start(&robot_task, &robot_run, this);
    }
}

void RobotController::robot_run(void *arg){
    RTIME now, previous, dxl_now, dxl_pre;
    previous = rt_timer_read();
    RobotController* pThis = static_cast<RobotController*>(arg);

    pThis->robot_thread_run = false;

    rt_task_set_periodic(&pThis->robot_task, TM_NOW, 50e6);

    int32_t present_position[6] = {0,};

    while(1){
        rt_task_wait_period(nullptr); //wait for next cycle
        if (pThis->tcpServer->isConnected()){
            pThis->robot_thread_run = true;
            now = rt_timer_read();

            switch(pThis->dataControl->opMode){
            case Status::ServoOn:
                pThis->robotServo(1);
                break;
            case Status::Initialize:
                break;
            case Status::Wait :
                if (pThis->dataControl->module == Module::FAR){
                    pThis->Module_FAR->getGroupSyncReadPresentPosition(present_position);
                }
                else if(pThis->dataControl->module == Module::SEA){
                    present_position[0] = pThis->Module_SEA->getPresentPosition(1);
                }

                pThis->robotKinematics();
                pThis->robotDynamics();

                break;
            case Status::JointMove:
                break;
            case Status::CartesianMove:
                break;
            default : break;
            }

            rt_printf("Present Position : %d, %d, %d, %d, %d, %d\n",
                      present_position[0], present_position[1], present_position[2], present_position[3], present_position[4], present_position[5]);
//            rt_printf("Comm : Time since last turn: %ld.%06ld ms\n",
//                      static_cast<unsigned long>(now - previous) / 1000000,
//                      static_cast<unsigned long>(now - previous) % 1000000);

            pThis->dataControl->ServerToClient.data_index = pThis->data_index;
            memcpy(pThis->dataControl->ServerToClient.presentJoint, present_position, sizeof(int32_t)*6);
//            if (pThis->dataControl->module == Module::FAR){
//                pThis->dataControl->ENC2DEG_FAR(present_position, pThis->dataControl->ServerToClient.presentJoint, pThis->dataControl->numJoint);
//            }

            previous = now;
            pThis->data_index++;
        }
    }
}

void RobotController::moduleInitSEA(){
    qDebug() << "Start SEA Module Initilization";
//    for(uint8_t i = 0; i < static_cast<uint8_t>(dataControl->ClientToServerInitParam.numJoint); i++){
//        Module_SEA->dxl_init(i+1);
//    }
    while(!module_init){
        int init_result = Module_SEA->dxl_init(1);
        if (init_result){
            module_init = true;
        }
    }
//    dxlTimer->start();
    robot_RT();
}

void RobotController::moduleInitFAR(){
    qDebug() << "Start FAR Module Initilization";
    uint8_t module_indx = 0;
    while(!module_init){
        int init_result = Module_FAR->dxl_init(module_indx);
        if (init_result){
            int32_t pos = Module_FAR->getPresentPosition(module_indx);
            qDebug() << module_indx << " axis present position : " << pos;
            if (pos != 0)
            {
                module_indx++;
            }
        }
        if (module_indx >= dataControl->ClientToServerInitParam.numJoint){
            module_init = true;
        }
    }
//    dxlTimer->start();
    robot_RT();
}

void RobotController::robotServo(int state){

}

void RobotController::robotKinematics(){

}

void RobotController::robotDynamics(){

}
