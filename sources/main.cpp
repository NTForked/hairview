#include <QtWidgets/qapplication.h>
#include <QtGui/qsurfaceformat.h>

#include "maingui.h"

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    
    QSurfaceFormat format = QSurfaceFormat::defaultFormat();
    format.setVersion(3, 3);
    format.setOption(QSurfaceFormat::DeprecatedFunctions, false);
    format.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(format);
    
    MainGui gui;
    gui.resize(800, 600);
    gui.show();

    return app.exec();
}
