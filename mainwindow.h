#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <vector>
#include <QMessageBox>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_toCompressTE_textChanged();

    void on_genNewKeyPB_clicked();

    void on_xorEncyptPB_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
