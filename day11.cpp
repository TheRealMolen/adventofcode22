#include "pch.h"
#include "harness.h"

#define CHECKED_RINGBUFFERx


template<typename T, size_t N>
class fixed_ring_buffer
{
public:
    using value_type = T;
    static constexpr size_t capacity = N;

    const value_type& front() const
    {
#ifdef CHECKED_RINGBUFFER
        if (m_front == m_back) [[unlikely]]
            throw "can't read from an empty ringbuf";
#endif

        return m_buf[m_front];
    }

    [[nodiscard]] bool empty() const
    {
        return m_front == m_back;
    }


    void push_back(const value_type& val)
    {
#ifdef CHECKED_RINGBUFFER
        if (m_front == ((m_back + 1) % capacity)) [[unlikely]]
            throw "can't push onto a full ringbuf";
#endif

        m_buf[m_back] = val;
        m_back = (m_back + 1) % capacity;
    }

    void pop_front()
    {
#ifdef CHECKED_RINGBUFFER
        if (m_front == m_back) [[unlikely]]
            throw "can't pop from an empty ringbuf";
#endif

        m_front = (m_front + 1) % capacity;
    }

private:
    value_type m_buf[capacity];
    u32 m_back = 0;
    u32 m_front = 0;
};



struct Monkey
{
    static Monkey parse(stringlist::const_iterator& itLine, stringlist::const_iterator itEnd);

    fixed_ring_buffer<i64, 128> items;
    u32 business = 0;

    int id = -1;

    int opAdd = 0;
    int opMul = 1;
    bool opSquare = false;

    int testMod = -1;
    int trueMonkey = -1;
    int falseMonkey = -1;
};



Monkey Monkey::parse(stringlist::const_iterator& itLine, stringlist::const_iterator itEnd)
{
    // skip to the Monkey def
    for (; itLine != itEnd; ++itLine)
        if (itLine->length() > 0 && itLine->at(0) == 'M')
            break;

    if (itLine == itEnd)
        throw "lost a monkey";

    Monkey m;

    m.id = atoi(itLine->c_str() + 7);
    ++itLine;

    auto items = split({ itLine->c_str() + strlen("  Starting items: ") }, ", ");
    ranges::transform(items, back_inserter(m.items), [](auto& s) -> int { return stoi(s); });
    ++itLine;

    string op = itLine->substr(strlen("  Operation: new = "));
    if (op == "old * old")
        m.opSquare = true;
    else if (op[4] == '*')
        m.opMul = atoi(op.c_str() + 6);
    else if (op[4] == '+')
        m.opAdd = atoi(op.c_str() + 6);
    else
        throw "bad op";
    ++itLine;

    m.testMod = atoi(itLine->c_str() + strlen("  Test: divisible by "));
    ++itLine;
    m.trueMonkey = atoi(itLine->c_str() + strlen("    If true: throw to monkey "));
    ++itLine;
    m.falseMonkey = atoi(itLine->c_str() + strlen("    If false: throw to monkey "));
    ++itLine;

    return m;
}



int day11(const stringlist& input)
{
    vector<Monkey> monkeys;
    for (auto itLine = begin(input); itLine != end(input); /**/)
        monkeys.emplace_back(move(Monkey::parse(itLine, end(input))));

    constexpr int numRounds = 20;
    for (int i=0; i<numRounds; ++i)
    {
        for (Monkey& m : monkeys)
        {
            while (!m.items.empty())
            {
                ++m.business;
                i64 currentWorry = m.items.front();

                if (m.opSquare)
                    currentWorry *= currentWorry;
                currentWorry += m.opAdd;
                currentWorry *= m.opMul;

                currentWorry /= 3;

                int destMonkey = m.falseMonkey;
                if (currentWorry % m.testMod == 0)
                    destMonkey = m.trueMonkey;

                monkeys[destMonkey].items.push_back(currentWorry);
                m.items.pop_front();
            }
        }
    }

    vector<u32> business;
    ranges::transform(monkeys, back_inserter(business), &Monkey::business);
    ranges::sort(business, greater<u32>());

    return business[0] * business[1];
}

i64 day11_2(const stringlist& input)
{
    vector<Monkey> monkeys;
    for (auto itLine = begin(input); itLine != end(input); /**/)
        monkeys.emplace_back(move(Monkey::parse(itLine, end(input))));

    i64 commonMod = accumulate(begin(monkeys), end(monkeys), 1ll, [](i64 acc, auto& m) { return acc * m.testMod; });

    constexpr int numRounds = 10000;
    for (int i = 0; i < numRounds; ++i)
    {
        for (Monkey& m : monkeys)
        {
            while (!m.items.empty())
            {
                ++m.business;
                i64 currentWorry = m.items.front();

                if (m.opSquare)
                    currentWorry *= currentWorry;
                currentWorry += m.opAdd;
                currentWorry *= m.opMul;

                currentWorry %= commonMod;

                int destMonkey = m.falseMonkey;
                if (currentWorry % m.testMod == 0)
                    destMonkey = m.trueMonkey;

                monkeys[destMonkey].items.push_back(currentWorry);
                m.items.pop_front();
            }
        }
    }

    vector<i64> business;
    ranges::transform(monkeys, back_inserter(business), &Monkey::business);
    ranges::sort(business, greater<i64>());

    return business[0] * business[1];
}


void run_day11()
{
    test(10605, day11(LOAD(11t)));
    gogogo(day11(LOAD(11)));

    test(2713310158ll, day11_2(LOAD(11t)));
    gogogo(day11_2(LOAD(11)), 15310845153ll);
}