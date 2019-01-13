#include <QFileDialog>
#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
}

MainWindow::~MainWindow()
{
	delete ui;
}

//void MainWindow::on_action_Quit_triggered()
//{
//	 QApplication::quit();
//}


//void MainWindow::on_action_Open_triggered()
//{
//	QString filename = QFileDialog::getOpenFileName(this, tr("Open PLY"), ".", tr("*.ply"));

//	ui->openGLWidget->loadMesh(filename);
//}


//MY FUNCTIONS:


//void MainWindow::on_restart_clicked()
//{
//    ui->openGLWidget->resetAnimation();
//}


void MainWindow::on_resetAnimation_clicked()
{
    ui->openGLWidget->resetAnimation();
}

void MainWindow::on_pathcrowdButton_clicked()
{
    bool previous_mode = ui->openGLWidget->getCrowdMode();
    ui->openGLWidget->setCrwodMode(!previous_mode);
    ui->openGLWidget->resetAnimation();
}

void MainWindow::on_boundingButton_clicked()
{
    bool previous_b = ui->openGLWidget->personBoundingOn;
    ui->openGLWidget->personBoundingOn = !previous_b;
}

void MainWindow::on_avoidanceButton_clicked()
{
   bool previous_b = ui->openGLWidget->accessPersonAnimation().getAvoidanceOn();
   ui->openGLWidget->setAvoidanceOn(!previous_b);
}
