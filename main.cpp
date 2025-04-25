#include <QApplication>
#include "mainWindow.h"
#include "GlobalSettings.h"


int main(int argc, char* argv[]){
    QApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/images/put.png"));

    QCoreApplication::setOrganizationName("putPoznan");
    QCoreApplication::setApplicationName("GIOS_API_GUI");

    GlobalSettings::instance();

    MainWindow window;
    window.show();
    return app.exec();
}