#include "glwidget.h"
#include <iostream>
#include <QApplication>
#include <QMatrix4x4>
#include <QMouseEvent>
#include <plyreader.h>
#include <QTimer>
#include <cal3Ddemo/demo.h>


using namespace std;


const float rotationFactor = 0.5f;
const float maxRotationCamera = 360.0f;
const float minDistanceCamera = 1.0f;
const float maxDistanceCamera = 30.0f;



GLWidget::GLWidget(QWidget *parent) : QOpenGLWidget(parent), bPolygonFill(true), angleX(90.0f), angleY (0.0f), distance(30.0f)
{

    std::cout<< "0.glWidget"<<std::endl;

    program = nullptr;
    programGeneral = nullptr;
    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setVersion(3, 3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(format);
    setFormat(format);

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(animate()));
    timer->start(33);

}

GLWidget::~GLWidget()
{
	if(program) delete program;
}


void GLWidget::initializeGL()
{
    std::cout<< " 1. initialize GL" <<std::endl;
	initializeOpenGLFunctions();


    particleMode = true;



    //*********************************************
    //*********timer*******************************
    //*********************************************

    //create Timer instead of tick
    //start timer
    elapsedTimer.start();
    // keep repainting
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start();



    if(particleMode)
    {


        //FUNTIONS DEFINTIONS
        typedef void (APIENTRY *_glGenVertexArrays) (GLsizei, GLuint*);
        typedef void (APIENTRY *_glBindVertexArray) (GLuint);
        _glGenVertexArrays glGenVertexArrays;
         _glBindVertexArray glBindVertexArray;
        glGenVertexArrays = _glGenVertexArrays (QOpenGLWidget::context()->getProcAddress("glGenVertexArrays"));
        glBindVertexArray = _glBindVertexArray (QOpenGLWidget::context()->getProcAddress("glBindVertexArray"));




        //*****************************
        //********** GENERAL **********
        //*****************************
        programGeneral = new QOpenGLShaderProgram();
        programGeneral->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/simpleshader.vert");
        programGeneral->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/simpleshader.frag");

        programGeneral->bindAttributeLocation("position",0);

        programGeneral->link();
        if(!programGeneral->isLinked())
        {
                cout << "Shader program has not linked GENERAL" << endl << endl << "Log: " << endl << endl << programGeneral->log().toStdString();
                QApplication::quit();
        }
        programGeneral->bind();

        //*****************************
        //********** Planes ***********
        //*****************************
        float f = floor_y - radius;
        float r = roof_y + radius;

        float c = cell_dim_param;

        float x_dimension = ground_columns*c;
        float z_dimension = ground_rows*c;

        float x_min = -x_dimension/2.0f;
        float z_min = -z_dimension/2.0f;
        float x_max = x_dimension/2.0f;
        float z_max = z_dimension/2.0f;


        cell_pos_r = x_max;
        cell_pos_l = x_min;
        cell_pos_f = z_min;
        cell_pos_b = z_max;


        //ground
        float x_i = x_min;
        float z_i = z_min;
        for(int col = 0; col < ground_columns; col ++)
        {
            x_i = x_min;
            for(int row = 0; row < ground_rows; row ++)
            {
                initializePlane(glm::vec3(x_i+c, f, z_i),glm::vec3(x_i+c, f, z_i+c),glm::vec3(x_i, f, z_i+c),glm::vec3(x_i, f, z_i));
                x_i = x_i + c;
            }
            z_i = z_i + c;
        }

        //floor
        //right
        initializePlane(glm::vec3(x_max, f, z_min),glm::vec3(x_max, r, z_min),glm::vec3(x_max, r, z_max),glm::vec3(x_max, f, z_max));
        //front
        initializePlane(glm::vec3(x_max, f, z_max),glm::vec3(x_min, f, z_max),glm::vec3(x_min, r, z_max),glm::vec3(x_max, r, z_max));
        //left
        initializePlane(glm::vec3(x_min, f, z_min),glm::vec3(x_min, r, z_min),glm::vec3(x_min, r, z_max),glm::vec3(x_min, f, z_max));
        //back
        initializePlane(glm::vec3(x_min, f, z_min),glm::vec3(x_max, f, z_min),glm::vec3(x_max, r, z_min),glm::vec3(x_min, r, z_min));



        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glEnable(GL_DEPTH_TEST);

        glUseProgram(0);
        glBindVertexArray(0);
        programGeneral->release();


        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);



        //************************
        //********OBSTACLES*******
        //************************
        programObstacles = new QOpenGLShaderProgram();
        programObstacles->addShaderFromSourceFile(QOpenGLShader::Vertex,  QString::fromStdString(basicPath) +"/shaders/obstacle.vert");
        programObstacles->addShaderFromSourceFile(QOpenGLShader::Fragment,  QString::fromStdString(basicPath) +"/shaders/obstacle.frag");
        programObstacles->link();
        if(!programObstacles->isLinked())
        {
                cout << "Shader program has not linked" << endl << endl << "Log: " << endl << endl << program->log().toStdString();
                QApplication::quit();
        }
        programObstacles->bind();
        meshObstacle.buildCube();
        if(!meshObstacle.init(programObstacles))
        {
                cout << "Could not create vbo" << endl;
                QApplication::quit();
        }

        programObstacles->release();


        //************************
        //********PATH************
        //************************
        programPath = new QOpenGLShaderProgram();
        programPath->addShaderFromSourceFile(QOpenGLShader::Vertex,  QString::fromStdString(basicPath) +"/shaders/path.vert");
        programPath->addShaderFromSourceFile(QOpenGLShader::Fragment,  QString::fromStdString(basicPath) +"/shaders/path.frag");
        programPath->link();
        if(!programPath->isLinked())
        {
                cout << "Shader program has not linked" << endl << endl << "Log: " << endl << endl << program->log().toStdString();
                QApplication::quit();
        }
        programPath->bind();
        meshPath.buildCube();
        if(!meshPath.init(programPath))
        {
                cout << "Could not create vbo" << endl;
                QApplication::quit();
        }

        programPath->release();





        //*************************
        //*********cal3d***********
        //*************************
        loadCal3dModels();

        //*********************************
        //**********  PEOPLE  *************
        //*********************************        
        programPers = new QOpenGLShaderProgram();
        programPers->addShaderFromSourceFile(QOpenGLShader::Vertex, QString::fromStdString(basicPath) + "/shaders/person.vert");
        programPers->addShaderFromSourceFile(QOpenGLShader::Fragment, QString::fromStdString(basicPath) + "/shaders/person.frag");

        programPers->link();
        if(!programPers->isLinked())
        {
                cout << "Shader program has not linked PEOPLE" << endl << endl << "Log: " << endl << endl << program->log().toStdString();
                QApplication::quit();
        }
        programPers->bind();

        makeCurrent();

        for(int i = 0; i < numOfParts; i++)
        {
            if(!personParts[i].init(programPers))
            {
                    cout << "Could not create vbo" << endl;
                    QApplication::quit();
            }


        }
        programPers->release();

        //*********************************
        //**********  PARTICLES  **********
        //*********************************
        program = new QOpenGLShaderProgram();
        program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/particle.vert");
        program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/particle.frag");

        program->bindAttributeLocation("position",0);
        program->bindAttributeLocation("value",1);

        program->link();
        if(!program->isLinked())
        {
                cout << "Shader program has not linked PARTICLES" << endl << endl << "Log: " << endl << endl << program->log().toStdString();
                QApplication::quit();
        }        glBindVertexArray(vao);

        program->bind();

       //initialize the quad
        std::cout << "defining the quad" << std::endl;
        GLfloat quadVertices[] =
        {
             // Positions
            1.0f,  -1.0f, 0.0f,
            1.0f,  1.0f,  0.0f,
            -1.0f, 1.0f,  0.0f,
            -1.0f, -1.0f, 0.0f,
        };

        //position
        glGenBuffers(1, &quadVBO);
        //Generate, bind and fill VBO for vertices
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);


        float valueBuffer[12]={
            //lx ly
            1.0, -1.0,
            1.0, 1.0,
            -1.0, 1.0,
            -1.0, -1.0,
        };

        //value
        glGenBuffers(1, &valueVBO);
        glBindBuffer(GL_ARRAY_BUFFER, valueVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(valueBuffer), &valueBuffer, GL_STATIC_DRAW);

        //*************************
        //*********ANIMATION*******
        //*************************
        initializeAnimation();
        initializeGround();

        glUseProgram(0);
        glBindVertexArray(0);


        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glEnable(GL_DEPTH_TEST);



        doneCurrent();
    }
    else
    {
        program = new QOpenGLShaderProgram();
        program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/simpleshader.vert");
        program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/simpleshader.frag");
        program->link();
        if(!program->isLinked())
        {
                cout << "Shader program has not linked" << endl << endl << "Log: " << endl << endl << program->log().toStdString();
                QApplication::quit();
        }
        program->bind();

        mesh.buildCube();
        if(!mesh.init(program))
        {
                cout << "Could not create vbo" << endl;
                QApplication::quit();
        }

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glEnable(GL_DEPTH_TEST);

    }


}

