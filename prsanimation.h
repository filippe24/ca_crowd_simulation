#ifndef PRSANIMATION_H
#define PRSANIMATION_H

#include <iostream>
#include <vector>
#include "geometry.h"
#include "person.h"
#include "groundgrid.h"

class prsanimation
{
public:

    //ground for A*
    groundgrid ground;
    std::vector<groundgrid::cellT> pathPos;
    uint current_position = 0;


    prsanimation(bool fix_y_on = true);
    void initializeValues();
    void setUpdateMode(int selected);
    void setPersonParam(int num_pers, float life, float bounce, float radius_in);
    void addPerson(int new_person = 1);
    void createPeople();

    std::vector<float> animate_frame();

    void setAvoidanceOn(bool b){ avoidance_on = b;}
    bool getAvoidanceOn(){return avoidance_on;}

    void setMaxSeeAhead(float f){ MAX_SEE_AHEAD = f;}
    float getMaxSeeAhead(){ return MAX_SEE_AHEAD;}



    //SETTERS
    void setRoomParam(float f, float c_r, float c_l, float c_b, float c_f);
    void setTriangleParam(glm::vec3 ver1, glm::vec3 ver2, glm::vec3 ver3);
    void setSphereParam(glm::vec3 cent, float rad);
    void setCrowd(float max_x, float max_z);
    void setCrowdMode(bool b);
    void setInitialVelocity(float vx, float vy, float vz);

    void setGravityPatam(float grav);

    //A* path
    void setPathMode(int ini_x, int ini_z, int goal_x, int goal_y);

    //GETTER Unused
    void getVelocity(uint pers, float &x, float &y, float &z);
    void getVelocity(uint pers, float &x, float &z);

    //Orientation Smooth
    float updateOrientation(uint pers, float new_angle);
    float computeOrienation(uint pers);
    float orientationAngle(glm::vec3 velocity);

    bool obstacle_on = false;
private:

    bool fixed_y_on = true;
    bool avoidance_on = false;

    std::vector<Person> current_people;
    std::vector<Plane> planes;
    std::vector<Triangle> triangles;
    std::vector<Sphere> spheres;

    Person::UpdateMethod current_method = Person::UpdateMethod::Verlet;
    bool first_frame = true;

    void collisionUpdatePos(Person &p, uint i);
    void planeCollisionDetection(Person &p);
    void personCollisionDetection(Person &p, uint i);
    void obstacleCollisionDetection(Person &p);
    bool isInside(glm::vec3 point, glm::vec3 center, bool twodimOn);

    //A*
    void pathUpdatePos(Person &p, uint i);
    void crowdUpdatePos(Person &p, uint i);

    //STEERING~COLLISION~AVOIDANCE
    float MAX_SEE_AHEAD = 1.0f;
    float MAX_AVOID_FORCE = 0.5f;
    void check_for_obstacles(Person &p, uint i);


    //person parameters
    int num_people_per_frame;
    //initial position of particles
    float initial_x = 0.0f;
    float initial_y = 0.5f;
    float initial_z = 0.0f;

    float radius = 0.0f;


    //life
    float dt = 0.01f;
    float tini = 0.0f;
    float lifetime = 5.0f;
    //moving
    float bouncing_par = 0.6f;
    float vel_multiplier = 3.0f;
    //gravity acceleration
    float g_a = -9.8f;

    //initial velocity (not fountain mode)
    float velocity_x = 0.0f;
    float velocity_y = 0.0f;
    float velocity_z = 0.0f;

    bool crowd_b = true;

    void createRoom();
    void createTriangle();
    void createSphere();
    void createParticles();

    //Spring Forces
    glm::vec3 spring_force(int i, int j, float l0, float ke, float kd);
    float distance_particles(int i, int j);

    //room parameters
    float floor = 0.0f;
    float dim_r, dim_l, dim_f, dim_b = 1.0f;

    //triangles parameters
    glm::vec3 triangle_vertex1 = glm::vec3(0.2f, 0.6f, 0.4f);
    glm::vec3 triangle_vertex2 = glm::vec3(0.3f, 0.7f, 0.1f);
    glm::vec3 triangle_vertex3 = glm::vec3(-0.2f, 0.6f, 0.3f);

    //sphere parameters
    glm::vec3 sphere_center = glm::vec3(-0.1f, 0.3f, 0.1f );
    float sphere_radius = 0.2f;




};

#endif // PRSANIMATION_H
