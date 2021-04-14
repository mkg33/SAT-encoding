**Project 01**

Start: 11.11.2020

Deadline: 10.12.2020 2pm


**Update 21.11**

GUI updates: browse files and open file now fully functional. The board is read and displayed graphically. I've also improved automatic resizing and the re-positioning of buttons. Also added some clean-up functions, i.e., resetting of spin box values, clearing the board when the 'back to menu' button is pressed, etc.
In 'open files', when (on MacOS, at least) the user doesn't go to the tents txt file directly (using the file browser) and uses the Recents folder or the search function, there occurs an exception (something NS null string etc.) but it ultimately doesn't affect our programme at all. I've looked up this exception online and found out that it could be some MacOS-specific vague bug or just some sort of unexpected browsing behaviour. Anyway, seems irrelevant - no crashes.

**Update 20.11**

I've tried to link the tents.cc and tents.h files with the GUI and discovered that there are quite a few issues with the implementation of the algorithm, i.e., a problem with the data structures and access at index / adding elements. I think we need to convert the `variables` set to vector to get the `Go through the unique variables to build adjacent restriction clauses.` part to work. Obviously, set has no access at index functionality, so the two for loops are meaningless right now. Is it very bad (performance-wise) to convert the set to a vector?

The good news is that I got the 'browse files' function to work in the GUI.

**Update 18.11**

It turns out that Qt is available for free (licence proviso etc. but we will document it properly). You have to create an account but the name/surname can be fictitious. I entered Baby Yoda and installed it successfully :D. You have to give a real e-mail, though. Here's the link: https://www.qt.io/download-open-source?hsCtaTracking=9f6a2170-a938-42df-a8e2-a9f0b1d6cdce%7C6cb0de4f-9bb5-4778-ab02-bfb62735f3e5. There's a ridiculous bug (latest version of Qt running on macOSCatalina) - the drag and drop function doesn't work for the widgets/elements in the form editor. There's a surprising fix for it here: https://forum.qt.io/topic/119147/qt-designer-unable-to-drag-drop-widgets/14. Really weird but it works...

Started working on GUI, created the menu page and added images to the resources file. The images/layout are just a suggestion - I really don't care what colours/images we set, so feel free to change the visual layout if you wish. The good thing about the icons is that they're free and have a transparent background. I set up an attribution file because we have to acknowledge the use of those pictures. The existing project (the tents.pro file) can be opened directly in QtCreator. 

**Update 17.11 morning**:

I think you are right with the variables and cadical. So we need variables 1...n.
The algorithm then might be like this:

```

/* We save the variables like this: */
std::vector<std::tuple<uint, uint, uint, uint, uint> > variables;
/* Where the first two uints are variable row and col, then adjacent tree row and col and at last the variable number. */

/* Vars for each row. */
std::vector<std::vector<uint> > > rows (rowRestrictions.size(), {});

/* Vars for each column. */
std::vector<std::vector<uint> > cols (colRestrictions.size(), {});

/* Clauses */
std::vector<std::vector<uint> > clauses;

/* Variable number */
uint varNum = 1;

/* Go through the board. */
for (uint i = 0; i < rows; i++) {
    for (uint j = 0; j < cols; j++) {

        if (board[i][j] == tree) {

            /* 1: For each variable orthogonally adjacent to tree (i, j): */
                /* Check if variable is already in variables vector<tuple>. */

                /* If variable already in variables vector<tuple>: Skip. */
                /* If variable not in variables vector<tuple>: */

                    /* 1.1: Make tuple for variable. */
                    auto variable = std::make_tuple(row, col, i, j, varNum); /* where row, col are coordinates of the variable. */

                    /* 1.2: Add variable to variables. */
                    variables.push_back(variable);

                    /* 1.3: Add variable to the correct row and column in the rows and cols vector<tuple>s. */
                    rows[i].push_back(varNum);
                    cols[j].push_back(varNum);

                    /* 1.4: Increase varNum. */
                    varNum++;

            /* 2. Build clauses (a v ... v d) and (-a v ... v -c), ... for tree (i, j) and push every single one into clauses vector. */

        }

    }
}

/* Go through unique vars. */
for (v1 : variables) {
    for (v2 : variables) {
        /* If v1 is adjacent to v2 (in any of the 8 directions) and v1.tree != v2.tree: */
            /* Add clause (-v1 v -v2). */
    }
}

/* Now build row restrictions. */

/* Now build col restrictions. */

```

