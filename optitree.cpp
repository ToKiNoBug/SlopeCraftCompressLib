#include "optitree.h"

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

inline void HeightLine::Sink(idpRegion & rg)
{
    if(rg.isEmpty()||rg.getEnd()>=Size)return;
    qDebug()<<"尝试沉降["<<rg.getBegin()<<','<<rg.getEnd()<<']';
    Height.segment(rg.getBegin(),rg.Length()).array()-=Height.segment(rg.getBegin(),rg.Length()).minCoeff();
    qDebug("沉降了一次");
}
idpRegion::idpRegion(short beg,short end)
{
    Begin=beg;End=end;
}

inline idpRegion::idpRegion(const idpRegion& parent)
{
    Begin=parent.Begin;End=parent.End;
}

inline idpRegion idpRegion::operator=(const idpRegion& parent)
{
    return idpRegion(parent);
}

inline bool idpRegion::isEmpty()
{
    return !(Begin>=0&&End>=Begin);
}

inline void idpRegion::setBegin(short beg)
{
    /*if(!isEmpty()||Begin<0)
    {qDebug("本idpRegion已有效/输入的Begin值无效，本次赋值无效");
        return;}*/
    Begin=beg;
}

inline void idpRegion::setEnd(short end)
{
    /*if(!isEmpty()||End<Begin)
    {qDebug("本idpRegion已有效/输入的End值无效，本次赋值无效");
        return;}*/
    End=end;
}

inline void idpRegion::setEmpty()
{
    Begin=-1;End=-1;
}

inline short idpRegion::getBegin()
{
    return Begin;
}

inline short idpRegion::getEnd()
{
    return End;
}

inline bool idpRegion::operator==(const idpRegion & b)
{
    return (Begin==b.Begin)&&(End==b.End);
}

inline bool idpRegion::operator<(const idpRegion & b)
{
    return (Begin>b.Begin)&&(End<b.End);
}

inline bool idpRegion::operator<=(const idpRegion & b)
{
    return (Begin>=b.Begin)&&(End<=b.End);
}

inline bool idpRegion::operator>(const idpRegion & b)
{
    return (Begin<b.Begin)&&(End>b.End);
}

inline bool idpRegion::operator>=(const idpRegion & b)
{
    return (Begin<=b.Begin)&&(End>=b.End);
}

inline bool idpRegion::operator!=(const idpRegion & b)
{
    return (Begin=!b.Begin)||(End!=b.End);
}

inline short idpRegion::Length()
{
    return End-Begin+1;
}

Node::Node(short beg,short end)
{
    Value.setBegin(beg);Value.setEnd(end);
    Child=NULL;Sib=NULL;isAble=true;
    Degree=-1;
}

Node::Node(const idpRegion& p)
{
    Value=p;Child=NULL;Sib=NULL;
    isAble=true;
    Degree=-1;
}

void Node::setEnable(bool able)
{
    isAble=able;
    Value.setEmpty();
    if(!able)
    {
        if(haveChild())Child->setEnable(able);
        if(haveSib())Sib->setEnable(able);
    }
}

inline void Node::creatChild()
{
    if(Child==NULL)
    {
        Child=new Node;
        qDebug("new了新Child节点");
    }

    Child->setEnable(true);
    Child->Value.setEmpty();
    Child->Degree=Degree+1;
}

inline void Node::creatSib()
{
    if(Sib==NULL)
    {
        Sib=new Node;
        qDebug("new了新的Sib节点");
    }
    Sib->setEnable(true);
    Sib->Value.setEmpty();
    Sib->Degree=Degree;
}

inline bool Node::isLeaf()
{
    return (Child==NULL||!Child->isAble);
}

Node::~Node()
{
    if(Child!=NULL)delete Child;
    if(Sib!=NULL)delete Sib;
}

inline bool Node::haveChild()
{
    return (Child!=NULL&&Child->isAble);
}

inline bool Node::haveSib()
{
    return (Sib!=NULL&&Sib->isAble);
}

inline void Node::setValue(short beg,short end)
{
    Value=idpRegion(beg,end);
}

