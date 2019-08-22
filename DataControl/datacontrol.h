#pragma once

#include <string.h>
#include <stdio.h>

#include "Dynamixel/H42_20_S300_RA.h"
#include "Dynamixel/XH_540_Series.h"

class DataControl{
public:
    typedef struct _StructClientToServerInitParam{
        char numJoint, numDof, module;
    }StructClientToServerInitParam;

    typedef struct _StructClientToServer{
        char opMode, subMode, ctrlMode;
        double *desiredJoint, *desiredCartesian;
    }StructClientToServer;
    typedef struct _StructServerToClient{
        unsigned char data_index;
        int *presentJoint;//, *presentCartesian;
    }StructServerToClient;

    int opMode, numJoint, numDof, module;

    DataControl();
    ~DataControl();
    void DataReset();
    StructClientToServerInitParam ClientToServerInitParam;
    StructClientToServer ClientToServer;
    StructServerToClient ServerToClient;

    void ENC2DEG_SEA(signed int *enc_pos, double* deg_pos, int size);
    void ENC2DEG_FAR(signed int *enc_pos, double* deg_pos, int size);
};
