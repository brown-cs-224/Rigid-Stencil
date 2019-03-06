#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    on_wireframeCheckBox_clicked(ui->wireframeCheckBox->isChecked());
    on_anchorPointsCheckBox_clicked(ui->anchorPointsCheckBox->isChecked());
    on_radioButton_clicked();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_wireframeCheckBox_clicked(bool checked)
{
    ui->view->setWireframe(checked);
}

void MainWindow::on_anchorPointsCheckBox_clicked(bool checked)
{
    ui->view->setAnchorPoints(checked);
}

void MainWindow::on_radioButton_clicked()
{
    int step;
    if (ui->radioButton->isChecked()) step = 0;
    if (ui->radioButton_2->isChecked()) step = 1;
    if (ui->radioButton_3->isChecked()) step = 2;

    ui->view->setStep(step);
}

void MainWindow::on_radioButton_2_clicked()
{
    on_radioButton_clicked();
}

void MainWindow::on_radioButton_3_clicked()
{
    on_radioButton_clicked();
}


void MainWindow::on_actionOpen_triggered()
{
    QString file = QFileDialog::getOpenFileName(this, "Open File", "");
    if (!file.isNull()) {
        ui->view->loadObj(file);
    }
}
