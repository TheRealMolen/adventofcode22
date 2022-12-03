#include "pch.h"
#include "harness.h"
#include <string_view>

int day3(const stringlist& input)
{
    int count = 0;

    for (auto& line : input)
    {
        auto itemsPerCompartment = line.length() / 2;
        auto comp2It = begin(line) + itemsPerCompartment;
        auto commonItem = find_first_of(begin(line), comp2It, comp2It, end(line));
        char common = *commonItem;

        if (common <= 'Z')
            count += (common - 'A' + 27);
        else
            count += (common - 'a' + 1);
    }

    return count;
}

int day3_2(const stringlist& input)
{
    int count = 0;

    for (auto it = begin(input); it != end(input); it+=3)
    {
        auto& elf1 = *it;
        auto& elf2 = *(it+1);
        auto& elf3 = *(it+2);

        char common = '!';
        for (char c1 : elf1)
        {
            if (elf2.find(c1) == string::npos)
                continue;
            if (elf3.find(c1) == string::npos)
                continue;

            common = c1;
            break;
        }

        if (common <= 'Z')
            count += (common - 'A' + 27);
        else
            count += (common - 'a' + 1);
    }

    return count;
}


void run_day3()
{
    string sample =
        R"(vJrwpWtwJgWrhcsFMMfFFhFp
jqHRNqRjqzjGDLGLrsFMfFZSrLrFZsSL
PmmdzqPrVvPwwTWBwg
wMqvLMZHhHMvwLHjbvcjnnSBnvTQFn
ttgJtRGJQctTZtZT
CrZsJsPPZsGzwwsLwLmpwMDw)";

    test(157, day3(READ(sample)));
    gogogo(day3(LOAD(3)));

    test(70, day3_2(READ(sample)));
    gogogo(day3_2(LOAD(3)));
}