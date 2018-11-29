#include "geometry.h"
#include <iostream>


Geometry::Geometry()
{

}

//****************************************************
// Plane
//****************************************************

Plane::Plane(const glm::vec3& point, const glm::vec3& normalVect){
    normal = glm::normalize(normalVect);
    dconst = -glm::dot(point, normal);
};

Plane::Plane(const glm::vec3& point0, const glm::vec3& point1, const glm::vec3& point2){
    glm::vec3 v1 = point1 - point0;
    glm::vec3 v2 = point2 - point0;
    normal = glm::normalize(glm::cross(v1, v2));
    dconst = -glm::dot(point0, normal);
};

void Plane::setPosition(const glm::vec3& newPos){
    dconst = -glm::dot(newPos, normal);
};

bool Plane::isInside(const glm::vec3& point){
    float dist;
    dist = glm::dot(point, normal) + dconst;
    if (dist > 1.e-7)
        return false;
    else
        return true;
};

float Plane::distPoint2Plane(const glm::vec3& point){
    float dist;
    return dist = glm::dot(point, normal) + dconst;
};

glm::vec3 Plane::closestPointInPlane(const glm::vec3& point){
    glm::vec3 closestP;
    float r = (-dconst - glm::dot(point, normal));
    return closestP = point + r*normal;
};

bool Plane::intersecSegment(const glm::vec3& point1, const glm::vec3& point2, glm::vec3& pTall){
    if (distPoint2Plane(point1)*distPoint2Plane(point2) > 0)	return false;
    float r = (-dconst - glm::dot(point1, normal)) / glm::dot((point2 - point1), normal);
    pTall = (1 - r)*point1 + r*point2;
    return true;
};




//****************************************************
// Triangle
//****************************************************
Triangle::Triangle(const glm::vec3& point0, const glm::vec3& point1, const glm::vec3& point2)
{
    vertex1 = point0;
    vertex2 = point1;
    vertex3 = point2;
    glm::vec3 norm = glm::cross((point1 - point0), (point2 - point1));
    normal = normalize(norm);
    dconst = -glm::dot(point0, normal);


}
void Triangle::setPosition(const glm::vec3& newPos)
{

}

bool Triangle::isInside(const glm::vec3& point)
{
    glm::vec3 closestP;
    float r = (-dconst - glm::dot(point, normal));
    closestP = point + r*normal;

    float totalArea = 0.5f* glm::length(glm::cross((vertex2 - vertex1),(vertex3 - vertex1)));
    float area1 = 0.5f* glm::length(glm::cross((vertex2 - closestP),(vertex3 - closestP)));
    float area2 = 0.5f* glm::length(glm::cross((closestP - vertex1),(vertex3 - vertex1)));
    float area3 = 0.5f* glm::length(glm::cross((vertex2 - vertex1),(closestP - vertex1)));
    float computedArea = area1 + area2 + area3;


    if((computedArea - totalArea) <= 0 )
        return true;
    else
        return false;
}

float Triangle::areaTriangle(glm::vec3 i, glm::vec3 j, glm::vec3 k)
{
    float area = 1/2* glm::length(glm::cross((j - i),(k - i)));
}
bool Triangle::intersecSegment(const glm::vec3& point1, const glm::vec3& point2, glm::vec3& pTall)
{

}




//****************************************************
// Sphere
//****************************************************
Sphere::Sphere(const glm::vec3& cent, const float rad)
{
    center = cent;
    radius = rad;
}

void Sphere::setPosition(const glm::vec3& new_center, const float new_radius)
{
    center = new_center;
    radius = new_radius;
}
bool Sphere::isInside(const glm::vec3& point)
{
    glm::vec3 distance_vec = center - point;
    float distance = glm::length(distance_vec);

    float distance2 = glm::dot(distance_vec, distance_vec);

    if(distance <= radius)
    {
        return true;
    }
    else
    {
        return false;
    }
}
