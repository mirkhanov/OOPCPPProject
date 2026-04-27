#include <QApplication>
#include <QDir>
#include <QStandardPaths>
#include "core/ClinicService.h"
#include "gui/MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // On macOS the .app bundle runs from inside Contents/MacOS — go up 3 levels to reach vpms/
#ifdef Q_OS_MAC
    QDir::setCurrent(QCoreApplication::applicationDirPath() + "/../../..");
#endif

    QDir().mkpath("data");

    ClinicService service;
    MainWindow window(service);
    window.show();

    return app.exec();
}
