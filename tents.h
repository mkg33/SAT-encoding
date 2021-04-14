#ifndef __TENTS_H__
#define __TENTS_H__

#include <string>
#include <tuple>
#include <utility>
#include <vector>

enum class CellState {
    EMPTY,
    TREE,
    TENT
};

using uint = unsinged int;

/*
 * Custom comparator to find already existing variables, but only looking
 * at the first two parameters of the tuple.
 */
struct compareVar
{
    /* Local copy of the variable we'll be looking for. */
    std::tuple<uint, uint, uint, uint, int> var;

    /* Constructor to set the variable we will compare other variables to. */
    compareVar(const std::tuple<uint, uint, uint, uint, int>& var): var(var) {}

    /* Check if the variables have the same position on the board. */
    bool operator()(const std::tuple<uint, uint, uint, uint, int>& var)
    {
        return (std::get<0>(var) == std::get<0>(this->var)
                && std::get<1>(var) == std::get<1>(this->var));
    }

}

class Tents {

private:

    uint rows;
    uint cols;

    bool valid;

    /* The game board is a matrix of size rows x cols. */
    std::vector<std::vector<CellState> > board;

    /* Row and column restrictions. */
    std::vector<uint> rowRestrictions;
    std::vector<uint> colRestrictions;

    /*
     * Variables for the SAT solver. The first two parameters denote the row
     * and column position of the variable, the next two parameters denote the
     * row and column position of the adjacent tree and the last parameter is
     * the variable number that is used in the SAT solver.
     */
    std::vector<std::tuple<uint, uint, uint, uint, int> > variables;

    /* The variables for every row and column. */
    std::vector<std::vector<int> > rowVariables;
    std::vector<std::vector<int> > colVariables;

    /* The clauses for the SAT solver. The inner vector is a single clause. */
    std::vector<std::vector<int> > clauses;

    /*
     * isTentPlacable(row, col):
     * Checks if a tent can be placed on the cell denoted by row ${row} and column
     * ${col}. Returns true if it is possible, false otherwise.
     */
    bool isTentPlacable(uint, uint);

    /*
     * getPossibleTents(row, col):
     * Returns the coordinates of possible tents that are orthogonally adjacent to
     * the tree denoted by row ${row} and column ${column}.
     */
    std::vector<std::pair<uint, uint> > getPossibleTents(uint, uint);

    /*
     * buildTreeClauses(numbers):
     * Builds the clauses for a tree. The variable numbers used in the clauses are
     * taken from the ${numbers} list.
     */
    void buildTreeClauses(const std::vector<int>&);

    /*
     * areSouthEastAdjacent(v1, v2):
     * Checks if the tree denoted by variable ${v1} is adjacent to the tree denoted
     * by variable ${v2} in any of the following directions: east, south, south
     * east. Returns true if they are adjacent, false otherwise.
     */
    bool areSouthEastAdjacent(const std::tuple<uint, uint, uint, uint, uint>&,
        const std::tuple<uint, uint, uint, uint, uint>&);
    
    /*
     * buildClauses():
     * Builds the clauses for the SAT solver.
     */
    void buildClauses();

public:

    /*
     * Tents(rows, cols):
     * Initialize the board of size ${rows} x ${cols}. At the beginning
     * every cell is empty.
     */
    Tents(uint, uint);

    /*
     * Tents(str):
     * Initialize the board with initialization string ${str}.
     * If unsuccessful, method ${isValid} will return false.
     */
    Tents(const std::string &str);

    /*
     * isValid():
     * Check if all required game parameters have been set successfully.
     */
    bool isValid();

    /*
     * setRowRestriction(rowRestrictions):
     * Initialize row restrictions. Returns true if successfull,
     * false otherwise.
     */
    bool setRowRestrictions(const std::vector<uint>&);

    /*
     * setColRestriction(rowRestrictions):
     * Initialize column restrictions. Returns true if successfull,
     * false otherwise.
     */
    bool setColRestrictions(const std::vector<uint>&);

    /*
     * placeTree(row, col):
     * If the cell denoted by row ${row} and column ${col} is empty,
     * place a tree on that cell. Returns true if successfull,
     * false otherwise.
     */
    bool placeTree(uint, uint);

    /*
     * removeTree(row, col):
     * Remove tree on cell denoted by row ${row} and column ${col}.
     * Returns true if successfull, false otherwise.
     */
    bool removeTree(uint, uint);

};

#endif
