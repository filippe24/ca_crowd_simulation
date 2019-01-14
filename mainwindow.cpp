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

void MainWindow::on_obstaclePercentage_valueChanged(int value)
{
    float new_percentage = float(value)/100.f;
    std::cout << "new obstacles percentage" << new_percentage << std::endl;
    ui->openGLWidget->accessPersonAnimation().ground.percentage_of_obstacles = new_percentage;
}

void MainWindow::on_start_x_valueChanged(int arg1)
{
    int max_x = int(ui->openGLWidget->accessPersonAnimation().ground.getRowDimension());
    if(arg1 > max_x)
    {
        ui->openGLWidget->start_cell_x = max_x-1;
    }
    else if(arg1 < 0)
    {
        ui->openGLWidget->start_cell_x = 0;
    }
    else
        ui->openGLWidget->start_cell_x = arg1;
}

void MainWindow::on_start_y_valueChanged(int arg1)
{
    int max_y = int(ui->openGLWidget->accessPersonAnimation().ground.getColumnDimension());
    if(arg1 > max_y)
    {
        ui->openGLWidget->start_cell_y = max_y-1;
    }
    else if(arg1 < 0)
    {
        ui->openGLWidget->start_cell_y = 0;
    }
    else
        ui->openGLWidget->start_cell_y = arg1;
}

void MainWindow::on_goal_x_valueChanged(int arg1)
{
    int max_x = int(ui->openGLWidget->accessPersonAnimation().ground.getRowDimension());
    if(arg1 > max_x)
    {
        ui->openGLWidget->goal_cell_x = max_x-1;
    }
    else if(arg1 < 0)
    {
        ui->openGLWidget->goal_cell_x = 0;

    }
    else
        ui->openGLWidget->goal_cell_x = arg1;
}

void MainWindow::on_goal_y_valueChanged(int arg1)
{
    int max_y = int(ui->openGLWidget->accessPersonAnimation().ground.getColumnDimension());
    if(arg1 > max_y)
    {
        ui->openGLWidget->goal_cell_y = max_y-1;
    }
    else if(arg1 < 0)
    {
        ui->openGLWidget->goal_cell_y = 0;
    }
    else
        ui->openGLWidget->goal_cell_y = arg1;
}

void MainWindow::on_obstaclesButton_clicked()
{
    bool previous_b = ui->openGLWidget->obstacles_on;
    ui->openGLWidget->accessPersonAnimation().obstacle_on = !previous_b;
    ui->openGLWidget->obstacles_on = !previous_b;
}
