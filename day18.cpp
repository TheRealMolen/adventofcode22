#include "pch.h"
#include "harness.h"
#include "pt3.h"

using Pt = Pt3<i16>;

constexpr Pt Directions[] = { {1,0,0}, {-1,0,0}, {0,1,0}, {0,-1,0}, {0,0,1}, {0,0,-1} };

int day18(const stringlist& input)
{
    set<Pt> droplets;
    for (auto& line : input)
        droplets.emplace(line);

    int nsides = 6 * int(size(droplets));
    for (const auto& d : droplets)
    {
        for (const Pt& dir : Directions)
        {
            if (droplets.contains(d + dir))
                --nsides;
        }
    }

    return nsides;
}

enum class Cell : u8 { Air, Lava, Water };

int day18_2(const stringlist& input)
{
    vector<Pt> droplets;
    ranges::transform(input, back_inserter(droplets), [](auto& line) { return Pt(line); });
    auto [loX, hiX] = ranges::minmax(droplets | views::transform([](auto& p) { return p.x; }));
    auto [loY, hiY] = ranges::minmax(droplets | views::transform([](auto& p) { return p.y; }));
    auto [loZ, hiZ] = ranges::minmax(droplets | views::transform([](auto& p) { return p.z; }));
    Pt lo(loX, loY, loZ);
    Pt hi(hiX, hiY, hiZ);
    Pt size = (hi - lo) + Pt{ 3,3,3 };
    Pt offset = lo - Pt{ 1,1,1 };

    vector<Cell> cells(size.x * size.y * size.z, Cell::Air);
    auto getCell = [&cells, &size, &offset](const Pt& cell)
    {
        Pt p = cell - offset;
        if (p.x < 0 || p.y < 0 || p.z < 0 || p.x >= size.x || p.y >= size.y || p.z >= size.z)
            return Cell::Water;
        return cells[p.x + (p.y * size.x) + (p.z * size.x * size.y)];
    };
    auto setCell = [&cells, &size, &offset](const Pt& cell, Cell c)
    {
        Pt p = cell - offset;
        cells[p.x + (p.y * size.x) + (p.z * size.x * size.y)] = c;
    };

    for (const auto& drop : droplets)
        setCell(drop, Cell::Lava);

    vector<Pt> open;
    open.push_back(lo - Pt(1,1,1));
    while (!open.empty())
    {
        Pt curr = open.back();
        open.pop_back();
        setCell(curr, Cell::Water);

        for (const Pt& dir : Directions)
        {
            Pt p = curr + dir;
            if (getCell(p) == Cell::Air)
                open.push_back(p);
        }
    }

    int nsides = 0;
    for (const auto& d : droplets)
    {
        for (const Pt& dir : Directions)
        {
            if (getCell(d + dir) == Cell::Water)
                ++nsides;
        }
    }

    return nsides;
}


void run_day18()
{
    string sample =
        R"(2,2,2
1,2,2
3,2,2
2,1,2
2,3,2
2,2,1
2,2,3
2,2,4
2,2,6
1,2,5
3,2,5
2,1,5
2,3,5)";

    test(10, day18(READ("1,1,1\n2,1,1")));
    test(64, day18(READ(sample)));
    gogogo(day18(LOAD(18)));

    test(58, day18_2(READ(sample)));
    gogogo(day18_2(LOAD(18)));
}