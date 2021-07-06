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
    Raw=HeightLine(32,'R');
    QPixmap pxm=QPixmap::fromImage(Raw.toQImage());
    //pxm.scaledToWidth(ui->ShowHeightLine->width(),Qt::SmoothTransformation);
    ui->ShowHeightLine->setPixmap(pxm);
    list<short> a;
    string b;
    Raw.toBrackets(a,b);
    cout<<b<<endl;
    /*for(unsigned int i=0;i<a.size();i++)
        cout<<setw(4)<<b[i];

    cout<<endl;
    for(unsigned int i=0;i<a.size();i++)
        cout<<setw(4)<<a[i];
    cout<<endl;*/

    //ui->ShowHeightLine->set
}


void MainWindow::on_doCompress_clicked()
{
    OptiTree Tree;
    Result=HeightLine(Raw.Height);
    cout<<Result.Height.rows()<<','<<Result.Height.cols()<<endl;
    Tree.NaturalOpti(Result.Height);
    //Tree.disp();
    ui->ShowResult->setPixmap(QPixmap::fromImage(Result.toQImage()));
    qDebug()<<"压缩率："<<100.0-100.0*Result.Height.maxCoeff()/Raw.Height.maxCoeff()<<"%";
}

