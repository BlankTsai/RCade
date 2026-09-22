#ifndef FRAME_PROFILE_H
#define FRAME_PROFILE_H
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <fstream>
#include <iomanip>
#include <vector>

struct FrameSummary {
    double mean = 0, p95 = 0, p99 = 0, maximum = 0;
    std::size_t over25 = 0, over50 = 0;
};
inline FrameSummary SummarizeFrames(std::vector<double> samples) {
    FrameSummary result;
    if (samples.empty()) return result;
    for (double ms : samples) {
        result.mean += ms;
        if (ms > 25.0) ++result.over25;
        if (ms > 50.0) ++result.over50;
    }
    result.mean /= samples.size();
    std::sort(samples.begin(), samples.end());
    result.p95 = samples[static_cast<std::size_t>(std::ceil(samples.size()*0.95))-1];
    result.p99 = samples[static_cast<std::size_t>(std::ceil(samples.size()*0.99))-1];
    result.maximum = samples.back();
    return result;
}
#endif
