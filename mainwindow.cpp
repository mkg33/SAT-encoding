#include "mainwindow.h"
#include "ui_mainwindow.h"

int row; //these are the variables for the 'Create Board Manually' page
int col;
bool generateClicked = false;
bool displayTent = true;
// M: I'll add proper documentation later..

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)

{
    ui->setupUi(this);

    setFixedSize(660,700);

    ui->board->hide();

    ui->board2->hide();

    ui->board3->hide();

    ui->stackedWidget->setCurrentIndex(0);

    connect(ui->loadPuzzleBtn, &QPushButton::clicked, this, [=]()
    {
        ui->stackedWidget->setCurrentIndex(2); //move to the specified page of the stacked widget

        setFixedSize(660,700);

        positionButtons();
    });

    connect(ui->createManuallyBtn, &QPushButton::clicked, this, [=]()
    {
        ui->stackedWidget->setCurrentIndex(3);

        setFixedSize(660,700);

        positionButtons();
    });

    connect(ui->randomBoardBtn, &QPushButton::clicked, this, [=]()
    {
        ui->stackedWidget->setCurrentIndex(1);

        setFixedSize(660,700);
    });

    connect(ui->documentationBtn, &QPushButton::clicked, this, [=]()
    {
        ui->stackedWidget->setCurrentIndex(4);

        setFixedSize(660,700);
    });
}


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_goBackBtn_clicked()
{
    ui->stackedWidget->setCurrentIndex(0); //go back to menu

    setFixedSize(660,700);
}

void MainWindow::on_goBackBtn_2_clicked()
{
    ui->board->hide();

    ui->nightImage->show(); //reset the tent image

    on_clearBoardBtn_clicked(); //clear the board

    ui->rowsSpinBox->setValue(1); //reset the values of spin boxes

    ui->columnsSpinBox->setValue(1);

    ui->stackedWidget->setCurrentIndex(0);

    setFixedSize(660,700);
}

void MainWindow::on_goBackBtn_3_clicked()
{
    ui->board2->clear();

    ui->nightImage2->show(); //reset the tent image

    ui->stackedWidget->setCurrentIndex(0);

    setFixedSize(660,700);
}

void MainWindow::on_goBackBtn_4_clicked()
{
    ui->board3->hide();

    ui->nightImage3->show(); //reset the tent image

    ui->stackedWidget->setCurrentIndex(0);

    setFixedSize(660,700);
}


void MainWindow::on_rowsSpinBox_valueChanged(int arg1)
{

    row = arg1;
}

void MainWindow::on_columnsSpinBox_valueChanged(int arg2)
{

    col = arg2;

}

void MainWindow::on_generateBtn_clicked()
{
    ui->nightImage->hide();

    ui->board->show();

    if (!generateClicked)
    {
        initTable(ui->board, row, col);

        for (int i=0; i<row; i++)
        {
            for (int j=col; j<col+1; j++)
            {

                showRestrictionBoxes(ui->board, i, j, true, true, 0);

            }
        }

        for (int j=0; j<col; j++)
        {
            for (int i=row; i<row+1; i++)
            {
                showRestrictionBoxes(ui->board, i, j, false, true, 0);
            }
        }

        generateClicked = true;

        displayTent = true;
    }

    else
    {
        displayTent = false;

        on_clearBoardBtn_clicked();

        on_generateBtn_clicked();
    }

}


void MainWindow::on_board_cellClicked(int row, int column)
{
    if (row != ui->board->rowCount()-1 && column != ui->board->columnCount()-1)
    {
        placeTree(ui->board, row, column);
    }
}


void MainWindow::on_board_cellDoubleClicked(int row, int column)
{

    ui->board->setItem(row, column, new QTableWidgetItem);

    ui->board->item(row, column)->setText("");

}


void MainWindow::on_clearBoardBtn_clicked()
{

  clearBoard(ui->board);

  generateClicked = false;

  if (displayTent)
  {
      ui->nightImage->show();

      ui->rowsSpinBox->setValue(1);

      ui->columnsSpinBox->setValue(1);
  }
}

void MainWindow::on_clearBoardBtn2_clicked()
{
    clearBoard(ui->board2);

    ui->nightImage2->show();
}


void MainWindow::on_clearBoardBtn3_clicked()
{
    clearBoard(ui->board3);

    ui->nightImage3->show();
}


void MainWindow::on_openFileBtn_clicked()
{

    ui->nightImage2->show();

    clearBoard(ui->board2);

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open file"), QDir::currentPath(), tr("Text files(*.txt)"));

    std::string fileName2 = fileName.toUtf8().constData();

    std::ifstream file(fileName2);

    /* Helper for skipping whitespace. */
    #define SKIPWS(stream) do {                                     \
        const std::string whitespace(" \t\r\n");                    \
        while (std::string::npos != whitespace.find(stream.peek())) \
            stream.ignore();                                        \
    } while (false)

    unsigned int restriction;

    std::vector<unsigned int> rowRestrictions;

    std::vector<unsigned int> colRestrictions;

    std::stringstream buf;

    if (file)
    {
        ui->nightImage2->hide();

        buf << file.rdbuf();

        file.close();

        /* Read row count. */
        if (!(buf >> this->rows)) {
            this->valid = false;
            return;
        }

        /* Read column count. */
        if (!(buf >> this->cols)) {
            this->valid = false;
            return;
        }

        initTable(ui->board2, this->rows, this->cols);

        ui->board2->show();

        SKIPWS(buf);

        /* Read row x column board. */
        for (unsigned int i = 0; i < this->rows; i++) {
            /* Read row. */
            for (unsigned int j = 0; j < this->cols; j++) {

                if (buf.get() == 'T')
                {
                   placeTree(ui->board2, i, j);
                 }
            }

            /* Read row restriction. */
            if (!(buf >> restriction)) {
                this->valid = false;
                return;
            }

            showRestrictionBoxes(ui->board2, i, this->cols, false, false, restriction);

            rowRestrictions.push_back(restriction);

            SKIPWS(buf);
        }

        /* Read column restrictions. */
        for (unsigned int i = 0; i < this->cols; i++) {
            if (!(buf >> restriction)) {
                this->valid = false;
                return;
            }

            showRestrictionBoxes(ui->board2, this->rows, i, false, false, restriction);

            colRestrictions.push_back(restriction);
    }

  }
}

