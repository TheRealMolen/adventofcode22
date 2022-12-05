#include "pch.h"
#include "harness.h"


using Stack = vector<char>;
using Stacks = vector<Stack>;

Stacks readInitialStacks(const stringlist& input)
{
    // every line has enough whitespace for all stacks; use that to figure numstacks
    const size_t numStacks = (input.front().size() + 1) / 4;

    Stacks stax(numStacks);

    for (auto& line : input)
    {
        if (line.find('1') != string::npos)
            break;

        for (size_t s = 0; s < numStacks; ++s)
        {
            char c = line[s * 4 + 1];
            if (c == ' ')
                continue;

            stax[s].insert(begin(stax[s]), c);
        }
    }

    return stax;
}


struct CraneRule
{
    uint8_t count;
    uint8_t from;
    uint8_t to;
};
using CraneRules = vector<CraneRule>;

CraneRules readCraneRules(const stringlist& input)
{
    CraneRules rules;
    for (auto& line : input)
    {
        if (line[0] != 'm')
            continue;

        CraneRule rule;
        istringstream is(line);
        is >> "move " >> rule.count >> " from " >> rule.from >> " to " >> rule.to;
    }

    return rules;
}




int day5(const stringlist& input)
{
    Stacks stax = readInitialStacks(input);
    CraneRules rules = readCraneRules(input);

    // read rules

    return -1;
}

int day5_2(const stringlist& input)
{
    for (auto& line : input)
    {
        (void)line;
    }

    return -1;
}


void run_day5()
{
    string sample =
        R"(    [D]    
[N] [C]    
[Z] [M] [P]
 1   2   3 

move 1 from 2 to 1
move 3 from 1 to 3
move 2 from 2 to 1
move 1 from 1 to 2)";

    test(-100, day5(READ(sample)));
    //gogogo(day5(LOAD(5)));

    //test(-100, day5_2(READ(sample)));
    //gogogo(day5_2(LOAD(5)));
}