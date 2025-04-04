#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)
#include<iostream>

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here

bool equalPathsRecursive(Node* root, int depth, int &leafDepth) {
  if (root == nullptr) { // base case if current node is null nothing to check
    return true;
  }
  // if it is a leaf
  if (root->left == nullptr && root->right == nullptr) {
      if (leafDepth == -1) { // if first leaf node seen
        leafDepth = depth; // set leafDepth to depth of first leaf
        return true; // return true since nothing to compare
      }
      return depth == leafDepth; // check if curr leaf depth matches stored
    }
    return equalPathsRecursive(root->left, depth + 1, leafDepth) &&  // recrusive check for left and right subtree, increase depth by 1 when going down a level
           equalPathsRecursive(root->right, depth + 1, leafDepth);

}


bool equalPaths(Node * root)
{
    // Add your code below
    int leafDepth = -1; // no leaf seen yet
    return equalPathsRecursive(root, 0, leafDepth);

}

