#include "optitree.h"
//#define HL_Rand_1

#define NoOutPut

#define HighThreshold 1.5/3
#define LowThreshold 0.1/3
#define maxIterTimes 5000

Vector3i HeightLine::Both(-1,2,-1);
Vector3i HeightLine::Left(-1,1,0);
Vector3i HeightLine::Right(0,1,-1);
MatrixXi HeightLine::Base;
short HeightLine::currentColum=0;

Pair::Pair(char _type,short _index)
{
    type=_type;index=_index;
}

bool Pair::equalto(char _type)
{
    return type==_type;
}

Pair::~Pair()
{
    return;
}

HeightLine::HeightLine(int _size,char method,int Low,int High)
{
    if (_size>0)Size=_size;
    else
        return;
    HighLine.setZero(Size);
    LowLine.setZero(Size);
    VectorXi Offset;
    Offset.setZero(Size);
    if(method=='R')
    {
#ifdef HL_Rand_1
        HighLine.setRandom();
        HighLine.array()-=HighLine.minCoeff();
        HighLine-=3*(HighLine/3);
        HighLine.array()-=1;
#else
        static bool isFirst=true;
        if(isFirst)
        {
        srand(time(0));
        isFirst=false;
        }
        float x=cos((rand()%32768)/32767.0)/1.1;
        for(int i=0;i<Size;i++)
        {
            x=4.0*x*(1.0-x);
            if(x>HighThreshold)
                HighLine(i)=1;
            if(x<LowThreshold)
                HighLine(i)=-1;
        }


        if(rand()%2)
            HighLine*=-1;
#endif
        /*Height=(Height.array()<=0).select(Height,1);
        Height=(Height.array()>=0).select(Height,-1);*/
        for(int i=1;i<Size;i++)
        {
            if(isAir(i))
            {
                Offset(i)=0;
                HighLine(i)+=HighLine(i-1);
            }
            else
                if(isWater(i)){
                switch (rand()%3)
                {
                case 0:
                    Offset(i)=1;
                    break;
                case 1:
                    Offset(i)=6;
                    break;
                case 2:
                    Offset(i)=11;
                    break;
                }
                if(Offset(i-1)>0)
                    HighLine(i)=HighLine(i-1);
                else
                    HighLine(i)=HighLine(i-1)+1;
            }
            else
            {
                if(Offset(i-1)>0)
                HighLine(i)+=HighLine(i-1)-1;
                else
                    HighLine(i)+=HighLine(i-1);
            }
        }
        LowLine=HighLine-Offset;
        HighLine.array()-=LowLine.minCoeff();
        LowLine.array()-=LowLine.minCoeff();

    }

    if(method=='L')
    {
        HighLine.setLinSpaced(Low,High);
        LowLine=HighLine;
        return;
    }

    //if(method=='Z')


    HighLine.array()-=LowLine.minCoeff();
    LowLine.array()-=LowLine.minCoeff();
    //cout<<Height.transpose()<<endl;
}

HeightLine::HeightLine(const VectorXi&HighL,const VectorXi&LowL)
{
    HighLine=HighL;
    LowLine=LowL;
    Size=HighLine.size();
}

inline bool HeightLine::isWater(int index)
{
    //return HighLine(index)-LowLine(index);//0->普通方块，>=1 -> 水柱
    if(index<=0)return false;
    return Base(index-1,currentColum)==12;
}//水柱罩顶玻璃计入最高，但托底玻璃不计入

inline bool HeightLine::isAir(int index)
{
    if(index<=0)return false;
    return Base(index-1,currentColum)==0;
}

inline bool HeightLine::isNormalBlock(int index)
{
    return !(isAir(index)||isWater(index));
    /*if(index<=0)return true;
    return (Base(index-1,currentColum)>0)&&(Base(index-1,currentColum)!=12);*/
}

QImage HeightLine::toQImage()
{
    int H=HighLine.maxCoeff()-LowLine.minCoeff()+1;
    QImage img(Size,max(H,min(Size/5,100)),QImage::Format_ARGB32);
    QRgb isT=qRgb(0,0,0),isF=qRgb(255,255,255),Water=qRgb(0,64,255),grey=qRgb(192,192,192);
    img.fill(isF);

    for(int i=0;i<Size;i++)
    {
        if(isAir(i))continue;
        img.setPixelColor(i,img.height()-1-HighLine(i),isT);
        if(isWater(i))
        {
            img.setPixelColor(i,img.height()-1-HighLine(i),grey);
            for(int y=LowLine(i);y<HighLine(i);y++)
                img.setPixelColor(i,img.height()-1-y,Water);
            if(LowLine(i)>=1)
                img.setPixelColor(i,img.height()-1-(LowLine(i)-1),grey);
        }
    }
    return img;
}

