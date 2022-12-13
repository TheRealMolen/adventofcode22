#include "pch.h"
#include "harness.h"


struct PacketVal
{
    int i = -1;
    vector<PacketVal> v;

    bool isInt() const { return i >= 0; }
};


PacketVal parseInt(const char*& s)
{
    char* endptr = nullptr;
    int i = strtol(s, &endptr, 10);
    s = endptr;

    return PacketVal{ i };
}

PacketVal parseList(const char*& s)
{
    auto has = [&](char c) -> bool
    {
        return *s == c;
    };
    auto expect = [&](char expected)
    {
        if (*s != expected) [[unlikely]]
            throw "invalid input";
        ++s;
    };
    auto eat = [&](char c)
    {
        if (*s == c)
            ++s;
    };

    PacketVal list;

    expect('[');

    while (!has(']'))
    {
        if (has('['))
            list.v.emplace_back(parseList(s));
        else
            list.v.emplace_back(parseInt(s));

        eat(',');
    }

    expect(']');

    return list;
}

PacketVal parsePacket(const string& s)
{
    const char* str = s.c_str();
    return parseList(str);
}


bool operator<(const PacketVal& l, const PacketVal& r)
{
    if (l.isInt() && r.isInt())
        return l.i < r.i;

    vector<PacketVal> dummy;
    const vector<PacketVal>& lv = l.isInt() ? dummy : l.v;
    const vector<PacketVal>& rv = r.isInt() ? dummy : r.v;
    if (l.isInt())
        dummy.push_back({ l.i });
    else if (r.isInt())
        dummy.push_back({ r.i });

    for (auto itL = begin(lv), itR = begin(rv); itL != end(lv) && itR != end(rv); ++itL, ++itR)
    {
        const bool lLTr = (*itL < *itR);
        const bool equal = !lLTr && !(*itR < *itL);
        if (!equal)
            return lLTr;
    }

    if (rv.size() > lv.size())
        return true;

    return false;
}

bool operator==(const PacketVal& l, const PacketVal& r)
{
    return !(l < r) && !(r < l);
}


int day13(const stringlist& input)
{
    int correctSum = 0;
    int pairIx = 1;
    for (auto itLine = begin(input); itLine != end(input); ++itLine)
    {
        if (itLine->empty())
            continue;

        auto pak1 = parsePacket(*itLine);
        ++itLine;
        auto pak2 = parsePacket(*itLine);

        if (pak1 < pak2)
            correctSum += pairIx;

        ++pairIx;
    }

    return correctSum;
}

int day13_2(const stringlist& input)
{
    auto dividerA = parsePacket("[[2]]");
    auto dividerB = parsePacket("[[6]]");

    vector<PacketVal> packets = { dividerA, dividerB };
    for (auto& line : input)
    {
        if (line.empty())
            continue;
        packets.emplace_back(parsePacket(line));
    }

    ranges::sort(packets, less<PacketVal>());

    auto itA = ranges::find(packets, dividerA);
    auto itB = ranges::find(packets, dividerB);

    auto distA = 1 + distance(begin(packets), itA);
    auto distB = 1 + distance(begin(packets), itB);

    return int(distA * distB);
}


void run_day13()
{
    test(13, day13(LOAD(13t)));
    gogogo(day13(LOAD(13)));

    test(140, day13_2(LOAD(13t)));
    gogogo(day13_2(LOAD(13)));
}