---

You are right with function g(). I think what I meant was:

`g(row, col): row * this->cols + col + 1;`

The inverse then is:

`g-1(var): row = (var - 1) / this->cols; col = (var - 1) % this->cols;`

Should be correct right?

---

In the algorithm below we are missing some negative clauses. After we build clauses `(a v ... v d)` from `posVars` we need to build the negativ clauses right away. Example:

```
01 02 03
04 TT 06
07 08 09
```

where `TT` is a tree. Using the algorithm below we build the following clauses:

- `(2 v 4 v 6 v 8)`
- `(-2 v -4)`
- `(-2 v -6)`
- `(-4 v -8)`
- `(-6 v -8)`

What is missing is:

- `(-4 v -6)`
- `(-2 v -8)`

Which means we could end up like this:

```
01 XX 03
04 TT 06
07 XX 09
```

where `XX` are trees. This is obviously incorrect.

Instead we should immediately build all the tree clauses:

- `(2 v 4 v 6 v 8)`
- `(-2 v -4 v -6)`
- `(-2 v -4 v -8)`
- `(-2 v -6 v -8)`
- `(-4 v -6 v -8)`

And then down below we build the adjacent restrictions. But now instead of looking at all unique variables, we only look at the variables that have a different orthogonally adjacent tree.
Doing it like that we avoid building the same negative clauses from directly above again.
In order to be able to do that, we need to save the coordinates of the orthogonally adjacent tree for each variable. We could do that by changing the set to:

`std::set<std::vector<uint> > variables` where the first two ints are the tree coordinates and the last two are the coordinates of the variable.
Another possibility is `std::set<std::tuple<uint, uint, uint, uint> > variables`. Which makes it more clear how many values we save.
Note that we need to overload the operator `=` for `std::tuple` in order for it to work the way we want it. Or we don't use a set and use a vector. I will have to research what the easiest solution might be.

And then, in the for loop below, where we go through the unique variables, we check if the tree is different but the variables are adjacent. If so, build the clause `(-var1 v -var2)`. If not, skip.


Update 16.11:

M: Hmm is the g() function (coordinates to variables) really injective? Say, we have two pairs (x,y) and (a,b). Then:
```
g(x,y) = x*y + y + 1 if it equals g(a,b) = a*b + b + 1, then:

x*y + y + 1 = a*b + b + 1, then

x*y + y = a*b + b, then

y(x+1) = b(a+1) but for y=0 and b=0, the values of x and a could be anything, that is:

for instance, the pairs: (4, 0) != (3, 0) but g(4,0) = g(3,0), 
hence g() isn't injective (and it should be, I guess?). Have I got it wrong?
```

Also, I think we should encode the pairs of coordinates by means of positive integers incremented by 1 (this is due to the fact that if we pass variables, say, 1, 2, 20, 34 to cadical and no other variables, then we would have all the possible variables from 1..34 because cadical outputs all the values, even if we use the -f flag). I reckon it'd be best to have a mapping g(i,j) -> n, where n is a positive integer starting from 1.


M: I think I get it now - your approach treats the board globally, so to speak, whereas mine is confined to the individual trees and treats all possible tents locally. Yes, I reckon that by treating the row/column restrictions globally, we will indeed end up with fewer clauses (but longer ones, which is alright). Also, I understand that we will only consider the 'potentially conflicting' neighbours and not just all the neighbours, so that's a major improvement. Let me know if I got it right and I'll start working on the concrete implementation

I think your approach is good but we might end up encoding more clauses than we need.
I wrote down the approach that I thought about yesterday. I believe this results in the
minimal amount of clauses, although I could be wrong.

