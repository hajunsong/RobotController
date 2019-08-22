#include "datacontrol.h"

DataControl::DataControl()
{
    memset(&ClientToServerInitParam, 0, sizeof(StructClientToServerInitParam));
    memset(&ClientToServer, 0, sizeof(StructClientToServer));
    memset(&ServerToClient, 0, sizeof(StructClientToServer));
    opMode = -1;
}

DataControl::~DataControl()
{
    if (ClientToServer.desiredJoint != nullptr){
        delete[] ClientToServer.desiredJoint;
    }
    if (ClientToServer.desiredCartesian != nullptr){
        delete[] ClientToServer.desiredCartesian;
    }
}

void DataControl::DataReset()
{
    memset(&ClientToServerInitParam, 0, sizeof(StructClientToServerInitParam));
    memset(&ClientToServer, 0, sizeof(StructClientToServer));
    memset(&ServerToClient, 0, sizeof(StructClientToServer));
}

void DataControl::ENC2DEG_SEA(signed int *enc_pos, double *deg_pos, int size)
{
    for(int i = 0; i < size; i++){
        deg_pos[i] = enc_pos[i]*SEA::POSITION_UNIT;
    }
}

void DataControl::ENC2DEG_FAR(signed int *enc_pos, double *deg_pos, int size)
{
    for(int i = 0; i < size; i++){
        deg_pos[i] = enc_pos[i]*FAR::POSITION_UNIT;
    }
}
