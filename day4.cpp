#include "pch.h"
#include "harness.h"


using Range = pair<int, int>;

Range parseRange(const string& range)
{
    auto nums = split(range, "-");
    return { stoi(nums[0]), stoi(nums[1]) };
}

bool isSubset(const Range& outer, const Range& inner)
{
    if ((inner.first < outer.first) || (inner.second > outer.second))
        return false;
    return true;
}

bool intersect(const Range& a, const Range& b)
{
    if (a.first > b.second || b.first > a.second)
        return false;
    return true;
}

int day4(const stringlist& input)
{
    int count = 0;

    for (auto& line : input)
    {
        auto elves = split(line, ",");
        vector<Range> ranges;
        ranges::transform(elves, back_inserter(ranges), [](auto& range) { return parseRange(range); });

        if (isSubset(ranges[0], ranges[1]) || isSubset(ranges[1], ranges[0]))
            ++count;
    }

    return count;
}

int day4_2(const stringlist& input)
{
    int count = 0;

    for (auto& line : input)
    {
        auto elves = split(line, ",");
        vector<Range> ranges;
        ranges::transform(elves, back_inserter(ranges), [](auto& range) { return parseRange(range); });

        if (intersect(ranges[0], ranges[1]))
            ++count;
    }

    return count;
}


void run_day4()
{
    string sample =
        R"(2-4,6-8
2-3,4-5
5-7,7-9
2-8,3-7
6-6,4-6
2-6,4-8)";

    test(2, day4(READ(sample)));
    gogogo(day4(LOAD(4)));

    test(4, day4_2(READ(sample)));
    gogogo(day4_2(LOAD(4)));
}