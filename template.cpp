#include "pch.h"
#include "harness.h"


int dayN(const stringlist& input)
{
    for (auto& line : input)
    {
        (void)line;
    }

    return -1;
}

int dayN_2(const stringlist& input)
{
    for (auto& line : input)
    {
        (void)line;
    }

    return -1;
}


void run_dayN()
{
    string sample =
R"(beep
boop
blarp)";

    test(-100, dayN(READ(sample)));
    //gogogo(dayN(LOAD(N)));

    //test(-100, dayN_2(READ(sample)));
    //gogogo(dayN_2(LOAD(N)));
}