#include "person.h"

#include <iostream>

Person::Person()
{

}

Person::Person(const float& x, const float& y, const float& z) :
m_previousPosition(0, 0, 0), m_force(0, 0, 0), m_velocity(0., 0., 0.), m_bouncing(1), m_lifetime(50), m_fixed(false)
{
    m_currentPosition.x = x;
    m_currentPosition.y = y;
    m_currentPosition.z = z;
}

Person::~Person()
{
}

//setters
void Person::setPosition(const float& x, const float& y, const float& z)
{
    glm::vec3 pos(x,y,z);
    m_currentPosition =  pos;
}
void Person::setPosition(glm::vec3 pos)
{
    m_currentPosition = pos;
}

void Person::setPreviousPosition(const float& x, const float& y, const float& z)
{
    glm::vec3 pos(x, y, z);
    m_previousPosition = pos;
}

void Person::setPreviousPosition(glm::vec3 pos)
{
    m_previousPosition = pos;
}

void Person::setForce(const float& x, const float& y, const float& z)
{
    glm::vec3 force(x, y, z);
    m_force = force;
}

void Person::setForce(glm::vec3 force)
{
    m_force = force;
}

void Person::addForce(const float& x, const float& y, const float& z)
{
    glm::vec3 force(x,y,z);
    m_force += force;
}

void Person::addForce(glm::vec3 force)
{
    m_force += force;
}

void Person::setVelocity(const float& x, const float& y, const float& z)
{
    glm::vec3 vel(x,y,z);
    m_velocity = vel;
}

void Person::setVelocity(glm::vec3 vel)
{
    m_velocity = vel;
}

void Person::setBouncing(float bouncing)
{
    m_bouncing = bouncing;
}

void Person::setLifetime(float lifetime)
{
    m_lifetime = lifetime;
}

void Person::setFixed(bool fixed)
{
    m_fixed = fixed;
}

//getters
glm::vec3 Person::getCurrentPosition()
{
    return m_currentPosition;
}

glm::vec3 Person::getPreviousPosition()
{
    return m_previousPosition;
}

glm::vec3 Person::getForce()
{
    return m_force;
}

glm::vec3 Person::getVelocity()
{
    return m_velocity;
}

float Person::getBouncing()
{
    return m_bouncing;
}

float Person::getLifetime()
{
    return m_lifetime;
}

bool Person::isFixed()
{
    return m_fixed;
}

void Person::updatePerson(const float& dt, UpdateMethod method)
{
    if (!m_fixed & m_lifetime > 0)
    {
        switch (method)
        {
        case UpdateMethod::EulerOrig:
        {
            m_previousPosition = m_currentPosition;
            m_currentPosition += m_velocity*dt;
            m_velocity += m_force*dt;
        }
            break;
        case UpdateMethod::EulerSemi:
        {
            m_previousPosition = m_currentPosition;
            m_velocity += m_force*dt;
            m_currentPosition += m_velocity*dt;

        }
            break;
        case UpdateMethod::Verlet:
        {
            float kd = 0.9f;

            m_velocity = (m_currentPosition - m_previousPosition)/dt;
            m_previousPosition = m_currentPosition;
            m_currentPosition += m_velocity*dt + m_force*dt*dt;
        }
            break;
        }

        if(m_fixed_y)
        {
            m_currentPosition.y = m_fixed_initial_y;
            m_previousPosition.y = m_fixed_initial_y;
        }
    }
    return;
}

void Person::setFixedY(bool fix_y, float pos_y)
{
    m_fixed_y = fix_y;
    m_fixed_initial_y = pos_y;
}




//boolean to check if the velocity vector changed so the orientation
void Person::setVelocityChangeBool(bool set)    {m_velocity_chenged = set;}
bool Person::isVelocityChanged()    {return m_velocity_chenged;}

void Person::setOrientationAngle(float a)   {orientation_angle = a;}
float Person::getOrientationAngle() {return orientation_angle;}

