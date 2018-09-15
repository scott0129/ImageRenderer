#include "objectCollection.h"
#include <limits>
#include <stdlib.h>


void recInsert(BVH::BVHNode* subroot, Hittable* object) {
  //every node it enters, if the object is bigger than the node, expand the node
    for (int i = 0; i < 3; i++) {
        if (object->hiCorner()[i] > subroot->hiPoint->get(i)) {
            subroot->hiPoint->get(i) = object->hiCorner()[i];
        }
        if (object->loCorner()[i] < subroot->loPoint->get(i)) {
            subroot->loPoint->get(i) = object->loCorner()[i];
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
            Vector* newHi;
            Vector* newLo;
            //Deciding which axis to divide on
            switch(axis) {
                case 0:
                newHi = new Vector(divide, subroot->hiPoint->y, subroot->hiPoint->z);
                newLo = new Vector(divide, subroot->loPoint->y, subroot->loPoint->z);
                break;
                case 1:
                newHi = new Vector(subroot->hiPoint->x, divide, subroot->hiPoint->z);
                newLo = new Vector(subroot->loPoint->x, divide, subroot->loPoint->z);
                break;
                default:
                newHi = new Vector(subroot->hiPoint->x, subroot->hiPoint->y, divide);
                newLo = new Vector(subroot->loPoint->x, subroot->loPoint->y, divide);
            }

            subroot->left = new BVH::BVHNode(subroot->loPoint, newHi);
            subroot->right = new BVH::BVHNode(newLo, subroot->hiPoint);

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
            if (subroot->left->hiPoint->get(i) < subroot->left->obj->hiCorner()[i]) {
                //@TODO there should be something weird with the hiPoint[]?
                subroot->left->hiPoint->get(i) = subroot->left->obj->hiCorner()[i];
            }
            if (subroot->right->loPoint->get(i) > subroot->right->obj->loCorner()[i]) {
                subroot->right->loPoint->get(i) = subroot->right->obj->loCorner()[i];
            }

            if (subroot->left->loPoint->get(i) > subroot->left->obj->loCorner()[i]) {
                subroot->left->loPoint->get(i) = subroot->left->obj->loCorner()[i];
                subroot->loPoint->get(i) = subroot->left->loPoint->get(i);
            }

            if (subroot->right->hiPoint->get(i) < subroot->right->obj->hiCorner()[i]) {
                subroot->right->hiPoint->get(i) = subroot->right->obj->hiCorner()[i];
                subroot->hiPoint->get(i) = subroot->right->hiPoint->get(i);
            }

        }

    }
    } else {
        if (object->getCenter()[subroot->axis] < subroot->left->hiPoint->get(subroot->axis) ) {
            recInsert(subroot->left, object);
        } else {
            recInsert(subroot->right, object);
        }
    }
}

BVH::BVH(std::vector<Hittable*> objArray, Vector* low, Vector* high) {
    setSize(objArray.size());

    root = new BVHNode(low, high);

    for (size_t i = 0; i < objArray.size(); i++) {
        recInsert(root, objArray[i]);
    }

}

bool boxIntersect(Vector* origin, Vector* direction, Vector* lo, Vector* hi) {
    double minT = std::numeric_limits<double>::min();
    double maxT = std::numeric_limits<double>::max();

    if (direction->x != 0) {
        double x1t = (lo->x - origin->x)/direction->x;
        double x2t = (hi->x - origin->x)/direction->x;

        minT = std::max(minT, std::min(x1t, x2t));
        maxT = std::min(maxT, std::max(x1t, x2t));
    }

    if (direction->y != 0) {
        double y1t = (lo->y - origin->y)/direction->y;
        double y2t = (hi->y - origin->y)/direction->y;

        minT = std::max(minT, std::min(y1t, y2t));
        maxT = std::min(maxT, std::max(y1t, y2t));
    }

    if (direction->z != 0) {
        double z1t = (lo->z - origin->z)/direction->z;
        double z2t = (hi->z - origin->z)/direction->z;

        minT = std::max(minT, std::min(z1t, z2t));
        maxT = std::min(maxT, std::max(z1t, z2t));
    }
    return maxT > minT;
}

Hittable* recIntersect(BVH::BVHNode* subroot, Vector* pos, Vector* vect) {
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

Hittable* BVH::intersect(Vector* pos, Vector* vect) {
    return recIntersect(root, pos, vect);
}

//--------------------------------------non-bvh---------------------------------

nonBVH::nonBVH(std::vector<Hittable*> list, Vector low, Vector high) {
    _list = list;
}

Hittable* nonBVH::intersect(Vector* pos, Vector* vect) {
    double shortest = std::numeric_limits<double>::max();
    Hittable* nearest = NULL;
    for (Hittable* obj : _list) {
        double dist = obj->collideT(pos, vect);
        if (!isnan(dist)) {
            if (shortest > dist) {
                shortest = dist;
                nearest = obj;
            }
        }

    }
    if (nearest != NULL) {
        return nearest;
    } else {
        return NULL;
    }
}
