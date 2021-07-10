#include "optitree.h"
//#define HL_Rand_1

#define HighThreshold 1.5/3
#define LowThreshold 0.1/3

Pair::Pair(char _type,short _index)
{
    type=_type;index=_index;
}

inline bool Pair::operator==(char _type)
{
    return type==_type;
}

Pair::~Pair()
{
    return;
}

Node::Node(short beg,short end)
{
    Begin=beg;
    End=end;
    Degree=0;
    Child=NULL;
    Sib=NULL;
    isAble=true;
}

void Node::disp()
{
    cout<<'['<<Begin<<','<<End<<']';
    if(haveChild())
    {
        cout<<'{';
        Child->disp();
        cout<<'}';
    }
    if(haveSib())
    {
        cout<<'-';
        Sib->disp();
    }
}

inline bool Node::haveChild()
{
    return (Child!=NULL)&&(Child->isAble);
}

inline bool Node::haveSib()
{
    return (Sib!=NULL)&&(Sib->isAble);
}

inline bool Node::isComplete()
{
    return (Begin>=0)&&(End>=Begin);
}

Node* Node::creatChild()
{
    if(haveChild())
    {
        qDebug("已存在激活的子树，不能creatChild");
        Child->Degree=Degree+1;
        return Child;
    }
    if(Child==NULL)Child=new Node;

    Child->isAble=true;
    Child->Degree=Degree+1;
    return Child;
}

Node* Node::creatSib()
{
    if(haveSib())
    {
        qDebug("已存在激活的侧链，不能creatSib");
        Sib->Degree=Degree;
        return Sib;
    }
    if(Sib==NULL)Sib=new Node;

    Sib->isAble=true;
    Sib->Degree=Degree;
    return Sib;
}

void Node::Freeze()
{
    Begin=-1;End=-1;
    if(!isAble)return;
    isAble=false;
    if(haveChild())Child->Freeze();
    if(haveSib())Sib->Freeze();
}


OptiTree::OptiTree()
{
    Root=new Node;
    Root->isAble=true;
    Root->Degree=0;
    Stack.push(Root);
}

OptiTree::~OptiTree()
{
    delete Root;
}

inline Node* OptiTree::Current()
{
    return Stack.top();
}

inline short Node::Length()
{
    return End-Begin+1;
}

Node::~Node()
{
    if(Child!=NULL)delete Child;
    if(Sib!=NULL)delete Sib;
}

void OptiTree::goDown()
{
    if(!Current()->haveChild())
    {
        qDebug("goDown失败");
        return;
    }
    Stack.push(Current()->Child);
    //qDebug("goDown成功");
}

void OptiTree::goNextSib()
{
    if(!Current()->haveSib())
    {
        qDebug("goNextSib失败");
        return;
    }
    Stack.push(Current()->Sib);
    //qDebug("goNextSib成功");
}

void OptiTree::goPrevSib()
{
    preventEmpty();
    Node*Temp=Current();
    Stack.pop();
    if(Current()->Degree!=Temp->Degree||Current()->Sib!=Temp)
    {
        qDebug("goPrevSib失败");
        Stack.push(Temp);
        return;
    }
    //qDebug("goPrevSib成功");
}

void OptiTree::goUp()
{
    preventEmpty();
    Node*Temp=Current();
    //qDebug("开始goUp");
    while(!(Current()->Degree+1==Temp->Degree))Stack.pop();
    //qDebug("goUp成功");
}

void OptiTree::gotoRoot()
{
    while(!Stack.empty())Stack.pop();
    preventEmpty();
    //qDebug("gotoRoot成功");
}

inline void OptiTree::preventEmpty()
{
    if(Stack.empty())Stack.push(Root);
}