void GLWidget::resizeGL(int w, int h)
{
    std::cout<< " resize GL" <<std::endl;
	glViewport(0,0,w,h);
	setProjection((float)w/h);
	setModelview();

}



void GLWidget::paintGL()
{
    typedef void (APIENTRY *_glBindVertexArray) (GLuint);
     _glBindVertexArray glBindVertexArray;
    glBindVertexArray = _glBindVertexArray (QOpenGLWidget::context()->getProcAddress("glBindVertexArray"));
    //*********************************************
    //*********timer*******************************
    //*********************************************
    float time = float(elapsedTimer.elapsed()/1000.0f);

    if(particleMode)
    {



        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);





        glBindVertexArray(vao);

        //*****************************
        //********** GENERAL **********
        //*****************************


        programGeneral->bind();

        programGeneral->setUniformValue("bLighting", bPolygonFill);

        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(0.5f, 1.0f);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDisable(GL_POLYGON_OFFSET_FILL);
        programGeneral->setUniformValue("color", QVector4D(0.05f, 0.05f, 0.15f, 1.0f));

        //*********************************
        //**********  Plane  **************
        //*********************************
        for(uint i = 0; i < planesVBO.size(); i++)
        {
            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER,planesVBO[i]);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        }
        //set back to fill
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        bPolygonFill = false;

        programGeneral->setUniformValue("color", QVector4D(0.9f, 0.8f, 0.6f, 1.0f));

        programGeneral->release();




        //*********************************
        //**********  PARTICLES  **********
        //*********************************
        glClearColor(0.8f, 0.8f, 0.8f, 1.0f);

        program->bind();
        enum
        {
            VERTICES,
            VALUE
        };

        glEnableVertexAttribArray(VERTICES);
        glBindBuffer(GL_ARRAY_BUFFER,quadVBO);
        glVertexAttribPointer(VERTICES, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

        glEnableVertexAttribArray(VALUE);
        glBindBuffer(GL_ARRAY_BUFFER,valueVBO);
        glVertexAttribPointer(VALUE, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

        for(uint i= 0; i<positions.size(); i= i+3)
        {
            QVector3D par_pos;
            par_pos.setX(positions[i]);
            par_pos.setY(positions[i+1]);
            par_pos.setZ(positions[i+2]);

            program->setUniformValue("color", QVector4D(0.5f, 0.5f, 0.5f, 1.0f));
            program->setUniformValue("particle_pos", par_pos);
            program->setUniformValue("radius", radius);


            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        }
        glBindBuffer(GL_ARRAY_BUFFER,0);
        glBindBuffer(GL_ARRAY_BUFFER,0);
        glDisableVertexAttribArray(0);
        program->release();


        //unbind


        //****************************
        //********  OBSTACLES  *******
        //****************************
        if(obstacles_on)
        {
            QMatrix4x4 scale;

            scale.scale(0.5f ,1.0f, 0.5f);

            programObstacles->bind();
            programObstacles->setUniformValue("color", QVector4D(0.3f, 0.3f, 0.3f, 1.0f));
            std::vector<std::pair<int,int>> obstacles_poitions = prsan.ground.getObstaclesPositions();
            for(uint obs = 0; obs < obstacles_poitions.size(); obs++)
            {
                glm::vec3 obs_pos = prsan.ground.getCellPosition(obstacles_poitions[obs].first,obstacles_poitions[obs].second);
                float offset =(cell_dim_param)/2.0f;
                programObstacles->setUniformValue("translation", QVector3D(obs_pos.x + offset ,2*radius, obs_pos.z + offset));
                meshObstacle.render(*this);
            }
            programObstacles->release();

        }


        //****************************
        //********  PATH  ************
        //****************************
        //goal
        if(!crowd_mode)
        {
            programPath->bind();
            programPath->setUniformValue("color", QVector4D(0.8f, 0.2f, 0.2f, 0.2f));
    //        std::vector<std::pair<int,int>> obstacles_poitions = prsan.ground.getGoalPosition();
    //                glm::vec3 obs_pos = prsan.ground.getCellPosition(obstacles_poitions[obs].first,obstacles_poitions[obs].second);
            glm::vec3 goal_pos = prsan.ground.getGoalPos();
            float offset = cell_dim_param/2.0f;
            programPath->setUniformValue("translation", QVector3D(goal_pos.x + offset, -21.0f, goal_pos.z + offset));
            meshPath.render(*this);
        }
        //starting
        if(!crowd_mode)
        {
            programPath->bind();
            programPath->setUniformValue("color", QVector4D(0.2f, 0.8f, 0.2f, 0.2f));
            glm::vec3 goal_pos = prsan.ground.getStartPos();
            float offset = cell_dim_param/2.0f;
            programPath->setUniformValue("translation", QVector3D(goal_pos.x + offset, -21.0f, goal_pos.z + offset));
            meshPath.render(*this);
        }
        //path
        if(!crowd_mode)
        {
            programPath->bind();
            programPath->setUniformValue("color", QVector4D(0.1f, 0.6f, 0.9f, 0.2f));
            std::vector<std::pair<int,int>> path_positions = prsan.ground.getPathPositions();
            for(uint pth = 0; pth < path_positions.size(); pth++)
            {
                glm::vec3 pth_pos = prsan.ground.getCellPosition(path_positions[pth].first,path_positions[pth].second);
                float offset = cell_dim_param/2.0f;
                programPath->setUniformValue("translation", QVector3D(pth_pos.x + offset, -21.01f, pth_pos.z + offset));
                meshPath.render(*this);
            }

        }



        //following
        for(uint i= 0; i<positions.size(); i= i+3)
        {
            programPath->setUniformValue("color", QVector4D(0.2f, 0.2f, 0.8f, 0.2f));
            float x = positions[i];
            float z = positions[i+2];
            int c = prsan.ground.getCurrentCell(x,0.0f,z).first;
            int r = prsan.ground.getCurrentCell(x,0.0f,z).second;
            glm::vec3 prs_cell_pos = prsan.ground.getCellPosition(c,r);
            float offset = cell_dim_param/2.0f;
            programPath->setUniformValue("translation", QVector3D(prs_cell_pos.x + offset, -21.0f, prs_cell_pos.z + offset));
            meshPath.render(*this);

        }

        programPath->release();








        //*********************************
        //**********  PEOPLE  *************
        //*********************************
        programPers->bind();
        programPers->setUniformValue("color", QVector4D(0.75f, 0.8f, 0.9f, 1.0f));

        //TO DO : MAKE THE UPDATE DEPENDT ON HOW FAST THE MODEL IS MOVING
        //or just make everymdoel go at the same speed
        animateCal3dModel(time-old_time);
        old_time = time;

        QMatrix4x4 rotation;
        QVector3D rot_vec = QVector3D(-1.0f, 0.0f, 0.0f);
        rotation.rotate(90, rot_vec);


        //compute angle between standard and direction

        for(uint i= 0; i<positions.size(); i= i+3)
        {
            QMatrix4x4 orientation;
            QVector3D pers_ax = QVector3D(0.0f, 1.0f, 0.0f);
            programPers->setUniformValue("color", QVector4D(0.75f, 0.8f, 0.9f, 1.0f));
            int pers = int((i+0.5f)/3);
            float x,z = 0.0f;
            prsan.getVelocity(pers,x,z);
            float dot = x*0.0f + z*(1.0f);
            float det = x*(1.0f) - z*0.0f;
            float angle = atan2(det, dot);
            angle = (180.0f*angle)/M_PI;
//            float angle = atan2(x-0.0f, z+1.0f);
//            cout << " person :" << i/3 <<" or "<< pers << endl;
//            cout << " veocity vector x: " << x << ", z: " << z << "   and angle with basic : " << angle << endl;
            orientation.rotate(angle,pers_ax);

            QVector3D pers_pos;
            pers_pos.setX(positions[i]);
            pers_pos.setY(0.0f - radius );
            pers_pos.setZ(positions[i+2]);

            programPers->setUniformValue("space_pos", pers_pos);
            programPers->setUniformValue("rotation", rotation);
            programPers->setUniformValue("orientation", orientation);

////            test
//            if(i == 0 || i==3)
//            {
//                std::cout << std::endl;
//                programPers->setUniformValue("color", QVector4D(0.8f, 0.3f, 0.3f+float(i/10.0f), 1.0f));
//                std::pair<int,int> cell = ground.getCurrentCell(pers_pos.x(),pers_pos.y(), pers_pos.z());
//                std::cout << " the i = " << i << " person, is in the cell: x = " << cell.first << "  z = " << cell.second << std::endl;
//                glm::vec3 test_pos = ground.getCellPosition(cell.first, cell.second);
//                std::cout << "  and the position check is x: " << test_pos.x << " == " << pers_pos.x() << "  and z : "<< test_pos.z << " == " << pers_pos.z() << std::endl;

//            }

            for(int m = 0; m < numOfParts; m++)
                personParts[m].render(*this);
        }
        programPers->release();



        glBindBuffer(GL_ARRAY_BUFFER,0);
        glDisableVertexAttribArray(0);
        glBindVertexArray(0);


    }
    else
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        program->bind();
        program->setUniformValue("bLighting", bPolygonFill);

        if(bPolygonFill)
            program->setUniformValue("color", QVector4D(0.75f, 0.8f, 0.9f, 1.0f));
        else
        {
            program->setUniformValue("color", QVector4D(1.0f, 1.0f, 1.0f, 1.0f));
            glEnable(GL_POLYGON_OFFSET_FILL);
            glPolygonOffset(0.5f, 1.0f);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            //cube.render(*this);
            mesh.render(*this);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDisable(GL_POLYGON_OFFSET_FILL);
            program->setUniformValue("color", QVector4D(0.05f, 0.05f, 0.15f, 1.0));
        }


        //cube.render(*this);
        mesh.render(*this);



        program->release();


    }
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
	lastMousePos = event->pos();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
	// Rotation
	if(event->buttons() & Qt::LeftButton)
	{
		angleX += rotationFactor * (event->y() - lastMousePos.y());
		angleX = max(-maxRotationCamera, min(angleX, maxRotationCamera));
		angleY += rotationFactor * (event->x() - lastMousePos.x());
	}
	// Zoom
	if(event->buttons() & Qt::RightButton)
	{
        distance += 0.1f * (event->y() - lastMousePos.y());
		distance = max(minDistanceCamera, min(distance, maxDistanceCamera));
	}

    lastMousePos = event->pos();

	makeCurrent();
	setModelview();
	doneCurrent();
	update();
}