```

    /* We could save the coordinates of the variables in a set (unique values) like this: */
    std::set<std::pair<uint, uint> > variables;

    /* We could save the clauses in an array of arrays like this: */
    std::vector<std::vector<int> > clauses;

    /* Here we save the possible vars for each row: */
    std::vector<std::vector<int> > rows (rowRestrictions.size(), {});

    /* Here we save the possible vars for each col: */
    std::vector<std::vector<int> > cols (colRestrictions.size(), {});

    /* Go through the board save any possible tents. */
    for (uint i = 0; i < rows; i++) {
        for (uint j = 0; j < cols; j++) {

            if (board[i][j] == tree) {

                /* Returns coordinates of possible tents that are orthogonally adjacent to tree (i, j). */
                posVars = f(i, j);

                /* For building a clause like (a v ... v d): */
                std::vector<int> clause;

                for (var : posVars)  {

                    /* Let a,...,d be the vars in posVars: Build clause (a v ... v d): */
                    clause.push_back(g(var));

                    /* Add variables to the unique set to later build adjacent restrictions. */
                    variables.insert(var);

                    /* To later build the row and col restrictions. */
                    /* Add var to its row restr vector: */
                    rows[var.first].push_back(g(var));

                    /* Add var to its col restr vector: */
                    cols[var.second].push_back(g(var));

                }

                clauses.push_back(clause);
            }

        }
    }

    /* Go through the unique variables to build adjacent restriction clauses. */
    for (var : variables) {

        /* If there is var2 in variables that is adjacent to var (in any of the 8 directions), build the clause (-var v -var2). */
        /* Example checking north direction: */
        auto var2 = variables.find(std::make_pair(var.first - 1, var.second));
        if (var2 != std::set::end) { /* Found: so build clause. */
            clauses.push_back(std::vector<int> > {-1 * g(var), -1 * g(var2)}); /* Note the * -1 here: It result in negated vars. */
        }

    }

    /* Now build row restrictions: */
    for (uint i = 0; i < rows.size(); i++) {
        /* Based on value of rowRestrictions[i] build clauses with variables from rows[i]. */
        /* Example. If vars 3, 5, 7 are in rows[i] and rowRestrictions[i] is 2 then: */
        /* Build clause: (-3 v -5 v -7). */
        /* Example. If vars 3, 5, 7 are in rows[i] and rowRestrictions[i] is 1 then: */
        /* Build clauses: (-3 v -5) ^ (-3 v -7) ^ (-5 v - 7). */
        /* If rows[i].size() == rowRestrictions[i] I think we don't need to build any clauses at all. */
    }

    /* Now build col restrictions: */
    for (uint i = 0; i < cols.size(); i++) {
        /* Based on value of colRestrictions[i] build clauses with variables from cols[i]. */
    }

    /* We should now have all relevant clauses, and I think the minimal amount of clauses as well. */

    /* Returns possible tents. */
    f(row, col) {
        std::vector<std::pair<uint, uint> > vars;
        /* Check neighbours in north, east, south, west directions. */
        /* This also includes checking row/col restriction != 0. */
        /* If some row/col restriction == 0: Skip neighbour accordingly. */
        /* For each direction: insert into variables if placing a tree is possible. */
        /* Ex for north: */
        vars.push_back(std::make_pair(row - 1, col));

        return vars
    }

    /* Returns variable number based on coordinates. */
    g(std::pair<uint, uint> &p) {
        return p.first * p.second + p.second + 1;
    }

```

Update 15.11:

*Evening update*

It took into consideration all possible (I hope) refinements that minimise the number of redundant clauses and variables.

## Minimal encoding scheme:

```
for(int i=0; i<rows; i++){
    for(int j=0; j<columns; j++){
        if(board[i][j] == tree){
            if(rowRestriction == true){ #we first check if there are any tent places left in the row
                #start with the left cell
                1. if the left (orthogonally adjacent) cell is not out of bounds and is empty, then
                2. if its colRestriction == true, then
                3. encode left's neighbours 
                /* essentially, this amounts to appending the -left -direction1 ... -left -direction7 clauses if the neighbouring cells
                  aren't out of bounds, of course */

                #move to the right cell
                1. if the right (orthogonally adjacent) cell is not out of bounds and is empty, then
                2. if its colRestriction == true, then
                3. encode right's neighbours
            }

            if(colRestriction == true) { #we now check if it's worth examining cells 'up/down'

                #start with the cell above
                1. if the (orthogonally adjacent) cell above is not out of bounds and is empty, then
                2. if its rowRestriction == true, then
                3. encode up's neighbours

                #move to the cell below
                1. if the (orthogonally adjacent) cell below is not out of bounds and is empty, then
                2. if its rowRestriction == true, then
                3. encode down's neighbours

            }
        }
    }
}
```

