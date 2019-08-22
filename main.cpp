#include <QCoreApplication>
#include "Input/keyinput.h"
#include "RobotController/robotcontroller.h"

static KeyInput *keyInput;
static RobotController *robotController;

void keyInputSlot(char key){
//        printf("Pressed Key : %d\n", static_cast<int>(key));
    if (key == 'q' || key == 27){
        qDebug() << "Pressed 'q' or 'ESC'";
        delete robotController;
        delete keyInput;
        exit(0);
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    keyInput = new KeyInput();
    QObject::connect(keyInput, &KeyInput::KeyPressed, keyInputSlot);
    keyInput->start();

    robotController = new RobotController();
    robotController->start();

    return a.exec();
}