void GLWidget::setProjection(float aspect)
{
    std::cout<< "       --set projection" <<std::endl;

	QMatrix4x4 projectionMatrix;

    projectionMatrix.perspective(60, aspect, 0.01f, 100.0);
    program->bind();
    program->setUniformValue("projection", projectionMatrix);
    program->release();

    programGeneral->bind();
    programGeneral->setUniformValue("projection", projectionMatrix);
    programGeneral->release();

    programPers->bind();
    programPers->setUniformValue("projection", projectionMatrix);
    programPers->release();


    programObstacles->bind();
    programObstacles->setUniformValue("projection", projectionMatrix);
    programObstacles->release();

    programPath->bind();
    programPath->setUniformValue("projection", projectionMatrix);
    programPath->release();
}

void GLWidget::setModelview()
{

	QMatrix4x4 modelviewMatrix;

	modelviewMatrix.translate(0, 0, -distance);
	modelviewMatrix.rotate(angleX, 1.0f, 0.0f, 0.0f);
	modelviewMatrix.rotate(angleY, 0.0f, 1.0f, 0.0f);
    program->bind();
    program->setUniformValue("modelview", modelviewMatrix);
    program->setUniformValue("normalMatrix", modelviewMatrix.normalMatrix());
    program->release();

    programGeneral->bind();
    programGeneral->setUniformValue("modelview", modelviewMatrix);
    programGeneral->setUniformValue("normalMatrix", modelviewMatrix.normalMatrix());
    programGeneral->release();

    programPers->bind();
    programPers->setUniformValue("modelview", modelviewMatrix);
    programPers->setUniformValue("normalMatrix", modelviewMatrix.normalMatrix());
    programPers->release();


    QMatrix4x4 modelviewMatrixObstacle;
    modelviewMatrixObstacle.translate(0, 0, -distance);
    modelviewMatrixObstacle.rotate(angleX, 1.0f, 0.0f, 0.0f);
    modelviewMatrixObstacle.rotate(angleY, 0.0f, 1.0f, 0.0f);
    modelviewMatrixObstacle.scale(cell_dim_param ,roof_y, cell_dim_param );
    programObstacles->bind();
    programObstacles->setUniformValue("modelview", modelviewMatrixObstacle);
    programObstacles->setUniformValue("normalMatrix", modelviewMatrixObstacle.normalMatrix());
    programObstacles->release();

    QMatrix4x4 modelviewMatrixPath;
    modelviewMatrixPath.translate(0, 0, -distance);
    modelviewMatrixPath.rotate(angleX, 1.0f, 0.0f, 0.0f);
    modelviewMatrixPath.rotate(angleY, 0.0f, 1.0f, 0.0f);
    modelviewMatrixPath.scale(cell_dim_param,0.01f,cell_dim_param);
    programPath->bind();
    programPath->setUniformValue("modelview", modelviewMatrixPath);
    programPath->setUniformValue("normalMatrix", modelviewMatrixPath.normalMatrix());
    programPath->release();



}

