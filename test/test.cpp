#include <iostream>
#include <vector>
#include <limits>
#include <algorithm>
#include "Hueristics.h"

int main(int argc, char* argv[]) {
	// require two command-line arguments
	if (argc != 3){
		cerr << "Usage: " << argv[0] << "<minDepth> <maxDepth>" << endl;
		return 1;
	}
	int minDepth = atoi(argv[1]);  // cutoff depth for MIN_PLAYER search
	int maxDepth = atoi(argv[2]);  // cutoff depth for MAX_PLAYER search
	
    Board board;
	Move compMove;

    // game loop until board filled, win, or loss
    while (!board.isFull() && !board.checkWin(MAX_PLAYER) && !board.checkWin(MIN_PLAYER)) {
		//clearScreen();
        board.printBoard();
		
		// computer (max) turn if game not ended
        if (!board.checkWin(MIN_PLAYER)) {
            compMove = bestMove(board, maxDepth, MAX_PLAYER);
            board.makeMove(compMove.col, MAX_PLAYER);
            cout << "Max Computer move: " << compMove.col << endl;
        }


        // human (min) turn
        /*int playerMove;
        cout << "Enter your move (0-6): ";
        cin >> playerMove;
        if (!board.isValidMove(playerMove)) {
            cout << "Invalid move. Try again.\n";
            continue;
        }
        board.makeMove(playerMove, MIN_PLAYER);*/
		
		
		// computer (min) turn
		if(!board.checkWin(MAX_PLAYER)){
			compMove = bestMove(board, minDepth, MIN_PLAYER);
			board.makeMove(compMove.col, MIN_PLAYER);
			cout << "Min Computer move: " << compMove.col << endl;
		}
		
    }
	
	

    // final board print and result
	//clearScreen();
    board.printBoard();

    return 0;
}