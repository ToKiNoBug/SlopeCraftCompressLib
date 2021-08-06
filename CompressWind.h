#ifndef COMPRESSWIND_H
#define COMPRESSWIND_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class compressWind; }
QT_END_NAMESPACE

class compressWind : public QMainWindow
{
    Q_OBJECT

public:
    compressWind(QWidget *parent = nullptr);
    ~compressWind();

private:
    Ui::compressWind *ui;
};
#endif // COMPRESSWIND_H
