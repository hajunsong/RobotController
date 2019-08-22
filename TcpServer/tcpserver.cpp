#include "tcpserver.h"

NRMKHelper::TcpServer::TcpServer(DataControl *dataControl_) : NRMKSocketBase (), dataReceiveEvent(true)
{
    commandkey = 0;
    connected = false;
    comm_thread_run = false;
    data_corrected = false;
    dataControl = dataControl_;
}

NRMKHelper::TcpServer::~TcpServer()
{
    if (comm_thread_run){
        rt_task_suspend(&comm_task);
        rt_task_delete(&comm_task);
    }
    if (IsOpen()){
        StopComm();
        CloseComm();
    }
}

void NRMKHelper::TcpServer::setting(QString _ip, int _port)
{
    IP = _ip;
    PORT = _port;
}

void NRMKHelper::TcpServer::OnEvent(UINT uEvent, LPVOID lpvData)
{
    switch( uEvent )
    {
    case EVT_CONSUCCESS:
        qDebug() << "One client has connected to the Command Server...";

        rt_print_auto_init(1);
        /* Avoids memory swapping for this program */
        mlockall(MCL_CURRENT|MCL_FUTURE);

        if (!comm_thread_run){
            int ret = rt_task_create(&comm_task, "Tcp Comm Task", 0, 50, T_FPU|T_CPU(2));
            if (ret < 0){
                qDebug() << "Failed to create Tcp Comm Task : " << strerror(-ret);
            }
            else{
                qDebug() << "Tcp Comm RT Task Start";
                rt_task_start(&comm_task, &comm_run, this);
            }
        }
        else{
            qDebug() << "Tcp Comm RT Task Resume";
            rt_task_resume(&comm_task);
            dataControl->opMode = 2;
        }

        break;

    case EVT_CONDROP:
        qDebug() << "Command Server Connection abandoned ";

        if (comm_thread_run){
            rt_task_suspend(&comm_task);
            qDebug() << "Tcp Comm RT Task Stop";
        }

        connected = false;
        commandkey = 0;

        StopComm();

        if (IsServer())
            waitForConnection(0);

        dataControl->opMode = 10;

        break;

    default:
        NRMKSocketBase::OnEvent(uEvent, lpvData);
        break;
    }
}

void NRMKHelper::TcpServer::sendKey(char key)
{
    //_key = key;
//    WriteComm((LPBYTE)&key, 1, INFINITE);
    WriteComm(reinterpret_cast<LPBYTE>(&key), 1, INFINITE);
}

void NRMKHelper::TcpServer::getKey(char &key)
{
    if (connected) {
        dataReceiveEvent.wait();
        key = commandkey;
    }
    commandkey = 0;
}

bool NRMKHelper::TcpServer::isConnected()
{
    return connected;
}

void NRMKHelper::TcpServer::OnDataReceived(const LPBYTE lpBuffer, DWORD dwCount)
{
//    qDebug() << "dwCount : " << dwCount;
//    QByteArray data;
//    data.clear();
//    for(uint i = 0; i < dwCount; i++){
//        data.append(static_cast<char>(lpBuffer[i]));
//    }
//    qDebug() << "data : " << data;

    if (!connected)
    {
        connected = true;
        commandkey = static_cast<char>(lpBuffer[0]); //first byte is Key
        dataReceiveEvent.set();
        return;
    }

//    if (dwCount == 1)
//    {
//        if (connected)
//        {
//            commandkey = static_cast<char>(lpBuffer[0]); //first byte is Key
//            dataReceiveEvent.set();
//        }
//    }
//    else
    if (dwCount >= 3)
    {
        if (connected)
        {
//            if ((lpBuffer[0] != 'N') || (lpBuffer[1] != 'S'))
//                return;
//            commandkey = 1;
//            if (dwCount == DATA_SIZE){
//                cmdbuf = new uchar [dwCount - 2];
//                memcpy(cmdbuf, &lpBuffer[2], dwCount - 2);
//                data_corrected = true;
//            }
//            else{
//                data_corrected = false;
//            }

            if (lpBuffer[0] == 'N' && lpBuffer[1] == 'D'){
                memcpy(&dataControl->ClientToServerInitParam, &lpBuffer[2], sizeof(char)*(dwCount - 4));
                dataControl->numJoint = dataControl->ClientToServerInitParam.numJoint;
                dataControl->numDof = dataControl->ClientToServerInitParam.numDof;
                dataControl->module = dataControl->ClientToServerInitParam.module;
                dataControl->ClientToServer.desiredJoint = new double[dataControl->numJoint];
                dataControl->ClientToServer.desiredCartesian = new double[dataControl->numDof];
                dataControl->ServerToClient.presentJoint = new int[dataControl->numJoint];
//                qDebug() << QString::number(static_cast<char>(dataControl->numJoint));
//                qDebug() << QString::number(static_cast<char>(dataControl->numDof));
//                qDebug() << QString::number(static_cast<char>(dataControl->module));
            }
            else {
                data_corrected = false;
                return;
            }
        }
    }
    else
    {
        // wrong format packet
    }
}

void NRMKHelper::TcpServer::getCmdBuf(unsigned char* CmdBuf)
{
//    memcpy(CmdBuf, cmdbuf, sizeof(unsigned char)*DATA_SIZE);
}

void NRMKHelper::TcpServer::comm_run(void *arg){
    RTIME now, previous;
    previous = rt_timer_read();
    NRMKHelper::TcpServer* pTcpServer = static_cast<NRMKHelper::TcpServer*>(arg);
    pTcpServer->comm_thread_run = false;

    rt_task_set_periodic(&pTcpServer->comm_task, TM_NOW, 5e6);

    while(pTcpServer->connected){
        pTcpServer->comm_thread_run = true;
        rt_task_wait_period(nullptr); //wait for next cycle

        if (pTcpServer->dataControl->opMode >= 2){
            pTcpServer->sendData();
        }

//        now = rt_timer_read();

//        pTcpServer->sendKey(static_cast<char>(now-previous));


//        rt_printf("Comm : Time since last turn: %ld.%06ld ms\n",
//                  static_cast<unsigned long>(now - previous) / 1000000,
//                  static_cast<unsigned long>(now - previous) % 1000000);
//        previous = now;
    }
}

void NRMKHelper::TcpServer::sendData()
{
    uchar _buf[29];
    uint indx = 0;

    memcpy(_buf, NRMK_SOCKET_START_TOKEN, NRMK_SOCKET_TOKEN_SIZE); // 2 byte, START token
    indx += NRMK_SOCKET_TOKEN_SIZE;
    memcpy(_buf + indx, &dataControl->ServerToClient.data_index, 1); // 1 byte, data index
    indx += 1;
    memcpy(_buf + indx, dataControl->ServerToClient.presentJoint, 24); //
    indx += 24;
    memcpy(_buf + indx, NRMK_SOCKET_END_TOKEN, NRMK_SOCKET_TOKEN_SIZE); // 2 byte, END token
    indx += NRMK_SOCKET_TOKEN_SIZE;
    WriteComm(_buf, indx, INFINITE);
}
