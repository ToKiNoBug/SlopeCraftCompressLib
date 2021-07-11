#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "optitree.h"
#include <iomanip>
using namespace std;
using namespace Eigen;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    /*OptiTree testTree;
    testTree.Root->SetValue(0,63);
    testTree.Current()->creatChild(0,32)->creatSib(33,63);
    testTree.goDown();
    testTree.Current()->creatChild(2,16)->creatSib(17,18)->creatSib(19,20)->creatSib(21,22)->creatSib(31,32);

    testTree.ShowTree();

    testTree.add({23,24});
    testTree.ShowTree();

    testTree.add({22,33});
    testTree.ShowTree();*/


}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_GenerateAndShow_clicked()
{
    Pair u;
    cout<<u.equalto('(')<<u.equalto(')')<<endl;
    Raw=HeightLine(128+1,'R');
    //QPixmap pxm=;
    //pxm.scaledToWidth(ui->ShowHeightLine->width(),Qt::SmoothTransformation);
    ui->ShowHeightLine->setPixmap(QPixmap::fromImage(Raw.toQImage().scaled(ui->ShowHeightLine->width(),ui->ShowHeightLine->height())));
    //list<short> a;
    //string b;
    //Raw.toBrackets(a,b);
    //cout<<b<<endl;
    qDebug()<<"Raw高度"<<Raw.HighLine.maxCoeff()+1;

}


void MainWindow::on_doCompress_clicked()
{
    static OptiTree Tree;
    Result=HeightLine(Raw.HighLine,Raw.LowLine);
    //cout<<Result.HighLine.rows()<<','<<Result.HighLine.cols()<<endl;
    auto t=clock();
    Tree.NaturalOpti(Result);
    qDebug()<<"用时："<<clock()-t;
    ui->ShowResult->setPixmap(QPixmap::fromImage(Result.toQImage().scaled(ui->ShowResult->width(),ui->ShowResult->height())));

    qDebug()<<"压缩后高度"<<Result.HighLine.maxCoeff();
    qDebug()<<"极值压缩率："<<100.0-100.0*Result.HighLine.maxCoeff()/Raw.HighLine.maxCoeff()<<"%";
    qDebug()<<"积分压缩率："<<100.0-100.0*Result.HighLine.sum()/Raw.HighLine.sum()<<"%";
    auto newDepth=Result.DepthLine();
    auto OldDepth=Raw.DepthLine();
    if((newDepth.array()!=OldDepth.array()).any())
        qDebug("旧Depth与新Depth不同");
    else
        qDebug("压缩后Depth不变");
    /*Tree.Root->disp();
    Tree.Root->Freeze();
    Tree.Root->disp();*/
}
