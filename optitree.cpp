#include "optitree.h"

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
        return Child;
    }
    if(Child==NULL)Child=new Node;
    else
        Child->isAble=true;
    Child->Degree=Degree+1;
    return Child;
}

Node* Node::creatSib()
{
    if(haveSib())
    {
        qDebug("已存在激活的侧链，不能creatSib");
        return Sib;
    }
    if(Sib==NULL)Sib=new Node;
    else
        Sib->isAble=true;
    Sib->Degree=Degree;
    return Sib;
}

void Node::Freeze()
{
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
    qDebug("goDown成功");
}

void OptiTree::goNextSib()
{
    if(!Current()->haveSib())
    {
        qDebug("goNextSib失败");
        return;
    }
    Stack.push(Current()->Sib);
    qDebug("goNextSib成功");
}

void OptiTree::goPrevSib()
{
    Node*Temp=Current();
    Stack.pop();
    if(Current()->Degree!=Temp->Degree)
    {
        qDebug("goPrevSib失败");
        Stack.push(Temp);
        return;
    }
    qDebug("goPrevSib成功");
}

void OptiTree::goUp()
{
    Node*Temp=Current();
    while(!(Current()->Degree+1==Temp->Degree))Stack.pop();
}

void OptiTree::gotoRoot()
{
    preventEmpty();
    while(Current()!=Root)Stack.pop();
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
        Height-=3*(Height/3);
        Height.array()-=1;
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

void HeightLine::toBrackets(vector<short> &index, string &brackets)
{
    if(Size<=0)return;
    index.clear();brackets.clear();
    brackets+='(';index.push_back(0);
    int LOffset=1,ROffset=1;
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

    qDebug()<<"LOffset="<<LOffset<<";  ROffset="<<ROffset;
    //cout<<"Raw="<<brackets<<endl;

    for(;LOffset>0;LOffset--)
    {
        index.insert(index.begin(),0);
        brackets.insert(brackets.begin(),'(');
    }
    for(;ROffset>0;ROffset--)
    {
        index.push_back(Size-1);
        brackets.push_back(')');
    }

}

inline void HeightLine::Sink(Node*rg)
{
    if(rg->isComplete())
    {
        Height.segment(rg->Begin,rg->Length()).array()-=Height.segment(rg->Begin,rg->Length()).minCoeff();
        qDebug()<<"沉降了["<<rg->Begin<<','<<rg->End<<']';
    }
}
