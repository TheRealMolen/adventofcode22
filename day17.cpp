#include "pch.h"
#include "harness.h"

#include <execution>

struct Rock
{
    u8 shape[4];
};

constexpr Rock Rocks[] = {
    { 0x1e, 0 },                // -
    { 0x08, 0x1c, 0x08, 0 },    // +
    { 0x1c, 0x04, 0x04, 0 },    // mirror L
    { 0x10, 0x10, 0x10, 0x10 }, // |
    { 0x18, 0x18, 0 },          // o
};

using Field = vector<u8>;

int getRockSize(const Rock& rock)
{
    return int(ranges::count_if(rock.shape, [](u8 row) { return row != 0; }));
}

void drawField(const Field& field, u32 window = 20)
{
    char rowStr[16] = "|!!!!!!!|\n";
    for (u8 placed : field | views::reverse | views::take(window))
    {
        u8 mask = 0x40;
        for (u32 i = 1; i < 8; ++i, mask >>= 1)
            rowStr[i] = (placed & mask) ? '#' : ' ';
        cout << rowStr;
    }

    if (size(field) <= window)
        cout << "+-------+\n";

    cout << endl;
}

void drawRock(const Rock& rock)
{
    char rowStr[16] = "|!!!!!!!|\n";
    for (u8 placed : rock.shape | views::filter([](auto r) { return r != 0; }) | views::reverse)
    {
        u8 mask = 0x40;
        for (u32 i = 1; i < 8; ++i, mask >>= 1)
            rowStr[i] = (placed & mask) ? '@' : '.';
        cout << rowStr;
    }
    cout << endl;
}



bool isSettled(const Rock& rock, auto itField)
{
    for (u8 row : rock.shape)
    {
        if (row & *itField)
            return true;
        ++itField;
    }
    return false;
}

void blowRock(Rock& rock, char dir)
{
  //  cout << "blowing rock " << dir << dir << dir << "\n";

    u8 mush = accumulate(begin(rock.shape), end(rock.shape), u8(0), [](u8 acc, u8 row) { return u8(acc | row); });
    if (dir == '<')
    {
        if (!(mush & 0x40))
        {
            for (auto& row : rock.shape)
                row <<= 1;
        }
    }
    else
    {
        if (!(mush & 0x01))
        {
            for (auto& row : rock.shape)
                row >>= 1;
        }
    }
}

void tryBlowRock(Rock& rock, char dir, auto itField)
{
    Rock testRock = rock;
    blowRock(testRock, dir);
    if (!isSettled(testRock, itField))
        rock = testRock;
}

bool placeRock(const Rock& rock, auto itField)
{
    bool tetris = false;
    for (u8 row : rock.shape)
    {
        if (row == 0)
            break;

        *itField |= row;
        if (*itField == 0x7f)
            tetris = true;

        ++itField;
    }
    return tetris;
}

inline void trimField(Field& field)
{
    while (!field.empty() && field.back() == 0)
        field.pop_back();
}

u64 wipeTetris(Field& field)
{
    u64 numDeleted = 0;

    auto itTetris = find(rbegin(field), rend(field), 0x7f);
    if (itTetris != rend(field))
    {
        //       cout << "==== TETRIS ====\n";
        //       drawField(field, 1000);
        numDeleted = distance(begin(field), itTetris.base() - 1);
        field.erase(begin(field), itTetris.base() - 1);

        //       cout << "\n xxxxxxxxxxxxx \n";
        //       drawField(field, 1000);
    }
    return numDeleted;
}

struct Jets
{
    const string& pattern;
    string::const_iterator itNext;

    Jets(const string& input) : pattern(input), itNext(begin(pattern)) { /**/ }

    char operator()()
    {
        char jet = *itNext;
        ++itNext;
        if (itNext == end(pattern))
            itNext = begin(pattern);
        return jet;
    }
};

u64 day17(const string& jetPattern, u64 iterations)
{
    Jets jet(jetPattern);

    Field field;
    //field.reserve(64 * 1024);
    u64 tetrisedRows = 0;
    u64 nextMsg = 100'000'000ull;

    u32 nextRockIx = 0;
    for (u64 i = 0; i < iterations; ++i)
    {
        Rock rock = Rocks[nextRockIx];
        ++nextRockIx;
        if (nextRockIx >= size(Rocks))
            nextRockIx = 0;

        // preroll 3 jets to reach the top
        for (int j = 0; j < 3; ++j)
            blowRock(rock, jet());
        
        constexpr int Spacing = 4;
        int offset = -Spacing;
        field.resize(size(field) + Spacing);
        auto itField = end(field) + offset;
        for (;;)
        {
            tryBlowRock(rock, jet(), itField);
            if (itField == begin(field) || isSettled(rock, itField - 1))
                break;

            --offset;
            --itField;
        }

        if (placeRock(rock, itField))
            tetrisedRows += wipeTetris(field);

        trimField(field);

        if (field.size() + tetrisedRows > nextMsg)
        {
            cout << "reached " << ((100.0 * double(nextMsg)) / double(iterations)) << "%..." << endl;
            nextMsg += 100'000'000ull;
        }
    }

    return field.size() + tetrisedRows;
}

u64 day17_2(const string& input)
{
    constexpr u64 TargetCount = 1000000000000ull;
    //return day17(input, Target);

    u64 inputLoopPoint = size(input) * size(Rocks);

    vector<u64> heightsPerLoop(50);
    vector<u32> indices(heightsPerLoop.size() - 1);
    ranges::iota(indices, 1);
    for_each(execution::par_unseq, begin(indices), end(indices), [&heightsPerLoop,&input,inputLoopPoint](u32 i)
        { heightsPerLoop[i] = day17(input, inputLoopPoint * i); });

    u64 loopCount = 0;
    u64 loopSize = 0;
    u64 stableCount = 0;
    u64 stableSize = 0;
    for (u64 reps = 1; reps + 1 < size(heightsPerLoop) / 3; ++reps)
    {
        auto lo = heightsPerLoop[reps + 1];
        auto mid = heightsPerLoop[reps * 2 + 1];
        auto hi = heightsPerLoop[reps * 3 + 1];

        if (hi - mid == mid - lo)
        {
            loopCount = inputLoopPoint * reps;
            loopSize = hi - mid;

            stableCount = inputLoopPoint * (reps + 1);
            stableSize = heightsPerLoop[reps + 1];
            break;
        }
    }
    if (loopCount == 0)
        throw "didn't find a loop";

    const u64 headCount = stableCount;

    const u64 bulkRepeats = ((TargetCount - stableCount) / loopCount);
    const u64 bulkSize = bulkRepeats * loopSize;

    const u64 tailCount = TargetCount - (loopCount * bulkRepeats);
    const u64 headTailSize = day17(input, tailCount + headCount);

    return headTailSize + bulkSize - stableSize;
}


void run_day17()
{
    string sample = ">>><<><>><<<>><>>><<<>>><<<><<<>><>><<>>";
        
    test(3068u, day17(sample, 2022));
    gogogo(day17(LOADSTR(17), 2022), 3067ull);

    //test(1514285714288ull, day17_2(sample));
    // 1514364285023 is too low
    //gogogo(day17_2(LOADSTR(17)));
    skip("come back to this...");
}