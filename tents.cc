#include <algorithm>
#include <string>
#include <sstream>
#include <tuple>
#include <utility>
#include <vector>

#include "tents.h"

using uint = unsigned int;

/*
 * isTentPlacable(row, col):
 * Checks if a tent can be placed on the cell denoted by row ${row} and column
 * ${col}. Returns true if it is possible, false otherwise.
 */
bool
Tents::isTentPlacable(uint row, uint col)
{
    /* Bounds check. */
    if (row >= this->rows || col >= this->cols)
        return false;

    /* Check restrictions. */
    if (this->rowRestrictions[row] > 0
            && this->colRestrictions[col] > 0
            && this->board[row][col] == CellState::EMPTY)
        return true;

    return false;
}

/*
 * getPossibleTents(row, col):
 * Returns the coordinates of possible tents that are orthogonally adjacent to
 * the tree denoted by row ${row} and column ${column}.
 */
std::vector<std::pair<uint, uint> >
Tents::getPossibleTents(uint row, uint col)
{
    /* Coordinates of possible tents. */
    std::vector<std::pair<uint, uint> > tents;

    /* Bounds check. */
    if (row >= this->rows || col >= this->cols)
        return tents;

    /* Check north neighbour. */
    if (isTentPlacable(row - 1, col))
        tents.push_back(std::make_pair(row - 1, col));

    /* Check east neighbour. */
    if (isTentPlacable(row, col + 1))
        tents.push_back(std::make_pair(row, col + 1));

    /* Check south neighbour. */
    if (isTentPlacable(row + 1, col))
        tents.push_back(std::make_pair(row + 1, col));

    /* Check west neighbour. */
    if (isTentPlacable(row, col - 1))
        tents.push_back(std::make_pair(row, col - 1));

    return tents;
}

/*
 * buildTreeClauses(numbers):
 * Builds the clauses for a tree. The variable numbers used in the clauses are
 * taken from the ${numbers} list.
 */
void
Tents::buildTreeClauses(const std::vector<int>& numbers)
{
    std::vector<int> clause;

    /* Build a clause of the form (a v ... v d). */
    for (auto num : numbers)
        clause.push_back(num);
    this->clauses.push_back(clause);

    /* If the clause is of size 2, build a clause of the form (-a v -d). */
    if (numbers.size() == 2) {
        clause.clear();
        for (auto num : numbers)
            clause.push_back(-num);
        this->clauses.push_back(clause);
    }

    /* If the clause is of size > 2, build clauses of the form (-a v ... v -d). */
    if (numbers.size() > 2) {
        for (auto num1 : numbers) {
            clause.clear();
            for (auto num2 : numbers) {
                if (num1 != num2)
                    clause.push_back(-num2);
            }
            this->clauses.push_back(clause);
        }
    }
}

/*
 * areSouthEastAdjacent(v1, v2):
 * Checks if the tent denoted by variable ${v1} is adjacent to the tent denoted
 * by variable ${v2} in any of the following directions: east, south east,
 * south. Returns true if they are adjacent, false otherwise.
 */
bool
Tents::areSouthEastAdjacent(const std::tuple<uint, uint, uint, uint, uint>& v1,
        const std::tuple<uint, uint, uint, uint, uint>& v2)
{
    /* Check east. */
    if (std::get<0>(v1) == std::get<0>(v2)
            && std::get<1>(v1) + 1 == std::get<1>(v2))
        return true;

    /* Check south east. */
    if (std::get<0>(v1) + 1 == std::get<0>(v2)
            && std::get<1>(v1) + 1 == std::get<1>(v2))
        return true;

    /* Check south. */
    if (std::get<0>(v1) + 1 == std::get<0>(v2)
            && std::get<1>(v1) == std::get<1>(v2))
        return true;

    return false;
}

/*
 * buildClauses():
 * Builds the clauses for the SAT solver.
 */
void
Tents::buildClauses()
{
    /* Increasing variable number. */
    int varNum = 1;

    for (uint row = 0; row < this->rows; row++) {
        for (uint col = 0; col < this->cols; col++) {
            if (this->board[row][col] == CellState::TREE) {

                /* Coordinates of possible tents adjacent to this tree. */
                auto tents = getPossibleTents(row, col);

                /* Variable numbers of possible tents adjacent to this tree. */
                std::vector<int> numbers;

                /* Go through possible tents. */
                for (auto& var : tents) {

                    /* Create the variable. */
                    auto variable = std::make_tuple(var.first, var.second, row, col, varNum);

                    /* Check if the variable is already in the variables list. */
                    auto it = std::find_if(this->variables.begin(), this->variables.end(), compareVar(variable));

                    /*
                     * If the variable is already in the variables list,
                     * add the variable number of the existing variable to
                     * the variable numbers list. Skip the rest.
                     */
                    if (it) {
                        numbers.push_back(std::get<4>(*it));
                        continue;
                    }

                    /* The variable is not in the variables list, so add it. */
                    this->variables.push_back(variable);
                    this->rowVariables[row].push_back(variable);
                    this->colVariables[col].push_back(variable);

                    /* Add the new variable number to the list. */
                    numbers.push_back(varNum++);
                }

                /* Build the clauses for this tree. */
                buildTreeClauses(numbers);
            }
        }
    }

    /*
     * Go through the variables and build adjacent restriction clauses of the
     * form (-a v -b). We only consider the directions east, south east and south.
     * Considering any other directions would result in redundant clauses.
     */
    for (auto v1 : this->variables) {
        for (auto v2 : this->variables) {
            if (areSouthEastAdjacent(v1, v2)) {
                this->clauses.push_back({-std::get<4>(v1), -std::get<4>(v2)});
            }
        }
    }

    /* TODO: Build row and column restriction clauses. */
}

