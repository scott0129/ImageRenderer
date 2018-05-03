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

  Vector getOrthoNormRight() const {
    return Vector(this->y, -this->x, 0).normalize();
  }

  Vector cross(const Vector* other) const {
    double newX = this->y * other->z - this->z * other->y;
    double newY = this->z * other->x - this->x * other->z;
    double newZ = this->x * other->y - this->y * other->x;
    return Vector(newX, newY, newZ);
  }


  double dot(const Vector* other) const {
    return (this->x * other->x + this->y * other->y + this->z * other->z);
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

  double& get(int i) {
    switch(i) {
      case 0:
      return x;
      case 1:
      return y;
      default:
      return z;
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
    std::string xStr = std::to_string (x);
    xStr.erase ( xStr.find_last_not_of('0') + 1, std::string::npos );

    std::string yStr = std::to_string (y);
    yStr.erase ( yStr.find_last_not_of('0') + 1, std::string::npos );

    std::string zStr = std::to_string (z);
    zStr.erase ( zStr.find_last_not_of('0') + 1, std::string::npos );
    return "(" + xStr + ", " + yStr + ", " + zStr + ")";
  }

  double x;
  double y;
  double z;

private:


};

#endif
