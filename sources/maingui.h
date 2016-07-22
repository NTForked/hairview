#ifdef _MSC_VER
#pragma once
#endif

#ifndef _MAINGUI_H_
#define _MAINGUI_H_

#include <QtWidgets/qmainwindow.h>
#include <QtWidgets/qgridlayout.h>

class OGLWidget;

class MainGui : public QMainWindow {
    Q_OBJECT

public:
    explicit MainGui(QWidget* parent = nullptr);
    ~MainGui();

private slots:
    void OnOpenActTriggered();
    void OnSaveActTriggered();
    void OnExportActTriggered();
    void OnMethodChanged(bool);

private:
    class Ui;
    Ui* ui = nullptr;
    OGLWidget* view = nullptr;

    QGridLayout* layout = nullptr;
    QWidget* widget = nullptr;

    QMenu* fileMenu = nullptr;
    QAction* openAct = nullptr;
    QAction* saveAct = nullptr;
    QAction* exportAct = nullptr;
};

#endif  // _MAINGUI_H_
