#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>		// string function definitions
#include <fcntl.h>		// File control definitions
#include <errno.h>		// Error number definitions
#include <termios.h>	// POSIX terminal control definitions
#include <time.h>		// time calls
#include <sys/ioctl.h>
#include <math.h>

//-xenomai-///////////////////////////////////////////////////////////////
#include <native/task.h>
#include <native/timer.h>
#include <native/mutex.h>
#include <rtdk.h>		//The rdtk real-time printing library
/****************************************************************************/

#include <Poco/Event.h>
#include <NRMKSocketBase.h>

#include <QString>
#include <QDebug>

#include "DataControl/datacontrol.h"

namespace NRMKHelper{
    class TcpServer : public NRMKSocketBase
    {
    public:
        TcpServer(DataControl *dataControl_);
        enum{
            NUM_JOINT = 1,
            NUM_DOF = 1,
            OP_MODE_SIZE = 1,
            SUB_CASE_SIZE = 1,
            CONTROL_MODE_SIZE = 1,
            CURRENT_JOINT_SIZE = 4,
            CURRENT_CARTESIAN_SIZE = 4,
            DESIRED_JOINT_SIZE = 4,
            DESIRED_CARTESIAN_SIZE = 4,
            EMPTY_SIZE = 4,
            DATA_CORRECTED_SIZE = 1,
        };
        enum Module{Dxl_SEA, Dxl_FAR, JS_R8};
        enum Comm{RS485, EtherCAT, RS232};
        enum Baud{B_57600, B_1M, B_2M, B_3M, B_4M};
//        enum{
//            DATA_SIZE = NRMK_SOCKET_TOKEN_SIZE + OP_MODE_SIZE + SUB_CASE_SIZE + CONTROL_MODE_SIZE +
//                CURRENT_JOINT_SIZE*NUM_JOINT + CURRENT_CARTESIAN_SIZE*NUM_DOF +
//                DESIRED_JOINT_SIZE*NUM_JOINT + DESIRED_CARTESIAN_SIZE*NUM_DOF +
//                EMPTY_SIZE*EMPTY_DATA_CNT + DATA_CORRECTED_SIZE + NRMK_SOCKET_TOKEN_SIZE
//        };
    public:
        TcpServer();
        ~TcpServer();
        void setting(QString _ip, int _port);
        void OnEvent(UINT uEvent, LPVOID lpvData);
        // sendKey is used to send key input only
        void sendKey(char key);
        void getKey(char & key);
        bool isConnected();
        void OnDataReceived(const LPBYTE lpBuffer, DWORD dwCount);
        int getPort(){return PORT;}
        QString getIP(){return IP;}
        bool getDataCorrected(){return data_corrected;}
        void getCmdBuf(unsigned char* CmdBuf);

    private:
        QString IP;
        int PORT;
        volatile char commandkey;
        Poco::Event dataReceiveEvent;
        volatile bool connected;

        RT_TASK comm_task;
        static void comm_run(void *arg);
        bool comm_thread_run;
        unsigned char *cmdbuf;
        bool data_corrected;
        DataControl *dataControl;

        void sendData();
    };
}
