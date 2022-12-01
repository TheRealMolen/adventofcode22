#include "pch.h"
#include "harness.h"


int day1(const stringlist& input)
{
    int currMax = -1;

    int curr = 0;
    for (auto& line : input)
    {
        if (line.empty() || &line == &input.back())
        {
            if (curr > currMax)
                currMax = curr;
            curr = 0;

            continue;
        }

        curr += std::stoi(line);
    }

    return currMax;
}


int64_t day1_2(const stringlist& input)
{
    vector<int> elves;
    int curr = 0;
    for (auto& line : input)
    {
        if (line.empty())
        {
            elves.push_back(curr);
            curr = 0;

            continue;
        }

        curr += std::stoi(line);
    }
    elves.push_back(curr);

    nth_element(begin(elves), begin(elves) + 3, end(elves), greater<int>());

    return accumulate(begin(elves), begin(elves) + 3, 0);
}


void run_day1()
{
    string testInput =
R"(1000
2000
3000

4000

5000
6000

7000
8000
9000

10000)";

    test(24000, day1(READ(testInput)));
    gogogo(day1(LOAD(1)));

    test(45000, day1_2(READ(testInput)));
    gogogo(day1_2(LOAD(1)));
}
