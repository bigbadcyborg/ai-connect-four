#include <cstdint>
#include <iostream>
#include <vector>
#include <chrono>
#include <iomanip>
#include "instrumentation.h"
#include "board.h"
#include "hueristics.h"
// globals
uint64_t g_nodesGenerated = 0;
uint64_t g_nodesExpanded  = 0;

// refactor your existing main‐loop into this:
char runGame(int minDepth, int maxDepth) {
    // reset instrumentation
    g_nodesGenerated = g_nodesExpanded = 0;

    Board board;
    Move mv;
    // alternate moves until game over
    while (!board.isFull() && !board.checkWin(MAX_PLAYER) && !board.checkWin(MIN_PLAYER)) {
        // MAX turn
        if (!board.checkWin(MIN_PLAYER)) {
            mv = bestMove(board, maxDepth, MAX_PLAYER);
            board.makeMove(mv.col, MAX_PLAYER);
        }
        // MIN turn
        if (!board.checkWin(MAX_PLAYER)) {
            mv = bestMove(board, minDepth, MIN_PLAYER);
            board.makeMove(mv.col, MIN_PLAYER);
        }
    }
    // return the winner
    if (board.checkWin(MAX_PLAYER)) return MAX_PLAYER;
    if (board.checkWin(MIN_PLAYER)) return MIN_PLAYER;
    return 'D';  // draw (if you ever allow)
}

int main() {
    vector<pair<int,int>> combos = {
        {2,2},{2,4},{2,8},
        {4,2},{4,4},{4,8},
        {8,2},{8,4},{8,8}
    };
    vector<Metrics> results;

    for (auto [minD,maxD] : combos) {
		// start the timer, run the game, stop the timer
        auto t0 = chrono::high_resolution_clock::now();
        char winner = runGame(minD, maxD);
        auto t1 = chrono::high_resolution_clock::now();

		// calculate game runtime duration
        double ms = chrono::duration<double, milli>(t1-t0).count();
		
		// calculate peak RSS memory
        long memKB = peakRSS_KB();

        results.push_back({minD, maxD,
                           g_nodesGenerated,
                           g_nodesExpanded,
                           ms,
                           memKB,
                           winner});
    }

		// eloquent table output
		cout << left
				  << setw(6)  << "minD"
				  << setw(6)  << "maxD"
				  << setw(18) << "nodesGenerated"
				  << setw(18) << "nodesExpanded"
				  << setw(12) << "time(s)"
				  << setw(10) << "mem(KB)"
				  << setw(8)  << "winner"
				  << "\n";

		for (auto &m : results) {
			cout << left
					  << setw(6)  << m.minD
					  << setw(6)  << m.maxD
					  << setw(18) << m.nodesGen
					  << setw(18) << m.nodesExp
					  << setw(12) << fixed << setprecision(3) << (m.elapsedMs/1000.0)
					  << setw(10) << m.memKB
					  << setw(8)  << m.winner
					  << "\n";
		}
    return 0;
}
