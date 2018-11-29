#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

    void addParticles(float x, float y, float z);
    void clearParticles();
private:

    //position movement
    float unit_pos = 0.1f;
    float unit_vel = 10.0f;

private slots:
	void on_action_Quit_triggered();


	void on_action_Open_triggered();


    void on_restart_clicked();

    void on_radius_slider_valueChanged(int value);

    void on_pos_but_clicked();

    void on_fount_vel_valueChanged(double arg1);


    void on_fount_bu_clicked();

    void on_velocity_bu_toggled(bool checked);

    void on_max_num_valueChanged(int arg1);


    void on_pos_x_p_clicked();

    void on_pos_x_m_clicked();

    void on_pos_y_p_clicked();

    void on_pos_y_m_clicked();

    void on_pos_z_p_clicked();

    void on_pos_z_m_clicked();

    void on_pos_unit_spin_valueChanged(double arg1);

    void on_bouncing_scroll_actionTriggered(int action);

    void on_Euler_bu_clicked(bool checked);

    void on_SemiEluer_bu_clicked(bool checked);

    void on_Verlet_bu_clicked(bool checked);


    void on_fountain_bu_clicked(bool checked);

    void on_vel_x_m_bu_clicked();

    void on_vel_x_p_bu_clicked();

    void on_vel_y_p_bu_clicked();

    void on_vel_y_m_bu_clicked();

    void on_vel_z_p_bu_clicked();

    void on_vel_z_m_bu_clicked();


private:
	Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
