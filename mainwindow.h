#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_wireframeCheckBox_clicked(bool checked);

    void on_anchorPointsCheckBox_clicked(bool checked);

    void on_radioButton_clicked();
    void on_radioButton_2_clicked();
    void on_radioButton_3_clicked();

    void on_actionOpen_triggered();

private:
    Ui::MainWindow *ui;

};

#endif // MAINWINDOW_H

