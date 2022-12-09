#include "pch.h"
#include "harness.h"

#include "chargrid.h"


vector<u8> findVisibleTrees(const stringlist& input)
{
    const u64 w = input.front().size();
    const u64 h = input.size();
    vector<u8> visible(w * h, 0);

    // rows
    fill(visible.begin(), visible.begin() + w, u8(1));
    fill(visible.end() - w, visible.end(), u8(1));
    for (u64 y = 1; (y + 1) < h; ++y)
    {
        auto& line = input[y];

        visible[y * w] = 1;
        visible[(y + 1) * w - 1] = 1;

        for (u64 x = 1; (x + 1) < w; ++x)
        {
            char tree = line[x];

            bool hiddenLeft = any_of(begin(line), begin(line) + x, [tree](char c) { return c >= tree; });
            bool hiddenRight = hiddenLeft ? any_of(begin(line) + (x + 1), end(line), [tree](char c) { return c >= tree; }) : false;

            visible[x + y * h] = !(hiddenLeft && hiddenRight);
        }
    }

    // cols
    for (u64 x = 1; (x + 1) < w; ++x)
    {
        for (u64 y = 1; (y + 1) < h; ++y)
        {
            if (visible[x + y * w])
                continue;

            char tree = input[y][x];

            bool hiddenTop = any_of(begin(input), begin(input) + y, [tree, x](auto& line) { return line[x] >= tree; });
            bool hiddenBottom = hiddenTop ? any_of(begin(input) + (y + 1), end(input), [tree, x](auto& line) { return line[x] >= tree; }) : false;

            visible[x + y * h] = !(hiddenTop && hiddenBottom);
        }
    }

    return visible;
}

u64 calcScenicScore(u64 treeX, u64 treeY, const stringlist& input, u64 w, u64 h)
{
    char tree = input[treeY][treeX];

    u64 distUp = 0;
    for (u64 y = treeY - 1; y < h; --y)
    {
        ++distUp;
        if (input[y][treeX] >= tree)
            break;
    }
    u64 distDn = 0;
    for (u64 y = treeY + 1; y < h; ++y)
    {
        ++distDn;
        if (input[y][treeX] >= tree)
            break;
    }
    u64 distLf = 0;
    for (u64 x = treeX - 1; x < w; --x)
    {
        ++distLf;
        if (input[treeY][x] >= tree)
            break;
    }
    u64 distRt = 0;
    for (u64 x = treeX + 1; x < w; ++x)
    {
        ++distRt;
        if (input[treeY][x] >= tree)
            break;
    }

    return distUp * distDn * distLf * distRt;
}



int day8(const stringlist& input)
{
    vector<u8> visible = findVisibleTrees(input);

    return accumulate(begin(visible), end(visible), 0);
}

u64 day8_2(const stringlist& input)
{
    const u64 w = input.front().size();
    const u64 h = input.size();

    u64 bestScore = 0;

    for (u64 y=0; y<h; ++y)
    {
        for (u64 x=0; x<w; ++x)
        {
            bestScore = max(bestScore, calcScenicScore(x, y, input, w, h));
        }
    }

    return bestScore;
}


void run_day8()
{
    string sample =
        R"(30373
25512
65332
33549
35390)";

    test(21, day8(READ(sample)));
    gogogo(day8(LOAD(8)));

    test(8, day8_2(READ(sample)));
    gogogo(day8_2(LOAD(8)));
}