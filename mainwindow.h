#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QFileDialog>
#include <QDebug>
#include <fstream>
#include <sstream>


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
    void on_goBackBtn_clicked();

    void on_goBackBtn_2_clicked();

    void on_goBackBtn_3_clicked();

    void on_goBackBtn_4_clicked();

    void on_rowsSpinBox_valueChanged(int row);

    void on_columnsSpinBox_valueChanged(int column);

    void on_generateBtn_clicked();

    void on_board_cellClicked(int row, int column);

    void on_board_cellDoubleClicked(int row, int column);

    void on_clearBoardBtn_clicked();

    void on_openFileBtn_clicked();

    void placeTree(QTableWidget *board, int row, int column);

    void placeTent(QTableWidget *board, int row, int col);

    void initTable(QTableWidget *board, int row, int col);

    void showRestrictionBoxes(QTableWidget *board, int row, int col, bool axis, bool manual, int value);

    bool isValid();

    void positionButtons();

    void on_clearBoardBtn3_clicked();

    void on_clearBoardBtn2_clicked();

    void clearBoard(QTableWidget *board);

private:
    Ui::MainWindow *ui;
    unsigned int rows;
    unsigned int cols;
    bool valid;
};
#endif // MAINWINDOW_H
