#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    void rotateLeft(AVLNode<Key, Value>* node);
    void rotateRight(AVLNode<Key, Value>* node);
    void rebalanceInsert(AVLNode<Key, Value>* pNode, AVLNode<Key, Value>* cNode); // rebalance after insert
    void rebalanceRemove(AVLNode<Key, Value>* node, int delta); // rebalance after removal using delta


};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // TODO
    if (this->root_ == nullptr) { // if tree is empty create a new node as root
      this->root_ = new AVLNode<Key, Value>(new_item.first, new_item.second, nullptr);
      return;
    }

    // standard BST insert using AVLNodes
    AVLNode<Key, Value>* parent = nullptr;
    AVLNode<Key, Value>* current = static_cast<AVLNode<Key, Value>*>(this->root_);

    while (current != nullptr) { // traverse down tree to find correct insert
      parent = current;
      if (new_item.first < current->getKey()) {
        current = current->getLeft();
      }
      else if (new_item.first > current->getKey()) {
        current = current->getRight();
      }
      else { // Key exists already, update value
        current->setValue(new_item.second);
        return;
      }
    }

    AVLNode<Key, Value>* newNode = new AVLNode<Key, Value>(new_item.first, new_item.second, parent); // create new node and attach it to found parent
    if (new_item.first < parent->getKey()) {
      parent->setLeft(newNode);
    }
    else {
      parent->setRight(newNode);
    }
    // after insert update parent balance, if nonzero set to zero
    // otherwise update based on whether new node was left or right
    if (parent->getBalance() != 0) {
      parent->setBalance(0);
    }
    else {
      if (newNode == parent->getLeft()) {
        parent->setBalance(-1);
      }
      else {
        parent->setBalance(1);
      }
      rebalanceInsert(parent, newNode);
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::rebalanceInsert(AVLNode<Key, Value>* pNode, AVLNode<Key, Value>* cNode) {
  if (pNode == nullptr) {
    return;
  }

  AVLNode<Key, Value>* beforeNode = pNode->getParent();
  if (beforeNode == nullptr) {
    return;
  }
  // if pNode is left child of grandparent
  if (pNode == beforeNode->getLeft()) {
    beforeNode->updateBalance(-1);
    if (beforeNode->getBalance() == 0) { // if grandparent becomes balanced stop
      return;
    }
    else if (beforeNode->getBalance() == -1) {
      rebalanceInsert(beforeNode, pNode); // continue rebalancing upward
    }
    else if (beforeNode->getBalance() == -2) { // new node is on left of pNode we have left-left case
      if (cNode == pNode->getLeft()) {
        rotateRight(beforeNode);
        pNode->setBalance(0);
        beforeNode->setBalance(0);
      }
      else { // Otherwise left right case perform a double rotation
        rotateLeft(pNode);
        rotateRight(beforeNode);
        int b = cNode->getBalance();
        if (b == -1) {
          pNode->setBalance(0);
          beforeNode->setBalance(1);
        }
        else if (b ==0) {
          pNode->setBalance(0);
          beforeNode->setBalance(0);
        }
        else { 
          pNode->setBalance(-1);
          beforeNode->setBalance(0);
        }
        cNode->setBalance(0);
      }
    }
  }
  else { // parent node is just right child of grandParent
    beforeNode->updateBalance(1);
    if (beforeNode->getBalance() == 0) {
      return;
    }
    else if (beforeNode->getBalance() == 1) {
      rebalanceInsert(beforeNode, pNode);
    }
    else if (beforeNode->getBalance() == 2) {
      if (cNode == pNode->getRight()) {
        rotateLeft(beforeNode);
        pNode->setBalance(0);
        beforeNode->setBalance(0);
      }
      else {
        rotateRight(pNode);
        rotateLeft(beforeNode);
        int b = cNode->getBalance();
        if (b == 1) {
          pNode->setBalance(0);
          beforeNode->setBalance(-1);
        }
        else if (b == 0) {
          pNode->setBalance(0);
          beforeNode->setBalance(0);
        }
        else {
          pNode->setBalance(1);
          beforeNode->setBalance(0);
        }
        cNode->setBalance(0);
      }
    }
  }
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    // TODO
    // find node to be removed
    Node<Key, Value>* foundNode = this->internalFind(key);
    if (foundNode == nullptr) {
      return;
    }
    // cast found node to AVLNode
    AVLNode<Key, Value>* target = static_cast<AVLNode<Key, Value>*>(foundNode);
    AVLNode<Key, Value>* pNode = target->getParent(); // get parent of target
    int delta = 0;
    // if target has two children swap it with its inorder predecessor
    if (target->getLeft() != nullptr && target->getRight() != nullptr) {
      AVLNode<Key, Value>* predecessor = static_cast<AVLNode<Key, Value>*>(this->predecessor(target));
      nodeSwap(target, predecessor);
      pNode = target->getParent(); // update parent pointer
    }

    AVLNode<Key, Value>* replaceNode = nullptr; // determine replacement node, target now has at most one child
    if (target->getLeft() != nullptr) {
      replaceNode = target->getLeft();
    }
    else if (target->getRight() != nullptr) {
      replaceNode = target->getRight();
    }

    if (pNode == nullptr) { // update parent's pointer to bypass target
      this->root_ = replaceNode; // if target was root
    }
    else if (target == pNode->getLeft()) {
      pNode->setLeft(replaceNode);
      delta = 1; // left subtree shrank
    }
    else {
      pNode->setRight(replaceNode);
      delta = -1; // right subtree shrank
    }

    if (replaceNode != nullptr) {
      replaceNode->setParent(pNode);
    }

    delete target; // delete target
    rebalanceRemove(pNode, delta);
}

template<class Key, class Value>
void AVLTree<Key, Value>::rebalanceRemove(AVLNode<Key, Value>* node, int delta) {
  if (node == nullptr) {
    return;
  }
  // determine if node is a left child of its parent
  AVLNode<Key, Value>* pNode = node->getParent();
  bool isLeft = (pNode != nullptr && node == pNode->getLeft());
  int nextDelta;

  if (delta == 1) { // if left subtree shrank
    node->updateBalance(delta);
    int balance = node->getBalance();
    if (balance == 1) {
      return;
    }
    else if (balance == 0) {
      if (pNode != nullptr) {
        if (isLeft) {
          nextDelta = 1;
        }
        else {
          nextDelta = -1;
        }
        rebalanceRemove(pNode, nextDelta);
      }
      return;
    }
    else if (balance == 2) {
      AVLNode<Key, Value>* rightChild = node->getRight();
      int rightBalance = rightChild->getBalance();
      if (rightBalance >= 0) {
        rotateLeft(node);
        if (rightBalance == 0) {
          node->setBalance(1);
          rightChild->setBalance(-1);
          return;
        }
        else {
          node->setBalance(0);
          rightChild->setBalance(0);
          if (pNode != nullptr) {
            if (isLeft) {
              nextDelta = 1;
            }
            else {
              nextDelta = -1;
            }
            rebalanceRemove(pNode, nextDelta);
          }
        }
      }
      else { // delta == -1 right subtree shrank
        AVLNode<Key, Value>* rightLeft = rightChild->getLeft();
        int rightLeftB = rightLeft->getBalance();
        rotateRight(rightChild);
        rotateLeft(node);
        if (rightLeftB == 0) {
          node->setBalance(0);
          rightChild->setBalance(0);
        }
        else if (rightLeftB == 1) {
          node->setBalance(-1);
          rightChild->setBalance(0);
        }
        else {
          node->setBalance(0);
          rightChild->setBalance(1);
        }
        rightLeft->setBalance(0);
        if (pNode != nullptr) {
          if (isLeft) {
            nextDelta = 1;
          }
          else {
            nextDelta = -1;
          }
          rebalanceRemove(pNode, nextDelta);
        }
      }
    }
  }
  else {
    node->updateBalance(delta);
    int balance = node->getBalance();
    if (balance == -1) {
      return;
    }
    else if (balance == 0) {
      if (pNode != nullptr) {
        if (isLeft) {
          nextDelta = 1;
        }
        else {
          nextDelta = -1;
        }
        rebalanceRemove(pNode, nextDelta);
      }
      return;
    }
    else if (balance == -2) {
      AVLNode<Key, Value>* leftChild = node->getLeft();
      int leftBalance = leftChild->getBalance();
      if (leftBalance <= 0) {
        rotateRight(node);
        if (leftBalance == 0) {
          node->setBalance(-1);
          leftChild->setBalance(1);
          return;
        }
        else {
          node->setBalance(0);
          leftChild->setBalance(0);
          if (pNode != nullptr) {
            if (isLeft) {
              nextDelta = 1;
            }
            else {
              nextDelta = -1;
            }
            rebalanceRemove(pNode, nextDelta);
          }
        }
      }
      else {
        AVLNode<Key, Value>* leftRight = leftChild->getRight();
        int leftRightB = leftRight->getBalance();
        rotateLeft(leftChild);
        rotateRight(node);
        if (leftRightB == 0) {
          node->setBalance(0);
          leftChild->setBalance(0);
        }
        else if (leftRightB == -1) {
          node->setBalance(1);
          leftChild->setBalance(0);
        }
        else {
          node->setBalance(0);
          leftChild->setBalance(-1);
        }
        leftRight->setBalance(0);
        if (pNode != nullptr) {
          if (isLeft) {
            nextDelta = 1;
          }
          else {
            nextDelta = -1;
          }
          rebalanceRemove(pNode, nextDelta);
        }
      }
    }
  }
}

template<class Key, class Value>
void AVLTree<Key,Value>::rotateLeft(AVLNode<Key, Value>* node) {
  AVLNode<Key, Value>* rightChild = node->getRight(); // move rightChild left subtree to be node;s right subtree
  node->setRight(rightChild->getLeft()); // ,pve rightChild left subtree to be node's right subtree
  if (rightChild->getLeft() != nullptr) {
    rightChild->getLeft()->setParent(node);
  }
  rightChild->setParent(node->getParent()); // connect rightChild to node's parent
  if (node->getParent() == nullptr) {
    this->root_ = rightChild; // node was root, update tree's root
  }
  else if (node == node->getParent()->getLeft()) {
    node->getParent()->setLeft(rightChild);
  }
  else {
    node->getParent()->setRight(rightChild);
  }
  rightChild->setLeft(node); // place node as left child of rightChild
  node->setParent(rightChild);
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* node) {
  AVLNode<Key, Value>* leftChild = node->getLeft();
  node->setLeft(leftChild->getRight()); // move leftChild's right subtree to be node's left subtree
  if (leftChild->getRight() != nullptr) {
    leftChild->getRight()->setParent(node);
  }
  leftChild->setParent(node->getParent()); // connect leftChild to node's parent
  if (node->getParent() == nullptr) {
    this->root_ = leftChild; // node was root, update tree's root
  }

  else if (node == node->getParent()->getRight()) {
    node->getParent()->setRight(leftChild);
  }
  else {
    node->getParent()->setLeft(leftChild);
  }
  leftChild->setRight(node); // place node as right child of leftChild
  node->setParent(leftChild);
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}


#endif
