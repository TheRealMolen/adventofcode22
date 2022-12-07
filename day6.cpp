#include "pch.h"
#include "harness.h"


template<int N>
bool n_unique_chars(const char* s)
{
    for (auto a = s; a < s+(N-1); ++a)
    {
        for (auto b = a+1; b < s+N; ++b)
        {
            if (*a == *b)
                return false;
        }
    }
    return true;
}

int day6(const string& input)
{
    const char* start = input.data();
    const char* curr = start + 3;
    const char* end = start + input.size();
    for (; curr != end; ++curr)
    {
        if (n_unique_chars<4>(curr - 3))
            break;
    }

    return int(1 + curr - start);
}

int day6_2(const string& input)
{
    const char* start = input.data();
    const char* curr = start + 13;
    const char* end = start + input.size();
    for (; curr != end; ++curr)
    {
        if (n_unique_chars<14>(curr - 13))
            break;
    }

    return int(1 + curr - start);
}


void run_day6()
{
    test(7, day6("mjqjpqmgbljsphdztnvjfqwrcgsmlb"));
    test(5, day6("bvwbjplbgvbhsrlpgdmjqwftvncz"));
    test(6, day6("nppdvjthqldpwncqszvftbrmjlhg"));
    test(10, day6("nznrnfrfntjfmvfwmzdfjlvtqnbhcprsg"));
    test(11, day6("zcfzfwzzqfrljwzlrfnpqdbhtmscgvjw"));
    gogogo(day6(LOADSTR(6)));

    test(19, day6_2("mjqjpqmgbljsphdztnvjfqwrcgsmlb"));
    test(23, day6_2("bvwbjplbgvbhsrlpgdmjqwftvncz"));
    test(23, day6_2("nppdvjthqldpwncqszvftbrmjlhg"));
    test(29, day6_2("nznrnfrfntjfmvfwmzdfjlvtqnbhcprsg"));
    test(26, day6_2("zcfzfwzzqfrljwzlrfnpqdbhtmscgvjw"));
    gogogo(day6_2(LOADSTR(6)));
}