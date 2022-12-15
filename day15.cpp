#include "pch.h"
#include "harness.h"
#include "pt2.h"


struct Report
{
    Pt2i s, b;
};

int manhattan(const Pt2i& a, const Pt2i& b)
{
    return abs(a.x - b.x) + abs(a.y - b.y);
}


vector<Report> readReports(const stringlist& input)
{
    vector<Report> reports;
    reports.reserve(input.size());

    for (auto& line : input)
    {
        istringstream is(line);
        Pt2i s, b;
        is >> "Sensor at x=" >> s.x >> ", y=" >> s.y >> ": closest beacon is at x=" >> b.x >> ", y=" >> b.y;
        reports.emplace_back(s, b);
    }

    return reports;
}


void coalesceIntervals(vector<pair<int, int>>& intervals)
{
    ranges::sort(intervals);

    for (auto it = begin(intervals); it != end(intervals) && (it + 1) != end(intervals); /**/)
    {
        auto itNext = it + 1;
        if (itNext->first <= it->second)
        {
            it->second = max(it->second, itNext->second);
            intervals.erase(itNext);
        }
        else
            ++it;
    }
}


int day15(const stringlist& input, int y)
{
    set<int> beaconsOnRow;
    vector<pair<int, int>> intervals;
    for (auto& line : input)
    {
        istringstream is(line);
        Pt2i s, b;
        is >> "Sensor at x=" >> s.x >> ", y=" >> s.y >> ": closest beacon is at x=" >> b.x >> ", y=" >> b.y;

        if (b.y == y)
            beaconsOnRow.insert(b.x);

        int distSB = manhattan(s, b);
        
        int distSY = abs(y - s.y);
        if (distSY > distSB)
            continue;

        int hSizeAtY = distSB - distSY;
        int minx = s.x - hSizeAtY;
        int maxx = s.x + hSizeAtY;

        intervals.emplace_back(minx, maxx);
    }

    coalesceIntervals(intervals);

    // cut holes for beacons
    for (int bx : beaconsOnRow)
    {
        bool done;
        do {
            done = true;
            for (auto& i : intervals)
            {
                if (i.first == bx)
                    ++i.first;
                else if (i.second == bx)
                    --i.second;
                else if (i.first < bx && i.second > bx)
                {
                    pair<int, int> newInt{ bx + 1,i.second };
                    i.second = bx - 1;
                    intervals.push_back(newInt);
                    done = false;
                    break;
                }
            }

        } while (!done);
    }

    // remove any inside-out intervals
    intervals.erase(remove_if(begin(intervals), end(intervals), [](auto& i) { return i.first > i.second; }), end(intervals));

    return accumulate(begin(intervals), end(intervals), 0, [](int acc, auto& interval) { return acc + (interval.second - interval.first + 1); });
}

i64 day15_2(const stringlist& input, int maxCoord)
{
    auto tuningFrequency = [](i64 x, i64 y) -> i64 { return (x * 4000000) + y; };

    auto reports = readReports(input);
    vector<pair<int, int>> intervals;
    for (int y=0; y<=maxCoord; ++y)
    {
        intervals.clear();
        for (auto& [s,b] : reports)
        {
            int distSB = manhattan(s, b);
            int distSY = abs(y - s.y);
            if (distSY > distSB)
                continue;

            int hSizeAtY = distSB - distSY;
            int minx = s.x - hSizeAtY;
            int maxx = s.x + hSizeAtY;

            intervals.emplace_back(minx, maxx);
        }
        if (intervals.empty())
            continue;

        coalesceIntervals(intervals);

        if (intervals.front().first > 0)
            return tuningFrequency(0, y);
        if (intervals.back().second < maxCoord)
            return tuningFrequency(maxCoord, 0);
        if (intervals.size() > 1)
            return tuningFrequency(intervals.front().second + 1, y);
    }

    return -1;
}


void run_day15()
{
    string sample =
R"(Sensor at x=2, y=18: closest beacon is at x=-2, y=15
Sensor at x=9, y=16: closest beacon is at x=10, y=16
Sensor at x=13, y=2: closest beacon is at x=15, y=3
Sensor at x=12, y=14: closest beacon is at x=10, y=16
Sensor at x=10, y=20: closest beacon is at x=10, y=16
Sensor at x=14, y=17: closest beacon is at x=10, y=16
Sensor at x=8, y=7: closest beacon is at x=2, y=10
Sensor at x=2, y=0: closest beacon is at x=2, y=10
Sensor at x=0, y=11: closest beacon is at x=2, y=10
Sensor at x=20, y=14: closest beacon is at x=25, y=17
Sensor at x=17, y=20: closest beacon is at x=21, y=22
Sensor at x=16, y=7: closest beacon is at x=15, y=3
Sensor at x=14, y=3: closest beacon is at x=15, y=3
Sensor at x=20, y=1: closest beacon is at x=15, y=3)";

    test(26, day15(READ(sample), 10));
    gogogo(day15(LOAD(15), 2000000));

    nest(56000011ll, day15_2(READ(sample), 20));
    nononoD(day15_2(LOAD(15), 4000000));
}