Note: as regards the encoding of the potential tent cells' neighbours - I think we need to create a search data structure that will store the coordinates of encoded cells and their corresponding variables. This is because some cells will reappear in subsequent iterations and they can't be encoded as two/more different variables.

## Decoding scheme:

Here are my thoughts on the decoding scheme - comments very welcome!

The decoding scheme appears to be rather easy because:
- we know that there is always AT LEAST one cell that will be passed as a candidate for the tent, that is, even if it turns out that 3 adjacent cells are not suitable (col/row restrictions or other trees), then we will pass the DIMACS encoding simply as '1' etc.
- to put it simply: the 'tent cells candidates' are always encoded as non-negated parts of the clause (for instance, 1 2 3 4 or 1 3 or 2 4 or 1, etc.) - we know it is always the first clause resulting from checking a new tree
- obviously, we need to 'remember' the coordinates of the passed cells so that we can use the SAT result and place the tents at the end of the game (just to display the result)
- could we have an array of pairs (don't know what the correct data structure is called)? This way we could have something like {(1, board[4][5]), ... , (41, board[14][3])} and we could then use it at the end to update and display the board with the tents.
- this is important because the size of our output clauses/variables will be, well, variable (no pun intended :-)) in each iteration

*Afternoon update*

M: I thought about the tools for the GUI part. How about using Qt? There's a free trial for 10 days and that's probably more time we will need for that part? I've read it's very intuitive and easy to use plus it has great reviews. Also, I suggest that we call our programme '6 ECTS, my ass!' :-).

P: Yes I think Qt was the framework that was suggested in stackoverflow. I didn't even realize that you need to purchase a licence, kind of strange... But I think we should be able to do it in 10 days. Our tutor was also saying that we can import cadical as a library directly into our code. I think we should do this, so we don't have to deal with file paths and stuff like that. We'll have to check how to do that as well. :)

Regarding the suggestion about splitting the `checkNeighbourTents` function. I don't think that we should do that for the following reasons:

- As an array check (such as: is there a tree nearby?) is in O(1) it doesn't really matter if we check more cells than necessary. What really matters for speed is the amount of variables and clauses. **That's right, we should only have one function. The variables are assigned independently anyway.**
- We need to check diagonally as well. In the game it is not allowed to have two diagonally adjacent tents. **I'm aware of that - my argument was that we might   want to avoid checking for neighbouring tents by two cells. Maybe that was not clear.**
- We end up with multiple functions that do almost the same thing. This will make the code more complicated. **True!**
- Using multiple functions, we need to check which function to use for every cell. This takes almost as much time as we saved. **Also true :D**
- The function is so far only used in `placeTent()`. That function is only called when a player places a tent manually and in that case, speed is not needed. **Hmm..perhaps I misunderstood the problem set but are we also supposed to implement the actual 'manual playing of the game' by the player? I thought we only needed to supply the GUI and allow for adjusting the initial board configuration by the player (only placing the trees and setting row/col restrictions)? The puzzle is then to be solved automatically by the SAT solver?** Ah yes you're right. I thought the last bonus part included adding a play manually functionality. In that case I think we don't even need the functions `placeTent()`, `checkNeighbourTents()` and `checkNeighbourTrees`. Although we should wait with deleting them.

---

I also thought about the encoding strategy, I think it is similar to yours. Let me know what you think.
**I think it's basically the same as my approach but it takes care of the 'refinements will follow'-part of my proposal :-). Sure, we won't need all those unnecessary variables if the tree is somewhere at/near the boundary or near the edges, etc. And, of course, if the row/col restrictions aren't met, then it doesn't make sense to check for all the neighbours, etc. So I actually think this approach integrates my thinking and the optimisation for boundary cases. That's what I intended to do for boundary cases anyway. Great, thanks! I'll soon add all the logical boundary scenarios and will devise a uniform strategy based on these ideas. I think the case I described below was the 'full restrictions scenario'. Incidentally, that's also the maximum number of clauses and variables we would ever need for this game (using this approach, that is).** Ok sounds great!

