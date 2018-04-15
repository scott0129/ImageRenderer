#ifndef OBJECTS_H
#define OBJECTS_H

#include "Hittable.h"
#include <png++/png.hpp>

class Sphere : public Hittable {
public:

  Sphere(Vector newLoc, double newRadius, png::color ambient = png::color(), png::color diffuse = png::color(), png::color specular = png::color(), double specularExp = 2);

  bool collisionDeterm(Vector point, Vector direction);

  Vector getCenter();

  Vector getFarthest(Vector point);

  Vector getNorm(Vector collisionPoint);

  Vector loCorner();

  Vector hiCorner();

  double collideT(Vector point, Vector direction);

private:
  double radius;
  Vector location;
};

class Plane : public Hittable {
public:
  Plane(Vector point1, Vector point2, Vector point3, png::color ambient, png::color diffuse, png::color specular, double specularExp);

  bool collisionDeterm(Vector point, Vector direction);

  Vector getCenter();

  Vector getFarthest(Vector point);

  Vector getNorm(Vector collisionPoint);

  Vector loCorner();

  Vector hiCorner();

  double collideT(Vector point, Vector direction);

private:
  Vector point1;
  Vector point2;
  Vector point3;
};

class Triangle : public Hittable {
public:
  Triangle(Vector point1, Vector point2, Vector point3, png::color ambient, png::color diffuse, png::color specular, double specularExp);

  bool collisionDeterm(Vector point, Vector direction);

  Vector getCenter();

  Vector getFarthest(Vector point);

  Vector getNorm(Vector collisionPoint);

  Vector loCorner();

  Vector hiCorner();

  double getArea();

  double collideT(Vector point, Vector direction);

private:
  double getArea(Vector p1, Vector p2, Vector p3);

  Vector point1;
  Vector point2;
  Vector point3;
};


#endif
