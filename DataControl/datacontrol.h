#pragma once

#include <string.h>
#include <stdio.h>

class DataControl{
public:
    typedef struct _StructClientToServerInitParam{
        char numJoint, numDof, module;
    }StructClientToServerInitParam;

    typedef struct _StructClientToServer{

    }StructClientToServer;
    typedef struct _StructServerToClient{

    }StructServerToClient;
    DataControl();
    void DataReset();
    StructClientToServerInitParam ClientToServerInitParam;
    StructClientToServer ClientToServer;
    StructServerToClient ServerToClient;
};
