#include "prsanimation.h"

prsanimation::prsanimation(bool fix_y_on)
{
    fixed_y_on = fix_y_on;
}

void prsanimation::initializeValues()
{
    planes.clear();
    triangles.clear();
    spheres.clear();
    triangles.clear();
    current_people.clear();
    createRoom();
    createTriangle();
    createSphere();
    createPeople();
}

void prsanimation::setUpdateMode(int selected)
{
    if(selected == 0)
        current_method = Person::UpdateMethod::EulerOrig;
    else if(selected == 1)
        current_method = Person::UpdateMethod::EulerSemi;
    else if(selected == 2)
        current_method = Person::UpdateMethod::Verlet;
    else
        current_method = Person::UpdateMethod::EulerSemi;
}
void prsanimation::setPersonParam(int num_pers, float life, float bounce, float radius_in)
{
    std::cout << "set " <<num_pers << " number of particles" << std::endl;
    num_people_per_frame = num_pers;
    lifetime = life;
    bouncing_par = bounce;
    radius = radius_in;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~ANIMATE~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
std::vector<float> prsanimation::animate_frame()
{

    std::vector<float> returned_positions;
    for (uint i = 0; i < current_people.size(); i++)
    {
        Person p = current_people[i];

        //orientation changed bool
        p.setVelocityChangeBool(false);


        // simulation loop
        if (p.getLifetime() > 0)
        {



            if(crowd_b)
            {
                if(avoidance_on)
                    crowdUpdatePos(p,i);
                else collisionUpdatePos(p, i);
//                collisionUpdatePos(p, i);
//                pathUpdatePos(p, i);


            }

            else
            {
              pathUpdatePos(p, i);

            }


            current_people[i] = p;

            float x = p.getCurrentPosition().x;
            float y = p.getCurrentPosition().y;
            float z = p.getCurrentPosition().z;

            returned_positions.push_back(x);
            returned_positions.push_back(y);
            returned_positions.push_back(z);
        }
    }

    return returned_positions;

}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~ADD~PERSONS
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void prsanimation::addPerson(int new_person)
{

    for (int i = 0; i < new_person; i++)
    {        
        Person p1;
        float in_x = (rand() % (ground.getColumnDimension()-1) * ground.getCellDim()) - ((ground.getColumnDimension()/2) * ground.getCellDim());
        float in_z = (rand() % (ground.getRowDimension()-1) * ground.getCellDim()) - ((ground.getRowDimension()/2) * ground.getCellDim());

        if(crowd_b)
             p1 = Person(in_x, initial_y, in_z);
        else
        {
            glm::vec3 new_initial = ground.getStartPos();
            p1 = Person(new_initial.x, initial_y, new_initial.z); //initial position of the particle
        }
        p1.setLifetime(lifetime);
        p1.setBouncing(bouncing_par);
        p1.addForce(0, g_a, 0);
        float vx,vy,vz;
        vx = velocity_x;
        vy = velocity_y;
        vz = velocity_z;

        if(fixed_y_on)
            p1.setFixedY(fixed_y_on, initial_y);

        if(crowd_b)
        {

            vx = velocity_x + (((double) rand() / (RAND_MAX))  - 0.5f) ;
            vy = 0.0;
            vz = velocity_z + (((double) rand() / (RAND_MAX))  - 0.5f);
            glm::vec3 ini_vel = glm::vec3(vx,vy,vz);
            ini_vel = glm::normalize(ini_vel);
            p1.setVelocity(vel_multiplier*ini_vel);
        }
        else
        {
            glm::vec3 ini_vel = glm::vec3(vx,vy,vz);
            ini_vel = glm::normalize(ini_vel);
            p1.setVelocity(vel_multiplier*ini_vel);
        }


        glm::vec3 defined_velocity = p1.getVelocity();
        p1.setOrientationAngle(orientationAngle(defined_velocity));


        current_people.push_back(p1);
    }
}






//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~PARAMETERS
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void prsanimation::setPathMode(int goal_x, int goal_z, int ini_x, int ini_z)
{
    glm::vec3 pos = ground.getCellPosition(ini_x, ini_z);
    initial_x = pos.x;
    initial_z = pos.z;
    ground.setGoal(goal_x,goal_z);
    ground.setStart(ini_x, ini_z);

    //TEST
    ground.computeAstar();
    pathPos = ground.getPathPositions();
    current_position = 1;
}

void prsanimation::setInitialVelocity(float vx, float vy, float vz)
{
    velocity_x = vx;
    velocity_y = vy;
    velocity_z = vz;
}
void prsanimation::setCrowdMode(bool b)
{
    crowd_b = b;
}
void prsanimation::createPeople()
{
    addPerson(num_people_per_frame);
}

void prsanimation::setSphereParam(glm::vec3 cent, float rad)
{
    sphere_center = cent;
    sphere_radius = rad;
    createSphere();
}
void prsanimation::createSphere()
{
    spheres.clear();
    //define the sphere for collision
    Sphere sphere(sphere_center, sphere_radius);
    spheres.push_back(sphere);
}

void prsanimation::setTriangleParam(glm::vec3 ver1, glm::vec3 ver2, glm::vec3 ver3)
{
    triangle_vertex1 = ver1;
    triangle_vertex2 = ver2;
    triangle_vertex3 = ver3;
    createTriangle();
}
void prsanimation::createTriangle()
{
    triangles.clear();

    Triangle triangle1(triangle_vertex1, triangle_vertex2, triangle_vertex3);
    triangles.push_back(triangle1);
}

void prsanimation::setRoomParam(float f, float c_r, float c_l, float c_b, float c_f)
{
    floor = f;
    dim_r = c_r;
    dim_l = c_l;
    dim_b = c_b;
    dim_f = c_f;
    createRoom();
}
void prsanimation::createRoom()
{
    planes.clear();

    //room parameters
    float f = floor;

    // define planes for collision
    //floor
    glm::vec3 punt(0.0f, f, 0.0f);
    glm::vec3 normal(0, 1, 0);
    Plane plane(punt, normal);
    planes.push_back(plane);
    //right
    glm::vec3 point2(dim_r, 0.0f, 0.0f);
    glm::vec3 normal2(-1, 0, 0);
    Plane plane2(point2, normal2);
    planes.push_back(plane2);
    //left
    glm::vec3 point3(dim_l, 0.0f, 0.0f);
    glm::vec3 normal3(1, 0, 0);
    Plane plane3(point3, normal3);
    planes.push_back(plane3);
    //back
    glm::vec3 point4(0.0f, 0.0f, dim_b);
    glm::vec3 normal4(0, 0, -1);
    Plane plane4(point4, normal4);
    planes.push_back(plane4);
    //front
    glm::vec3 point5(0.0f, 0.0f, dim_f);
    glm::vec3 normal5(0, 0, 1);
    Plane plane5(point5, normal5);
    planes.push_back(plane5);
}




void prsanimation::setGravityPatam(float grav)
{
    g_a = grav;
}







//UNUSED GETTERS
void prsanimation::getVelocity(uint pers, float &x, float &y, float &z)
{
    Person p = current_people[pers];
    glm::vec3 velocity = p.getVelocity();
    velocity = glm::normalize(velocity);
    x = velocity.x;
    y = velocity.y;
    z = velocity.z;
}

void prsanimation::getVelocity(uint pers, float &x,float &z)
{
    Person p = current_people[pers];
    glm::vec2 velocity = glm::vec2(p.getVelocity().x, p.getVelocity().z);
    velocity = glm::normalize(velocity);
    x = velocity.x;
    z = velocity.y;
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~ORIENTATION
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
float prsanimation::computeOrienation(uint pers)
{
    Person p = current_people[pers];

    glm::vec3 defined_velocity = p.getVelocity();
    return orientationAngle(defined_velocity);
}
float prsanimation::orientationAngle(glm::vec3 velocity)
{
    float dot = velocity.x*0.0f + velocity.z*(1.0f);
    float det = velocity.x*(1.0f) - velocity.z*0.0f;

    float angle = atan2(det, dot);
    angle = (180.0f*angle)/M_PI;
    return angle;

}
float prsanimation::updateOrientation(uint pers, float new_angle)
{

    Person p = current_people[pers];
    float cur_orienation = p.getOrientationAngle();

    //parameters
    float epsilon = 2.0f;
    float delta = 10.0f;
    int frameOfRotation = 10;



    //under a certain treshold the orientation are considered the same
    if(new_angle >= cur_orienation - epsilon && new_angle <= cur_orienation +epsilon)
    {
        current_people[pers].setOrientationAngle(new_angle);
        return new_angle;
    }
    else
    {
        //the new angle is bigger then the stored one
        if(new_angle > cur_orienation)
        {
            float diff = new_angle - cur_orienation;
            //manage the case of a small angle betwen the border -180 and 180
            if(diff > 180)
            {
                diff = -(360 - diff);
            }
            if(abs(diff) >delta)
            {
                cur_orienation = cur_orienation + diff/frameOfRotation;

             }
            else
            {
                cur_orienation = cur_orienation + delta;
            }
        }
        else
        {
            float diff = cur_orienation - new_angle;

            if(diff > 180)
            {
                diff = -(360 - diff);
            }

            if(abs(diff) >delta)
            {
                cur_orienation = cur_orienation - diff/frameOfRotation;
             }
            else
            {
                cur_orienation = cur_orienation - delta;
            }
        }
        //check if we get over the border and fix the value
        if(abs(cur_orienation) >= 180)
            cur_orienation = -(180-abs(180-cur_orienation));
        current_people[pers].setOrientationAngle(cur_orienation);
        return cur_orienation;



    }



}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~STEERING~COLLISION~AVOIDANCE
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void prsanimation::check_for_obstacles(Person &p, uint i)
{
    glm::vec3 cur_vel = p.getVelocity();
    glm::vec3 cur_pos = p.getCurrentPosition();
    glm::vec3 ahead_v = cur_pos + glm::normalize(cur_vel)*MAX_SEE_AHEAD;
    glm::vec3 half_ahead_v = ahead_v*0.5f;

    //check for collision with other agent and normal obstacles
    //all the obstacles are considered like spheres

    uint mostClosePers = 0;
    bool futurePersCollision = false;
    float max_distance = ground.getRowDimension()*ground.getCellDim()*ground.getColumnDimension()*ground.getCellDim();
    float closestPersDist = max_distance;

    //COLLISION WITH PERSONS
    for (uint prs_i = 0; prs_i < current_people.size(); prs_i++)
    {
        if(prs_i != i)
        {
            glm::vec3 other_pos = current_people[prs_i].getCurrentPosition();
            //actual distance
            glm::vec3 distance_v = cur_pos - other_pos;
            //distance with ahead and half ahead
            glm::vec3 distance_ahead_v = ahead_v - other_pos;
            glm::vec3 distance_half_ahead_v = half_ahead_v - other_pos;

            float dist = glm::length(distance_v);
            float dist_ahead = glm::length(distance_ahead_v);
            float dist_half_ahead = glm::length(distance_half_ahead_v);

            if((dist_ahead < 3*radius) || (dist_half_ahead < 3*radius) )
            {
                std::cout << std::endl;
                std::cout << " max possible distance " << max_distance << std::endl;
                std::cout << " >>>>>>>>> future person collision detected <<<<<<<<" << std::endl;
                if(dist < closestPersDist)
                {
                    futurePersCollision = true;
                    mostClosePers = prs_i;
                    closestPersDist = dist;
                }
            }
        }
    }

    uint mostCloseObs = 0;
    bool futureObsCollision = false;
    float closestObsDist = max_distance;

    //COLLISION WITH STATIC OBSTACLES --> same radius of person for now
    std::vector<groundgrid::cellT> obstacles = ground.getObstaclesPositions();
    for(uint obs = 0; obs < obstacles.size(); obs++)
    {
        //obstacle pos
        glm::vec3 other_pos = ground.getCellPosition(obstacles[obs].first, obstacles[obs].second);
        //actual distance
        glm::vec3 distance_v = cur_pos - other_pos;
        //distance with ahead and half ahead
        glm::vec3 distance_ahead_v = ahead_v - other_pos;
        glm::vec3 distance_half_ahead_v = half_ahead_v - other_pos;

        float dist = glm::length(distance_v);
        float dist_ahead = glm::length(distance_ahead_v);
        float dist_half_ahead = glm::length(distance_half_ahead_v);

        if((dist_ahead < 3*radius) || (dist_half_ahead < 3*radius) )
        {
            std::cout << std::endl;
            std::cout << " max possible distance " << max_distance << std::endl;
            std::cout << " >>>>>>>>> future obstacle collision detected <<<<<<<<" << std::endl;
            if(dist < closestObsDist)
            {
                futureObsCollision = true;
                mostCloseObs = obs;
                closestObsDist = dist;
            }
        }
    }

    glm::vec3 avoidance_v = glm::vec3(0.0f);
    if(futurePersCollision || futureObsCollision)
    {
       glm::vec3 selected_obstacle =  glm::vec3(0.0f);
       if(futurePersCollision && futureObsCollision)
       {
            std::cout << "both person and object collision detected at distances : pers " << closestPersDist << "  obs: " << closestObsDist << std::endl;
       }
       else if(futurePersCollision && !futureObsCollision)
       {
           selected_obstacle = current_people[mostClosePers].getCurrentPosition();
           std::cout << "only person collision detected at distance : " << closestPersDist << std::endl;
       }
       else if(!futurePersCollision && futureObsCollision)
       {
           selected_obstacle = ground.getCellPosition(obstacles[mostCloseObs].first, obstacles[mostCloseObs].second);
           std::cout << "only obstacle collision detected at distace: "<< closestObsDist << std::endl;
       }

       avoidance_v = ahead_v - selected_obstacle;
       avoidance_v = glm::normalize(avoidance_v)*MAX_AVOID_FORCE;

       glm::vec3 current_vel = p.getVelocity();
       current_vel = current_vel + avoidance_v;
       current_vel = glm::normalize(current_vel)*vel_multiplier;
       std::cout << "here" << std::endl;
       p.setVelocity(current_vel);
       if(current_method == Person::UpdateMethod::Verlet)
       {
           glm::vec3 updated_pos = p.getCurrentPosition();
           glm::vec3 updated_vel = p.getVelocity();
           p.setPreviousPosition(updated_pos.x - (updated_vel.x)*dt, updated_pos.y, updated_pos.z - (updated_vel.z)*dt);
       }
    }




}


















//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~A* UPDATE~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void prsanimation::pathUpdatePos(Person &p, uint i)
{

    glm::vec3 nextStep = ground.getCellPosition(pathPos[current_position].first, pathPos[current_position].second);
    glm::vec3 curPos = p.getCurrentPosition();
    glm::vec3 curVel = p.getVelocity();
    glm::vec3 distance_v = (nextStep - curPos);
    float distance = glm::length(distance_v);


//    std::cout << "  velocity: :" << p.getVelocity().x << " , "<< p.getVelocity().y << " , "<< p.getVelocity().z << distance << std::endl;

    //SET VELOCITY
//    distance_v = glm::normalize(distance_v);
    glm::vec3 updated_velocity = curVel + distance_v;
    updated_velocity = glm::normalize(updated_velocity);
    p.setVelocity(updated_velocity*vel_multiplier);


    //SET VERLET VELOCITY
    if(current_method == Person::UpdateMethod::Verlet)
    {
        glm::vec3 verl_pos = p.getCurrentPosition();
        glm::vec3 verl_vel = p.getVelocity();
        p.setPreviousPosition(verl_pos.x - (verl_vel.x)*dt, verl_pos.y, verl_pos.z - (verl_vel.z)*dt);
    }


    //UPDATE PERSON MOVEMENT
    p.updatePerson(dt, current_method);
    p.setLifetime(p.getLifetime() - dt);



    //**********************
    //**plane collisions****
    //**********************
    planeCollisionDetection(p);



    //AFTER CHECK FOR PATH
    glm::vec3 newPos = p.getCurrentPosition();
    glm::vec3 newDistance_v = (nextStep - newPos);
    glm::vec3 newVel = p.getVelocity();
    float newDistance = glm::length(newDistance_v);

    //update the next cell
//    if( (distance < ground.getCellDim()/2.0f + dt) || (newDistance > distance))
    if( (newDistance < ground.getCellDim()/2.0f + dt))
    {

        if( current_position == (pathPos.size() - 1))
        {
            p.setLifetime(0.0f);
            return;
        }
        current_position++;

        nextStep = ground.getCellPosition(pathPos[current_position].first, pathPos[current_position].second);
        distance_v = nextStep - curPos;
        distance = glm::length(distance_v);
        distance_v = glm::normalize(distance_v);

        //set the velocity to the new waypoint
        updated_velocity = newVel + distance_v;
        updated_velocity = glm::normalize(updated_velocity);
        p.setVelocity(updated_velocity*vel_multiplier);


//        p.setVelocity(distance_v*vel_multiplier);
        //set verlet velocity
        if(current_method == Person::UpdateMethod::Verlet)
        {
            glm::vec3 updated_pos = p.getCurrentPosition();
            glm::vec3 updated_vel = p.getVelocity();
            p.setPreviousPosition(updated_pos.x - (updated_vel.x)*dt, updated_pos.y, updated_pos.z - (updated_vel.z)*dt);
        }
    }



    //COLLISION AVOIDANCE CHECK
    if(avoidance_on)
        check_for_obstacles(p,i);
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~COLLISION~UPDATE
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void prsanimation::crowdUpdatePos(Person &p, uint i)
{
    if(current_method == Person::UpdateMethod::Verlet)
    {
        if(p.getLifetime() > (lifetime - dt))
        {
            glm::vec3 ini_pos = p.getCurrentPosition();
            glm::vec3 ini_vel = p.getVelocity();
            p.setPreviousPosition(ini_pos.x - (ini_vel.x)*dt, ini_pos.y - (ini_vel.y)*dt, ini_pos.z - (ini_vel.z)*dt);
        }
    }
    p.updatePerson(dt, current_method);
    p.setLifetime(p.getLifetime() - dt);

    //**********************
    //**plane collisions****
    //**********************
    planeCollisionDetection(p);

    //**********************
    //**person collision****
    //**********************
    personCollisionDetection(p, i);

    //**********************
    //**obstacle avoidance**
    //**********************
    check_for_obstacles(p,i);

}








//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~COLLISION~UPDATE
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void prsanimation::collisionUpdatePos(Person &p, uint i)
{
    if(current_method == Person::UpdateMethod::Verlet)
    {
        if(p.getLifetime() > (lifetime - dt))
        {
            glm::vec3 ini_pos = p.getCurrentPosition();
            glm::vec3 ini_vel = p.getVelocity();
            p.setPreviousPosition(ini_pos.x - (ini_vel.x)*dt, ini_pos.y - (ini_vel.y)*dt, ini_pos.z - (ini_vel.z)*dt);
        }
    }
    p.updatePerson(dt, current_method);
    p.setLifetime(p.getLifetime() - dt);

    //**********************
    //**plane collisions****
    //**********************
    planeCollisionDetection(p);

    //**********************
    //**triangle collision*
    //**********************
    for (uint t_i = 0; t_i < triangles.size(); t_i++)
    {
        float disact, disant;

        Triangle t = triangles[t_i];
        disant = t.distPoint2Plane(p.getPreviousPosition());
        disact = t.distPoint2Plane(p.getCurrentPosition());
        if(disant*disact < 0.0f)
        {
            if ( t.isInside(p.getCurrentPosition()))
            {

                //orientation changed bool
                p.setVelocityChangeBool(false);

                float b = p.getBouncing();

                glm::vec3 n = t.normal;
                glm::vec3 pos = p.getCurrentPosition();
                glm::vec3 newPos = (glm::dot(pos,n) + t.dconst)*n;
                p.setPosition(pos.x - (1+b)*newPos.x, pos.y - (1+b)*newPos.y, pos.z - (1+b)*newPos.z);

                glm::vec3 vel = p.getVelocity();
                glm::vec3 newVel = (glm::dot(vel,n))*n;
                p.setVelocity((vel.x - (1+b)*newVel.x), (vel.y - (1+b)*newVel.y), (vel.z - (1+b)*newVel.z));

                //update of previous position for velrlet (maybe not necessary)
                if(current_method == Person::UpdateMethod::Verlet)
                {
                    glm::vec3 normal_plane = t.normal;
                    // use distance with previous plane: disant
                    glm::vec3 normal_x_distance = normal_plane*disant;
                    glm::vec3 prev_pos = p.getPreviousPosition();
                    glm::vec3 n_o_p = prev_pos - 2.0f*normal_x_distance;
                    p.setPreviousPosition(n_o_p);
                }
            }
        }


    }

    //**********************
    //**sphere collision****
    //**********************
    for (uint s_i = 0; s_i < spheres.size(); s_i++)
    {
        bool inOld = spheres[s_i].isInside(p.getPreviousPosition());
        bool inNew = spheres[s_i].isInside(p.getCurrentPosition());

        if(!inOld and inNew)
        {

            //orientation changed bool
            p.setVelocityChangeBool(false);

            //computing the intersection
            glm::vec3 interPoint, interPoint1, interPoint2;
            float lambda1, lambda2;
            float rad = spheres[s_i].radius;
            glm::vec3 p_old = p.getPreviousPosition();
            glm::vec3 p_new = p.getCurrentPosition();
            glm::vec3 c = spheres[s_i].center;
            glm::vec3 v = p_new - p_old;
            float alpha = glm::dot(v, v);
            float beta = 2*glm::dot((v), (p_old - c));
            float gamma = glm::dot(c,c) + glm::dot(p_old,p_old) - 2*glm::dot(p_old, c) - (rad*rad);

            float delta = beta*beta - 4*alpha*gamma;
            if(delta >= 0)
            {
                lambda1 = -beta + sqrt(delta);
                lambda2 = -beta - sqrt(delta);



                interPoint1 = p_old + lambda1*v;
                interPoint2 = p_old + lambda2*v;
                //default value
                interPoint = interPoint1;

            }
            else
                std::cerr << "segment-sphere intersection have no solution" << std::endl;

            if(lambda1 >= 0.0f and lambda1 <= 1.0f)
                interPoint = interPoint1;
            else if(lambda2 >= 0.0f and lambda2 <= 1.0f)
                interPoint = interPoint2;
            else
                std::cerr << "segment-sphere intersection : none of the 2 lambdas is between 0 and 1"<< std::endl;


            glm::vec3 norm_vec = glm::normalize(interPoint - c);
            Plane tang_plane(interPoint, norm_vec);

            //bouncing
            float b = p.getBouncing();

            glm::vec3 pos = p.getCurrentPosition();

            glm::vec3 newPos = (glm::dot(pos,norm_vec) + tang_plane.dconst)*norm_vec;
            p.setPosition(pos.x - (1+b)*newPos.x, pos.y - (1+b)*newPos.y, pos.z - (1+b)*newPos.z);

            glm::vec3 vel = p.getVelocity();

            glm::vec3 newVel = (glm::dot(vel,norm_vec))*norm_vec;
            p.setVelocity((vel.x - (1+b)*newVel.x), (vel.y - (1+b)*newVel.y), (vel.z - (1+b)*newVel.z));

        }
        if(inNew)
        {
                    //std::cerr << "NOW INSIDE THE SPHERE" << std::endl;
        }
    }

    //**********************
    //**person collision****
    //**********************
    personCollisionDetection(p, i);

}


void prsanimation::planeCollisionDetection(Person &p)
{
    //**********************
    //**floor collisions****
    //**********************
    for (uint p_j = 0; p_j < planes.size(); p_j++)
    {
        float disact, disant;

        Plane plane = planes[p_j];
        disant = plane.distPoint2Plane(p.getPreviousPosition());
        disact = plane.distPoint2Plane(p.getCurrentPosition());


        if (disant*disact < 0.0f)
        {
            std::cout << "here in plane collision" << std::endl;
            //orientation changed bool
            p.setVelocityChangeBool(true);

            float b = p.getBouncing();

            glm::vec3 n = plane.normal;
            glm::vec3 pos = p.getCurrentPosition();
            glm::vec3 newPos = (glm::dot(pos,n) + plane.dconst)*n;
            p.setPosition(pos.x - (1+b)*newPos.x, pos.y - (1+b)*newPos.y, pos.z - (1+b)*newPos.z);

            glm::vec3 vel = p.getVelocity();
            glm::vec3 newVel = (glm::dot(vel,n))*n;
            p.setVelocity((vel.x - (1+b)*newVel.x), (vel.y - (1+b)*newVel.y), (vel.z - (1+b)*newVel.z));

            //update of previous position for velrlet (maybe not necessary)
            if(current_method == Person::UpdateMethod::Verlet)
            {
                glm::vec3 normal_plane = planes[p_j].normal;
                // use distance with previous plane: disant
                glm::vec3 normal_x_distance = normal_plane*disant;
                glm::vec3 prev_pos = p.getPreviousPosition();
                glm::vec3 n_o_p = prev_pos - 2.0f*normal_x_distance;
                p.setPreviousPosition(n_o_p);
            }
        }
    }
}
void prsanimation::personCollisionDetection(Person &p, uint i)
{
    for (uint prs_i = 0; prs_i < current_people.size(); prs_i++)
    {
        if(prs_i != i)
        {
            Person other_prs = current_people[prs_i];
            glm::vec3 distance_v = other_prs.getCurrentPosition() - p.getCurrentPosition();
            float dist = glm::length(distance_v);
            if(dist < 2*radius)
            {
                //collision
                float b = p.getBouncing();

                glm::vec3 o_dist_dir = -(glm::normalize(distance_v));

                //1
                glm::vec3 cur_pos = p.getCurrentPosition();
                p.setPosition(p.getPreviousPosition());

                glm::vec3 new_vel = p.getVelocity();
                new_vel = -new_vel*o_dist_dir*b;
                new_vel = glm::normalize(new_vel)*vel_multiplier;
                p.setVelocity(new_vel);

                if(current_method == Person::UpdateMethod::Verlet)
                {
                    p.setPreviousPosition(cur_pos);

                }

                //2
//                        glm::vec3 interPoint = p.getCurrentPosition() + o_dist_dir/2.0f;
//                        glm::vec3 norm_vec = glm::normalize(interPoint - other_prs.getCurrentPosition());
//                        Plane tang_plane(interPoint, norm_vec);

//                        //bouncing
//                        glm::vec3 pos = p.getCurrentPosition();

//                        glm::vec3 newPos = (glm::dot(pos,norm_vec) + tang_plane.dconst)*norm_vec;
//                        p.setPosition(pos.x - (1+b)*newPos.x, pos.y, pos.z - (1+b)*newPos.z);

//                        glm::vec3 vel = p.getVelocity();

//                        glm::vec3 newVel = (glm::dot(vel,norm_vec))*norm_vec;
//                        p.setVelocity((vel.x - (1+b)*newVel.x), vel.y, (vel.z - (1+b)*newVel.z));



            }
        }
    }
}
