#ifndef HITTABLE_H
#define HITTABLE_H

#include "Vector.h"
#include <iostream>
#include <png++/png.hpp>

struct Color {

  Color(double newRed, double newGreen, double newBlue) : red(newRed), green(newGreen), blue(newBlue) {};

  double red;
  double green;
  double blue;
};

class Hittable {
public:

  virtual double collideT(const Vector* point, const Vector* direction) = 0;
  virtual bool collisionDeterm(const Vector* point, const Vector* direction) = 0;
  virtual Vector getNorm(Vector* collisionPoint) = 0;
  virtual Vector getCenter() = 0;
  virtual Vector getFarthest(Vector point) = 0;
  virtual Vector loCorner() = 0;
  virtual Vector hiCorner() = 0;
  //virtual png::color phongModel(Vector castPoint, Vector castRay, double tVal, Vector toLight, png::color lightColor,  png::color ambientLight) = 0;
  png::color getAmbientK();
  png::color getDiffuseK();
  png::color getSpecularK();
  double getSpecularExp();
  void setAmbientK(png::color newColor);
  void setDiffuseK(png::color newColor);
  void setSpecularK(png::color newColor);
  void setSpecularExp(double newSpec);

private:
  double specularExp;
  png::color ambientK;
  png::color diffuseK;
  png::color specularK;
};

#endif