/*
void HeightLine::toBrackets(list<Pair> &List)
{
    if(Size<=0)return;
    List.clear();
    queue<Region>Pure;
    queue<Region> disPure;//极大值区间
    Region Temp;
    bool isReady=false;
    VectorXi VHL=ValidHighLine();
    VectorXi ScanBoth=VHL,ScanLeft=VHL,ScanRight=VHL;
    ScanBoth.setZero();ScanLeft.setZero();ScanRight.setZero();
    for(int i=1;i<Size-1;i++)//分别用三个算子处理
    {
        if(isWater(i))
        {
            ScanBoth(i)=2;
            ScanLeft(i)=1;
            ScanRight(i)=1;
            continue;
        }
        ScanBoth(i)=(VHL.segment(i-1,3).array()*Both.array()).sum();
        ScanLeft(i)=(VHL.segment(i-1,3).array()*Left.array()).sum();
        ScanRight(i)=(VHL.segment(i-1,3).array()*Right.array()).sum();
    }
ScanBoth=(ScanBoth.array()>=0).select(ScanBoth,0);
ScanLeft=(ScanLeft.array()>=0).select(ScanLeft,0);
ScanRight=(ScanRight.array()>=0).select(ScanRight,0);
    isReady=false;
    for(int i=1;i<Size-1;i++)
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
    Temp.Begin=0;
    Temp.End=Size-1;
    while(!disPure.empty())
    {
        Temp.End=disPure.front().Begin-1;
        Pure.push(Temp);
        Temp.Begin=disPure.front().End+1;
        disPure.pop();
        if(Temp.Begin>=Size-1)Temp.Begin=Size-1;
        Temp.End=Size-1;
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
    List.push_front(Pair('(',0));
    List.push_back(Pair(')',Size-1));
#ifndef NoOutPut
    disp(List);
#endif
}
*/
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
        if(isWater(i))
        {
            /*ScanBoth(i)=2;
            ScanLeft(i)=1;
            ScanRight(i)=1;
            continue;*/
        }
        ScanBoth(i)=(VHL.segment(i-1,3).array()*Both.array()).sum();
        ScanLeft(i)=(VHL.segment(i-1,3).array()*Left.array()).sum();
        ScanRight(i)=(VHL.segment(i-1,3).array()*Right.array()).sum();
    }
ScanBoth=(ScanBoth.array()>=0).select(ScanBoth,0);
ScanLeft=(ScanLeft.array()>=0).select(ScanLeft,0);
ScanRight=(ScanRight.array()>=0).select(ScanRight,0);
/*
cout<<"ScanBoth="<<endl<<ScanBoth.transpose()<<endl;
cout<<"ScanLeft="<<endl<<ScanLeft.transpose()<<endl;
cout<<"ScanRight="<<endl<<ScanRight.transpose()<<endl;*/
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


inline int HeightLine::validHigh(int index)
{
    if(isAir(index))return -1;
    if(isWater(index))return HighLine(index)-1;
    else return HighLine(index);
}

void HeightLine::toSubRegion(queue<Region> &Queue)
{
    while(!Queue.empty())Queue.pop();
    Region Temp;
    Temp.Begin=0;Temp.End=Size-1;
    for(int i=1;i<Size;i++)
    {
        if(!isNormalBlock(i))
        {
            Temp.End=i-1;
            Queue.push(Temp);
            Temp.End=Size-1;
            Temp.Begin=i;
            if(isAir(i)&&i<Size-1)
            {
                HighLine(i)=HighLine(i+1);
                LowLine(i)=LowLine(i+1);
            }
#ifndef NoOutPut
            qDebug()<<"出现断点"<<i;
#endif
        }
    }
    Queue.push(Temp);
#ifndef NoOutPut
    /*qDebug("输出Queue：");
    qDebug()<<"size of Queue="<<Queue.size();
    QString disper="";
    while(!Queue.empty())
    {
        //cout<<'['<<Queue.back().Begin<<','<<Queue.back().End<<"]->";
        disper+='[';
        disper+=QString::number(Queue.front().Begin)+',';
        disper+=QString::number(Queue.front().End)+"]->";
        Queue.pop();
    }
    qDebug()<<disper;*/
#endif
}

