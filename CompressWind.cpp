#include "CompressWind.h"
#include "ui_CompressWind.h"

compressWind::compressWind(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::compressWind)
{
    ui->setupUi(this);
    getBHL(OptiChain::Base,sampleHighMap,sampleLowMap);
    qDebug("读入样本数据成功");

    //截取前32/33列
    sampleHighMap=sampleHighMap.block(0,0,33,256);
    sampleLowMap=sampleLowMap.block(0,0,33,256);
    OptiChain::Base=OptiChain::Base.block(0,0,32,256);

    qDebug()<<"size(Base)=["<<OptiChain::Base.rows()<<','<<OptiChain::Base.cols()<<']';
    qDebug()<<"size(HighMap)=["<<sampleHighMap.rows()<<','<<sampleHighMap.cols()<<']';
    qDebug()<<"size(LowMap)=["<<sampleLowMap.rows()<<','<<sampleLowMap.cols()<<']';

    ui->ColIndex->setMaximum(OptiChain::Base.cols()-1);
#ifdef showImg
    //OptiChain::SinkIDP=ui->ShowSinkIDP;
#endif
}

compressWind::~compressWind()
{
    delete ui;
}


void compressWind::on_LoadColumn_clicked()
{
    int colindex=ui->ColIndex->value();
    Compressor=OptiChain(sampleHighMap.col(colindex),sampleLowMap.col(colindex),colindex);
    ui->ShowRaw->setPixmap(QPixmap::fromImage(Compressor.toQImage(3)));
}


void compressWind::on_Compress_clicked()
{
    OptiChain::SinkAll=ui->ShowSinkIDP;
    OptiChain::AllowSinkHang=false;
    Compressor.divideAndCompress();


    OptiChain::SinkAll=ui->ShowSinkSuspended;
    OptiChain::AllowSinkHang=true;
    Compressor=OptiChain(sampleHighMap.col(Compressor.Col),sampleLowMap.col(Compressor.Col),Compressor.Col);
    Compressor.divideAndCompress();
}

