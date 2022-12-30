#include "pch.h"
#include "harness.h"

union Char4
{
    u32 asU32;
    char asChars[4];
};
u32 u32FromStr(const char* s)
{
    Char4 c4;
    copy(s, s + 4, c4.asChars);
    return c4.asU32;
}

const u32 RootName = u32FromStr("root");
const u32 HumanName = u32FromStr("humn");

struct NMonkey
{
    i64 val=-100000;
    u32 name=0;
    u32 monkA=0, monkB=0;
    char op = '!';
    bool resolved = false;
    NMonkey* a = nullptr;
    NMonkey* b = nullptr;

    NMonkey(const string& str)
    {
        name = u32FromStr(str.data());

        if (isdigit(str[6]))
        {
            resolved = true;
            val = atoi(str.data() + 6);
        }
        else
        {
            monkA = u32FromStr(str.data() + 6);
            monkB = u32FromStr(str.data() + 13);
            op = str[11];
        }
    }
};

i64 doOp(i64 a, char op, i64 b)
{
    switch (op)
    {
    case '+': return a + b;
    case '-': return a - b;
    case '*': return a * b;
    case '/': return a / b;
    }
    return numeric_limits<i64>::max();
}

i64 day21(const stringlist& input)
{
    unordered_map<u32, i64> resolved;
    deque<NMonkey> unresolved;
    for (auto& line : input)
    {
        NMonkey m(line);
        if (m.resolved)
            resolved.try_emplace(m.name, m.val);
        else
            unresolved.push_back(m);
    }

    while (!unresolved.empty())
    {
        for (auto it=begin(unresolved); it!=end(unresolved); /**/)
        {
            if (resolved.contains(it->monkA) && resolved.contains(it->monkB))
            {
                i64 val = doOp(resolved[it->monkA], it->op, resolved[it->monkB]);
                if (it->name == RootName) [[unlikely]]
                    return val;

                resolved.try_emplace(it->name, val);
                it = unresolved.erase(it);
            }
            else
                ++it;
        }
    }
    return -1;
}

bool tryResolve(NMonkey* m, auto&& findMonkey)
{
    if (m->resolved)
        return true;
    if (m->name == HumanName)
        return false;

    bool aIsResolved = m->a->resolved;
    if (!aIsResolved)
        aIsResolved = tryResolve(m->a, findMonkey);

    bool bIsResolved = m->b->resolved;
    if (!bIsResolved)
        bIsResolved = tryResolve(m->b, findMonkey);

    if (aIsResolved && bIsResolved)
    {
        m->val = doOp(m->a->val, m->op, m->b->val);
        m->resolved = true;
        return true;
    }

    return false;
}

i64 solveHumanChain(NMonkey* m, i64 target)
{
    if (m->name == HumanName)
        return target;

    if (m->a->resolved)
    {
        i64 newTarget = -999999;
        switch (m->op)
        {
        case '+':    newTarget = target - m->a->val; break;
        case '-':    newTarget = m->a->val - target; break;
        case '*':    newTarget = target / m->a->val; break;
        case '/':    newTarget = target / m->a->val; break;
        }
        return solveHumanChain(m->b, newTarget);
    }

    i64 newTarget = -999999;
    switch (m->op)
    {
    case '+':    newTarget = target - m->b->val; break;
    case '-':    newTarget = target + m->b->val; break;
    case '*':    newTarget = target / m->b->val; break;
    case '/':    newTarget = target * m->b->val; break;
    }
    return solveHumanChain(m->a, newTarget);
}

i64 day21_2(const stringlist& input)
{
    vector<NMonkey> monkeys;
    monkeys.reserve(input.size());
    ranges::transform(input, back_inserter(monkeys), [](auto& s) { return NMonkey(s); });
    auto findMonkey = [&monkeys](u32 name) -> NMonkey* { return &*ranges::find(monkeys, name, &NMonkey::name); };

    for (auto& m : monkeys | views::filter([](auto& m) { return !m.resolved; }))
    {
        m.a = findMonkey(m.monkA);
        m.b = findMonkey(m.monkB);
    }

    NMonkey* root = findMonkey(RootName);
    NMonkey* human = findMonkey(HumanName);
    human->resolved = false;

    tryResolve(root->a, findMonkey);
    bool aIsHuman = tryResolve(root->b, findMonkey);

    auto humanChain = aIsHuman ? root->a : root->b;
    i64 target = aIsHuman ? root->b->val : root->a->val;

    return solveHumanChain(humanChain, target);;
}


void run_day21()
{
    string sample =
R"(root: pppw + sjmn
dbpl: 5
cczh: sllz + lgvd
zczc: 2
ptdq: humn - dvpt
dvpt: 3
lfqf: 4
humn: 5
ljgn: 2
sjmn: drzm * dbpl
sllz: 4
pppw: cczh / lfqf
lgvd: ljgn * ptdq
drzm: hmdt - zczc
hmdt: 32)";

    test(152ll, day21(READ(sample)));
    gogogo(day21(LOAD(21)));

    test(301, day21_2(READ(sample)));
    gogogo(day21_2(LOAD(21)));
}