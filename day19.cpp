#include "pch.h"
#include "harness.h"

#include <execution>


enum Resource { Ore, Clay, Obsidian, Geode, ResourceTypeCount };

struct Recipe
{

    int ore = 0;
    int clay = 0;
    int obsidian = 0;
};
struct Blueprint
{
    int id = 0;
    Recipe oreBot;
    Recipe clayBot;
    Recipe obsidianBot;
    Recipe geodeBot;

    explicit Blueprint(const string& line)
    {
        istringstream is(line);

        is >> "Blueprint " >> id >> ": "
            "Each ore robot costs " >> oreBot.ore >> " ore. "
            "Each clay robot costs " >> clayBot.ore >> " ore. "
            "Each obsidian robot costs " >> obsidianBot.ore >> " ore and " >> obsidianBot.clay >> " clay. "
            "Each geode robot costs " >> geodeBot.ore >> " ore and " >> geodeBot.obsidian >> " obsidian.";
    }
};

struct Inventory : Recipe
{
    int bots[ResourceTypeCount] = { 1,0,0,0 };
    
    int geodes = 0;
};


// plans are compressed strings of "the next bot to build"
struct BotPlan
{
    enum { MaxPlanLen = 28 };
    enum { PlanBytes = (MaxPlanLen + 3) / 4 };
    u8 plan[PlanBytes];
    u8 len = 0;

    void setStep(int step, Resource res)
    {
        const u8 byte = u8(step) / 4;
        const u8 shift = u8((step & 3) * 2);
        const u8 mask = u8(3 << shift);
        const u8 old = plan[byte];
        plan[byte] = u8((old & ~mask) | (res << shift));
        len = max(len, u8(step + 1));
    }
    Resource getStep(u32 step) const
    {
        if (step < len)
        {
            const u32 byte = step / 4;
            const u8 shift = u8((step & 3) * 2);
            return Resource((plan[byte] >> shift) & 3);
        }
        return ResourceTypeCount;
    }
};
using BotPlans = vector<BotPlan>;

ostream& operator<<(ostream& os, const BotPlan& plan)
{
    for (u32 step = 0; step < plan.len; ++step)
    {
        Resource r = plan.getStep(step);
        os << char('a' + r);
    }
    return os;
}


Resource tryBuildBot(const BotPlan& plan, u32 step, Inventory& inv, const Blueprint& bp)
{
    switch (plan.getStep(step))
    {
    case Ore:
        if (inv.ore >= bp.oreBot.ore)
        {
            inv.ore -= bp.oreBot.ore;
            return Ore;
        }
        break;

    case Clay:
        if (inv.ore >= bp.clayBot.ore)
        {
            inv.ore -= bp.clayBot.ore;
            return Clay;
        }
        break;

    case Obsidian:
        if (inv.ore >= bp.obsidianBot.ore && inv.clay >= bp.obsidianBot.clay)
        {
            inv.ore -= bp.obsidianBot.ore;
            inv.clay -= bp.obsidianBot.clay;
            return Obsidian;
        }
        break;

    case Geode:
        if (inv.ore >= bp.geodeBot.ore && inv.obsidian >= bp.geodeBot.obsidian)
        {
            inv.ore -= bp.geodeBot.ore;
            inv.obsidian -= bp.geodeBot.obsidian;
            return Geode;
        }
        break;
    }

    return ResourceTypeCount;
}

template<int MaxMinutes = 25>
int runPlan(const Blueprint& bp, const BotPlan& plan)
{
    Inventory inv;
    u32 step = 0;
    for (int minute = 1; minute < MaxMinutes; ++minute)
    {
        if (step < plan.len)
        {
            Resource newBot = tryBuildBot(plan, step, inv, bp);

            inv.ore += inv.bots[Ore];
            inv.clay += inv.bots[Clay];
            inv.obsidian += inv.bots[Obsidian];
            inv.geodes += inv.bots[Geode];

            if (u32(newBot) < ResourceTypeCount)
            {
                ++inv.bots[newBot];
                ++step;
            }
        }
        else
        {
            inv.geodes += inv.bots[Geode];
        }
    }

    return inv.geodes;
}


