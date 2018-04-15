#include "Hittable.h"

png::color Hittable::getAmbientK() {
  return ambientK;
}
png::color Hittable::getDiffuseK() {
  return diffuseK;
}
png::color Hittable::getSpecularK() {
  return specularK;
}
double Hittable::getSpecularExp() {
  return specularExp;
}


void Hittable::setAmbientK(png::color newColor) {
  ambientK = newColor;
}
void Hittable::setDiffuseK(png::color newColor) {
  diffuseK= newColor;
}
void Hittable::setSpecularK(png::color newColor) {
  specularK = newColor;
}
void Hittable::setSpecularExp(double newSpec) {
  specularExp = newSpec;
}
