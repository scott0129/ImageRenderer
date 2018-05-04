#ifndef OBJECTCOLLECTION_H
#define OBJECTCOLLECTION_H

#include "Vector.h"
#include "Objects.h"
#include <list>



class objectCollection {
public:

  virtual Hittable* intersect(Vector* pos, Vector* vect) = 0;

  void setSize(size_t newSize) {size = newSize;}
  size_t getSize() { return size; }

private:
  size_t size;

};

class BVM : public objectCollection {

public:

  struct BVMNode {

    BVMNode(Vector* lo, Vector* hi) : axis(-1), left(NULL), right(NULL), obj(NULL) {
      loPoint = new Vector(*lo);
      hiPoint = new Vector(*hi);
    };

    ~BVMNode() {
        delete loPoint;
        delete hiPoint;
        delete left;
        delete right;
    }

    bool isLeaf() {
      return (left == NULL && right == NULL);
    }

    int axis;
    double divide;

    Vector* loPoint;
    Vector* hiPoint;

    BVMNode* left;
    BVMNode* right;

    Hittable* obj;

  };

  BVM(std::vector<Hittable*> objArray, Vector* low, Vector* high);

  Hittable* intersect(Vector* pos, Vector* vect);

  std::vector<int> getHeights() {
      std::vector<int> vect;
      getRecHeight(root, &vect, 0);
      return vect;
  }

  void getRecHeight(BVMNode* sroot, std::vector<int>* vector, int dist) {
      if (sroot == NULL) {
          vector->push_back(dist);
          return;
      }
      getRecHeight(sroot->left, vector, dist + 1);
      getRecHeight(sroot->right, vector, dist + 1);
  }

  void printTree() {
    recPrint(root);
  }

private:
  bool isLeaf(BVMNode node);
  BVMNode* root;

  void recPrint(BVMNode* root) {
    if (root == NULL) {return;}
    std::cout << "hiPoint is: " << root->hiPoint->toString() << " and loPoint is: " << root->loPoint->toString() << " axis: " << root->axis << " ";
    if (root->isLeaf()) {
      std::cout << "<- leaf containing " << root->obj->getCenter().toString();
      std::cout << " leafLoPoint: " << root->obj->loCorner().toString();
    }
    std::cout << std::endl;
    recPrint(root->left);
    recPrint(root->right);
  }

};

class nonBVM : public objectCollection {
public:
  nonBVM(std::vector<Hittable*> list, Vector low, Vector high);
  Hittable* intersect(Vector* pos, Vector* vect);

private:
  std::vector<Hittable*> _list;
};


#endif
