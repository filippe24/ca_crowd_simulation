#ifndef GLWIDGET_H
#define GLWIDGET_H


#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include "trianglemesh.h"
#include "prsanimation.h"
#include "groundgrid.h"
//#include<cal3d/cal3d.h>

#include <QElapsedTimer>

#include "cal3Ddemo/modelloader.h"



class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
	GLWidget(QWidget *parent);
	~GLWidget();

	void loadMesh(const QString &filename);
	void setPolygonMode(bool bFill);
    void setPartMode(bool bPart);


    //animation functions
    void resetAnimation();
    void initializeAnimation();
    void clearParticles();


    //cal3d functions
    bool loadCal3dModels();
    void initializeCal3dModels();
    void animateCal3dModel(float elapsedSeconds);


    //ground
    void initializeGround();


    //comunication functions
    void update_radius(float rad);
    void update_radius_int(int rad);
    void update_max_num(int num);

    void update_updating_mode(int mode);

    void update_fountain_vel_y(float vy);

    void setFountainMode(bool b);

    void set_elast_ke(float ke);
    void set_elast_l0(float l0);
    void set_damp_kd(float kd);
    void set_length_spring(int s_l);
    void set_spring_param(float ke, float l0, float kd, int s_l);

    void update_pos(float x_delta, float y_delta, float z_delta);
    void update_vel(float x_delta, float y_delta, float z_delta);

    void update_bouncing(float boun);
    void update_spring_bool(bool spring_active);
    void update_spring2D_bool(bool spring2d_active);


public slots:
    void animate();

protected:
	void initializeGL();
	void resizeGL(int w, int h);
	void paintGL();

	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);

private:
	void setProjection(float aspect);
	void setModelview();

    void initializePlane(glm::vec3 rb, glm::vec3 rt, glm::vec3 lt, glm::vec3 lb);
    void initializeTriangle(glm::vec3 ver1, glm::vec3 ver2, glm::vec3 ver3);


private:

    //TIMER
    QElapsedTimer elapsedTimer;
    float current_time = 0.0f;

    std::string basicPath = "/home/al/Documents/Un/animation/lab/2crowdanimation/";

    bool bPolygonFill, particleMode;
	float angleX, angleY, distance;
	QPoint lastMousePos;

    QOpenGLShaderProgram *program, *programGeneral, *programPers;
	TriangleMesh mesh;

    GLuint quad_vertex_buffer, particles_position_buffer, particles_color_buffer;
    GLuint quadVBO, valueVBO, quadTextCoordVBO,vao;

    std::vector<GLuint> planesVBO, trianglesVBO;


    GLuint activeId;
    GLuint particleShaderId,generalShaderId;



    //ground
    groundgrid ground;
    int ground_columns = 10;
    int ground_rows = 10;

    //people
    prsanimation prsan;



    //update mode euler-semi-verlet
    enum update_modes { euler, semi, verlet};
    update_modes selected_update_mode = euler;

    //dimension of the room
    float cell_dim_param = 1.0f;
    float floor_y = 0.0f;
    float roof_y = 2.0f;
    float cell_pos_r, cell_pos_l, cell_pos_b, cell_pos_f;

    //triangle paramters
    bool triangle_on = false;
    glm::vec3 tri1 = glm::vec3(0.2f, 0.6f, 0.4f);
    glm::vec3 tri2 = glm::vec3(0.3f, 0.7f, 0.1f);
    glm::vec3 tri3 = glm::vec3(-0.2f, 0.6f, 0.3f);

    //gravity force
    float gravity = -9.8f;

    //PEOPLE PARAMETERS
    //radius of PERSON
    bool fixed_y_on = true;
    float radius = 0.2f;
    unsigned int num_people_per_frame = 10;
    float pers_lifetime = 20.0f;
    float pers_bouncing = 0.8f;
    unsigned int max_num_of_people =  10;
    glm::vec3 pers_initial_position = glm::vec3(0.0f, 0.3f, 0.0f);
    float pers_fountain_y = 0.0f;
    bool fountain_mode = true;

    float pers_i_velocity_x = 0.0f;
    float pers_i_velocity_y = 0.0f;
    float pers_i_velocity_z = 0.0f;

    //SPHERE PARAMETERS
    bool sphere_on = false;
    glm::vec3 sphere_center = glm::vec3(4.0f, 0.3f, 0.1f );
    float sphere_radius = 0.2f;
    float sphere_radius_delta = 0.1f;


    //positions of all the particles in each frame
    std::vector<float> positions;




    //*******************
    //****cal3d**********
    //*******************
    unsigned int selected_model = 1;
//    Modelloader modelload;
    std::vector<Modelloader *> allModels;
    std::string calDemoPath;
    unsigned int id_currentModel = 2;


    vector<vector<QVector3D>> verticesPers; // (x,y,z)
    vector<float> normalsPers; // (nx,ny,nz)
    vector<float> colorsPers;  // (r,g,b)
    vector<vector<int>> indicesPers;
    int numOfParts;

//    vector<GLuint> vao_Pers;
//    vector<GLuint> vboVert_Pers, vboIndex_Pers, vboNorm_Pers;


    static const int MAX_NUM_PARTS = 50;
    TriangleMesh personParts[MAX_NUM_PARTS];




};

#endif // GLWIDGET_H
