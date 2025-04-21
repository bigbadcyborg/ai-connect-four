#include <iostream>
#include <vector>
#include <limits>
#include <algorithm>
#include "Board.h"

int main() {
    Board board;
    int depth = 4;  // cutoff depth for search (adjustable)

    // game loop until board filled, win, or loss
    while (!board.isFull() && !board.checkWin(MAX_PLAYER) && !board.checkWin(MIN_PLAYER)) {
        board.printBoard();

        // human (min) turn
        int playerMove;
        cout << "Enter your move (0-6): ";
        cin >> playerMove;
        if (!board.isValidMove(playerMove)) {
            cout << "Invalid move. Try again.\n";
            continue;
        }
        board.makeMove(playerMove, MIN_PLAYER);

        // computer (max) turn if game not ended
        if (!board.checkWin(MIN_PLAYER)) {
            Move compMove = bestMove(board, depth, MAX_PLAYER);
            board.makeMove(compMove.col, MAX_PLAYER);
            cout << "Computer move: " << compMove.col << "\n";
        }
    }

    // final board print and result
    board.printBoard();
    if (board.checkWin(MAX_PLAYER))
        cout << "MAX (Computer) Wins!\n";
    else if (board.checkWin(MIN_PLAYER))
        cout << "MIN (You) Wins!\n";
    else
        cout << "It's a Draw!\n";

    return 0;
}