void GLWidget::setPolygonMode(bool bFill)
{

    std::cout<< "       --set polygonal mode" <<std::endl;

    bPolygonFill = bFill;

    makeCurrent();
    if(bFill)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    doneCurrent();
	update();
}

void GLWidget::setPartMode(bool bPart)
{

    std::cout<< "       --set particle mode" <<std::endl;

    particleMode = bPart;

    update();
}


void GLWidget::loadMesh(const QString &filename)
{
	PLYReader reader;

	mesh.destroy();
	reader.readMesh(filename, mesh);
	makeCurrent();
    if(!mesh.init(programGeneral))
	{
			cout << "Could not create vbo" << endl;
			QApplication::quit();
	}
	doneCurrent();
	update();
}



//***********************************
//******* animation *****************
//***********************************

void GLWidget::animate()
{
    positions.clear();
    positions = prsan.animate_frame();

    if(crowd_mode)
    {
        if(positions.size() < max_num_of_people )
                prsan.addPerson(int(num_people_per_frame));

        if(positions.size() == 0)
            initializeAnimation();
    }
    else
    {
        if(positions.size() == 0)
            prsan.addPerson(1);
    }


    //animateCal3dModel(current_time);

    update();
}

void GLWidget::initializeAnimation()
{
    //initialize animation
    prsan = prsanimation(fixed_y_on);
    prsan.setUpdateMode(selected_update_mode);
    prsan.setRoomParam(floor_y, cell_pos_r, cell_pos_l, cell_pos_b, cell_pos_f);
    prsan.setTriangleParam(tri1,tri2, tri3);
    prsan.setSphereParam(sphere_center, sphere_radius);
    if(crowd_mode)
    {
        if(num_people_per_frame > max_num_of_people)
            prsan.setPersonParam(int(max_num_of_people), pers_lifetime, pers_bouncing);
        else
            prsan.setPersonParam(int(num_people_per_frame), pers_lifetime, pers_bouncing);
    }
    else {prsan.setPersonParam(1, pers_lifetime, pers_bouncing); }

    prsan.ground = groundgrid(cell_dim_param);
    prsan.ground.createGrid(ground_columns,ground_rows);

    prsan.setCrowdMode(crowd_mode);
    prsan.setPathMode(goal_cell_x,goal_cell_z,start_cell_x,start_cell_y);
    prsan.setInitialVelocity(pers_i_velocity_x, pers_i_velocity_y, pers_i_velocity_z);
    prsan.setGravityPatam(gravity);
    prsan.initializeValues();
}

