#ifndef COMPRESSWIND_H
#define COMPRESSWIND_H

#define useBHL

#include <QMainWindow>
#include <Eigen/Dense>
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

private:
    Ui::compressWind *ui;
};
#ifdef useBHL
void getBHL(ArrayXXi&Base,ArrayXXi&High,ArrayXXi&Low);
#endif

#endif // COMPRESSWIND_H
