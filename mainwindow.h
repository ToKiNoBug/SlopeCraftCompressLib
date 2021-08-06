#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
//#include "heightline.h"
#include "OptiChain.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    ArrayXXi HighFullMap;
    ArrayXXi LowFullMap;
private slots:
    void on_GenerateAndShow_clicked();

    void on_doCompress_clicked();

private:
    Ui::MainWindow *ui;
};

void getBHL(ArrayXXi&Base,ArrayXXi&High,ArrayXXi&Low);
#endif // MAINWINDOW_H
