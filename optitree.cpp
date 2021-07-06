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

void HeightLine::toBrackets(list<short> &index, string &brackets)
{
    if(Size<=0)return;
    index.clear();brackets.clear();
    brackets+='(';index.push_back(0);
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

void OptiTree::NaturalOpti(VectorXi &Raw)
{
    Root->Freeze();
    gotoRoot();
    HeightLine HL(Raw);
    BuildTree(HL);
    gotoRoot();
    Compress(HL);
}


void OptiTree::BuildTree(HeightLine &HL)
{
    list<short> Index;string Brackets;
    HL.toBrackets(Index,Brackets);
    cout<<Brackets<<endl;
    auto iI=Index.cbegin();
    auto iB=Brackets.cbegin();

    for(;iI!=Index.cend();)
    {
        if(*iB=='(')
        {//左括号
            if(Current()->isComplete())
            {//如果当前节点已完成，创建侧链并写入Begin
                Current()->creatSib()->Begin=*iI;
                goNextSib();
                iI++;iB++;continue;
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
            if(Current()->isComplete())
            {//如果当前节点已完成，则向上，写入End
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
    qDebug("优化树构建完毕");
    cout<<endl;
    Root->disp();
    cout<<endl;
}

void OptiTree::Compress(HeightLine &HL)
{
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
