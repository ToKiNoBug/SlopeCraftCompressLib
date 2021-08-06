#include "OptiChain.h"

#define NInf -10000000
#define MapSize (Base.rows()+1)
ArrayXXi OptiChain::Base=MatrixXi::Zero(0,0);
Array3i OptiChain::Both(-1,2,-1);
Array3i OptiChain::Left(-1,1,0);
Array3i OptiChain::Right(0,1,-1);

Region::Region(short _Beg,short _End,RegionType _Type)
{
    Beg=_Beg;End=_End;type=_Type;
}

inline bool Region::isHang() const
{
    return(type==Hang);
}

inline bool Region::isIDP() const
{
    return(type==idp);
}

inline bool Region::isValid() const
{
    return(type!=Invalid)&&(size()>=1);
}

inline int Region::size() const
{
    return (End-Beg+1);
}

inline short Region::indexLocal2Global(short indexLocal) const
{
    return indexLocal+Beg;
}

inline short Region::indexGlobal2Local(short indexGlobal) const
{
    return indexGlobal-Beg;
}

OptiChain::OptiChain(int size)
{
    if(size<0)
        return;

}

OptiChain::OptiChain(ArrayXi High,ArrayXi Low,int _col)
{
    Col=_col;
    HighLine=High;
    LowLine=Low;
    SubChain.clear();
}

int OptiChain::validHeight(int index)
{
    if(index<0||index>=MapSize)return NInf;
    if(isAir(index))return NInf;
    return HighLine(index);
}

inline bool OptiChain::isAir(int index)
{
    if(index<0||index>=MapSize)return true;
    if(index==0)return false;
    return (Base(index-1,Col)==0);
}

inline bool OptiChain::isWater(int index)
{
    if(index<=0||index>=MapSize)return false;
    return (Base(index-1,Col)==12);
}

inline bool OptiChain::isSolidBlock(int index)
{
    if(index<0||index>=MapSize)return false;
    return(Base(index-1,Col)!=0&&Base(index-1,Col)!=12);
}

/*
void HeightLine::segment2Brackets(list<Pair>&List,short sBeg,short sEnd)
{
    if(sEnd<sBeg||sBeg<0)return;
    List.clear();

    if(sBeg==sEnd)
    {
        List.push_back(Pair('(',sBeg));
        List.push_back(Pair(')',sEnd));
        return;
    }

    queue<Region>Pure;
    queue<Region> disPure;//极大值区间
    Region Temp;
    bool isReady=false;
    VectorXi VHL=ValidHighLine();
    VectorXi ScanBoth=VHL,ScanLeft=VHL,ScanRight=VHL;
    ScanBoth.setZero();ScanLeft.setZero();ScanRight.setZero();
    for(int i=sBeg+1;i<sEnd-1;i++)//分别用三个算子处理
    {
        ScanBoth(i)=(VHL.segment(i-1,3).array()*Both.array()).sum();
        ScanLeft(i)=(VHL.segment(i-1,3).array()*Left.array()).sum();
        ScanRight(i)=(VHL.segment(i-1,3).array()*Right.array()).sum();
    }
ScanBoth=(ScanBoth.array()>=0).select(ScanBoth,0);
ScanLeft=(ScanLeft.array()>=0).select(ScanLeft,0);
ScanRight=(ScanRight.array()>=0).select(ScanRight,0);
    isReady=false;
    for(int i=sBeg+1;i<sEnd-1;i++)
    {
        if(!isReady&&ScanBoth(i)&&ScanLeft(i))
        {
            isReady=true;
            Temp.Begin=i;
        }
        if(isReady&&ScanBoth(i)&&ScanRight(i))
        {
            Temp.End=i;
            disPure.push(Temp);
            Temp.Begin=-1;
            Temp.End=-1;
            isReady=false;
        }
    }
    Temp.Begin=sBeg;
    Temp.End=sEnd;
    while(!disPure.empty())
    {
        Temp.End=disPure.front().Begin-1;
        Pure.push(Temp);
        Temp.Begin=disPure.front().End+1;
        disPure.pop();
        if(Temp.Begin>=sEnd)Temp.Begin=sEnd;
        Temp.End=sEnd;
    }
    Pure.push(Temp);

    while(!Pure.empty())
    {
        DealRegion(Pure.front(),List);
#ifndef NoOutPut
        cout<<'['<<Pure.front().Begin<<','<<Pure.front().End<<']'<<"->";
#endif
        Pure.pop();
    }
    List.push_front(Pair('(',sBeg));
    List.push_back(Pair(')',sEnd));
#ifndef NoOutPut
    disp(List);
#endif
}

inline void HeightLine::DealRegion(Region PR, list<Pair> &List)
{
    if(PR.Begin<0||PR.End<PR.Begin)return;
    List.push_back(Pair('(',PR.Begin));
    List.push_back(Pair(')',PR.End));
}
*/

void OptiChain::divideToChain()
{
    while(!Chain.empty())Chain.pop();
    Region Temp(0,MapSize-1,idp);
    for(int i=1;i<MapSize;i++)
    {
        if(isAir(i))
        {
            Temp.End=i-1;
            Chain.push(Temp);
            Temp.Beg=i;
        }
        if(isWater(i))
        {
            Temp.End=i-1;
            Chain.push(Temp);
            Temp.Beg=i;
        }
        if(isSolidBlock(i)&&isAir(i))
        {
            Temp.End=i-1;
            Chain.push(Temp);
            Temp.Beg=i;
        }
    }
    Temp.End=MapSize-1;
    Chain.push(Temp);
}

void OptiChain::divideToSubChain()
{
    if(Chain.empty())return;
    if(!Chain.front().isValid())
    {
        Chain.pop();
        return;
    }
    SubChain.clear();


}

void OptiChain::divideToSubChain(const Region &Cur)
{
    ArrayXi HL;
    HL<<HighLine.segment(Cur.Beg,Cur.size()),NInf;

    ArrayXi ScanBoth,ScanLeft,ScanRight;
    ScanBoth.setZero(Cur.size());
    ScanLeft.setZero(Cur.size());
    ScanRight.setZero(Cur.size());
    for(int i=1;i<Cur.size();i++)//用三个算子扫描一个大孤立区间
    {
        ScanBoth(i)=(HL.segment(i-1,3)*Both).sum()>0;
        ScanLeft(i)=(HL.segment(i-1,3)*Left).sum()>0;
        ScanRight(i)=(HL.segment(i-1,3)*Right).sum()>0;
    }
    ScanLeft*=ScanBoth;
    ScanRight*=ScanBoth;
    bool isReady=false;
    //表示已经检测出极大值区间的入口，找到出口就装入一个极大值区间
    Region Temp(-1,-1,Hang);//均写入绝对index而非相对index
    for(int i=0;i<Cur.size();i++)
    {
        if(!isReady&&ScanLeft(i))
        {
            isReady=true;
            Temp.Beg=Cur.indexLocal2Global(i);
        }
        if(isReady&&ScanRight(i))
        {
            isReady=false;
            Temp.End=Cur.indexLocal2Global(i);
            SubChain.push_back(Temp);
        }
    }
}
