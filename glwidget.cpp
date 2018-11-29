#include "glwidget.h"
#include <iostream>
#include <QApplication>
#include <QMatrix4x4>
#include <QMouseEvent>
#include <plyreader.h>
#include <QTimer>


using namespace std;


const float rotationFactor = 0.5f;
const float maxRotationCamera = 75.0f;
const float minDistanceCamera = 1.0f;
const float maxDistanceCamera = 10.0f;

float current_time = 0.0f;


GLWidget::GLWidget(QWidget *parent) : QOpenGLWidget(parent), bPolygonFill(true), angleX(0.0f), angleY(0.0f), distance(4.0f)
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
	initializeOpenGLFunctions();

    std::cout<< " 1. initialize GL" <<std::endl;

    particleMode = true;


    if(particleMode)
    {

        //FUNTIONS DEFINTIONS

        typedef void (APIENTRY *_glGenVertexArrays) (GLsizei, GLuint*);
        typedef void (APIENTRY *_glBindVertexArray) (GLuint);
        _glGenVertexArrays glGenVertexArrays;
         _glBindVertexArray glBindVertexArray;
        glGenVertexArrays = (_glGenVertexArrays) QOpenGLWidget::context()->getProcAddress("glGenVertexArrays");
        glBindVertexArray = (_glBindVertexArray) QOpenGLWidget::context()->getProcAddress("glBindVertexArray");




        //*****************************
        //********** GENERAL **********
        //*****************************


        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        programGeneral = new QOpenGLShaderProgram();
        programGeneral->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/simpleshader.vert");
        programGeneral->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/simpleshader.frag");

        programGeneral->bindAttributeLocation("position",0);

        programGeneral->link();
        if(!programGeneral->isLinked())
        {
                cout << "Shader program has not linked" << endl << endl << "Log: " << endl << endl << programGeneral->log().toStdString();
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
        initializeGround();
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
        //initializePlane(glm::vec3(d, f, -d),glm::vec3(d, f, d),glm::vec3(-d, f, d),glm::vec3(-d, f, -d));
        //right
        initializePlane(glm::vec3(x_max, f, z_min),glm::vec3(x_max, r, z_min),glm::vec3(x_max, r, z_max),glm::vec3(x_max, f, z_max));
        //initializePlane(glm::vec3(d, f, -d),glm::vec3(d, r, -d),glm::vec3(d, r, d),glm::vec3(d, f, d));
        //front
        initializePlane(glm::vec3(x_max, f, z_max),glm::vec3(x_min, f, z_max),glm::vec3(x_min, r, z_max),glm::vec3(x_max, r, z_max));
        //initializePlane(glm::vec3(d, f, d),glm::vec3(-d, f, d),glm::vec3(-d, r, d),glm::vec3(d, r, d));
        //left
        initializePlane(glm::vec3(x_min, f, z_min),glm::vec3(x_min, r, z_min),glm::vec3(x_min, r, z_max),glm::vec3(x_min, f, z_max));
        //initializePlane(glm::vec3(-d, f, -d),glm::vec3(-d, r, -d),glm::vec3(-d, r, d),glm::vec3(-d, f, d));
        //back
        initializePlane(glm::vec3(x_min, f, z_min),glm::vec3(x_max, f, z_min),glm::vec3(x_max, r, z_min),glm::vec3(x_min, r, z_min));
        //initializePlane(glm::vec3(-d, f, -d),glm::vec3(d, f, -d),glm::vec3(d, r, -d),glm::vec3(-d, r, -d));


        //addTriangle
        initializeTriangle(tri1, tri2, tri3);

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glEnable(GL_DEPTH_TEST);

        generalShaderId = programGeneral->programId();

        glUseProgram(generalShaderId);

        if(particleShaderId != generalShaderId)
            std::cout << "****** general shader id e particle shader id sono diversi" << std::endl;
        else
            std::cout <<" ****** the ids are the same :()" << std::endl;

        glUseProgram(0);
        glBindVertexArray(0);

        programGeneral->release();






        //*********************************
        //**********  PEOPLE  *************
        //*********************************


        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        program = new QOpenGLShaderProgram();
        program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/particle.vert");
        program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/particle.frag");

        program->bindAttributeLocation("position",0);
        program->bindAttributeLocation("value",1);

        program->link();
        if(!program->isLinked())
        {
                cout << "Shader program has not linked" << endl << endl << "Log: " << endl << endl << program->log().toStdString();
                QApplication::quit();
        }
        program->bind();

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glEnable(GL_DEPTH_TEST);


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

        //*************************
        //*********cal3d***********
        //*************************
//        initializeCal3d();
//        createCal3dModel();





        particleShaderId = program->programId();
        activeId = particleShaderId;


        glUseProgram(0);
        program->release();





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

    //std::cout<< " 2. paint GL" <<std::endl;

    if(particleMode)
    {

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);




        //*****************************
        //********** GENERAL **********
        //*****************************

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        programGeneral->bind();
        activeId = generalShaderId;

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

        //*********************************
        //**********  Triangle  ***********
        //*********************************
        if(triangle_on)
        {
            for(uint i = 0; i < trianglesVBO.size(); i++)
            {
                glEnableVertexAttribArray(0);
                glBindBuffer(GL_ARRAY_BUFFER,trianglesVBO[i]);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

                glDrawArrays(GL_TRIANGLES, 0, 3);
            }
        }



        //unbind
        glBindBuffer(GL_ARRAY_BUFFER,0);
        glBindBuffer(GL_ARRAY_BUFFER,0);



        programGeneral->release();










        //*********************************
        //**********  PEOPLE  *************
        //*********************************


//        glUseProgram(particleShaderId);
//        activeId = particleShaderId;
        program->bind();

        for(uint i= 0; i<positions.size(); i= i+3)
        {
            QVector3D par_pos;
            par_pos.setX(positions[i]);
            par_pos.setY(positions[i+1]);
            par_pos.setZ(positions[i+2]);



//            program->bind();
            program->setUniformValue("color", QVector4D(0.5f, 0.5f, 0.5f, 1.0f));
//            glUniform4f(glGetUniformLocation(particleShaderId, "color"), 0.75f, 0.8f, 0.9f, 1.0f);
            program->setUniformValue("particle_pos", par_pos);
//            glUniform3f(glGetUniformLocation(particleShaderId, "particle_pos"), positions[i], positions[i+1], positions[i+2]);
            program->setUniformValue("radius", radius);
//            glUniform1f(glGetUniformLocation(particleShaderId, "radius"), radius);

            glClearColor(0.8f, 0.8f, 0.8f, 1.0f);

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

            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);


            //unbind
            glBindBuffer(GL_ARRAY_BUFFER,0);
            glBindBuffer(GL_ARRAY_BUFFER,0);

            glDisableVertexAttribArray(0);

        }



        //*********************************
        //**********  SPHERE  ************
        //*********************************
        //draw the sphere as a big particle
        enum
        {
            VERTICES,
            VALUE
        };

        if(sphere_on)
        {
            QVector3D center(sphere_center.x, sphere_center.y, sphere_center.z );
            float rad = sphere_radius + sphere_radius_delta;

            program->setUniformValue("color", QVector4D(0.8f, 0.2f, 0.0f, 1.0f));
            program->setUniformValue("particle_pos", center);
            program->setUniformValue("radius", rad);

            glEnableVertexAttribArray(VERTICES);
            glBindBuffer(GL_ARRAY_BUFFER,quadVBO);
            glVertexAttribPointer(VERTICES, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

            glEnableVertexAttribArray(VALUE);
            glBindBuffer(GL_ARRAY_BUFFER,valueVBO);
            glVertexAttribPointer(VALUE, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

            //unbind
            glBindBuffer(GL_ARRAY_BUFFER,0);
            glBindBuffer(GL_ARRAY_BUFFER,0);

            glDisableVertexAttribArray(0);
        }


        //*********************************
        //**********  POSITION DOT  *******
        //*********************************
        //it is a red dot the show the new intiali position of the particles, it is itself a particle
        //and if in velocity mode, also point of the selected direction

        QVector3D dot_pos(pers_initial_position.x, pers_initial_position.y, pers_initial_position.z );
        float dot_rad = 0.01f;

        program->setUniformValue("color", QVector4D(1.0f, 0.0f, 0.0f, 1.0f));
        program->setUniformValue("particle_pos", dot_pos);
        program->setUniformValue("radius", dot_rad);

        glEnableVertexAttribArray(VERTICES);
        glBindBuffer(GL_ARRAY_BUFFER,quadVBO);
        glVertexAttribPointer(VERTICES, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

        glEnableVertexAttribArray(VALUE);
        glBindBuffer(GL_ARRAY_BUFFER,valueVBO);
        glVertexAttribPointer(VALUE, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        //unbind
        glBindBuffer(GL_ARRAY_BUFFER,0);
        glBindBuffer(GL_ARRAY_BUFFER,0);

        glDisableVertexAttribArray(0);

        //velocity direction

        QVector3D dir_pos = dot_pos + QVector3D(pers_i_velocity_x/10.0f,pers_i_velocity_y/10.0f,pers_i_velocity_z/10.0f);

        program->setUniformValue("color", QVector4D(0.0f, 0.0f, 1.0f, 1.0f));
        program->setUniformValue("particle_pos", dir_pos);
        program->setUniformValue("radius", dot_rad);
        glEnableVertexAttribArray(VERTICES);
        glBindBuffer(GL_ARRAY_BUFFER,quadVBO);
        glVertexAttribPointer(VERTICES, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(VALUE);
        glBindBuffer(GL_ARRAY_BUFFER,valueVBO);
        glVertexAttribPointer(VALUE, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        //unbind
        glBindBuffer(GL_ARRAY_BUFFER,0);
        glBindBuffer(GL_ARRAY_BUFFER,0);
        glDisableVertexAttribArray(0);





        program->release();




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
            program->setUniformValue("color", QVector4D(0.05, 0.05, 0.15, 1.0));
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
		distance += 0.01f * (event->y() - lastMousePos.y());
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

	projectionMatrix.perspective(60, aspect, 0.01, 100.0);
    program->bind();
    program->setUniformValue("projection", projectionMatrix);
    program->release();

    programGeneral->bind();
    programGeneral->setUniformValue("projection", projectionMatrix);
    programGeneral->release();

//    glUseProgram(activeId);
//    glUniformMatrix4fv(glGetUniformLocation(activeId, "projection"), 1, GL_FALSE, projectionMatrix.data());
//    glUseProgram(0);

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


//    glUseProgram(activeId);
//    glUniformMatrix4fv(glGetUniformLocation(activeId, "modelview"), 1, GL_FALSE, modelviewMatrix.data());
//    glUniformMatrix4fv(glGetUniformLocation(activeId, "normalMatrix"), 1, GL_FALSE, modelviewMatrix.data());
//    glUseProgram(0);

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

    if(positions.size() < max_num_of_people )
    {
        if(fountain_mode)
            prsan.addPerson(num_people_per_frame);
        else
            prsan.addPerson(1);
    }
    if(positions.size() == 0)
        initializeAnimation();


    //animateCal3dModel(current_time);
    current_time + 0.01f;

    update();
}

void GLWidget::initializeAnimation()
{
    //initialize animation
    prsan = prsanimation(fixed_y_on);
    prsan.setUpdateMode(selected_update_mode);
    //prsan.setRoomParam(floor_y, cell_dim_param);
    prsan.setRoomParam(floor_y, cell_pos_r, cell_pos_l, cell_pos_b, cell_pos_f);
    prsan.setTriangleParam(tri1,tri2, tri3);
    prsan.setSphereParam(sphere_center, sphere_radius);
    if(num_people_per_frame > max_num_of_people)
        prsan.setPersonParam(max_num_of_people, pers_lifetime, pers_bouncing);
    else
        prsan.setPersonParam(num_people_per_frame, pers_lifetime, pers_bouncing);
    prsan.setFountain(pers_initial_position.x, pers_initial_position.y, pers_initial_position.z, pers_fountain_y);
    prsan.setInitialVelocity(pers_i_velocity_x, pers_i_velocity_y, pers_i_velocity_z);
    prsan.setFountainMode(fountain_mode);
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

void GLWidget::initializeCal3d()
{
    myCoreModel = new CalCoreModel("dummy");

    //load skeleton
    myCoreModel->loadCoreSkeleton("/home/al/Documents/Un/animation/lab/2crowdanimation/cal3d/zombi/data/zombi/Skeleton.csf");

    //load animations
    idleAnimationId = myCoreModel->loadCoreAnimation("/home/al/Documents/Un/animation/lab/2crowdanimation/cal3d/zombi/data/zombi/Idle.caf");
    walkAnimationId = myCoreModel->loadCoreAnimation("/home/al/Documents/Un/animation/lab/2crowdanimation/cal3d/zombi/data/zombi/WalkForward.caf");

    //load mesh
    upperBodyMeshId = myCoreModel->loadCoreMesh("hero_upperbody.cmf");
    lowerBodyMeshId = myCoreModel->loadCoreMesh("hero_lowerbody.cmf");

    // load materials
    upperBodyChainmailMaterialId = myCoreModel->loadCoreMaterial("hero_upperbody_chainmail.crf");
    upperBodyPlatemailMaterialId = myCoreModel->loadCoreMaterial("hero_upperbody_platemail.crf");

}


void GLWidget::createCal3dModel()
{
    std::cout << "arrivato sano e salvo qui" << std::endl;

    myModel = new CalModel(myCoreModel);



    if(!myModel->attachMesh(upperBodyMeshId))
    {
        // error handling ...
        std::cerr << " mesh not attached correctly " << std::endl;
    }


    if(!myModel->detachMesh(upperBodyMeshId))
    {

    }

    //set level of detail
    myModel->setLodLevel(0.5f);


    // assign a material for each material thread/set pair
    const int CHAINMAIL_MATERIAL_SET = 0;
    const int PLATEMAIL_MATERIAL_SET = 1;

    myModel->setMaterialSet(CHAINMAIL_MATERIAL_SET);
    myModel->getMesh(upperBodyMeshId)->setMaterialSet(PLATEMAIL_MATERIAL_SET);



}

void GLWidget::animateCal3dModel(float elapsedSeconds)
{

    myModel->getMixer()->clearCycle(idleAnimationId, 0.3f);
    myModel->getMixer()->blendCycle(walkAnimationId, 0.8f, 0.3f);
//    myModel->getMixer()->executeAction(walkAnimationId, 0.3f, 0.3f);
    myModel->update(elapsedSeconds);

}





//***********************************
//******* ground ********************
//***********************************
void GLWidget::initializeGround()
{
    ground = groundgrid();
    ground.createGrid(10,10);
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

void GLWidget::initializeTriangle( glm::vec3 ver1, glm::vec3 ver2, glm::vec3 ver3)
{

    GLfloat triangleVertices[] =
    {
         // Positions
        ver1.x,  ver1.y, ver1.z,
        ver2.x,  ver2.y, ver2.z,
        ver3.x,  ver3.y, ver3.z,
    };
    GLuint tempVBO;
    //position
    glGenBuffers(1, &tempVBO);
    //Generate, bind and fill VBO for vertices
    glBindBuffer(GL_ARRAY_BUFFER, tempVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), &triangleVertices, GL_STATIC_DRAW);
    trianglesVBO.push_back(tempVBO);
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
    radius = (float)rad/100.0f;;
}

void GLWidget::update_max_num(int num)
{
    max_num_of_people = num;
//    initializeAnimation();
}


void GLWidget::update_fountain_vel_y(float vy)
{
    pers_fountain_y = vy;
//    resetAnimation();
}

void GLWidget::setFountainMode(bool b)
{
    fountain_mode = b;
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
    selected_update_mode = (update_modes)mode;
    initializeAnimation();
}
void GLWidget::update_vel(float x_delta, float y_delta, float z_delta)
{
    pers_i_velocity_x = pers_i_velocity_x + x_delta;
    pers_i_velocity_y = pers_i_velocity_y + y_delta;
    pers_i_velocity_z = pers_i_velocity_z + z_delta;
}
