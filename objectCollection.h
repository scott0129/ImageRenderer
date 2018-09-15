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

class BVH : public objectCollection {

public:

    struct BVHNode {

        BVHNode(Vector* lo, Vector* hi) : axis(-1), left(NULL), right(NULL), obj(NULL) {
            loPoint = new Vector(*lo);
            hiPoint = new Vector(*hi);
        };

        ~BVHNode() {
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

        BVHNode* left;
        BVHNode* right;

        Hittable* obj;

    };

    BVH(std::vector<Hittable*> objArray, Vector* low, Vector* high);

    Hittable* intersect(Vector* pos, Vector* vect);

    std::vector<int> getHeights() {
        std::vector<int> vect;
        getRecHeight(root, &vect, 0);
        return vect;
    }

    //Resursive height
    void getRecHeight(BVHNode* sroot, std::vector<int>* vector, int dist) {
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
    bool isLeaf(BVHNode node);
    BVHNode* root;

    void recPrint(BVHNode* root) {
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

//nonBVH objects list created to quantify performance improvement from using BVH
class nonBVH : public objectCollection {
public:
    nonBVH(std::vector<Hittable*> list, Vector low, Vector high);
    Hittable* intersect(Vector* pos, Vector* vect);

private:
    std::vector<Hittable*> _list;
};


#endif
