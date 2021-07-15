#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "optitree.h"
#include <iomanip>

#define Rows 512
#define Freq 5
using namespace std;
using namespace Eigen;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    HeightLine::Base=MatrixXi::Random(Rows,Rows);
    HeightLine::Base=HeightLine::Base-Freq*(HeightLine::Base/Freq);
    HeightLine::Base=(HeightLine::Base.array()<Freq-1).select(HeightLine::Base,12);
    HeightLine::currentColum=2;

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_GenerateAndShow_clicked()
{
    Pair u;
    cout<<u.equalto('(')<<u.equalto(')')<<endl;
    Raw=HeightLine(Rows+1,'R');
    queue<Region> p;
    ui->ShowHeightLine->setPixmap(QPixmap::fromImage(Raw.toQImage().scaled(ui->ShowHeightLine->width(),ui->ShowHeightLine->height())));

    qDebug()<<"Raw高度"<<Raw.HighLine.maxCoeff()+1;

}


void MainWindow::on_doCompress_clicked()
{
    static OptiTree Tree;
    Result=HeightLine(Raw.HighLine,Raw.LowLine);
    auto t=clock();
    Tree.NaturalOpti(Result.HighLine,Result.LowLine);
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
}
