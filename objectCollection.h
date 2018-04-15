#ifndef OBJECTCOLLECTION_H
#define OBJECTCOLLECTION_H

#include "Vector.h"
#include "Objects.h"
#include <list>



class objectCollection {
public:

  virtual Hittable* intersect(Vector pos, Vector vect) = 0;

  void setSize(size_t newSize) {size = newSize;}
  size_t getSize() { return size; }
  void setObjArray(Hittable** newObj) {objects = newObj;}
  Hittable** objArray() { return objects; }

private:
  size_t size;
  Hittable** objects;

};

class BVM : public objectCollection {

public:

  struct BVMNode {

    BVMNode(Vector lo, Vector hi) : axis(-1), obj(NULL){
      loPoint = lo;
      hiPoint = hi;
    };

    bool isLeaf() {
      return (left == NULL && right == NULL);
    }

    int axis;
    double divide;

    Vector loPoint;
    Vector hiPoint;

    BVMNode* left;
    BVMNode* right;

    Hittable* obj;

  };

  BVM(Hittable** objArray, size_t size, Vector low, Vector high);
  Hittable* intersect(Vector pos, Vector vect);

private:
  bool isLeaf(BVMNode node);
  BVMNode* root;

};

class nonBVM : objectCollection {
public:
  nonBVM(Hittable** objArray, size_t size);
  Hittable* intersect(Vector pos, Vector vect);

};


#endif
