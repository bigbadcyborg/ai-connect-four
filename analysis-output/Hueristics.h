#ifndef HEURISTICS_H
#define HEURISTICS_H

#include "Board.h"

// evaluateWithCenterBias()
// description: prioritize control of the center by assigning a weight based on 
//   distance from the center. uses static constexpr to ensure weighted values are computed once
//	 and reused on every cell.
// returns: the weight ( positive if MAX_PLAYER, negative otherwise )
// runtime: linear O(R*C=42) 
// memory: ~17 bytes of stack storage
//    + 28 bytes of static, read-only memory, not allocated each call.
inline int evaluateWithCenterBias(const Board &board) {
    // Precomputed weights for center priority
    static constexpr int colWeight[COLS] = {1, 2, 3, 4, 3, 2, 1};
    int score = 0;

	// check win condition via a ternary conditional determined return value
    int winResult = board.checkWin(MAX_PLAYER) ? 100000 :
                    board.checkWin(MIN_PLAYER) ? -100000 : 0;
    if (winResult) return winResult;

    for (int r = 0; r < ROWS; ++r) {
        for (int c = 0; c < COLS; ++c) {
            char cell = board.grid[r][c];
            if (cell == EMPTY) continue;

            int weight = colWeight[c];
            score += (cell == MAX_PLAYER) ? weight : -weight;
        }
    }

    return score;
}


// evaluateWithSparseBias()
// description: prioritize playing into less-filled columns.
// returns: the weight
// runtime: linear O(R*C=42)
// memory: heights[COLS] -> 7 ints = 28 bytes on stack 
// 		+ negligible loop/control variables
inline int evaluateWithSparseBias(const Board &board) {
    int score = 0;

	// check win condition via a ternary conditional determined return value
    int winResult = board.checkWin(MAX_PLAYER) ? 100000 :
                    board.checkWin(MIN_PLAYER) ? -100000 : 0;
    if (winResult) return winResult;

    // use fixed-size array instead of vector (faster, no heap allocation)
    int heights[COLS] = {0};

    // count heights of each column
    int maxHeight = 0;
    for (int c = 0; c < COLS; ++c) {
        for (int r = 0; r < ROWS; ++r) {
            if (board.grid[r][c] != EMPTY) {
                heights[c]++;
            }
        }
        if (heights[c] > maxHeight) maxHeight = heights[c];
    }

    // use sparse weight to evaluate board
    for (int c = 0; c < COLS; ++c) {
        int weight = (maxHeight - heights[c] + 1);
        for (int r = 0; r < ROWS; ++r) {
            char cell = board.grid[r][c];
            if (cell == MAX_PLAYER) score += weight;
            else if (cell == MIN_PLAYER) score -= weight;
        }
    }

    return score;
}



