#include "FrameProfile.h"
#include <cassert>
int main() {
    auto empty = SummarizeFrames({});
    assert(empty.mean == 0 && empty.p99 == 0);
    auto one = SummarizeFrames({60});
    assert(one.mean == 60 && one.p95 == 60 && one.over50 == 1);
    std::vector<double> values;
    for (int i=100; i>=1; --i) values.push_back(i);
    auto s = SummarizeFrames(values);
    assert(s.mean == 50.5 && s.p95 == 95 && s.p99 == 99 && s.maximum == 100);
    assert(s.over25 == 75 && s.over50 == 50);
    auto boundary = SummarizeFrames({25,50});
    assert(boundary.over25 == 1 && boundary.over50 == 0);
}
