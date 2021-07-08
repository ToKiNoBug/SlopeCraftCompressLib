#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
//#include "heightline.h"
#include "optitree.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    HeightLine Raw;
    HeightLine Result;
private slots:
    void on_GenerateAndShow_clicked();

    void on_doCompress_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