void GLWidget::resetAnimation()
{
    initializeAnimation();
}




//***********************************
//******* CAL3D *********************
//***********************************

bool GLWidget::loadCal3dModels()
{
//ON INIT
//    modelload = Modelloader();

    allModels.clear();

    calDemoPath = ":/cal3Ddemo";
    calDemoPath = "/home/al/Documents/Un/animation/lab/2crowdanimation/cal3Ddemo";

    std::string tempPath;

    // initialize models
    Modelloader *pModel;

    // load 'cally' model
    std::cout << "Loading 'cally' model ..." << std::endl;

    pModel = new Modelloader();


    if (calDemoPath != "")
    {
        tempPath = calDemoPath + "/" + "cally/" ;
        pModel->setPath( tempPath );
    }

    if(!pModel->onInit(calDemoPath + "/" + "cally.cfg"))
    {
      delete pModel;
      std::cerr << "Model initialization failed! (cally)" << std::endl;
      return false;
    }

    allModels.push_back(pModel);

    std::cout << std::endl;

    // load 'skeleton' model
    std::cout << "Loading 'skeleton' model ..." << std::endl;

    pModel = new Modelloader();

    if (calDemoPath != "")
    {
        tempPath = calDemoPath + "/" + "skeleton/" ;
        pModel->setPath( tempPath );
    }
    if(!pModel->onInit(calDemoPath + "/" + "skeleton.cfg"))
    {
      delete pModel;
      std::cerr << "Model initialization failed! (skeleton)" << std::endl;
      return false;
    }

    allModels.push_back(pModel);

    std::cout << std::endl;

    // load 'paladin' model
    std::cout << "Loading 'paladin' model ..." << std::endl;
    pModel = new Modelloader();

    if (calDemoPath != "")
    {
        tempPath = calDemoPath + "/" + "paladin/" ;
        pModel->setPath( tempPath );
    }

    if(!pModel->onInit(calDemoPath + "/" + "paladin.cfg"))
    {
      delete pModel;
      std::cerr << "Model initialization failed! (paladin)" << std::endl;
      return false;
    }

    allModels.push_back(pModel);

    std::cout << std::endl;


    // we're done
    std::cout << "Initialization done." << std::endl;
    std::cout << std::endl;

    std::cout << " all models creted : size = " << allModels.size() << std::endl;
    std::cout << std::endl;


    initializeCal3dModels();


    return true;
}


