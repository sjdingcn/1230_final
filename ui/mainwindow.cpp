#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "view.h"
#include <QGLFormat>

//#include "src/databinding.h"
#include "src/settings.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    settings.loadSettingsOrDefaults();

    ui->directionalLightR->setValue(settings.directionalLightR);
    ui->directionalLightG->setValue(settings.directionalLightG);
    ui->directionalLightB->setValue(settings.directionalLightB);
    ui->crepuscularRays->setChecked(settings.crepuscularRays);
    ui->shadowMapping->setChecked(settings.shadowMapping);
    ui->hdr->setChecked(settings.hdr);
    ui->hdrExposure->setValue(settings.hdrExposure);
//    ui->geometryShaders->setChecked(settings.geometryShaders);

    connect(ui->directionalLightR, SIGNAL(valueChanged(int)), this, SLOT(directionalLightRChanged(int)));
    connect(ui->directionalLightG, SIGNAL(valueChanged(int)), this, SLOT(directionalLightGChanged(int)));
    connect(ui->directionalLightB, SIGNAL(valueChanged(int)), this, SLOT(directionalLightBChanged(int)));
    connect(ui->crepuscularRays, SIGNAL(toggled(bool)), this, SLOT(crepuscularRaysChanged(bool)));
    connect(ui->shadowMapping, SIGNAL(toggled(bool)), this, SLOT(shadowMappingChanged(bool)));
    connect(ui->hdr, SIGNAL(toggled(bool)), this, SLOT(hdrChanged(bool)));
    connect(ui->hdrExposure, SIGNAL(valueChanged(int)), this, SLOT(hdrExposureChanged(int)));
//    connect(ui->geometryShaders, SIGNAL(toggled(bool)), this, SLOT(geometryShadersChanged(bool)));

//    restoreGeometry(qtSettings.value("geometry").toByteArray());
//    restoreState(qtSettings.value("windowState").toByteArray());
//    dataBind();
}

void MainWindow::closeEvent(QCloseEvent* event) {
    settings.saveSettings();
    QMainWindow::closeEvent(event);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::directionalLightRChanged(int newValue) {
    settings.directionalLightR = newValue;
}

void MainWindow::directionalLightGChanged(int newValue) {
    settings.directionalLightG = newValue;
}

void MainWindow::directionalLightBChanged(int newValue) {
    settings.directionalLightB = newValue;
}

void MainWindow::crepuscularRaysChanged(bool newValue) {
    settings.crepuscularRays = newValue;
}

void MainWindow::shadowMappingChanged(bool newValue) {
    settings.shadowMapping = newValue;
}

void MainWindow::hdrChanged(bool newValue) {
    settings.hdr = newValue;
}

void MainWindow::hdrExposureChanged(int newValue) {
    settings.hdrExposure = newValue;
}

void MainWindow::geometryShadersChanged(bool newValue) {
    settings.geometryShaders = newValue;
}