bool MainWindow::isValid()
{
    return this->valid;
}

void MainWindow::placeTree(QTableWidget *board, int row, int col)
{
    QTableWidgetItem *tree = new QTableWidgetItem;

     QIcon treeIcon(":/new/src/tree.png");

     tree->setIcon(treeIcon);

     board->setIconSize(QSize(45,45));

     board->setItem(row, col, tree);
}

void MainWindow::placeTent(QTableWidget *board, int row, int col)
{
    QTableWidgetItem *tent = new QTableWidgetItem;

    QIcon tentIcon(":/new/src/tent.png");

    tent->setIcon(tentIcon);

    board->setIconSize(QSize(45,45));

    board->setItem(row, col, tent);
}

void MainWindow::initTable(QTableWidget *board, int rows, int cols) //if manual == true, then the spin boxes are editable, else they are disabled and view-only
{
    board->setRowCount(rows+1);

    board->setColumnCount(cols+1);

    board->horizontalHeader()->setDefaultSectionSize(50);

    board->verticalHeader()->setDefaultSectionSize(50);

    board->setStyleSheet("QTableView {selection-background-color: black;}");

    board->setEditTriggers(QAbstractItemView::NoEditTriggers);

    board->setSelectionMode(QAbstractItemView::NoSelection);

    int width = 24+(board->columnCount())*50;

    int height = 23+(board->rowCount())*50;

    int x, y;

    if (width < ui->centralwidget->width() - 100)
    {
        x = (ui->centralwidget->width())/2 - width/2;

    }
    else {

        setFixedWidth(width+150);

        x = (ui->centralwidget->width())/2 - width/2;

    }

    if (height < ui->centralwidget->height() - 300)
    {
        y = (ui->centralwidget->height())/2 - height/2 + 150;
    }

    else
    {
        setFixedHeight(height+300);

        y = (ui->centralwidget->height())/2 - height/2 + 175;

    }


    board->setGeometry(x, y, width, height);

    positionButtons();

    board->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    board->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

}

void MainWindow::showRestrictionBoxes(QTableWidget *board, int row, int col, bool axis, bool manual, int value) // axis == true means restrictionBoxes for columns; if false, set restrictionBoxes in rows, manual means Create Board mode, false for load file
{
    if (manual)
    {
        QSpinBox *restrictionBox = new QSpinBox();

        restrictionBox->setMinimum(0);

        if (axis)
        {
            restrictionBox->setMaximum(board->columnCount()-1);
        }
        else
        {
            restrictionBox->setMaximum(board->rowCount()-1);
        }

        board->setItem(row, col, new QTableWidgetItem);

        board->setCellWidget(row, col, restrictionBox);
    }
    else
    {
        QSpinBox *restrictionBox = new QSpinBox();

        restrictionBox -> setValue(value);

        restrictionBox -> setEnabled(!isEnabled());

        board->setItem(row, col, new QTableWidgetItem);

        board->setCellWidget(row, col, restrictionBox);

    }

}

void MainWindow::positionButtons()
{
    ui->clearBoardBtn->setGeometry(ui->centralwidget->width()/2 - 140/2, 100, 140, 32);

    ui->rowsText->setGeometry(ui->centralwidget->width()/2 - 120, 140, 124, 21);

    ui->rowsSpinBox->setGeometry(ui->centralwidget->width()/2 + 40, 140, 71, 21);

    ui->columnsText->setGeometry(ui->centralwidget->width()/2 - 120, 170, 124, 21);

    ui->columnsSpinBox->setGeometry(ui->centralwidget->width()/2 + 40, 170, 71, 21);

    ui->generateBtn->setGeometry(ui->columnsText->x()-8, 200, 140, 32);

    ui->solveBtn->setGeometry(ui->rowsSpinBox->x()-5, 200, 100, 32);

    ui->openFileBtn->setGeometry(ui->centralwidget->width()/2 - 160/2, 100, 160, 32);

    ui->clearBoardBtn2->setGeometry(ui->centralwidget->width()/2 - 160/2, 130, 160, 32);

    ui->generateRandomBtn->setGeometry(ui->centralwidget->width()/2 - 160/2, 100, 160, 32);

    ui->solveBtn2->setGeometry(ui->centralwidget->width()/2 - 160/2, 160, 160, 32);

    ui->solveBtn3->setGeometry(ui->centralwidget->width()/2 - 160/2, 160, 160, 32);

    ui->clearBoardBtn3->setGeometry(ui->centralwidget->width()/2 - 160/2, 130, 160, 32);

}

void MainWindow::clearBoard(QTableWidget *board){

    board->clear();

    board->hide();

    setFixedSize(660,700);

    positionButtons();
}

