#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <limits>
#include <algorithm>
#include "Hueristics.h" 

using namespace std;

// constants for board dimensions and players
const int ROWS = 6;                   // number of rows in connect four
const int COLS = 7;                   // number of columns in connect four
const char EMPTY = '.';               // symbol for an empty board cell
const char MAX_PLAYER = 'X';          // symbol for the maximizing player
const char MIN_PLAYER = 'O';          // symbol for the minimizing player


// Move structure representing a column choice
struct Move {
    int row;  // not used here but could store the row if needed
    int col;  // column index (0-based) where the move is made
};


// Board class representing the game state
class Board {
public:
    vector<vector<char>> grid;  // 2d grid for board cells

    // constructor initializes an empty board
    Board() : grid(ROWS, vector<char>(COLS, EMPTY)) {}

    // check if a move (dropping a piece in column 'col') is valid
    bool isValidMove(int col) const {
        // a move is valid if the topmost cell in the column is empty
        return grid[0][col] == EMPTY;
    }


    // make a move for 'player' in column 'col'
    void makeMove(int col, char player) {
        // drop piece to the lowest empty cell in the column
        for (int i = ROWS - 1; i >= 0; --i) {
            if (grid[i][col] == EMPTY) {
                grid[i][col] = player;
                break;
            }
        }
    }


    // undo the most recent move in column 'col'
    void undoMove(int col) {
        // remove the topmost non-empty cell in the column
        for (int i = 0; i < ROWS; ++i) {
            if (grid[i][col] != EMPTY) {
                grid[i][col] = EMPTY;
                break;
            }
        }
    }



    // check if the board is completely filled
    bool isFull() const {
        for (int c = 0; c < COLS; ++c)
            if (grid[0][c] == EMPTY) return false;
        return true;
    }
	


    // check win condition for 'player' (4 in a row: horizontal, vertical, or diagonal)
    bool checkWin(char player) const {
        for (int r = 0; r < ROWS; ++r) {
            for (int c = 0; c < COLS; ++c) {
                if (grid[r][c] != player) continue;
                // check horizontal (right)
                if (c + 3 < COLS && grid[r][c+1] == player && grid[r][c+2] == player && grid[r][c+3] == player)
                    return true;
                // check vertical (down)
                if (r + 3 < ROWS && grid[r+1][c] == player && grid[r+2][c] == player && grid[r+3][c] == player)
                    return true;
                // check diagonal (down-right)
                if (c + 3 < COLS && r + 3 < ROWS &&
                    grid[r+1][c+1] == player && grid[r+2][c+2] == player && grid[r+3][c+3] == player)
                    return true;
                // check diagonal (down-left)
                if (c - 3 >= 0 && r + 3 < ROWS &&
                    grid[r+1][c-1] == player && grid[r+2][c-2] == player && grid[r+3][c-3] == player)
                    return true;
            }
        }
        return false;
    }
	
	

    // print the current board to the console
    void printBoard(ofstream & fout) const {
        for (const auto &row : grid) {
            for (char cell : row)
                fout << cell << ' ';
            fout << '\n';
        }
        fout << "---------------\n0 1 2 3 4 5 6\n";
		
		
		if (this->checkWin(MAX_PLAYER))
			fout << "MAX (Computer) Wins!\n";
		else if (this->checkWin(MIN_PLAYER))
			fout << "MIN (You) Wins!\n";
	
    }
	
	
	
}; // End of Board class


#endif // BOARD_H