// last is always Geode
// no Obsidian before Clay
// no Geode before Obsidian
void generateRestOfPlan(BotPlan& curr, int i, int len, Resource maxBotBuilt, BotPlans& plans)
{
    if (i >= len - 1) [[unlikely]]
    {
        plans.push_back(curr);
    }
    else if (i >= len-2 && (maxBotBuilt < Obsidian))
    {
        curr.setStep(i, Obsidian);
        generateRestOfPlan(curr, i + 1, len, Obsidian, plans);
    }
    else if (i >= len-3 && (maxBotBuilt < Clay))
    {
        curr.setStep(i, Clay);
        generateRestOfPlan(curr, i + 1, len, Clay, plans);
    }
    else
    {
        if (i < len/2 && (maxBotBuilt < Obsidian))  // questionable
        {
            curr.setStep(i, Ore);
            generateRestOfPlan(curr, i + 1, len, maxBotBuilt, plans);
        }

        if (maxBotBuilt < Geode) // questionable
        {
            curr.setStep(i, Clay);
            generateRestOfPlan(curr, i + 1, len, max(Clay, maxBotBuilt), plans);
        }
            
        if (maxBotBuilt >= Clay)
        {
            curr.setStep(i, Obsidian);
            generateRestOfPlan(curr, i + 1, len, max(Obsidian, maxBotBuilt), plans);
        }

        if (maxBotBuilt >= Obsidian)
        {
            curr.setStep(i, Geode);
            generateRestOfPlan(curr, i + 1, len, Geode, plans);
        }
    }
}

template<int MaxLen>
vector<BotPlan> generateAllPlans()
{
    static_assert(MaxLen < BotPlan::MaxPlanLen);

    BotPlan curr = { 0 };
    vector<BotPlan> plans;
    plans.reserve(2'000'000'000);

    for (int len = 5; len <= MaxLen; ++len)
    {
        curr.setStep(len - 1, Geode);

        for (Resource res=Ore; res <= Clay; res = Resource(res + 1))
        {
            curr.setStep(0, res);
            generateRestOfPlan(curr, 1, len, res, plans);
        }
    }
    return plans;
}

template<int MaxMinutes = 25>
int findBestPlan(const Blueprint& bp, const BotPlans& plans)
{
    using GeodesPlanPair = pair<int, const BotPlan*>;
    auto countGeodes = [&bp](const BotPlan& plan) -> GeodesPlanPair
    {
        return { runPlan<MaxMinutes>(bp, plan), &plan };
    };
    auto reduceMaxGeodes = [](const GeodesPlanPair& acc, const GeodesPlanPair& val)
    {
        if (acc.first >= val.first)
            return acc;
        return val;
    };

    auto [maxGeodes,bestPlan] = transform_reduce(
        execution::par_unseq,
        begin(plans), end(plans),
        GeodesPlanPair{0, nullptr},
        reduceMaxGeodes,
        countGeodes);

    if (maxGeodes > 0)
        cout << "  BP " << bp.id << ": best result with plan '" << *bestPlan << "'\n";
    else
        cout << "  BP " << bp.id << ": no viable plan !!!\n";

    return maxGeodes;
}


int day19(const stringlist& input, const BotPlans& plans)
{
    int totalQuality = 0;
    for (auto& line : input)
    {
        Blueprint bp(line);
        totalQuality += (bp.id * findBestPlan(bp, plans));
    }

    return totalQuality;
}

int day19_2(const stringlist& input, const BotPlans& plans)
{
    int res = 1;
    for (auto& line : input | views::take(3))
    {
        Blueprint bp(line);
        res *= findBestPlan<33>(bp, plans);
    }

    return res;
}


void run_day19()
{
    string sample =
R"(Blueprint 1: Each ore robot costs 4 ore. Each clay robot costs 2 ore. Each obsidian robot costs 3 ore and 14 clay. Each geode robot costs 2 ore and 7 obsidian.
Blueprint 2: Each ore robot costs 2 ore. Each clay robot costs 3 ore. Each obsidian robot costs 3 ore and 8 clay. Each geode robot costs 3 ore and 12 obsidian.)";

    if constexpr (false)
    {
        {
            auto plans = generateAllPlans<18>();
            test(33, day19(READ(sample), plans));
            gogogo(day19(LOAD(19), plans), 1624);
        }
        {
            auto plans = generateAllPlans<25>();
            test(56, findBestPlan<33>(Blueprint("Blueprint 1: Each ore robot costs 4 ore. Each clay robot costs 2 ore. Each obsidian robot costs 3 ore and 14 clay. Each geode robot costs 2 ore and 7 obsidian."), plans));
            test(62, findBestPlan<33>(Blueprint("Blueprint 2: Each ore robot costs 2 ore. Each clay robot costs 3 ore. Each obsidian robot costs 3 ore and 8 clay. Each geode robot costs 3 ore and 12 obsidian."), plans));
            gogogo(day19_2(LOAD(19), plans));
        }
    }
    else
    {
        skip("too slow!");
        skip("too slow!");
    }
}