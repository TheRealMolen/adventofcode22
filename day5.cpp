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

void execute9000(const CraneRule& rule, Stacks& stax)
{
    for (uint32_t i = 0; i < rule.count; ++i)
    {
        stax[rule.to].push_back(stax[rule.from].back());
        stax[rule.from].pop_back();
    }
}

void execute9001(const CraneRule& rule, Stacks& stax)
{
    auto& from = stax[rule.from];
    auto& to = stax[rule.to];
    auto fromBegin = end(from) - rule.count;
    copy(fromBegin, end(from), back_inserter(to));
    from.erase(fromBegin, end(from));
}

CraneRules readCraneRules(const stringlist& input)
{
    CraneRules rules;
    for (auto& line : input)
    {
        if (line[0] != 'm')
            continue;

        int count, from, to;
        istringstream is(line);
        is >> "move " >> count >> " from " >> from >> " to " >> to;

        rules.emplace_back(uint8_t(count), uint8_t(from-1), uint8_t(to-1));
    }

    return rules;
}

string day5(const stringlist& input)
{
    Stacks stax = readInitialStacks(input);
    CraneRules rules = readCraneRules(input);

    for (auto& rule : rules)
        execute9000(rule, stax);

    string tops;
    for (auto& stack : stax)
        tops += stack.back();

    return tops;
}

string day5_2(const stringlist& input)
{
    Stacks stax = readInitialStacks(input);
    CraneRules rules = readCraneRules(input);

    for (auto& rule : rules)
        execute9001(rule, stax);

    string tops;
    for (auto& stack : stax)
        tops += stack.back();

    return tops;
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

    test("CMZ", day5(READ(sample)));
    gogogo(day5(LOAD(5)));

    test("MCD", day5_2(READ(sample)));
    gogogo(day5_2(LOAD(5)));
}