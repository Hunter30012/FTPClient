#include "client_controller.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    qputenv("QT_QPA_PLATFORM", "windows:darkmode=0");

    ClientController controller(argc, argv);
    return controller.init();
}
