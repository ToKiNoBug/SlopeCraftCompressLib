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


}

MainWindow::~MainWindow()
{
    delete ui;
}