void GLWidget::initializeCal3dModels()
{

    //not_used
    normalsPers.clear(); // (nx,ny,nz)
    colorsPers.clear();  // (r,g,b)


    verticesPers.clear(); // (x,y,z)
    indicesPers.clear();
    numOfParts = 0;

    //clear the personParts
    for ( int i = 0; i < MAX_NUM_PARTS; i ++)
        personParts[i].clearVertices();

    allModels[selected_model]->setScale(0.005f);
    numOfParts = allModels[selected_model]->initializeRendering(&verticesPers, &indicesPers);

    if((numOfParts != int(verticesPers.size())) || (numOfParts != int(indicesPers.size())) || (indicesPers.size() != verticesPers.size()))
        std::cerr << "FAILED glwidget : initializedCal3Dmodel sizes: numOfParts: " << numOfParts << " num of Vertices Parts: " << verticesPers.size() << " num of Indices Parts: " << indicesPers.size() << std::endl;

    //adding the created vertices to the personMesh structure
    for(unsigned int s = 0; s < verticesPers.size(); s++)
    {
        for(unsigned int v = 0; v < verticesPers[s].size(); v++)
        {
            personParts[s].addVertex(verticesPers[s][v]);
        }
    }

    for(unsigned int s = 0; s < indicesPers.size(); s++)
    {
        for(unsigned int f = 0; f < indicesPers[s].size(); f = f+3)
        {
            personParts[s].addTriangle(indicesPers[s][f], indicesPers[s][f+1], indicesPers[s][f+2]);
        }
    }

    float motion = 0;
//    allModels[selected_model]->getMotionBlend(&motion);
    std::cout << "      selected model at state: " << allModels[selected_model]->getState() << "  and motion blend : " <<motion << std::endl;
    for (int p=0; p< verticesPers.size(); p++)
    {
        cout<<endl;
        std::cout << "      part s = " << p <<"   mesh with vertices: " << verticesPers[p].size() << " and indices "  << indicesPers[p].size() << std::endl;
        std::cout << "      number of replicated vertices on personPats: " <<personParts[p].getTrianglesSize() << "  and indices " << personParts[p].getTrianglesSize() << std::endl;
        cout<<endl;
    }
}


