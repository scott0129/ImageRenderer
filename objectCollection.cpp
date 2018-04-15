#include "objectCollection.h"
#include <limits>
#include <stdlib.h>


void recInsert(BVM::BVMNode* subroot, Hittable* object) {
  if ( subroot->isLeaf() ) {
    if (subroot->obj == NULL) {
      //If it's a leaf and there's no object (only happens at root) put it there.
      subroot->obj = object;
      return;
    } else {
      /* If there IS an object there, divide the segment into 2 parts on a random axis.
      Then, create 2 new subBoxes, and put each object in the respective upper/lower box.
      If the objects overlap each other on that axis, adjust the 2 new Node's corners accordingly.
      */
      subroot->axis = rand()%3;
      int axis = subroot->axis;
      double divide = (subroot->obj->getCenter()[axis] + object->getCenter()[axis])/2.0;
      subroot->divide = divide;
      Vector newHi;
      Vector newLo;
      //Deciding which axis to divide on
      switch(axis) {
        case 0:
        newHi = Vector(divide, subroot->hiPoint.y, subroot->hiPoint.z);
        newLo = Vector(divide, subroot->loPoint.y, subroot->loPoint.z);
        break;
        case 1:
        newHi = Vector(subroot->hiPoint.x, divide, subroot->hiPoint.z);
        newLo = Vector(subroot->loPoint.x, divide, subroot->loPoint.z);
        break;
        default:
        newHi = Vector(subroot->hiPoint.x, subroot->hiPoint.y, divide);
        newLo = Vector(subroot->loPoint.x, subroot->loPoint.y, divide);
      }

      subroot->left = new BVM::BVMNode(subroot->loPoint, newHi);
      subroot->right = new BVM::BVMNode(newLo, subroot->hiPoint);

      //Giving the appropriate subtree the appropriate object for e-z searching.
      if (subroot->obj->getCenter()[axis] < divide) {
        subroot->left->obj = subroot->obj;
        subroot->right->obj = object;
      } else {
        subroot->left->obj = object;
        subroot->right->obj = subroot->obj;
      }

      //These next if statements allow for overlap, so that objects don't get "cut"

      if (subroot->left->hiPoint[axis] < subroot->left->obj->hiCorner()[axis]) {
        subroot->left->hiPoint[axis] = subroot->left->obj->hiCorner()[axis];
      }
      if (subroot->right->loPoint[axis] > subroot->right->obj->loCorner()[axis]) {
        subroot->right->loPoint[axis] = subroot->right->obj->loCorner()[axis];
      }
    }
  } else {
    if (object->getCenter()[subroot->axis] < subroot->divide) {
      //following if statement is a clip-guard

      recInsert(subroot->left, object);
    } else {
      //following if statement is a clip-guard

      recInsert(subroot->right, object);
    }
  }
}

BVM::BVM(Hittable** objArray, size_t size, Vector low, Vector high) {

  setSize(size);
  setObjArray(objArray);

  root = new BVMNode(low, high);

  for (size_t i = 0; i < size; i++) {
    recInsert(root, objArray[i]);
  }

}

bool boxIntersect(Vector origin, Vector direction, Vector lo, Vector hi) {
  double minT = std::numeric_limits<double>::min();
  double maxT = std::numeric_limits<double>::max();

  if (direction.x != 0) {
    double x1t = (lo.x - origin.x)/direction.x;
    double x2t = (hi.x - origin.x)/direction.x;

    minT = std::max(minT, std::min(x1t, x2t));
    maxT = std::min(maxT, std::max(x1t, x2t));
  }

  if (direction.y != 0) {
    double y1t = (lo.y - origin.y)/direction.y;
    double y2t = (hi.y - origin.y)/direction.y;

    minT = std::max(minT, std::min(y1t, y2t));
    maxT = std::min(maxT, std::max(y1t, y2t));
  }

  if (direction.z != 0) {
    double z1t = (lo.z - origin.z)/direction.z;
    double z2t = (hi.z - origin.z)/direction.z;

    minT = std::max(minT, std::min(z1t, z2t));
    maxT = std::min(maxT, std::max(z1t, z2t));
  }
  return maxT > minT;
}

Hittable* recIntersect(BVM::BVMNode* subroot, Vector pos, Vector vect) {
  if (subroot->isLeaf()) {

    if (isnan(subroot->obj->collideT(pos, vect))) {
      return NULL;
    } else {
      return subroot->obj;
    }
  } else if (subroot == NULL || !boxIntersect(pos, vect, subroot->loPoint, subroot->hiPoint)) {
    return NULL;
  }
  Hittable* leftResult = recIntersect(subroot->left, pos, vect);
  Hittable* rightResult = recIntersect(subroot->right, pos, vect);
  if (leftResult == NULL) {
    return rightResult;
  } else if (rightResult == NULL) {
    return leftResult;
  } else {
    if (leftResult->collideT(pos, vect) < rightResult->collideT(pos, vect)) {
      return leftResult;
    } else {
      return rightResult;
    }
  }
}

Hittable* BVM::intersect(Vector pos, Vector vect) {
  return recIntersect(root, pos, vect);
}

nonBVM::nonBVM(Hittable** objArray, size_t size) {
  setSize(size);
  setObjArray(objArray);
}

Hittable* nonBVM::intersect(Vector pos, Vector vect) {
  double minT = nan("1");
  Hittable* closestObj = NULL;

  for(size_t i = 0; i < getSize(); i++) {
    double tVal = objArray()[i]->collideT(pos, vect);
    if ((isnan(minT) && !isnan(tVal) )|| tVal < minT) {
      minT = tVal;
      closestObj = objArray()[i];
    }
  }

  return closestObj;
}
