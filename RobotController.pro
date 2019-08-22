QT -= gui
QT += network core

CONFIG += c++11 console
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
#DEFINES += QT_DEPRECATED_WARNINGS


# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        Dynamixel/H42_20_S300_RA.cpp \
        Dynamixel/XH_540_Series.cpp \
        Input/keyinput.cpp \
        RobotController/robotcontroller.cpp \
        TcpServer/tcpserver.cpp \
        DataControl/datacontrol.cpp \
        main.cpp

HEADERS += \
        Dynamixel/H42_20_S300_RA.h \
        Dynamixel/XH_540_Series.h \
        Input/keyinput.h \
        RobotController/robotcontroller.h \
        DataControl/datacontrol.h \
        TcpServer/tcpserver.h \

NRMK_LIB_PATH = $$PWD/../../dev/lib/NRMK
DXL_LIB_PATH = $$PWD/Dynamixel/lib

INCLUDEPATH += $$NRMK_LIB_PATH/xenomai/include\
                $$NRMK_LIB_PATH\
                $$NRMK_LIB_PATH/core/3rdparty/Poco/lib/i686\
                $$NRMK_LIB_PATH/core/3rdparty/Poco/include\
                $$NRMK_LIB_PATH/helper/include\

DEPENDPATH += $$NRMK_LIB_PATH/xenomai/include\
                $$NRMK_LIB_PATH\
                $$NRMK_LIB_PATH/core/3rdparty/Poco/lib/i686\
                $$NRMK_LIB_PATH/core/3rdparty/Poco/include\
                $$NRMK_LIB_PATH/helper/include\

LIBS += -L$$NRMK_LIB_PATH/xenomai/lib/ -lnative\
        -L$$NRMK_LIB_PATH/xenomai/lib/ -lxenomai\
        -L$$NRMK_LIB_PATH/xenomai/lib/ -lrtdm\
        -L$$NRMK_LIB_PATH/lib/ -lNRMKHelperi686\
        -L$$NRMK_LIB_PATH/core/3rdparty/Poco/lib/i686/ -lPocoFoundation\
        -L$$NRMK_LIB_PATH/core/3rdparty/Poco/lib/i686/ -lPocoNet\
        -L$$DXL_LIB_PATH -ldxl_x86_cpp

PRE_TARGETDEPS += $$NRMK_LIB_PATH/lib/libNRMKHelperi686.a

target.path = /home/user/Project/RobotController
INSTALLS += target
