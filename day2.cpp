#include "pch.h"
#include "harness.h"


enum Move { Rok, Ppr, Siz };
constexpr int MoveScores[] = { 1, 2, 3 };
constexpr int DrawScore = 3;
constexpr int WinScore = 6;


bool doIWin(Move myMove, Move theirMove)
{
    return (myMove == ((theirMove + 1) % 3));
}


int day2(const stringlist& input)
{
    int myScore = 0;

    for (auto& line : input)
    {
        char theirC = line[0];
        char myC = line[2];

        Move theirMove = Move(theirC - 'A');
        Move myMove = Move(myC - 'X');

        myScore += MoveScores[myMove];

        if (myMove == theirMove)
            myScore += DrawScore;
        else if (doIWin(myMove, theirMove))
            myScore += WinScore;
    }

    return myScore;
}

int day2_2(const stringlist& input)
{
    int myScore = 0;

    for (auto& line : input)
    {
        char theirC = line[0];
        char myResult = line[2];

        Move theirMove = Move(theirC - 'A');
        Move myMove = theirMove;
        switch(myResult)
        {
        case 'X':   // lose
            myMove = Move((theirMove + 2) % 3);
            break;
        case 'Z':   // win
            myMove = Move((theirMove + 1) % 3);
            break;
        }

        myScore += MoveScores[myMove];

        if (myMove == theirMove)
            myScore += DrawScore;
        else if (doIWin(myMove, theirMove))
            myScore += WinScore;
    }

    return myScore;
}


void run_day2()
{
    string sample =
R"(A Y
B X
C Z)";

    test(15, day2(READ(sample)));
    gogogo(day2(LOAD(2)));

    test(12, day2_2(READ(sample)));
    gogogo(day2_2(LOAD(2)));
}