inline bool OptiTree::isRoot()
{
    return Current()==Root;
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
        float x=(rand()%32768)/32767.0;
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

            if(rand()%5==6){
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
    return HighLine(index)-LowLine(index);//0->普通方块，>=1 -> 水柱
}//水柱罩顶玻璃计入最高，但托底玻璃不计入

QImage HeightLine::toQImage()
{
    int H=HighLine.maxCoeff()-LowLine.minCoeff()+1;
    QImage img(Size,max(H,min(Size/5,100)),QImage::Format_ARGB32);
    QRgb isT=qRgb(0,0,0),isF=qRgb(255,255,255),Water=qRgb(0,64,255),grey=qRgb(192,192,192);
    img.fill(isF);

    for(int i=0;i<Size;i++)
    {
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

void HeightLine::toBrackets(list<short>&index,list<char>&brackets)
{
    if(Size<=0)return;
    index.clear();brackets.clear();
    brackets.push_back('(');index.push_back(0);
    int LOffset=0,ROffset=0;
    stack<short>S;//最后一步再加最外侧的大括号，将L与R的补偿设为1，自动囊括。
    short lastLeftIndex=0;
    //考虑更优的补全括号方式：不一定要补全到两端，也许可以补全到最近的一个左/又括号
    for(int i=1;i<Size;i++)
    {
        /*if(isWater(i))
        {//遇到水柱，则先画一个右括号，再画一个左括号
            if(S.empty())LOffset++;
            else
                S.pop();
            brackets.push_back(')');
            index.push_back(i-1);
            S.push('F');
            brackets.push_back('(');
            index.push_back(i);
            continue;
        }*/
        if(isWater(i)||validHigh(i)>validHigh(i-1))//上升处，i-1为孤立区间的末尾
        {
            if(S.empty())LOffset++;
            else
                S.pop();
            brackets.push_back(')');
            index.push_back(i-1);
        }
        if(isWater(i)||validHigh(i)<validHigh(i-1))//下降处，是孤立区间的起始
        {
            S.push('F');
            brackets.push_back('(');
            index.push_back(i);
        }



    }
    ROffset+=1+S.size();

    //qDebug()<<"LOffset="<<LOffset<<";  ROffset="<<ROffset;
    //cout<<"Raw="<<brackets<<endl;

    for(;LOffset>0;LOffset--)
    {
        index.insert(index.begin(),0);
        brackets.push_front('(');
    }
    for(;ROffset>0;ROffset--)
    {
        index.push_back(Size-1);
        brackets.push_back(')');
    }

}

void HeightLine::toBrackets(list<Pair> &List)
{
    if(Size<=0)return;
    List.clear();
    queue<Region>Pure;
    Region Temp;
    bool isReady=false;
    for(int i=1;i<Size;i++)//寻找极大值区间
    {
        if(!isReady)
        {
            if(isWater(i)||validHigh(i-1)<validHigh(i))//找到极大值区间的起点
            {
                isReady=true;
            }
        }
    }


    disp(List);
}

inline void HeightLine::DealRegion(Region PR, list<Pair> &List)
{
    if(PR.Begin<0||PR.End<PR.Begin)return;
    List.push_back(Pair('(',PR.Begin));
    List.push_back(Pair(')',PR.End));
}

inline bool HeightLine::isContinious()
{
    return (HighLine.segment(1,Size-2).array()-HighLine.segment(0,Size-2).array()==0).all();
}
inline int HeightLine::validHigh(int index)
{
    if(isWater(index))return HighLine(index)-1;
    else return HighLine(index);
}
VectorXi HeightLine::DepthLine()
{
    VectorXi Depth=HighLine.segment(1,Size-1).array()*0;
    for(int i=0;i<Size-1;i++)
    {
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

/*
void HeightLine::toBrackets_Near(list<short> &index, string &brackets)
{

}*/

/*void HeightLine::SinkMonotonous()
{
    if(isContinious())return;
    int IndexB=0,IndexE=0;
    int gapB=0,gapE=0;//表示不连续段的落差绝对值
    bool isReady=false,isIncrease=false;
    for(int i=1;i<Size;i++)
    {
        isReady=isReady&&(isIncrease==(Height(i)>Height(i-1)));//如果区间不单调，不予沉降
        if(abs(Height(i)-Height(i-1))>=2)//出现不连续
        {
            if(!isReady)//进入了不连续区间
            {
                isIncrease=Height(i)>Height(i-1);
                gapB=abs(Height(i)-Height(i-1))-1;
                isReady=true;
                IndexB=i;
                continue;
            }
            if(isReady)//将要结束一个不连续单调区间
            {
                gapE=abs(Height(i)-Height(i-1))-1;
                IndexE=i-1;
                Height.segment(IndexB,IndexE-IndexB+1).array()-=min(isIncrease?gapB:gapE,Height.segment(IndexB,IndexE-IndexB+1).minCoeff());
                isReady=false;
                IndexB=-1;IndexE=-1;
                qDebug()<<"沉降了一个漂浮单调递"<<(isIncrease?"增":"减")<<"区间";
            }
        }
    }
}*/

inline void HeightLine::Sink(Node*rg)
{
    if(rg->isComplete())
    {
        HighLine.segment(rg->Begin,rg->Length()).array()-=LowLine.segment(rg->Begin,rg->Length()).minCoeff();
        LowLine.segment(rg->Begin,rg->Length()).array()-=LowLine.segment(rg->Begin,rg->Length()).minCoeff();
        //qDebug()<<"沉降了["<<rg->Begin<<','<<rg->End<<']';
    }
}

void HeightLine::SinkBoundary()
{
    if(isContinious())return;
    int gapB=0,gapE=0;
    for(int i=0;i<Size-1;i++)//正向遍历，去除前端浮空
    {
        if(validHigh(i)-validHigh(i+1)>=2)//右浮空
        {
            gapE=validHigh(i)-validHigh(i+1);//表示不连续段的落差
            HighLine.segment(0,i+1).array()-=min(gapE-1,LowLine.segment(0,i+1).minCoeff());
            LowLine.segment(0,i+1).array()-=min(gapE-1,LowLine.segment(0,i+1).minCoeff());
            break;
        }
    }

    for(int i=Size-1;i>0;i--)
    {
        if(validHigh(i)-validHigh(i-1)>=2)//左浮空
        {
            gapB=validHigh(i)-validHigh(i-1);
            HighLine.segment(i,Size-i).array()-=min(gapB-1,LowLine.segment(i,Size-i).minCoeff());
            LowLine.segment(i,Size-i).array()-=min(gapB-1,LowLine.segment(i,Size-i).minCoeff());
            break;
        }
    }

    int FBegin=1,FEnd=Size-1;
    bool isReady=false;
    //bool isBSinkable=false,isESinkable=false;
    /*
    for(int i=1;i<Size;i++)
    {
        if(abs(Height(i)-Height(i-1))>=2)
        {
            if(!isReady)
            {
                FBegin=i;
                gapB=Height(i)-Height(i-1);
                isBSinkable=gapB>0;
                gapB=abs(gapB)-1;
                isReady=true;
                continue;
            }
            if(isReady)
            {
                FEnd=i-1;
                gapE=Height(i)-Height(i-1);
                isESinkable=gapE<0;
                gapE=abs(gapE)-1;
                Height.segment(FBegin,FEnd-FBegin+1).array()-=min(min(isBSinkable*gapB,isESinkable*gapE),Height.segment(FBegin,FEnd-FBegin+1).minCoeff());
                isReady=false;

            }
        }
    }*/


    for(int i=1;i<Size-1;i++)//从i=1遍历至i=Size-2
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

/*void HeightLine::SinkInner()
{
    queue<short>Gap,Index;//取断崖的前值为index
    int GapVal=0,IndexVal=0;
    bool isBSinkable=false,isESinkable=false;
    int Offset=0;
    for(int i=1;i<Size;i++)
    {
        if(abs(GapVal=Height(i)-Height(i-1))>=2)
        {
            Gap.push(GapVal);//Gap值为后减前
            Index.push(i-1);
            qDebug()<<i-1;
        }
    }
    if(Gap.size()<=1)return;
    do
    {
        GapVal=Gap.front();IndexVal=Index.front();
        Gap.pop();Index.pop();
        isBSinkable=GapVal>0;//对于一段内部连续的区间，前沟大于0才有意义
        isESinkable=Gap.front()<0;
        if(!isBSinkable&&!isESinkable)continue;
        if(isBSinkable&&isESinkable)
            Offset=min(GapVal,abs(Gap.front()))-1;
        if(isBSinkable&&!isESinkable)
            Offset=abs(Gap.front())-1;
        if(!isBSinkable&&isESinkable)
            Offset=GapVal-1;
        Height.segment(IndexVal,Index.front()-IndexVal+1).array()-=min(Offset,Height.segment(IndexVal,Index.front()-IndexVal+1).minCoeff());
    }
    while(!Gap.empty());

}*/


void OptiTree::NaturalOpti(VectorXi &HighL,VectorXi&LowL)
{
    HeightLine HL(HighL,LowL);
    NaturalOpti(HL);
    LowL=HL.LowLine;
    HighL=HL.HighLine;
}


void OptiTree::BuildTree(HeightLine &HL)
{
    FreezeTree();
    gotoRoot();
    preventEmpty();
    //list<short> Index;list<char> Brackets;
    list<Pair> Index;
    //HL.toBrackets(Index,Brackets);
    HL.toBrackets(Index);
    //cout<<Brackets<<endl;
    auto iter=Index.begin();
    //auto iter=Brackets.begin();
    qDebug("开始BuildTree");

    for(;iter!=Index.end();)
    {
        if(*iter=='(')
        {//左括号
            //cout<<'(';
            if(Current()->isComplete())
            {//如果当前节点已完成，创建侧链并写入Begin
                Current()->creatSib()->Begin=iter->index;
                goNextSib();
                //iter++;iter++;continue;
            }
            else
            {//如果当前节点Begin完成但End未完成，则创建子树，写入Begin
                if(Current()->Begin>=0)
                {
                    Current()->creatChild()->Begin=iter->index;
                    goDown();
                }
                else
                {//否则写入Begin
                    Current()->Begin=iter->index;
                }
            }
        }

        if(*iter==')')
        {//右括号
            //cout<<')';
            if(Current()->isComplete())
            {//如果当前节点已完成，则向上，写入End
                //qDebug()<<"当前度数"<<Current()->Degree<<"，即将goUp";
                goUp();
                Current()->End=iter->index;
            }
            else
            {//如果当前节点未完成，则Begin必然已经完成（否则报错），写入End
                if(Current()->Begin<0)
                {
                    qDebug("出现错误：不成对的括号：过多的右括号");
                    return;
                }
                Current()->End=iter->index;
            }
        }
        iter++;iter++;
    }
    cout<<endl;
    qDebug("优化树构建完毕");

}

void OptiTree::Compress(HeightLine &HL)
{
    /*queue<Node*> Que;
    Que.push(Root);
    Node *Temp=NULL;
    while(!Que.empty())
    {
        HL.Sink(Temp=Que.front());
        Que.pop();
        if(Temp->haveSib())
        {
            Que.push(Temp->Sib);
        }
        if(Temp->haveChild())
        {
            Que.push(Temp->Child);
        }

    }*/
    //DFS中序遍历
    HL.Sink(Current());
    if(Current()->haveChild())
    {
        goDown();
        Compress(HL);
        goUp();
    }
    if(Current()->haveSib())
    {
        goNextSib();
        Compress(HL);
        goPrevSib();
    }
}

void OptiTree::FreezeTree()
{
    Root->Freeze();
    Root->isAble=true;
}

void OptiTree::ShowTree()
{
    cout<<endl;
    Root->disp();
    cout<<endl;
}

void OptiTree::NaturalOpti(HeightLine& HL)
{
    BuildTree(HL);
    gotoRoot();
    Compress(HL);
    HL.SinkBoundary();
    HL.SinkBoundary();
}

void disp(const list<Pair>&L)
{
    if(L.empty())return;
    cout<<endl;
    for(auto i=L.cbegin();i!=L.cend();i++)
        cout<<"   "<<i->type;
    cout<<endl;
    for(auto i=L.cbegin();i!=L.cend();i++)
        cout<<" "<<i->index;
    cout<<endl;
    cout<<endl;
}
