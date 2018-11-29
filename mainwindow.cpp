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

void MainWindow::on_action_Quit_triggered()
{
	 QApplication::quit();
}


void MainWindow::on_action_Open_triggered()
{
	QString filename = QFileDialog::getOpenFileName(this, tr("Open PLY"), ".", tr("*.ply"));

	ui->openGLWidget->loadMesh(filename);
}





void MainWindow::on_restart_clicked()
{
    ui->openGLWidget->resetAnimation();
}

void MainWindow::on_radius_slider_valueChanged(int value)
{
    ui->openGLWidget->update_radius_int(value);
}

void MainWindow::on_pos_but_clicked()
{
    ui->openGLWidget->initializeAnimation();
}

void MainWindow::on_fount_vel_valueChanged(double arg1)
{
    ui->openGLWidget->update_fountain_vel_y(arg1);
}


void MainWindow::on_fount_bu_clicked()
{
    ui->openGLWidget->initializeAnimation();
}

void MainWindow::on_velocity_bu_toggled(bool checked)
{
    ui->openGLWidget->setFountainMode(not(checked));
    ui->fount_bu->setEnabled(not(checked));
    ui->fount_vel->setEnabled(not(checked));
    ui->vel_x_m_bu->setEnabled(checked);
    ui->vel_y_m_bu->setEnabled(checked);
    ui->vel_z_m_bu->setEnabled(checked);
    ui->vel_x_p_bu->setEnabled(checked);
    ui->vel_y_p_bu->setEnabled(checked);
    ui->vel_z_p_bu->setEnabled(checked);
}
void MainWindow::on_fountain_bu_clicked(bool checked)
{
    ui->openGLWidget->setFountainMode(checked);
    ui->fount_bu->setEnabled(checked);
    ui->fount_vel->setEnabled(checked);
    ui->vel_x_m_bu->setEnabled(not(checked));
    ui->vel_y_m_bu->setEnabled(not(checked));
    ui->vel_z_m_bu->setEnabled(not(checked));
    ui->vel_x_p_bu->setEnabled(not(checked));
    ui->vel_y_p_bu->setEnabled(not(checked));
    ui->vel_z_p_bu->setEnabled(not(checked));
}

void MainWindow::on_max_num_valueChanged(int arg1)
{
    ui->openGLWidget->update_max_num(arg1);
}

void MainWindow::on_pos_x_p_clicked()
{
    ui->openGLWidget->update_pos(unit_pos,0.0,0.0);
}

void MainWindow::on_pos_x_m_clicked()
{
    ui->openGLWidget->update_pos(-unit_pos,0.0,0.0);
}

void MainWindow::on_pos_y_p_clicked()
{
    ui->openGLWidget->update_pos(0.0,unit_pos,0.0);
}

void MainWindow::on_pos_y_m_clicked()
{
    ui->openGLWidget->update_pos(0.0,-unit_pos,0.0);
}

void MainWindow::on_pos_z_p_clicked()
{
    ui->openGLWidget->update_pos(0.0,0.0,unit_pos);
}

void MainWindow::on_pos_z_m_clicked()
{
    ui->openGLWidget->update_pos(0.0,0.0,-unit_pos);
}

void MainWindow::on_pos_unit_spin_valueChanged(double arg1)
{
    unit_pos = arg1;
}

void MainWindow::on_bouncing_scroll_actionTriggered(int action)
{
    ui->openGLWidget->update_bouncing((float)action/10.0f);
}


void MainWindow::on_Euler_bu_clicked(bool checked)
{
    if(checked)
        ui->openGLWidget->update_updating_mode(0);
}

void MainWindow::on_SemiEluer_bu_clicked(bool checked)
{
    if(checked)
        ui->openGLWidget->update_updating_mode(1);
}

void MainWindow::on_Verlet_bu_clicked(bool checked)
{
    if(checked)
        ui->openGLWidget->update_updating_mode(2);
}



void MainWindow::on_vel_x_m_bu_clicked()
{
    ui->openGLWidget->update_vel(-unit_pos,0.0,0.0);
}

void MainWindow::on_vel_x_p_bu_clicked()
{
    ui->openGLWidget->update_vel(unit_pos,0.0,0.0);
}

void MainWindow::on_vel_y_p_bu_clicked()
{
    ui->openGLWidget->update_vel(0.0,unit_pos,0.0);
}

void MainWindow::on_vel_y_m_bu_clicked()
{
    ui->openGLWidget->update_vel(0.0,-unit_pos,0.0);
}

void MainWindow::on_vel_z_p_bu_clicked()
{
    ui->openGLWidget->update_vel(0.0,0.0,unit_pos);
}

void MainWindow::on_vel_z_m_bu_clicked()
{
    ui->openGLWidget->update_vel(0.0,0.0,-unit_pos);
}

