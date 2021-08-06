#include "CompressWind.h"
#include "ui_CompressWind.h"

compressWind::compressWind(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::compressWind)
{
    ui->setupUi(this);
}

compressWind::~compressWind()
{
    delete ui;
}

