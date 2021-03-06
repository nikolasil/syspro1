#include <iostream>

#include "tree.h"
#include "../../util.h"

treeNode::treeNode()
{
}

treeNode::~treeNode()
{
    delete this->citizen;
    delete this->left;
    delete this->right;
}

treeNode *treeNode::newTreeNode(citizenRecord *citizen)
{
    treeNode *node = new treeNode();
    node->citizen = citizen;
    node->left = NULL;
    node->right = NULL;
    node->balanceHeight = 1;
    node->key = node->citizen->getID();
    return node;
}

treeNode *treeNode::rightRotation(treeNode *y)
{
    treeNode *x = y->left;
    treeNode *temp = x->right;

    // Perform rotation
    x->right = y;
    y->left = temp;

    // Update heights
    y->balanceHeight = max(getHeight(y->left), getHeight(y->right)) + 1;
    x->balanceHeight = max(getHeight(x->left), getHeight(x->right)) + 1;

    // Return new root
    return x;
}

treeNode *treeNode::leftRotation(treeNode *x)
{
    treeNode *y = x->right;
    treeNode *temp = y->left;

    // Perform rotation
    y->left = x;
    x->right = temp;

    // Update heights
    x->balanceHeight = max(getHeight(x->left), getHeight(x->right)) + 1;
    y->balanceHeight = max(getHeight(y->left), getHeight(y->right)) + 1;

    // Return new root
    return y;
}

treeNode *treeNode::insert(treeNode *node, citizenRecord *citizen, int *duplicate)
{
    /* 1. Perform the normal BST insertion */
    if (node == NULL)
        return (newTreeNode(citizen));

    if (citizen->getID() < node->citizen->getID())
        node->left = insert(node->left, citizen, duplicate);
    else if (citizen->getID() > node->citizen->getID())
        node->right = insert(node->right, citizen, duplicate);
    else // Equal keys are not allowed in BST
    {
        if (node->getCitizen()->getStatus()->getVirusStatus(citizen->getStatus()->getVirusName()) == NULL)
        {
            node->getCitizen()->getStatus()->mergeStatus(citizen->getStatus());
        }
        else
        {
            *duplicate = 1;
        }
        delete citizen;
        return node;
    }
    /* 2. Update height of this ancestor node */
    node->balanceHeight = max(getHeight(node->left), getHeight(node->right)) + 1;

    /* 3. Get the balance factor of this ancestor 
        node to check whether this node became 
        unbalanced */
    int balance = getBalance(node);

    // If this node becomes unbalanced, then
    // there are 4 cases

    // Left Left Case
    if (balance > 1 && citizen->getID() < node->left->citizen->getID())
        return rightRotation(node);

    // Right Right Case
    if (balance < -1 && citizen->getID() > node->right->citizen->getID())
        return leftRotation(node);

    // Left Right Case
    if (balance > 1 && citizen->getID() > node->left->citizen->getID())
    {
        node->left = leftRotation(node->left);
        return rightRotation(node);
    }

    // Right Left Case
    if (balance < -1 && citizen->getID() < node->right->citizen->getID())
    {
        node->right = rightRotation(node->right);
        return leftRotation(node);
    }

    /* return the (unchanged) node pointer */
    return node;
}

treeNode *treeNode::search(treeNode *root, int key)
{
    if (root == NULL || root->key == key)
    {
        return root;
    }
    if (root->key < key)
    {
        return search(root->right, key);
    }
    return search(root->left, key);
}

citizenRecord *treeNode::getCitizen()
{
    return this->citizen;
}

int treeNode::getBalance(treeNode *node)
{
    if (node == NULL)
        return 0;
    return getHeight(node->left) - getHeight(node->right);
}

int treeNode::getHeight(treeNode *node)
{
    if (node == NULL)
        return 0;
    return node->balanceHeight;
}

struct Trunk
{
    Trunk *prev;
    string str;

    Trunk(Trunk *prev, string str)
    {
        this->prev = prev;
        this->str = str;
    }
};

void showTrunks(Trunk *p)
{
    if (p == nullptr)
    {
        return;
    }

    showTrunks(p->prev);
    cout << p->str;
}

void printTree(treeNode *root, Trunk *prev = NULL, bool isLeft = true)
{
    if (root == nullptr)
    {
        return;
    }

    string prev_str = "    ";
    Trunk *trunk = new Trunk(prev, prev_str);

    printTree(root->left, trunk, true);

    if (!prev)
    {
        trunk->str = "———";
    }
    else if (isLeft)
    {
        trunk->str = ".———";
        prev_str = "    |";
    }
    else
    {
        trunk->str = "`———";
        prev->str = prev_str;
    }

    showTrunks(trunk);
    cout << root->key << endl;

    if (prev)
    {
        prev->str = prev_str;
    }
    trunk->str = "    |";

    printTree(root->right, trunk, false);
    delete trunk;
}

void treeNode::print(treeNode *node)
{
    printTree(node);
}