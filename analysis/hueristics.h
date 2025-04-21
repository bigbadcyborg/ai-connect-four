#ifndef HEURISTICS_H
#define HEURISTICS_H

#include "Board.h"
#include "instrumentation.h"   // include our instrumentation counters
#include <limits>

// Evaluation functions:
inline int evaluateWithCenterBias(const Board &board) {
    static constexpr int colWeight[COLS] = {1,2,3,4,3,2,1};
    int winResult = board.checkWin(MAX_PLAYER) ? 100000 :
                    board.checkWin(MIN_PLAYER) ? -100000 : 0;
    if (winResult) return winResult;

    int score = 0;
    for (int r = 0; r < ROWS; ++r) {
        for (int c = 0; c < COLS; ++c) {
            char cell = board.grid[r][c];
            if (cell == EMPTY) continue;
            score += (cell == MAX_PLAYER) ? colWeight[c] : -colWeight[c];
        }
    }
    return score;
}

inline int evaluateWithSparseBias(const Board &board) {
    int winResult = board.checkWin(MAX_PLAYER) ? 100000 :
                    board.checkWin(MIN_PLAYER) ? -100000 : 0;
    if (winResult) return winResult;

    int heights[COLS] = {0};
    int maxHeight = 0;
    for (int c = 0; c < COLS; ++c) {
        for (int r = 0; r < ROWS; ++r) {
            if (board.grid[r][c] != EMPTY) heights[c]++;
        }
        maxHeight = std::max(maxHeight, heights[c]);
    }

    int score = 0;
    for (int c = 0; c < COLS; ++c) {
        int weight = maxHeight - heights[c] + 1;
        for (int r = 0; r < ROWS; ++r) {
            char cell = board.grid[r][c];
            if (cell == MAX_PLAYER) score += weight;
            else if (cell == MIN_PLAYER) score -= weight;
        }
    }
    return score;
}

inline int minMaxAB(Board &board, int depth, int alpha, int beta, bool isMaximizer) {
    // instrumentation: count nodes
    bool isLeaf = (depth == 0) || board.checkWin(MAX_PLAYER)
                  || board.checkWin(MIN_PLAYER) || board.isFull();
    noteNode(isLeaf);

    if (isLeaf) {
        return isMaximizer ? evaluateWithCenterBias(board)
                           : evaluateWithSparseBias(board);
    }

    if (isMaximizer) {
        int maxEval = std::numeric_limits<int>::min();
        for (int c = 0; c < COLS; ++c) {
            if (!board.isValidMove(c)) continue;
            board.makeMove(c, MAX_PLAYER);
            int eval = minMaxAB(board, depth-1, alpha, beta, false);
            board.undoMove(c);
            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);
            if (beta <= alpha) break;
        }
        return maxEval;
    } else {
        int minEval = std::numeric_limits<int>::max();
        for (int c = 0; c < COLS; ++c) {
            if (!board.isValidMove(c)) continue;
            board.makeMove(c, MIN_PLAYER);
            int eval = minMaxAB(board, depth-1, alpha, beta, true);
            board.undoMove(c);
            minEval = std::min(minEval, eval);
            beta = std::min(beta, eval);
            if (beta <= alpha) break;
        }
        return minEval;
    }
}

inline Move bestMove(Board &board, int depth, char player) {
    int bestVal = (player == MAX_PLAYER)
        ? std::numeric_limits<int>::min()
        : std::numeric_limits<int>::max();
    Move bestMv{-1,-1};

    for (int c = 0; c < COLS; ++c) {
        if (!board.isValidMove(c)) continue;
        board.makeMove(c, player);
        int eval = minMaxAB(board, depth-1,
                            std::numeric_limits<int>::min(),
                            std::numeric_limits<int>::max(),
                            player == MAX_PLAYER ? false : true);
        board.undoMove(c);
        if ((player == MAX_PLAYER && eval > bestVal) ||
            (player == MIN_PLAYER && eval < bestVal)) {
            bestVal = eval;
            bestMv.col = c;
        }
    }
    return bestMv;
}

#endif // HEURISTICS_H