// minMaxAB performs the minimax search with alpha-beta pruning.
// - 'depth' controls how many plies ahead to search (cutoff depth).
// - 'alpha' is the best value that the maximizer currently can guarantee.
// - 'beta' is the best value that the minimizer currently can guarantee.
// - 'playerTypeFlag' indicates whether this call is for MAX_PLAYER (true) or MIN_PLAYER (false).
inline int minMaxAB(Board &board, int depth, int alpha, int beta, bool playerTypeFlag) {
    // base-case: if we've reached depth 0, someone has won, or board is full
    if (depth == 0 || board.checkWin(MAX_PLAYER) || board.checkWin(MIN_PLAYER) || board.isFull()) {
        // evaluate this board position and return its score,
        //   choosing perspective based on whose turn it was
		if( playerTypeFlag )
			return evaluateWithCenterBias(board);
		else
			return evaluateWithSparseBias(board);
    }

    if (playerTypeFlag) {
        int maxEval = numeric_limits<int>::min();
        // iterate through all possible moves (columns)
        for (int c = 0; c < COLS; ++c) {
            if (board.isValidMove(c)) {
                // simulate move for MAX_PLAYER
                board.makeMove(c, MAX_PLAYER);
                // recursively evaluate resulting position from minimizer's turn
                int eval = minMaxAB(board, depth - 1, alpha, beta, false);
                // undo the move to restore state
                board.undoMove(c);

                // keep track of the maximum score seen
                maxEval = max(maxEval, eval);
                // update alpha (best guaranteed for maximizer so far)
                alpha = max(alpha, eval);
                // if beta <= alpha, prune remaining branches
                if (beta <= alpha)
                    break;
            }
        }
        return maxEval;
    } else {
		// initialize worst starting point for minimizer
        int minEval = numeric_limits<int>::max();
        // iterate through all possible moves (columns)
        for (int c = 0; c < COLS; ++c) {
            if (board.isValidMove(c)) {
                // simulate move for MIN_PLAYER
                board.makeMove(c, MIN_PLAYER);
                // recursively evaluate resulting position from maximizer's turn
                int eval = minMaxAB(board, depth - 1, alpha, beta, true);
                // undo the move to restore state
                board.undoMove(c);

                // keep track of the minimum score seen
                minEval = min(minEval, eval);
                // update beta (best guaranteed for minimizer so far)
                beta = min(beta, eval);
                // if beta <= alpha, prune remaining branches
                if (beta <= alpha)
                    break;
            }
        }
        return minEval;
    }
}


// function: bestMove()
// this function finds and returns the optimal column move for the given 'player' using minMaxAB:
// 1. initialize bestVal to the worst-case score for this player:
//      - for MAX_PLAYER (AI), start at -infinity so any move will be better
//      - for MIN_PLAYER (human), start at +infinity so any move will be better
// 2. initialize bestMv to an invalid state (-1, -1)
// 3. for each column c in [0..COLS-1]:
//      a) skip if column is full (invalid move)
//      b) simulate the move by dropping the player's disc into column c
//      c) call minMaxAB on the resulting board with:
//            - depth reduced by 1
//            - fresh alpha = -infinity, beta = +infinity
//            - playerTypeFlag flag flipped
//         this returns a score (moveVal) estimating the board's outcome
//      d) undo the simulated move to restore the board
//      e) if moveVal improves upon bestVal (higher for MAX, lower for MIN),
//            update bestVal and record bestMv.col = c
// 4. return bestMv, containing the optimal column choice
inline Move bestMove(Board &board, int depth, char player) {
    int bestVal = (player == MAX_PLAYER)
                  ? numeric_limits<int>::min()
                  : numeric_limits<int>::max();
    Move bestMv = {-1, -1};

    for (int c = 0; c < COLS; ++c) {
        if (!board.isValidMove(c))
            continue;  // skip full columns

        // simulate player's move in column c
        board.makeMove(c, player);

        // recursively evaluate with minMaxAB:
        // next depth = depth - 1
        // reset alpha and beta to worst values
        // toggle playerTypeFlag (player != MAX_PLAYER)
        int moveVal = minMaxAB(
            board,
            depth - 1,
            numeric_limits<int>::min(),
            numeric_limits<int>::max(),
            player != MAX_PLAYER
        );

        // undo simulation to keep board state unchanged
        board.undoMove(c);

        // check if this move is better than any before:
        if ((player == MAX_PLAYER && moveVal > bestVal) ||
            (player == MIN_PLAYER && moveVal < bestVal)) {
            bestVal = moveVal;           // update best score
            bestMv = {0, c};             // record best move's column
        }
    }

    return bestMv;  // optimal move for player
}



// on Windows, the macro _WIN32 is defined, so it runs "cls"
// on Unix-like systems (Linux, macOS), it runs "clear"
// using conditional compilation ensures portability across platforms
inline void clearScreen() {
#ifdef _WIN32
    system("cls");  // windows clear screen command
#else
    system("clear");  // unix clear screen command
#endif
}


#endif // HEURISTICS_H