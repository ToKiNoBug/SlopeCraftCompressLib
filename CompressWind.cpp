#include "CompressWind.h"
#include "ui_CompressWind.h"

compressWind::compressWind(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::compressWind)
{
    ui->setupUi(this);
    getBHL(OptiChain::Base,sampleHighMap,sampleLowMap);
    qDebug("读入样本数据成功");
    qDebug()<<"size(Base)=["<<OptiChain::Base.rows()<<','<<OptiChain::Base.cols()<<']';
    qDebug()<<"size(HighMap)=["<<sampleHighMap.rows()<<','<<sampleHighMap.cols()<<']';
    qDebug()<<"size(LowMap)=["<<sampleLowMap.rows()<<','<<sampleLowMap.cols()<<']';

}

compressWind::~compressWind()
{
    delete ui;
}