void Node::disp()
{
    //深度优先遍历，Self,Child,Sib
    //[0,15]{[1,14]{}--[2,13]}
    cout<<'['<<Value.getBegin()<<','<<Value.getEnd()<<']';
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

OptiTree::OptiTree()
{
Root=new Node;
Root->Degree=0;
Root->Sib=NULL;
Root->Child=NULL;
Root->setEnable(true);
Root->Value.setEmpty();
Stack.push(Root);
preventEmpty();
}

OptiTree::~OptiTree()
{
    delete  Root;
}

inline Node* OptiTree::Current()
{
    return Stack.top();
}

void OptiTree::goNextSib()
{
    preventEmpty();
    if(Stack.top()->haveSib())
    {Stack.push(Stack.top()->Sib);
    qDebug("goNextSib成功");}
    else qDebug("goNextSib失败：Sib不存在或失活");
}

void OptiTree::goDown()
{
    preventEmpty();
    if(Stack.top()->haveChild())
    {Stack.push(Stack.top()->Child);
    qDebug("goDown成功");}
    else qDebug("goDown失败：Child不存在或失活");
}

inline bool OptiTree::isRoot()
{
    preventEmpty();
    return Stack.top()==Root;
}

void OptiTree::goPrevSib()
{
    preventEmpty();
    if(isRoot()||Stack.empty()){qDebug("错误：goPrevSib已达到根节点或栈清空");return;}
    Node*CurTemp=Stack.top();
    Stack.pop();
    if(Stack.empty()||Stack.top()->Degree!=CurTemp->Degree)
        Stack.push(CurTemp);
    else
    {qDebug("goPrevSib成功");return;}
    qDebug("goPrevSib失败：栈中前一个节点不是同深度的树");
}

void OptiTree::goUp()
{
    preventEmpty();
    if(isRoot()||Stack.empty()){qDebug("错误：goUp已达到根节点或栈清空");return;}
    Node *CurTemp=Stack.top();
    Stack.pop();
    if(Stack.top()->Degree+1==CurTemp->Degree&&Stack.top()->Child==CurTemp)
    {
        qDebug("goUp成功");
        return;
    }
    else  goUp();
}

inline bool OptiTree::isLeaf()
{
    preventEmpty();
    return Stack.top()->isLeaf();
}

inline void OptiTree::preventEmpty()
{
    if(Stack.empty())Stack.push(Root);
    Root->Degree=0;
}

inline void OptiTree::gotoRoot()
{
    preventEmpty();
    while(!isRoot())Stack.pop();
}

void OptiTree::addChild(short beg, short end)
{
    preventEmpty();
    /*if(beg<Stack.top()->Value.getBegin())
    {beg=Stack.top()->Value.getBegin();end=-1;}*/
    if(Stack.top()->haveChild())
    {
        qDebug("此节点已经有子树");
    }
    else
    Stack.top()->creatChild();

    Stack.top()->Child->setValue(beg,end);
}

void OptiTree::addSib(short beg, short end)
{
    preventEmpty();/*
    if(beg<Stack.top()->Value.getEnd())//Sib的起始不应小于左侧的末尾
    {
        Stack.top()->Value.getEnd();
        end=-1;
    }*/
    if(Stack.top()->haveSib())
    {
        qDebug("此节点已经有侧链");
    }
    else
    Stack.top()->creatSib();

    Stack.top()->Sib->setValue(beg,end);
}

void OptiTree::Freeze()
{
    gotoRoot();
    if(Root->haveChild())Root->Child->setEnable(false);
    preventEmpty();
    qDebug("Freeze完成");
}

void OptiTree::Clear()
{
    if(Root->Child!=NULL)delete Root->Child;
    if(Root->Sib!=NULL)delete Root->Sib;//其实Root就不应该与Sib
    gotoRoot();
}

void OptiTree::NaturalOpti(VectorXi & He)
{
    if(He.size()<=0)return;
    He.array()-=He.minCoeff();
    HeightLine HL(He);
    BuildTree(HL);
    gotoRoot();
    Compress(HL);
    He=HL.Height;
}

void OptiTree::BuildTree(HeightLine &HL)
{
    vector<short>index;string Brackets;
    HL.toBrackets(index,Brackets);
    Freeze();gotoRoot();Root->Value.setEmpty();
    disp();
    qDebug("括号式构建完成，开始构造优化树");
    for(unsigned int i=0;i<index.size();i++)
    {
        if(Brackets[i]=='(')
        {//qDebug()<<"rua!";
            //遇到左括号，如果当前节点的Beg无效，则写入；如果有效且当前节点已完成，则创建侧树；否则创建子树
            if(!Current()->Value.isEmpty())
            {//遇到左括号，如果当前节点完整，则创建侧链
                addSib(index[i]);
                goNextSib();
            }
            else
            {//遇到左括号且当前节点不完整
                if(Current()->Value.getBegin()<0)
                {
                    Current()->Value.setBegin(index[i]);
                }
                else//遇到左括号且当前节点前完整后不完整，则创建子树
                {
                    addChild(index[i]);
                    goNextSib();
                }

            }
            continue;
        }
        if(Brackets[i]==')')
        {//遇到右括号
            //如果当前节点完整，则向上，写入end
            //如果当前节点不完整，写入end
            if(Current()->Value.isEmpty())
            {
                goUp();Current()->Value.setEnd(index[i]);
            }
            else
            {
                Current()->Value.setEnd(index[i]);
            }
        }
    }
    qDebug("BuildTree构建完成");
    disp();
}

void OptiTree::Compress(HeightLine &HL)
{
    HL.Sink(Current()->Value);
    if(Current()->haveChild())
    {
        goDown();
        Compress(HL);
    }
    if(Current()->haveSib())
    {
        goNextSib();
        Compress(HL);
    }

}

void OptiTree::disp()
{
    cout<<endl;
Root->disp();
cout<<endl;
}