Lets say we have a 5 x 5 board. Each cell gets a variable number as such:

```
01 02 03 04 05
06 07 08 09 10
11 12 13 14 15
16 17 18 19 20
21 22 23 24 25
```

This assignment is easy to implement and we build the clauses as follows:
1. Go through the board and check each cell if there is a tree.
2. In case we find a tree, call a function `f()`. Function `f()` returns a list of variables. Let's say there is a tree at cell #15. Then `f()` will return variables 10, 14 and 20. This will result in clauses `(10 v 14 v 20) ^ (-10 v -14) ^ (-10 v -20) ^ (-14 v -20)`. There might also be the case when a row (or column) restriction is 0. For example, if the row restriction for row #2 is 0, `f()` will only return 14 and 20. In that case the clauses will be `(14 v 20) ^ (-14 v -20)`.
3. Save the variables returned by `f()` in a set `s`.
4. Go through set `s`. For each variable `a` check if there are any other adjacent (in any direction) variables in `s`. If there are any, say variables `b` and `c`, add clauses `(-a v -b) ^ (-a v -c)`.
5. Build the clauses for the row and column restrictions: For each row (and column) get the relevant variables in `s` and build the clauses accordingly. Using only the variables in `s` we end up building much shorter clauses, as each row (and column) will only have very few relevent variables/cells.

Update 14.11:

I suggest that we split the function `checkNeighbourTents` into functions for each 'potential tent' cell, that is:

for cell 1 (see the diagram below), we would have: check all adjacent cells to one (7 directions, excluding the tree to the right), for cell 2 the same but excluding the left, for cell 3 the same but excluding the down direction, for cell 4 the same but excluding the up direction. 

Also, I propose that we only check the non-redundant directions, that is, the functions for cells 1, 2 both check the status of cells 3, 4, etc. 
So I propose we exclude those redundant directions in the respective functions. For instance, the cells 1 and 3 both check the status of cell 9.

Also, the function `checkNeighbourTrees` should specify which cell (i.e., assign a truth value) 1, 2, 3, 4 is a potential tree (another tree than the current tree).

Apart from the fact that we save checking one redundant direction for each cell (the tree itself), we need to get the truth value of every neighbour for 1, 2, 3, 4 each. My idea for generating the truth values of the literals for every tree is as follows:

```
here goes the encoding in 28 variables for every encountered tree on the board [see below]

here we need to supply the truth values of the literals, 
i.e., describe the state of the [at most] 20 cells surrounding the tree; 

I suggest the following approach:

[this is some sort of pseudocode]

for(i=0; i<rows; i++){

    for(j=0; j<columns; j++){
    
        if(board[i][j] == tree){
        
            checkNeighbourTents(i, j-1); #cell 1, setting the truth values of variables 8, 9, 3, 12, 14, 15, 4
            
            checkRowCol(i, j-1); #cell 1, setting the truth value of variable 25
            
            checkNeighbourTents(i, j+1); #cell 2, setting the truth values of variables 10, 11, 13, 16, 17
            
            checkRowCol(i, j+1); #cell 2, setting the true value of variable 26
            
            checkNeighbourTents(i-1, j); #cell 3, setting the truth values of variables 5, 6, 7, 1, 2
            
            checkRowCol(i-1, j); #cell 3, setting the truth value of variable 27
            
            checkNeighbourTents(i+1, j); #cell 4, setting the truth values of variables 18, 19,  20
            
            checkRowCol(i+1, j); #cell 4, setting the truth value of variable 28
            
            checkNeighbourTrees(i,j); #check if any of the cells 1, 2, 3, 4 is another tree; setting the truth values of variables 21, 22, 23, 24
            
        }
    }
}
```

Update 13.11:

Proposed encoding:

Suppose we number the relevant cells (T denotes the tree) as follows:

(this is the universal scenario, i.e., the tree is in the central cell; refinements later)