/*
 * Tents(rows, cols):
 * Initialize the board of size ${cols} x ${rows}. At the beginning every cell
 * is empty.
 */
Tents::Tents(uint rows, uint cols):
    rows(rows),
    cols(cols),
    board(rows, std::vector<CellState>(cols, CellState::EMPTY)),
    rowVariables(rows, std::vector<int>()),
    colVariables(cols, std::vector<int>()) {};

/*
 * Tents(str):
 * Initializes the board with initialization string ${str}.
 * If unsuccessful, method ${isValid} will return false.
 */
Tents::Tents(const std::string &str) {

    uint restriction;
    std::stringstream buf(str);

    std::vector<uint> rowRestrictions;
    std::vector<uint> colRestrictions;

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

    /* Initialize the board. At the beginning every cell is empty. */
    board.assign(this->rows, std::vector<CellState>(this->cols, CellState::EMPTY));

    SKIPWS(buf);

    /* Read row x column board. */
    for (uint i = 0; i < this->rows; i++) {
        /* Read row. */
        for (uint j = 0; j < this->cols; j++) {
            if (buf.get() == 'T')
                placeTree(i, j);
        }

        /* Read row restriction. */
        if (!(buf >> restriction)) {
            this->valid = false;
            return;
        }
        rowRestrictions.push_back(restriction);

        SKIPWS(buf);
    }

    /* Read column restrictions. */
    for (uint i = 0; i < this->cols; i++) {
        if (!(buf >> restriction)) {
            this->valid = false;
            return;
        }
        colRestrictions.push_back(restriction);
    }

    /* Set restrictions. */
    if (!setRowRestrictions(rowRestrictions)) {
        this->valid = false;
        return;
    }
    if (!setColRestrictions(colRestrictions)) {
        this->valid = false;
        return;
    }
}

/*
 * isValid():
 * Checks if all required game parameters have been set successfully.
 */
bool
Tents::isValid()
{
    return this->valid;
}

/*
 * setRowRestriction(rowRestrictions):
 * Initializes row restrictions by copying from ${rowRestrictions}.
 * Returns true if successful, false otherwise.
 */
bool
Tents::setRowRestrictions(const std::vector<uint> &rowRestrictions)
{
    if (rowRestrictions.size() != this->rows) {
        this->valid = false;
        return false;
    }

    /* Clear old row restrictions, if any. */
    this->rowRestrictions.clear();

    for (auto restriction : rowRestrictions)
        this->rowRestrictions.push_back(std::make_pair(restriction, restriction));

    return true;
}

/*
 * setColRestriction(rowRestrictions):
 * Initializes column restrictions by copying from ${colRestrictions}.
 * Returns true if successful, false otherwise.
 */
bool
Tents::setColRestrictions(const std::vector<uint> &colRestrictions)
{
    if (colRestrictions.size() != this->cols) {
        this->valid = false;
        return false;
    }

    /* Clear old column restrictions, if any. */
    this->colRestrictions.clear();

    for (auto restriction : colRestrictions)
        this->colRestrictions.push_back(std::make_pair(restriction, restriction));

    return true;
}

/*
 * placeTree(row, col):
 * If the cell denoted by row ${row} and column ${col} is empty,
 * place a tree on that cell. Returns true if successful,
 * false otherwise.
 */
bool
Tents::placeTree(uint row, uint col)
{
    /* Bounds check. */
    if (row >= this->rows || col >= this->cols)
        return false;

    /* Check if cell is empty. */
    if (this->board[row][col] != CellState::EMPTY)
        return false;

    /* Plant tree. */
    this->board[row][col] = CellState::TREE;

    return true;
}

/*
 * removeTree(row, col):
 * Remove tree on cell denoted by row ${row} and column ${col}.
 * Returns true if successful, false otherwise.
 */
bool
Tents::removeTree(uint row, uint col)
{
    /* Bounds check. */
    if (row >= this->rows || col >= this->cols)
        return false;

    /* Check if there is a tree. */
    if (this->board[row][col] != CellState::TREE)
        return false;

    /* Uproot tree. */
    this->board[row][col] = CellState::EMPTY;

    return true;
}
