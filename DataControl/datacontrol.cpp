#include "datacontrol.h"

DataControl::DataControl()
{
    memset(&ClientToServerInitParam, 0, sizeof(StructClientToServerInitParam));
    memset(&ClientToServer, 0, sizeof(StructClientToServer));
    memset(&ServerToClient, 0, sizeof(StructClientToServer));
}

void DataControl::DataReset()
{
    memset(&ClientToServerInitParam, 0, sizeof(StructClientToServerInitParam));
    memset(&ClientToServer, 0, sizeof(StructClientToServer));
    memset(&ServerToClient, 0, sizeof(StructClientToServer));
}
