#ifndef PERSON_H
#define PERSON_H

#include <glm/glm.hpp>
#include <vector>


class Person
{
public:
    enum class UpdateMethod : std::int8_t { EulerOrig, EulerSemi, Verlet };

    Person();
    Person(const float& x, const float& y, const float& z);
//	Person(glm::vec3 pos, glm::vec3 vel, float bouncing = 1.0f, bool fixed = false, int lifetime = -1, glm::vec3 force = glm::vec3(0, 0, 0));
    ~Person();
    //setters
    void setPosition(const float& x, const float& y, const float& z);
    void setPosition(glm::vec3 pos);
    void setPreviousPosition(const float& x, const float& y, const float& z);
    void setPreviousPosition(glm::vec3 pos);
    void setVelocity(const float& x, const float& y, const float& z);
    void setVelocity(glm::vec3 vel);
    void setForce(const float& x, const float& y, const float& z);
    void setForce(glm::vec3 force);
    void setBouncing(float bouncing);
    void setLifetime(float lifetime);
    void setFixed(bool fixed);

    //getters
    glm::vec3 getCurrentPosition();
    glm::vec3 getPreviousPosition();
    glm::vec3 getForce();
    glm::vec3 getVelocity();
    float getBouncing();
    float getLifetime();
    bool isFixed();

    //other
    void addForce(glm::vec3 force);
    void addForce(const float& x, const float& y, const float& z);
    void updatePerson(const float& dt, UpdateMethod method = UpdateMethod::EulerOrig);

    //fix y
    void setFixedY(bool fix_y, float pos_y);

    void setVelocityChangeBool(bool set);
    bool isVelocityChanged();


    //A* path finding
    std::pair<int,int> getGoal();
    void setGoal(int i, int j);
    void setOrientationAngle(float a);
    float getOrientationAngle();



    float orientation_angle = 0.0f;

private:
    glm::vec3 m_currentPosition;
    glm::vec3 m_previousPosition;
    glm::vec3 m_force;
    glm::vec3 m_velocity;

    float m_bouncing;
    float m_lifetime;
    bool  m_fixed;

    bool m_fixed_y;
    float m_fixed_initial_y;

    //boolean to check if the velocity vector changed so the orientation
    bool m_velocity_chenged = false;



    //A* path finding
    std::pair<int,int> goal;




};
#endif // PERSON_H
