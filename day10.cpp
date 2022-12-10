#include "pch.h"
#include "harness.h"


constexpr u32 NumCols = 40;
constexpr u32 NumRows = 6;


class CrtCpu
{
public:
    explicit CrtCpu(i32 initialClock=0) : m_nextVsync(initialClock)
    {
        m_display.reserve((NumCols+1) * NumRows + 20);
        m_display += "\n";  // for cleaner output
    }

    i64 run(const stringlist& program);

    const string& getDisplay() const { return m_display; }

private:
    void startCycle();


    u32 m_cycles = 0;
    u32 m_nextVsync;
    u32 m_rowPos = 0;

    i64 m_x = 1;

    i64 m_signalStrengthSum = 0;

    string m_display;
};


bool isSpriteOnColumn(i64 x, u32 col)
{
    if (x > col+1)
        return false;
    if (x+2 < col+1)
        return false;
    return true;
}

void CrtCpu::startCycle()
{
    ++m_cycles;

    const bool isOnFirstFrame = m_nextVsync <= (NumRows * NumCols);
    if (isOnFirstFrame)
    {
        m_display += (isSpriteOnColumn(m_x, m_rowPos) ? '#' : ' ');
        ++m_rowPos;

        if (m_cycles >= m_nextVsync)
        {
            i64 signalStrength = m_x * m_nextVsync;
            m_signalStrengthSum += signalStrength;
            m_nextVsync += NumCols;
            m_display += "\n";
            m_rowPos = 0;
        }
    }
}


i64 CrtCpu::run(const stringlist& program)
{
    for (auto& line : program)
    {
        startCycle();

        if (line[0] == 'n')         // noop
        {
        }
        else if (line[0] == 'a')    // addx
        {
            startCycle();
            m_x += strtoll(line.data() + 5, nullptr, 10);
        }
        else
            throw "bad instr";
    }

    startCycle();

    return m_signalStrengthSum;
}



i64 day10(const stringlist& input)
{
    CrtCpu cpu(20);
    i64 sum = cpu.run(input);

    return sum;
}

string day10_2(const stringlist& input)
{
    CrtCpu cpu(40);
    cpu.run(input);

    return cpu.getDisplay();
}


void run_day10()
{
    test(13140, day10(LOAD(10t)));
    gogogo(day10(LOAD(10)), 14620ll);

    string testOutput =
R"(
##  ##  ##  ##  ##  ##  ##  ##  ##  ##  
###   ###   ###   ###   ###   ###   ### 
####    ####    ####    ####    ####    
#####     #####     #####     #####     
######      ######      ######      ####
#######       #######       #######     
)";
    test(testOutput, day10_2(LOAD(10t)));
    gogogo(day10_2(LOAD(10)));
}