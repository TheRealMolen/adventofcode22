#include "pch.h"
#include "harness.h"


int day20(const stringlist& input)
{
    using El = pair<i16, u16>;
    vector<El> file;
    file.reserve(input.size());
    u16 ogix = 0;
    ranges::transform(input, back_inserter(file), [&ogix](const string& line) { return El{i16(stoi(line)), ogix++}; });

    const u16 numEls = ogix;
    auto itBegin = begin(file);
    for (u16 ix = 0; ix<numEls; ++ix)
    {
        auto itEl = ranges::find_if(file, [ix](auto& el) { return el.second == ix; });
        const u32 currLoc = u32(distance(begin(file), itEl));
        // note: -1 because we jump from N-1=>1 without hitting 0
        const u32 val = ((10 * (numEls-1)) + itEl->first) % (numEls-1);   // make sure we're always moving to the right
        u32 newLoc = currLoc + val;   
        newLoc = newLoc % (numEls - 1);

        if (val && newLoc == 0)
            newLoc = numEls - 1;

        if (newLoc > currLoc)
            rotate(itEl, itEl + 1, itBegin + (newLoc + 1));
        else if (newLoc < currLoc)
            rotate(itBegin + newLoc, itEl, itEl + 1);
    }

    auto itZero = ranges::find_if(file, [](auto& el) { return el.first == 0; });
    u32 zeroLoc = u32(distance(begin(file), itZero));
    auto el = [=,&file](int ix) -> int
    {
        int v = file[(ix + zeroLoc) % numEls].first;
        return v;
    };

    return (el(1000) + el(2000) + el(3000));
}


i64 day20_2(const stringlist& input)
{
    using El = pair<i16, u16>;
    vector<El> file;
    file.reserve(input.size());
    u16 ogix = 0;
    ranges::transform(input, back_inserter(file), [&ogix](const string& line) { return El{ i16(stoi(line)), ogix++ }; });

    const u16 numEls = ogix;
    constexpr i64 Key = 811589153;
    const i64 lcd = Key * (numEls - 1);
    auto itBegin = begin(file);
    for (int rep = 0; rep < 10; ++rep)
    {
        for (u16 ix = 0; ix < numEls; ++ix)
        {
            auto itEl = ranges::find_if(file, [ix](auto& el) { return el.second == ix; });
            const u32 currLoc = u32(distance(begin(file), itEl));
            // note: -1 because we jump from N-1=>1 without hitting 0
            const u32 val = i64(2 * lcd + itEl->first*Key) % i64(numEls - 1);   // make sure we're always moving to the right
            u32 newLoc = currLoc + val;
            newLoc = newLoc % (numEls - 1);

            if (val && newLoc == 0)
                newLoc = numEls - 1;

            if (newLoc > currLoc)
                rotate(itEl, itEl + 1, itBegin + (newLoc + 1));
            else if (newLoc < currLoc)
                rotate(itBegin + newLoc, itEl, itEl + 1);
        }
    }

    auto itZero = ranges::find_if(file, [](auto& el) { return el.first == 0; });
    u32 zeroLoc = u32(distance(begin(file), itZero));
    auto el = [=, &file](int ix) -> i64
    {
        return i64(file[(ix + zeroLoc) % numEls].first) * Key;
    };

    return (el(1000) + el(2000) + el(3000));
}


void run_day20()
{
    string sample =
R"(1
2
-3
3
-2
0
4)";

    test(3, day20(READ(sample)));
    nononoD(day20(LOAD(20)));

    test(1623178306ll, day20_2(READ(sample)));
    nononoD(day20_2(LOAD(20)));
}