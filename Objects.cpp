#include "Objects.h"


Sphere::Sphere(Vector newLoc, double newRadius, png::color ambient, png::color diffuse, png::color specular, double specularExp) {
  location = newLoc;
  radius = newRadius;
  setAmbientK(ambient);
  setDiffuseK(diffuse);
  setSpecularK(specular);
  setSpecularExp(specularExp);
}

Vector Sphere::getNorm(Vector collisionPoint) {
  Vector v = (collisionPoint - location).normalize();
  return v;
}

Vector Sphere::getCenter() {
  return location;
}

Vector Sphere::getFarthest(Vector point) {
  Vector toCenter = location - point;
  Vector normalized = toCenter.normalize();
  Vector ret = toCenter + normalized.scalar(radius);
  return ret;
}

Vector Sphere::loCorner() {
  double x = location.x - radius;
  double y = location.y - radius;
  double z = location.z - radius;
  return Vector(x, y, z);
}

Vector Sphere::hiCorner() {
  double x = location.x + radius;
  double y = location.y + radius;
  double z = location.z + radius;
  return Vector(x, y, z);
}

bool Sphere::collisionDeterm(Vector point, Vector direction) {
  double a = direction.dot(direction);
  double b = ((point-location).scalar(2) ).dot(direction);
  double c = (point-location).dot( (point - location) ) - (radius*radius);
  return (b*b - 4*a*c) >= 0;
}

double Sphere::collideT(Vector point, Vector direction) {
  double a = direction.dot(direction);
  double b = ((point-location).scalar(2) ).dot(direction);
  double c = (point-location).dot( (point - location) ) - (radius*radius);

  double potentialT1 = ( (-1*b) + sqrt(b*b - 4*a*c) ) / (2*a);
  double potentialT2 = ( (-1*b) - sqrt(b*b - 4*a*c) ) / (2*a);

  double tVal;

  if (potentialT1 < 0 || isnan(potentialT1)) {
    tVal = potentialT2;
  } else if (potentialT2 < 0 || isnan(potentialT2)) {
    tVal = potentialT1;
  } else {
    tVal = std::min(potentialT1, potentialT2);
  }

  return tVal;
}


Plane::Plane(Vector p1, Vector p2, Vector p3, png::color ambient, png::color diffuse, png::color specular, double specularExp) {
  point1 = p1;
  point2 = p2;
  point3 = p3;
  setAmbientK(ambient);
  setDiffuseK(diffuse);
  setSpecularK(specular);
  setSpecularExp(specularExp);
}

Vector Plane::getNorm(Vector collisionPoint) {
  //Cross product of 1->3 and 1->2. Follows right-hand-rule of the 3 points in a clockwise order
  return ((point1-point3).cross(point1-point2)).normalize();
}

bool Plane::collisionDeterm(Vector point, Vector direction) {
  return true;
}

Vector Plane::getCenter() {
  std::cout << "something's wrong! called getCenter on Plane!" << std::endl;
  return point1;
}

Vector Plane::getFarthest(Vector v) {
  std::cout << "something's wrong! called getFarthest on Plane!" << std::endl;
  return point1;
}

Vector Plane::hiCorner() {
  std::cout << "something's wrong! called hiCorner on Plane!" << std::endl;
  return point1;
}

Vector Plane::loCorner() {
  std::cout << "something's wrong! called loCorner on Plane!" << std::endl;
  return point1;
}

double Plane::collideT(Vector origin, Vector direction) {
  // ((a-o)dot n)/(d dot n)
  Vector normal = Plane::getNorm(Vector());
  if (normal.dot(direction) == 0) {
    return nan("1");
  }
  return ( (point1 - origin).dot(normal) ) / (direction.dot(normal));
}



Triangle::Triangle(Vector p1, Vector p2, Vector p3, png::color ambient, png::color diffuse, png::color specular, double specularExp) {
  point1 = p1;
  point2 = p2;
  point3 = p3;
  setAmbientK(ambient);
  setDiffuseK(diffuse);
  setSpecularK(specular);
  setSpecularExp(specularExp);
}

Vector Triangle::getNorm(Vector collisionPoint) {
  //Cross product of 1->3 and 1->2. Follows right-hand-rule of the 3 points in a counter-clockwise order
  return ((point1-point2).cross(point1-point3)).normalize();
}

Vector Triangle::getCenter() {
  double x = (point1.x + point2.x + point3.x)/3.0;
  double y = (point1.y + point2.y + point3.y)/3.0;
  double z = (point1.z + point2.z + point3.z)/3.0;
  return Vector(x, y, z);
}

Vector Triangle::getFarthest(Vector point) {
  int farthestPoint = 0;
  //finds the farthest corner from point
  if ( (point1 - point).length() > ((point2 - point)).length() ) {
    if ( (point3 - point).length() > (point1-point).length() ) {
      farthestPoint = 3;
    } else {
      farthestPoint = 1;
    }
  } else {
    if ( (point3 - point).length() > (point2-point).length() ) {
      farthestPoint = 3;
    } else {
      farthestPoint = 2;
    }
  }

  switch (farthestPoint) {
    case 1:
      return point1;
    case 2:
      return point2;
    default:
      return point3;
  }
}

Vector Triangle::loCorner() {
  double x = std::min(point1.x, std::min(point2.x, point3.x) ) - 0.0001;
  double y = std::min(point1.y, std::min(point2.y, point3.y) ) - 0.0001;
  double z = std::min(point1.z, std::min(point2.z, point3.z) ) - 0.0001;
  return Vector(x, y, z);
}

Vector Triangle::hiCorner() {
  double x = std::max(point1.x, std::max(point2.x, point3.x) ) + 0.0001;
  double y = std::max(point1.y, std::max(point2.y, point3.y) ) + 0.0001;
  double z = std::max(point1.z, std::max(point2.z, point3.z) ) + 0.0001;
  return Vector(x, y, z);
}

bool Triangle::collisionDeterm(Vector point, Vector direction) {
  return true;
}

double Triangle::getArea() {
  return getArea(point1, point2, point3);
}

double Triangle::getArea(Vector first, Vector second, Vector third) {
  return ((first-third).cross(first-second)).length()/2;
}

double Triangle::collideT(Vector origin, Vector direction) {
  // ((a-o)dot n)/(d dot n)
  Vector normal = Triangle::getNorm(Vector());
  double tVal = ( (point1 - origin).dot(normal) ) / (direction.dot(normal));
  if (isnan(tVal) || normal.dot(direction) >= 0) {
    return nan("1");
  }
  Vector collisionPoint = origin + direction.scalar(tVal);
  double baryCentArea = getArea(point1, point2, collisionPoint) + getArea(point2, point3, collisionPoint) + getArea(point3, point1, collisionPoint);

  double threshold = 0.000000000001;
  if (baryCentArea > getArea() + threshold) {
    return nan("1");
  } else {
    return tVal;
  }
}
