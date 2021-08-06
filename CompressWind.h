#ifndef COMPRESSWIND_H
#define COMPRESSWIND_H

#define useBHL

#include <QMainWindow>
#include <Eigen/Dense>
#include <iostream>
#include <fstream>
#include "OptiChain.h"

using namespace std;
using namespace Eigen;
QT_BEGIN_NAMESPACE
namespace Ui { class compressWind; }
QT_END_NAMESPACE

class compressWind : public QMainWindow
{
    Q_OBJECT

public:
    compressWind(QWidget *parent = nullptr);
    ~compressWind();
    ArrayXXi sampleHighMap;
    ArrayXXi sampleLowMap;
    OptiChain Compressor;
private slots:
    void on_LoadColumn_clicked();

    void on_Compress_clicked();

private:
    Ui::compressWind *ui;
};

void getBHL(ArrayXXi&Base,ArrayXXi&High,ArrayXXi&Low);


#endif // COMPRESSWIND_H