VectorXi HeightLine::DepthLine()
{
    VectorXi Depth=HighLine.segment(1,Size-1).array()*0;
    for(int i=0;i<Size-1;i++)
    {
        if(isAir(i+1))
        {
            Depth(i)=1;
            continue;
        }
        if(isWater(i+1))
        {
            switch (HighLine(i+1)-LowLine(i+1)) {
            case 0+1:
                Depth(i)=0;
                break;
            case 5+1:
                Depth(i)=1;
                break;
            case 10+1:
                Depth(i)=2;
                break;
            }
        }
        else
        {
                if(validHigh(i+1)<validHigh(i))
                {
                    Depth(i)=0;
                    continue;
                }
                if(validHigh(i+1)==validHigh(i))
                {
                    Depth(i)=1;
                    continue;
                }
                if(validHigh(i+1)>validHigh(i))
                {
                    Depth(i)=2;
                    continue;
                }
        }
    }
    return Depth;
}

inline VectorXi HeightLine::ValidHighLine()
{
    return ((HighLine-LowLine).array()==0).select(HighLine,HighLine.array()-1);
}


inline void HeightLine::Sink(Node*rg)
{
    if(rg->isComplete())
    {
        HighLine.segment(rg->Begin,rg->Length()).array()-=LowLine.segment(rg->Begin,rg->Length()).minCoeff();
        LowLine.segment(rg->Begin,rg->Length()).array()-=LowLine.segment(rg->Begin,rg->Length()).minCoeff();
        //qDebug()<<"沉降了["<<rg->Begin<<','<<rg->End<<']';
    }
}

void HeightLine::SinkBoundary(short Beg,short End)
{
    if(Beg<0||End<Beg)
    {
        Beg=0;End=Size-1;
    }
    int gapB=0,gapE=0;

    for(int i=Beg;i<End;i++)//正向遍历，去除前端浮空
    {
        if(validHigh(i)-validHigh(i+1)>=2)//右浮空
        {
            gapE=validHigh(i)-validHigh(i+1);//表示不连续段的落差
            HighLine.segment(Beg,i-Beg+1).array()-=min(gapE-1,LowLine.segment(Beg,i-Beg+1).minCoeff());
            LowLine.segment(Beg,i-Beg+1).array()-=min(gapE-1,LowLine.segment(Beg,i-Beg+1).minCoeff());
            break;
        }
    }
    for(int i=End;i>Beg;i--)
    {
        if(validHigh(i)-validHigh(i-1)>=2)//左浮空
        {
            gapB=validHigh(i)-validHigh(i-1);
            HighLine.segment(i,End+1-i).array()-=min(gapB-1,LowLine.segment(i,End+1-i).minCoeff());
            LowLine.segment(i,End+1-i).array()-=min(gapB-1,LowLine.segment(i,End+1-i).minCoeff());
            break;
        }
    }

    int FBegin=Beg+1,FEnd=End;
    bool isReady=false;
    for(int i=Beg+1;i<End-2;i++)//从i=1遍历至i=Size-2
    {
        if(validHigh(i)-validHigh(i-1)>=2)//左浮空
        {FBegin=i;
            gapB=validHigh(i)-validHigh(i-1);
            isReady=true;
        }
        if(validHigh(i)-validHigh(i+1)>=2&&isReady)//右浮空
        {
            FEnd=i;
            gapE=validHigh(i)-validHigh(i+1);
            HighLine.segment(FBegin,FEnd-FBegin+1).array()-=min(min(gapB,gapE)-1,LowLine.segment(FBegin,FEnd-FBegin+1).minCoeff());
            LowLine.segment(FBegin,FEnd-FBegin+1).array()-=min(min(gapB,gapE)-1,LowLine.segment(FBegin,FEnd-FBegin+1).minCoeff());
            //qDebug("沉降了中间的漂浮段");
            isReady=false;
        }
    }
}

void disp(const list<Pair>&L)
{
    if(L.empty())return;
    cout<<endl;
    for(auto i=L.cbegin();i!=L.cend();i++)
        cout<<" "<<i->type;
    cout<<endl;
    for(auto i=L.cbegin();i!=L.cend();i++)
        cout<<" "<<i->index;
    cout<<endl;
    cout<<endl;
}
