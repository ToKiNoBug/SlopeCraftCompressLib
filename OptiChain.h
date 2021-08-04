#ifndef OPTICHAIN_H
#define OPTICHAIN_H
#include <iostream>
#include <QDebug>
#include <Eigen/Dense>
#include <list>
#define NInf -10000000
using namespace std;
using namespace Eigen;
enum RegionType {idp,Hang,Invalid};

class Region
{
public:
    Region(short=-1,short=-1,RegionType=Invalid);
    short Beg;
    short End;
    RegionType type;
    bool isIDP() const;
    bool isHang() const;
    bool isValid() const;
};

class OptiChain
{
public:
    //Index一律以Height矩阵中的索引为准。Height(r,c+1)<->Base(r,c)
    //高度矩阵一律不含水柱顶的玻璃方块
    OptiChain(int Size=-1);//default Random Constructor
    OptiChain(ArrayXi,ArrayXi,int);
    ~OptiChain();

    static ArrayXXi Base;
    int Col;
    ArrayXi HighLine;
    ArrayXi LowLine;
    list<Region> Chain;

    void Sink(const Region&);
    ArrayXi toDepth();
    int validHeight(int index);
    QImage toQImage(int pixelSize);
};

QImage Mat2Image(ArrayXXi&,int pixelSize);

#endif // OPTICHAIN_H
