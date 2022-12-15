#include "pch.h"
#include "harness.h"
#include "pt2.h"
#include "vector2d.h"


using Pt = Pt2i16;

struct Barrier
{
    vector<Pt> points;
    Pt lo{20000, 20000};
    Pt hi{-20000,-20000};
};

Barrier parseBarrier(const string& line)
{
    Barrier b;
    auto numPoints = ranges::count(line, '>') + 1;
    b.points.reserve(numPoints);

    istringstream is(line);
    Pt p;
    while (is)
    {
        is >> p.x >> "," >> p.y;
        b.points.push_back(p);

        b.lo.x = min(p.x, b.lo.x);
        b.lo.y = min(p.y, b.lo.y);
        b.hi.x = max(p.x, b.hi.x);
        b.hi.y = max(p.y, b.hi.y);

        if (is.peek() == istringstream::traits_type::eof())
            break;

        is >> "->";
    }

    return b;
}

auto sign(auto x)
{
    using T = decltype(x);
    if (x > T(0))
        return T(1);
    else if (x < T(0))
        return T(-1);
    return T(0);
}

Pt calcRockStep(const Pt& start, const Pt& end)
{
    Pt diff = end - start;
    return { sign(diff.x), sign(diff.y) };
}

void addRocks(vector2d<char>& map, const Pt& start, const Pt& end)
{
    Pt step = calcRockStep(start, end);
    for (Pt p = start; p != end; p += step)
    {
        map[p] = '#';
    }
    map[end] = '#';
}

bool dropSand(vector2d<char>& map, const Pt& source)
{
    constexpr Pt dn{ 0,1 };
    constexpr Pt dl{ -1,1 };
    constexpr Pt dr{ 1,1 };

    auto s = source;
    if (map[s] != '.' && map[s] != '+')
        return false;

    for (;;)
    {
        auto next = s + dn;
        if (next.y + 1 > int(map.height()))
            return false;

        if (map[next] == '.')
        {
            s = next;
            continue;
        }

        next = s + dl;
        if (map[next] == '.')
        {
            s = next;
            continue;
        }

        next = s + dr;
        if (map[next] == '.')
        {
            s = next;
            continue;
        }

        if (map.isInMap(s))
        {
            map[s] = 'o';
            return true;
        }
    }
}


constexpr i16 SandSourceX = 500;

vector2d<char> loadMap(const stringlist& input, bool abyss)
{
    TIME_SCOPE(loadMap);
    vector<Barrier> barriers;
    barriers.reserve(input.size());
    ranges::transform(input, back_inserter(barriers), parseBarrier);

    i16 maxy = -1;
    for (const Barrier& b : barriers)
        maxy = max(maxy, b.hi.y);

    maxy += 2;

    i16 minx = SandSourceX - maxy;
    i16 maxx = SandSourceX + maxy;

    vector2d<char> map(maxx - minx + 1, maxy + 1, '.');
    map.setOffset({ minx, 0 });

    for (const Barrier& b : barriers)
    {
        for (auto itPoint = begin(b.points); (itPoint + 1) != end(b.points); ++itPoint)
            addRocks(map, *itPoint, *(itPoint + 1));
    }

    if (!abyss)
        addRocks(map, { minx, maxy }, { maxx, maxy });

    return map;
}


int day14(const stringlist& input)
{
    vector2d<char> map = loadMap(input, true);

    auto source = Pt{ SandSourceX, 0 };
    map[source] = '+';

    int restingSand = 0;
    for (;;)
    {
        if (!dropSand(map, source))
            break;

        ++restingSand;
    }

    return restingSand;
}

int day14_2(const stringlist& input)
{
    vector2d<char> map = loadMap(input, false);

    auto source = Pt{ SandSourceX, 0 };
    map[source] = '+';

    int restingSand = 0;
    for (;;)
    {
        if (!dropSand(map, source))
            break;

        ++restingSand;
    }

    return restingSand;
}


void run_day14()
{
    string sample =
        R"(498,4 -> 498,6 -> 496,6
503,4 -> 502,4 -> 502,9 -> 494,9)";

    test(24, day14(READ(sample)));
    gogogo(day14(LOAD(14)));

    test(93, day14_2(READ(sample)));
    gogogo(day14_2(LOAD(14)));
}