Note: we interpret 'other trees' as empty neighbouring cells and add a flag variable to the potential tent cells (the orthogonally adjacent 1, 2, 3, 4) that ensures 1, 2, 3, 4 really IS empty and can host a tent. If it's a tree, we set the flag of 1, 2, 3, 4 to True and prevent the placing of a tent there. Incidentally, this encoding is better than simply introducing another variable for each cell (to encode TENT xor TREE xor EMPTY). We could use this 'worse encoding' to demonstrate that our current approach is better. This could automatically take care of the corresponding bonus point. I really can't think of a better encoding than this one (for now, at least). More examples follow.

```
    5  6  7
 8  9  3  10 11
12  1  T  2  13
14  15 4  16 17
    18 19 20
```

5 is directly above 9, 6 above 3, 7 above 10, etc.


Let's encode this case as follows:

```
1 => -8 & -9 & -3 & -12 & -14 & -15 & -4 & -2 & -3 & -21 & -25
```

That is: tent in cell 1 means no conflicts with the neighbouring cells and also no other 'tent cells' (2,3,4) are to be filled because the number of trees equals the number of tents. Tent means 'true', an empty cell is interpreted 'false'. Also note: variable `21` is the aforementioned flag. That is: the cell 1 (or 2, 3, 4) is empty if `-21` is true. If it contains another tree, we set it to false and thus prevent 'placing a tent on an already existing tree'. Variable 25 ensures that there is still place for a tent in the corresponding row and column. It is true iff the number of rows and columns for cell 1 are not zero. Analogously: variable 26 takes care of cell 2, 27 of cell 3, 28 of cell 4.

Previous approach: Variables 25 and 26 ensure that there is still place for a tent in the corresponding row and column, respectively. Observe that cells 1 and 2 share the same 'tents in a row value' and cells 3 and 4 share the same 'tents in a column value', so we need 6 additional variables to encode these constraints (variables 25, 26, 27, 28, 29, 30 below). This is also an example of a 'worse encoding'. The approach above is better because we only need 4 variables (instead of 6).

Analogously for the possible 'tent cells', i.e., 2, 3, 4.

And so we obtain the following DIMACS encoding:

```
p cnf 28 34

1 2 3 4 0
-1 -8 0
-1 -9 0
-1 -3 0
-1 -12 0
-1 -14 0
-1 -15 0
-1 -4 0
-1 -2 0
-1 -21 0
-1 -25 0
-2 -3 0
-2 -10 0
-2 -11 0
-2 -13 0
-2 -4 0
-2 -16 0
-2 -17 0
-2 -22 0
-2 -26 0
-3 -5 0
-3 -6 0
-3 -7 0
-3 -9 0
-3 -10 0
-3 -23 0
-3 -27 0
-4 -15 0
-4 -16 0
-4 -18 0
-4 -19 0
-4 -20 0
-4 -24 0
-4 -28 0

```

So far, I've tested the possible scenarios and this encoding seems to work (that is, setting some cells to empty or tents - this can be done by adding a clause with e.g., `18 0` means cell 18 already contains a tent, and so on). We will use the board's coordinates to get the truth values of the respective cells. 


Below is a draft in DNF.

In DNF:
`b[x][y]` are the board's coordinates; a total of 25 variables needed for DIMACS encoding

The directions below represent the possible cells where the tent can be placed.

```
Left:

(b[i][j-1] & -b[i-1][j-1] & -b[i+1][j-1] & -b[i-1][j] & -b[i+1][j] & -b[i][j-1] & -b[i-1][j-2] & -b[i+1][j-2] & rows[i]!=0 & columns[j-1]!=0) ||

Right:

(b[i][j+1] & -b[i-1][j+1] & -b[i+1][j+1] & -b[i-1][j] & -b[i+1][j] & -b[i][j+2] & -b[i-1][j+2] & b[i+1][j+2] & rows[i]!=0 & columns[j+1]!=0) ||

Up:

(b[i-1][j] & -b[i-2][j-1] & -b[i-2][j] & -b[i-2][j+1] & -b[i-1][j-1] & -b[i-1][j+1] & -b[i][j-1] & -b[i][j+1] & rows[i-1]!=0 & columns[j]!=0) ||

Down:

(b[i+1][j] & -b[i][j-1] & -b[i][j+1] & -b[i+1][j-1] & -b[i+1][j+1] & -b[i+2][j-1] & -b[i+2][j] & -b[i+2][j+1] & rows[i+1]!=0 & columns[j]!=0)
```
