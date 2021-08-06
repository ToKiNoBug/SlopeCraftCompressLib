#include "mainwindow.h"
#include "ui_mainwindow.h"

//#include "optitree.h"
#include <iomanip>
#include "OptiChain.h"

using namespace std;
using namespace Eigen;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    HeightLine::Base=MatrixXi::Random(Rows,Rows);
    HeightLine::Base=HeightLine::Base-Freq*(HeightLine::Base/Freq);
    //HeightLine::Base=(HeightLine::Base.array()<Freq-1).select(HeightLine::Base,12);
    HeightLine::currentColum=2;

}

MainWindow::~MainWindow()
{
    delete ui;
}
