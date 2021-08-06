#ifndef OPTICHAIN_H
#define OPTICHAIN_H
#define showImg


#include <iostream>
#include <string>
#include <QDebug>
#include <Eigen/Dense>
#include <list>
#include <queue>
#include <QRgb>
#include <QImage>
#include <QLabel>
using namespace std;
using namespace Eigen;
enum RegionType {idp,Hang,Invalid};

extern QRgb isTColor;
extern QRgb isFColor;
extern QRgb WaterColor;
extern QRgb greyColor;

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
    int size() const;
    short indexLocal2Global(short) const;
    short indexGlobal2Local(short) const;
    string toString() const;
};

class OptiChain
{
public:
    //Index一律以Height矩阵中的索引为准。Height(r,c+1)<->Base(r,c)
    //高度矩阵一律不含水柱顶的玻璃方块
    OptiChain(int Size=-1);//default Random Constructor
    OptiChain(ArrayXi,ArrayXi,int);
    ~OptiChain();

    void divideAndCompress();

    static ArrayXXi Base;
    static Array3i Both;
    static Array3i Left;
    static Array3i Right;

#ifdef showImg
    static QLabel *SinkIDP;
    static QLabel *SinkAll;
#endif

    int Col;
    ArrayXi HighLine;
    ArrayXi LowLine;
    queue<Region> Chain;//将一整列按水/空气切分为若干个大的孤立区间
    list<Region> SubChain;//将Chain中每个大的孤立区间切分为若干“最大单孤立区间”和“悬空区间”组成的串

    void divideToChain();
    void divideToSubChain();

    bool isAir(int index);
    bool isWater(int index);
    bool isSolidBlock(int index);

    void Sink(const Region&);
    ArrayXi toDepth();
    int validHeight(int index);
    QImage toQImage(int pixelSize);
    //void dispChain();
    void dispSubChain();
private:
    void divideToSubChain(const Region&);
};

QImage Mat2Image(const ArrayXXi&,int pixelSize);

#endif // OPTICHAIN_H
