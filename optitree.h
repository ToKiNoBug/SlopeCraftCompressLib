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
class idpRegion
{
public:
    idpRegion(short beg=-1,short end=-1);
    idpRegion(const idpRegion&);
    void setBegin(short);
    void setEnd(short);
    short getBegin();
    short getEnd();
    bool isEmpty();
    void setEmpty();
    short Length();
    idpRegion operator=(const idpRegion&);
    bool operator==(const idpRegion&);
    bool operator>(const idpRegion&);
    bool operator<(const idpRegion&);
    bool operator>=(const idpRegion&);
    bool operator<=(const idpRegion&);
    bool operator!=(const idpRegion&);
private:
    short Begin;
    short End;
};

class HeightLine
{
public:
    HeightLine(int _size=129,char method='Z');//Z->zeros,R->Random
    HeightLine(const VectorXi&);
    int Size;//totalSize
    VectorXi Height;
    QImage toQImage();
    void toBrackets(vector<short>&index,string&brackets);
    void Sink(idpRegion&);
};

class Node//二叉链，与二叉树没有本质区别
{
public:
    Node(short beg=-1,short end=-1);
    Node(const idpRegion&);
    ~Node();
    void disp();
private:
    Node *Child;
    Node *Sib;//NextSib
    idpRegion Value;
    bool isAble;
    short Degree;
    friend class OptiTree;
    void setEnable(bool able=false);
    bool isLeaf();//只考虑Child，不考虑Sib
    bool haveChild();//指有无激活的子树
    bool haveSib();//指有无激活的平级树
    void creatChild();
    void creatSib();
    void insertChild(Node*);
    void setValue(short =-1,short =-1);
};

class OptiTree
{
public:
    OptiTree();
    Node *Current();
    void NaturalOpti(VectorXi&);
    ~OptiTree();//析构函数，删除所有节点
    void disp();
private:
    void goUp();
    void goDown();
    void goNextSib();
    void goPrevSib();
    bool isRoot();
    bool isLeaf();
    void addChild(short=-1,short=-1);
    //void addChild(const idpRegion&);
    void addSib(short=-1,short=-1);
    //void addSib(const idpRegion&);

    void Freeze();//冻结下面的所有子树
    void Clear();//删除根节点外所有节点

    void gotoRoot();

    void BuildTree(HeightLine&);
    void Compress(HeightLine&);

    Node*Root;
    stack<Node*> Stack;
    void preventEmpty();
};

#endif // OPTITREE_H
