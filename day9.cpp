#include "pch.h"
#include "harness.h"

#include "pt2.h"
#include <span>


Pt2i getMoveFromDir(char dir)
{
    switch(dir)
    {
    case 'R': return { 1,0 };
    case 'L': return { -1,0 };
    case 'U': return { 0,1 };
    case 'D': return { 0,-1 };
    }
    throw "whu?";
}


inline int sign(int i)
{
    if (i > 0)
        return 1;
    if (i < 0)
        return -1;
    return 0;
}


void dumpGrid(const Pt2i& head, const Pt2i& tail, const Pt2i& hi = {5,5}, const Pt2i& lo = {0,0})
{
    if (head.x < 0 || head.y < 0 || tail.x < 0 || tail.y < 0)
        throw "barf";

    for (int y = hi.y; y >= lo.y; --y)
    {
        for (int x = lo.x; x <= hi.x; ++x)
        {
            if (head == Pt2i(x, y))
                cout << 'H';
            else if (tail == Pt2i(x, y))
                cout << 'T';
            else if (Pt2i(0, 0) == Pt2i(x, y))
                cout << 's';
            else
                cout << '.';
        }
        cout << '\n';
    }
    cout << endl;
}

void dumpGrid(span<const Pt2i> knots, const Pt2i& hi = { 5,5 }, const Pt2i& lo = { 0,0 })
{
    for (int y = hi.y; y >= lo.y; --y)
    {
        for (int x = lo.x; x <= hi.x; ++x)
        {
            bool written = false;
            for (u32 k=0; k<knots.size(); ++k)
            {
                if (knots[k] == Pt2i(x, y))
                {
                    char c = 'H';
                    if (k > 0)
                        c = char('0' + k);

                    cout << c;
                    written = true;
                    break;
                }
            }

            if (!written && Pt2i(0, 0) == Pt2i(x, y))
                cout << 's';
            else if (!written)
                cout << '.';
        }
        cout << '\n';
    }
    cout << endl;
}


void moveTowards(Pt2i& knot, const Pt2i& target)
{
    Pt2i d = target - knot;
    if (abs(d.x) > 1 || abs(d.y) > 1)
    {
        if (d.x == 0)
            knot.y += sign(d.y);
        else if (d.y == 0)
            knot.x += sign(d.x);
        else
        {
            Pt2i tailMove{ sign(d.x), sign(d.y) };
            knot += tailMove;
        }
    }
}


int day9(const stringlist& input)
{
    Pt2i head{ 0, 0 }, tail{ 0, 0 };
    unordered_set<Pt2i> visited;

    visited.insert(tail);
    for (auto& line : input)
    {
        istringstream is(line);
        char dir;
        int repeats;
        is >> dir >> repeats;

        Pt2i move = getMoveFromDir(dir);
        for (int repeat = 0; repeat < repeats; ++repeat)
        {
            head += move;
            moveTowards(tail, head);
            visited.insert(tail);
        }
    }

    return int(visited.size());
}

int day9_2(const stringlist& input)
{
    constexpr int numKnots = 10;
    Pt2i knots[numKnots];
    ranges::fill(knots, Pt2i{ 0,0 });

    const Pt2i& tail = knots[numKnots - 1];
    unordered_set<Pt2i> visited;

    visited.insert(tail);
    for (auto& line : input)
    {
        istringstream is(line);
        char dir;
        int repeats;
        is >> dir >> repeats;

        Pt2i move = getMoveFromDir(dir);
        for (int repeat = 0; repeat < repeats; ++repeat)
        {
            knots[0] += move;

            for (int knot = 1; knot < numKnots; ++knot)
                moveTowards(knots[knot], knots[knot - 1]);

            visited.insert(tail);
        }
    }

    return int(visited.size());
}


void run_day9()
{
    string sample =
        R"(R 4
U 4
L 3
D 1
R 4
D 1
L 5
R 2)";
    string sample2 =
        R"(R 5
U 8
L 8
D 3
R 17
D 10
L 25
U 20)";

    test(13, day9(READ(sample)));
    gogogo(day9(LOAD(9)));

    test(1, day9_2(READ(sample)));
    test(36, day9_2(READ(sample2)));
    gogogo(day9_2(LOAD(9)));
}