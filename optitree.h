#ifndef OPTITREE_H
#define OPTITREE_H
#include<stack>
#include <QDebug>
#include <vector>
#include <QImage>
#include <Eigen/Dense>
#include <QRgb>
#include <iostream>
#include <cmath>
#include <stack>
#include <queue>
//#include "heightline.cpp"
using namespace std;
using namespace Eigen;
class Node;
class HeightLine
{
public:
    HeightLine(int _size=129,char method='Z');//Z->zeros,R->Random
    HeightLine(const VectorXi&);
    int Size;//totalSize
    VectorXi Height;
    QImage toQImage();
    void toBrackets(vector<short>&index,string&brackets);
    void Sink(Node*);
};

class Node//二叉链，与二叉树没有本质区别
{
public:
    short Begin;
    short End;
    Node(short beg=-1,short end=-1);
    ~Node();
    void disp();
    Node *Child;
    Node *Sib;
    bool isAble;
    short Degree;
    bool isComplete();
    void Freeze();
    bool haveChild();//指有无激活的子树
    bool haveSib();//指有无激活的平级树

    Node* creatChild();
    Node* creatSib();
};

class OptiTree
{
public:
    OptiTree();
    Node *Current();
    ~OptiTree();//析构函数，删除所有节点
    void goUp();
    void goDown();
    void goNextSib();
    void goPrevSib();
    bool isRoot();

    void gotoRoot();
    void NaturalOpti(VectorXi&);
    void BuildTree(HeightLine&);
    void Compress(HeightLine&);

    Node*Root;
    stack<Node*> Stack;
    void preventEmpty();
};

#endif // OPTITREE_H
