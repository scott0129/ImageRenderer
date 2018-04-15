#ifndef VECTOR_H
#define VECTOR_H

#include <iostream>
#include <math.h>
#include <string>
#include <ostream>

class Vector {
public:

  Vector(double newX = 0, double newY = 0, double newZ = 0) : x(newX), y(newY), z(newZ) {}

  Vector( const Vector &obj) {
    x = obj.x;
    y = obj.y;
    z = obj.z;
  }

  void setXYZ(double newX, double newY, double newZ) {
    x = newX;
    y = newY;
    z = newZ;
  };

  double length() const {
    return sqrt(x*x + y*y + z*z);
  }

  Vector normalize() const {
    double currentLength = length();
    double newX = x/currentLength;
    double newY = y/currentLength;
    double newZ = z/currentLength;
    return Vector(newX, newY, newZ);
  }


  Vector cross(const Vector& other) const {
    double newX = this->y * other.z - this->z * other.y;
    double newY = this->z * other.x - this->x * other.z;
    double newZ = this->x * other.y - this->y * other.x;
    return Vector(newX, newY, newZ);
  }


  double dot(const Vector& other) const {
    return (this->x * other.x + this->y * other.y + this->z * other.z);
  }

  Vector scalar(double mult) const {
    double newX = this->x * mult;
    double newY = this->y * mult;
    double newZ = this->z * mult;
    return Vector(newX, newY, newZ);
  }

  Vector operator-(const Vector& other) const {
    return Vector(this->x - other.x, this->y - other.y, this->z - other.z);
  }

  Vector operator+(const Vector& other) const {
    return Vector(this->x + other.x, this->y + other.y, this->z + other.z);
  }

  bool operator==(const Vector& other) const {
    if (this->x != other.x || this->y != other.y || this->z != other.z) {
      return false;
    } else {
      return true;
    }
  }

  double& operator[](int i) {
    switch(i) {
      case 0:
      return x;
      case 1:
      return y;
      default:
      return z;
    }
  }


  std::string toString() const {
    return "x: " + std::to_string(x) + ", y:" + std::to_string(y) + ", z:" + std::to_string(z);
  }

  double x;
  double y;
  double z;

private:


};

#endif
