#include <QApplication>
#include "utilities/CSVHelper.h"
#include "emulation/definitions.h"
#include "windows/MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qRegisterMetaType<IMUDataU>("IMUDataU");
    qRegisterMetaType<PressureDataU>("PressureDataU");
    qRegisterMetaType<FlightEventStatus>("FlightEventStatus");
    MainWindow w;
    w.show();

    return QApplication::exec();
}
