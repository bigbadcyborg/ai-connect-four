// instrumentation.h
#ifndef INSTRUMENTATION_H
#define INSTRUMENTATION_H

#include <sys/resource.h>
#include <chrono>
#include <tuple>

// globals to be reset before each run:
extern uint64_t g_nodesGenerated;
extern uint64_t g_nodesExpanded;

// call at entry of minMaxAB:
inline void noteNode(bool isLeaf) {
    ++g_nodesGenerated;
    if (!isLeaf) ++g_nodesExpanded;
}

// wrapper to get peak RSS in KB
inline long peakRSS_KB() {
    struct rusage ru;
    getrusage(RUSAGE_SELF, &ru);
    return ru.ru_maxrss;  
}

// a little struct to collect metrics
struct Metrics {
    int minD, maxD;
    uint64_t nodesGen, nodesExp;
    double  elapsedMs;
    long    memKB;
    char    winner;  // 'X' or 'O' or 'D' (draw)
};

#endif // INSTRUMENTATION_H
