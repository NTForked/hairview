#include "maingui.h"

#include "oglwidget.h"

#include <QtWidgets/qgroupbox.h>
#include <QtWidgets/qradiobutton.h>
#include <QtWidgets/qmenubar.h>
#include <QtWidgets/qfiledialog.h>
#include <QtWidgets/qradiobutton.h>

#include "common.h"
#include "hairloader.h"

class MainGui::Ui : public QWidget {
public:
    explicit Ui(QWidget* parent = nullptr)
        : QWidget{ parent } {
        layout = new QVBoxLayout(this);
        layout->setAlignment(Qt::AlignTop);
        this->setLayout(layout);

        radiobox = new QGroupBox("Methods", this);
        radioLayout = new QVBoxLayout(radiobox);
        radiobox->setLayout(radioLayout);
        layout->addWidget(radiobox);

        kajiyaRadio = new QRadioButton("Kajiya-Kay", this);
        kajiyaRadio->setChecked(true);
        radioLayout->addWidget(kajiyaRadio);
        marschnerRadio = new QRadioButton("Marschner", this);
        radioLayout->addWidget(marschnerRadio);
    }

    ~Ui() {
        delete layout;
    }

    QVBoxLayout* layout = nullptr;
    QGroupBox* radiobox = nullptr;
    QVBoxLayout* radioLayout = nullptr;
    
    QRadioButton* kajiyaRadio = nullptr;
    QRadioButton* marschnerRadio = nullptr;
};

MainGui::MainGui(QWidget* parent)
    : QMainWindow{ parent } {
    setFont(QFont("Meiryo UI"));

    widget = new QWidget(this);
    layout = new QGridLayout(widget);
    widget->setLayout(layout);
    setCentralWidget(widget);
    
    ui = new Ui(this);
    layout->addWidget(ui, 0, 1);
    layout->setColumnStretch(1, 1);

    view = new OGLWidget(this);
    layout->setColumnStretch(0, 4);
    layout->addWidget(view, 0, 0);

    fileMenu = menuBar()->addMenu(tr("&File"));
    
    openAct = new QAction(tr("&Open"), this);
    openAct->setShortcut(tr("Ctrl+O"));
    fileMenu->addAction(openAct);
    connect(openAct, SIGNAL(triggered()), this, SLOT(OnOpenActTriggered()));

    saveAct = new QAction(tr("&Save"), this);
    saveAct->setShortcut(tr("Ctrl+S"));
    fileMenu->addAction(saveAct);
    connect(saveAct, SIGNAL(triggered()), this, SLOT(OnSaveActTriggered()));

    exportAct = new QAction(tr("&Export"), this);
    exportAct->setShortcut(tr("Ctrl+E"));
    fileMenu->addAction(exportAct);
    connect(exportAct, SIGNAL(triggered()), this, SLOT(OnExportActTriggered()));

    connect(ui->kajiyaRadio, SIGNAL(toggled(bool)), this, SLOT(OnMethodChanged(bool)));
    connect(ui->marschnerRadio, SIGNAL(toggled(bool)), this, SLOT(OnMethodChanged(bool)));
}

MainGui::~MainGui() {
    delete ui;
    delete view;
    delete layout;
}

void MainGui::OnOpenActTriggered() {
    QString filename = QFileDialog::getOpenFileName(this,
        "Open", QString(DATA_DIRECTORY), "Hair data (*.hair)");
    if (filename == "") return;

    view->setHairModel(filename);
}

void MainGui::OnSaveActTriggered() {
}

void MainGui::OnExportActTriggered() {
}

void MainGui::OnMethodChanged(bool unused) {
    if (ui->kajiyaRadio->isChecked()) {
        view->setMethod(RefModel::KajiyaKay);
    } else if (ui->marschnerRadio->isChecked()) {
        view->setMethod(RefModel::Marschner);    
    }
}
