#ifndef OBJECTS_H
#define OBJECTS_H

#include "Hittable.h"
#include <png++/png.hpp>

class Sphere : public Hittable {
public:

    Sphere(Vector newLoc, double newRadius, png::color ambient = png::color(), png::color diffuse = png::color(), png::color specular = png::color(), double specularExp = 2);

    bool collisionDeterm(const Vector* point, const Vector* direction);

    Vector getCenter();

    Vector getFarthest(Vector point);

    Vector getNorm(Vector* collisionPoint);

    /*
    Returns a coordinate vector containing the lowest X, Y, and Z value that contains the sphere
    Example, sphere with center (1, 2, 3,) with radius 1 would return (0, 1, 2);
    */
    Vector loCorner();

    /*
    Returns a coordinate vector containing the highest X, Y, and Z value.
    Example, sphere with center (1, 2, 3,) with radius 1 would return (2, 3, 4);
    */
    Vector hiCorner();

    /*
    Returns the minimum t-value of a vector that would collide with the sphere where
    point + direction * t = collisionPoint
    returns NaN if such line would not collide at any point.
    */
    double collideT(const Vector* point, const Vector* direction);

private:
    double radius;
    Vector location;
};

class Plane : public Hittable {
public:
    Plane(Vector point1, Vector point2, Vector point3, png::color ambient, png::color diffuse, png::color specular, double specularExp);

    bool collisionDeterm(const Vector* point, const Vector* direction);

    Vector getCenter();

    Vector getFarthest(Vector point);

    Vector getNorm(Vector* collisionPoint);

    Vector loCorner();

    Vector hiCorner();

    double collideT(const Vector* point, const Vector* direction);

private:
    Vector point1;
    Vector point2;
    Vector point3;
};

class Triangle : public Hittable {
public:
    Triangle(Vector point1, Vector point2, Vector point3, png::color ambient, png::color diffuse, png::color specular, double specularExp);

    bool collisionDeterm(const Vector* point, const Vector* direction);

    Vector getCenter();

    Vector getFarthest(Vector point);

    Vector getNorm(Vector* collisionPoint);

    Vector loCorner();

    Vector hiCorner();

    double getArea();

    double collideT(const Vector* point, const Vector* direction);

private:
    double getArea(Vector* p1, Vector* p2, Vector* p3);

    Vector point1;
    Vector point2;
    Vector point3;
};


#endif
