#ifndef OPTITREE_H
#define OPTITREE_H
#include<stack>
#include <QDebug>
//#include <QList>
#include <QImage>
#include <Eigen/Dense>
#include <QRgb>
#include <iostream>
#include <cmath>
#include <list>
#include <queue>
#include <cmath>
//#include "heightline.cpp"
using namespace std;
using namespace Eigen;
class Pair
{
public:
    Pair(char='(',short=0);
    char type;
    short index;
    bool equalto(char);
    //bool operator==(char);
    ~Pair();
};


struct Region
{
    short Begin;
    short End;
};//Region是两端均为右端点/水/极大值区间的区间，其内不含极大值


class Node;
class HeightLine
{
public:
    HeightLine(int _size=129,char method='Z',int Low=0,int High=0);//Z->zeros,R->Random
    HeightLine(const VectorXi&HighL,const VectorXi&LowL);
    //VectorXi HighLine();
    //VectorXi LowLine();
    VectorXi DepthLine();
    bool isWater(int);
    bool isAir(int);
    bool isNormalBlock(int);
    int validHigh(int);
    int Size;//totalSize
    VectorXi HighLine;//Height refers to LowLine
    VectorXi LowLine;
    static MatrixXi Base;
    static short currentColum;
    static Vector3i Both;
    static Vector3i Left;
    static Vector3i Right;

    QImage toQImage();
    //void toBrackets(list<short>&index,list<char>&brackets);
    //void toBrackets(list<Pair>&);
    void segment2Brackets(list<Pair>&,short,short);
    //void toWaterRegion(queue<Region>&);
    void DealRegion(Region,list<Pair>&);
    void toSubRegion(queue<Region>&);
    VectorXi ValidHighLine();
    //void toBrackets_Near(list<short>&index,list<char>&brackets);
    //bool isContinious();
    void Sink(Node*);
    //void SinkMonotonous();
    void SinkBoundary(short=-1,short=-1);
    //void SinkInner();
    friend class OptiTree;
};

void disp(const list<Pair>&);

/*
bool operator>=(Region,Node*);
bool operator>=(Node*,Region);
bool conflictWith(Region,Node*);
bool conflictWith(Node*,Region);
bool isRightBeside(Region,Node*);
bool isRightBeside(Node*,Region);*/

#endif // OPTITREE_H
