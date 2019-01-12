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
void prsanimation::setPersonParam(int num_pers, float life, float bounce)
{
    std::cout << "set " <<num_pers << " number of particles" << std::endl;
    num_people_per_frame = num_pers;
    lifetime = life;
    bouncing_par = bounce;
}


//COLLISION OF PARTICLES
std::vector<float> prsanimation::animate_frame()
{

    std::vector<float> returned_positions;
    for (uint i = 0; i < current_people.size(); i++)
    {
        Person p = current_people[i];

        //orientation changed bool
        p.setVelocityChangeBool(false);


        // simulation loop
        if (p.getLifetime() > 0) {

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
            for (uint prs_i = 0; prs_i < current_people.size(); prs_i++)
            {
                if(prs_i != i)
                {
                    Person other_prs = current_people[prs_i];
                    glm::vec3 distance_v = other_prs.getCurrentPosition() - p.getCurrentPosition();
                    float dist = glm::length(distance_v);
                    if(dist < 2*0.2f)
                    {
                        //collision
                        float b = p.getBouncing();

                        glm::vec3 o_dist_dir = -(glm::normalize(distance_v));

                        //1
                        glm::vec3 cur_pos = p.getCurrentPosition();
                        p.setPosition(p.getPreviousPosition());

                        glm::vec3 new_vel = p.getVelocity();
                        new_vel = -new_vel*o_dist_dir*b;
                        new_vel = glm::normalize(new_vel)*crowd_vel_multiplier;
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
            p1 = Person(initial_x, initial_y, initial_z); //initial position of the particle
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
            p1.setVelocity(crowd_vel_multiplier*ini_vel);
            current_people.push_back(p1);
        }
        //velocity mode
        else
        {
            glm::vec3 ini_vel = glm::vec3(vx,vy,vz);
            ini_vel = glm::normalize(ini_vel);
            p1.setVelocity(crowd_vel_multiplier*ini_vel);
            current_people.push_back(p1);
        }
    }
}



void prsanimation::setPathMode(int goal_x, int goal_z, int ini_x, int ini_z)
{
    glm::vec3 pos = ground.getCellPosition(ini_x, ini_z);
    initial_x = pos.x;
    initial_z = pos.z;
    ground.setGoal(goal_x,goal_z);
    ground.setStart(ini_x, ini_z);

    //TEST

    ground.computeAstar();
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








void prsanimation::getVelocity(int pers, float &x, float &y, float &z)
{
    Person p = current_people[pers];
    glm::vec3 velocity = p.getVelocity();
    velocity = glm::normalize(velocity);
    x = velocity.x;
    y = velocity.y;
    z = velocity.z;
}

void prsanimation::getVelocity(int pers, float &x,float &z)
{
//    std::cout << "prsn :numero totale di persone :" << current_people.size() << "  persona selezionata " << pers << std::endl;
    Person p = current_people[pers];
    glm::vec2 velocity = glm::vec2(p.getVelocity().x, p.getVelocity().z);
    velocity = glm::normalize(velocity);
    x = velocity.x;
    z = velocity.y;
//    std::cout << "prsn :velocity : x" << x << " and :y " << z << std::endl;
}













//A * ALGORITHM USAGE













