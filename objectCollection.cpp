#include "objectCollection.h"
#include <limits>
#include <stdlib.h>


void recInsert(BVM::BVMNode* subroot, Hittable* object) {
  //every node it enters, if the object is bigger than the node, expand the node
  for (int i = 0; i < 3; i++) {
    if (object->hiCorner()[i] > subroot->hiPoint[i]) {
      subroot->hiPoint[i] = object->hiCorner()[i];
    }
    if (object->loCorner()[i] < subroot->loPoint[i]) {
      subroot->loPoint[i] = object->loCorner()[i];
    }
  }
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

      //Giving the appropriate subtree the appropriate object for easy searching.
      if (subroot->obj->getCenter()[axis] < divide) {
        subroot->left->obj = subroot->obj;
        subroot->right->obj = object;
      } else {
        subroot->left->obj = object;
        subroot->right->obj = subroot->obj;
      }

      //These next if statements allow for overlap, so that objects don't get "cut"
      for (int i = 0; i < 3; i++) {
        if (subroot->left->hiPoint[i] < subroot->left->obj->hiCorner()[i]) {
          subroot->left->hiPoint[i] = subroot->left->obj->hiCorner()[i];
        }
        if (subroot->right->loPoint[i] > subroot->right->obj->loCorner()[i]) {
          subroot->right->loPoint[i] = subroot->right->obj->loCorner()[i];
        }

        if (subroot->left->loPoint[i] > subroot->left->obj->loCorner()[i]) {
          subroot->left->loPoint[i] = subroot->left->obj->loCorner()[i];
          subroot->loPoint[i] = subroot->left->loPoint[i];
        }

        if (subroot->right->hiPoint[i] < subroot->right->obj->hiCorner()[i]) {
          subroot->right->hiPoint[i] = subroot->right->obj->hiCorner()[i];
          subroot->hiPoint[i] = subroot->right->hiPoint[i];
        }

      }

    }
  } else {

    if (object->getCenter()[subroot->axis] < subroot->left->hiPoint[subroot->axis] ) {
      recInsert(subroot->left, object);
    } else {
      recInsert(subroot->right, object);
    }
    // for (int i = 0; i < 3; i++) {
    //   if (subroot->right->hiPoint[i] > subroot->hiPoint[i]) {
    //     subroot->hiPoint[i] = subroot->right->hiPoint[i];
    //   }
    //   if (subroot->left->hiPoint[i] > subroot->hiPoint[i]) {
    //     subroot->hiPoint[i] = subroot->left->hiPoint[i];
    //   }
    //
    //   if (subroot->left->loPoint[i] < subroot->loPoint[i]) {
    //     subroot->loPoint[i] = subroot->left->loPoint[i];
    //   }
    //   if (subroot->right->loPoint[i] < subroot->loPoint[i]) {
    //     subroot->loPoint[i] = subroot->right->loPoint[i];
    //   }
    //
    // }
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
  if (subroot == NULL) {
    return NULL;
  }
  if (subroot->isLeaf()) {

    if (isnan(subroot->obj->collideT(pos, vect))) {
      return NULL;
    } else {
      return subroot->obj;
    }

  } else if (!boxIntersect(pos, vect, subroot->loPoint, subroot->hiPoint)) {
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
