#include "mainwindow.h"
#include "ui_mainwindow.h"
//#include "heightline.h"
#include "optitree.h"
#include <iomanip>
using namespace std;
using namespace Eigen;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_GenerateAndShow_clicked()
{
    Raw=HeightLine(64+1,'R');
    //QPixmap pxm=;
    //pxm.scaledToWidth(ui->ShowHeightLine->width(),Qt::SmoothTransformation);
    ui->ShowHeightLine->setPixmap(QPixmap::fromImage(Raw.toQImage().scaled(ui->ShowHeightLine->width(),ui->ShowHeightLine->height())));
    //list<short> a;
    //string b;
    //Raw.toBrackets(a,b);
    //cout<<b<<endl;
    qDebug()<<"Raw高度"<<Raw.Height.maxCoeff()+1;

}


void MainWindow::on_doCompress_clicked()
{
    static OptiTree Tree;
    Result=HeightLine(Raw.Height);
    //cout<<Result.Height.rows()<<','<<Result.Height.cols()<<endl;
    auto t=clock();
    Tree.NaturalOpti(Result.Height);
    qDebug()<<"用时："<<clock()-t;
    ui->ShowResult->setPixmap(QPixmap::fromImage(Result.toQImage().scaled(ui->ShowResult->width(),ui->ShowResult->height())));

    qDebug()<<"压缩后高度"<<Result.Height.maxCoeff();
    qDebug()<<"极值压缩率："<<100.0-100.0*Result.Height.maxCoeff()/Raw.Height.maxCoeff()<<"%";
    qDebug()<<"积分压缩率："<<100.0-100.0*Result.Height.sum()/Raw.Height.sum()<<"%";
    auto newDepth=Result.Height.segment(1,Result.Size-1).array()-Result.Height.segment(0,Result.Size-1).array();
    auto OldDepth=Raw.Height.segment(1,Raw.Size-1).array()-Raw.Height.segment(0,Raw.Size-1).array();
    if((newDepth*OldDepth<0).any())
        qDebug("旧Depth与新Depth不同");
    else
        qDebug("压缩后Depth不变");
    /*Tree.Root->disp();
    Tree.Root->Freeze();
    Tree.Root->disp();*/
}
