#include "optitree.h"
#define HL_Rand_1
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

HeightLine::HeightLine(int _size,char method)
{
    if (_size>0)Size=_size;
    else
        return;
    Height.resize(Size);
    if(method=='R')
    {
        Height.setRandom();
        Height.array()-=Height.minCoeff();
#ifdef HL_Rand_1
        Height-=3*(Height/3);
        Height.array()-=1;
#else
        Height-=3*(Height/3);
        Height.array()-=5;//-5~4
        Height=(Height.array()>0).select(Height,-1);
        Height=(Height.array()<2).select(Height,0);
        Height=(Height.array()<=0).select(Height,1);
        srand(time(0));
        if(rand()<=RAND_MAX/2)
            Height*=-1;
#endif
        /*Height=(Height.array()<=0).select(Height,1);
        Height=(Height.array()>=0).select(Height,-1);*/
        for(int i=1;i<Size;i++)
            Height(i)+=Height(i-1);
        Height.array()-=Height.minCoeff();
    }
    else
        Height.setZero();

    //cout<<Height.transpose()<<endl;
}

HeightLine::HeightLine(const VectorXi&parent)
{
    Height=parent;Size=Height.size();
}

QImage HeightLine::toQImage()
{
    QImage img(Size,max(Height.maxCoeff()+1,Size/5),QImage::Format_ARGB32);
    QRgb isT=qRgb(0,0,0),isF=qRgb(255,255,255);
    img.fill(isF);

    for(int i=0;i<Size;i++)
        img.setPixelColor(i,img.height()-1-Height(i),isT);
    return img;
}

void HeightLine::toBrackets(list<short>&index,list<char>&brackets)
{
    if(Size<=0)return;
    index.clear();brackets.clear();
    brackets.push_back('(');index.push_back(0);
    int LOffset=0,ROffset=0;
    stack<char>S;//最后一步再加最外侧的大括号，将L与R的补偿设为1，自动囊括。
    for(int i=1;i<Size;i++)
    {
        if(Height(i)<Height(i-1))//下降处，是孤立区间的起始
        {
            S.push('F');
            brackets.push_back('(');
            index.push_back(i);
        }
        if(Height(i)>Height(i-1))//上升处，i-1为孤立区间的末尾
        {
            if(S.empty())LOffset++;
            else
                S.pop();
            brackets.push_back(')');
            index.push_back(i-1);
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

bool HeightLine::isContinious()
{
    return (Height.segment(1,Size-2).array()-Height.segment(0,Size-2).array()==0).all();
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
        Height.segment(rg->Begin,rg->Length()).array()-=Height.segment(rg->Begin,rg->Length()).minCoeff();
        //qDebug()<<"沉降了["<<rg->Begin<<','<<rg->End<<']';
    }
}

void HeightLine::SinkBoundary()
{
    if(isContinious())return;
    int gapB=0,gapE=0;
    for(int i=0;i<Size-1;i++)//正向遍历，去除前端浮空
    {
        if(Height(i)-Height(i+1)>=2)//右浮空
        {
            gapE=Height(i)-Height(i+1);//表示不连续段的落差
            Height.segment(0,i+1).array()-=min(gapE-1,Height.segment(0,i+1).minCoeff());
            break;
        }
    }

    for(int i=Size-1;i>0;i--)
    {
        if(Height(i)-Height(i-1)>=2)//左浮空
        {
            gapB=Height(i)-Height(i-1);
            Height.segment(i,Size-i).array()-=min(gapB-1,Height.segment(i,Size-i).minCoeff());
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
        if(Height(i)-Height(i-1)>=2)//左浮空
        {FBegin=i;
            gapB=Height(i)-Height(i-1);
            isReady=true;
        }
        if(Height(i)-Height(i+1)>=2&&isReady)//右浮空
        {
            FEnd=i;
            gapE=Height(i)-Height(i+1);
            Height.segment(FBegin,FEnd-FBegin+1).array()-=min(min(gapB,gapE)-1,Height.segment(FBegin,FEnd-FBegin+1).minCoeff());
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


void OptiTree::NaturalOpti(VectorXi &Raw)
{

    HeightLine HL(Raw);
    BuildTree(HL);
    gotoRoot();
    Compress(HL);

    Raw=HL.Height;
}


void OptiTree::BuildTree(HeightLine &HL)
{
    FreezeTree();
    gotoRoot();
    preventEmpty();
    list<short> Index;list<char> Brackets;
    HL.toBrackets(Index,Brackets);
    //cout<<Brackets<<endl;
    auto iI=Index.begin();
    auto iB=Brackets.begin();
    qDebug("开始BuildTree");

    for(;iI!=Index.end();)
    {
        if(*iB=='(')
        {//左括号
            //cout<<'(';
            if(Current()->isComplete())
            {//如果当前节点已完成，创建侧链并写入Begin
                Current()->creatSib()->Begin=*iI;
                goNextSib();
                //iI++;iB++;continue;
            }
            else
            {//如果当前节点Begin完成但End未完成，则创建子树，写入Begin
                if(Current()->Begin>=0)
                {
                    Current()->creatChild()->Begin=*iI;
                    goDown();
                }
                else
                {//否则写入Begin
                    Current()->Begin=*iI;
                }
            }
        }

        if(*iB==')')
        {//右括号
            //cout<<')';
            if(Current()->isComplete())
            {//如果当前节点已完成，则向上，写入End
                //qDebug()<<"当前度数"<<Current()->Degree<<"，即将goUp";
                goUp();
                Current()->End=*iI;
            }
            else
            {//如果当前节点未完成，则Begin必然已经完成（否则报错），写入End
                if(Current()->Begin<0)
                {
                    qDebug("出现错误：不成对的括号：过多的右括号");
                    return;
                }
                Current()->End=*iI;
            }
        }
        iI++;iB++;
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
