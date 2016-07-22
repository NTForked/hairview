#include <QtWidgets/qapplication.h>

#include "maingui.h"

int main(int argc, char **argv) {
    QApplication app(argc, argv);

    MainGui gui;
    gui.resize(800, 600);
    gui.show();

    return app.exec();
}