void GLWidget::animateCal3dModel(float elapsedSeconds)
{
    verticesPers.clear();

    //code for checking the motion blend
//    float motionBlend[3] = {0,0,0};
//    allModels[selected_model]->getMotionBlend(motionBlend);
//    std::cout <<" motion blend: " << motionBlend[0] << " " << motionBlend[1] << " " << motionBlend[2] <<" " << motionBlend[3] <<  endl;

    allModels[selected_model]->onUpdate(elapsedSeconds);
    if( numOfParts != allModels[selected_model]->updateVertices(&verticesPers))
        std::cout << "errore initializazione" << std::endl;
    for(unsigned int s = 0; s < numOfParts; s++)
    {
        personParts[s].clearVertices();

        for(unsigned int v = 0; v < verticesPers[s].size(); v++)
        {
            personParts[s].addVertex(verticesPers[s][v]);
        }
        personParts[s].reloadVertices(programPers);

    }


}





//***********************************
//******* ground ********************
//***********************************
void GLWidget::initializeGround()
{

}








void GLWidget::initializePlane( glm::vec3 rb, glm::vec3 rt, glm::vec3 lt, glm::vec3 lb)
{

    GLfloat planeVertices[] =
    {
         // Positions
        rb.x,  rb.y, rb.z,
        rt.x,  rt.y, rt.z,
        lt.x,  lt.y, lt.z,
        lb.x,  lb.y, lb.z,
    };
    GLuint tempVBO;
    //position
    glGenBuffers(1, &tempVBO);
    //Generate, bind and fill VBO for vertices
    glBindBuffer(GL_ARRAY_BUFFER, tempVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
    planesVBO.push_back(tempVBO);


}







//***********************************
//******* buttons *******************
//***********************************

void GLWidget::update_radius(float rad)
{
    radius = rad;
}
void GLWidget::update_radius_int(int rad)
{
    radius = float(rad/100.0f);
}

void GLWidget::update_max_num(int num)
{
    max_num_of_people = num;
//    initializeAnimation();
}




void GLWidget::setCrwodMode(bool b)
{
    crowd_mode = b;
    initializeAnimation();
}

void GLWidget::update_pos(float x_delta, float y_delta, float z_delta)
{
    pers_initial_position.x = pers_initial_position.x + x_delta;
    pers_initial_position.y = pers_initial_position.y + y_delta;
    pers_initial_position.z = pers_initial_position.z + z_delta;
}
void GLWidget::update_bouncing(float boun)
{
    pers_bouncing = boun;
}

void GLWidget::update_updating_mode(int mode)
{
    selected_update_mode = update_modes(mode);
    initializeAnimation();
}
void GLWidget::update_vel(float x_delta, float y_delta, float z_delta)
{
    pers_i_velocity_x = pers_i_velocity_x + x_delta;
    pers_i_velocity_y = pers_i_velocity_y + y_delta;
    pers_i_velocity_z = pers_i_velocity_z + z_delta;
}
