#include "OptiChain.h"

ArrayXXi OptiChain::Base=MatrixXi::Zero(0,0);

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
    return(type!=Invalid);
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
    Chain.clear();
}
