#include "pch.h"
#include "harness.h"


using ValveIx = u8;

struct Valve
{
    int rate;
    vector<ValveIx> neighbours;
    string name;
    vector<string> tunnelNames;
};

struct ValveEdge    // just one way; a tunnel is a pair of these
{
    ValveIx end;
    u8 cost;
};
struct ValveNode
{
    u16 rate;
    string name;
    vector<u8> costToValve;
};
struct ValveGraph
{
    vector<ValveNode> nodes;
    u32 maxFlowRate = 0;
    u16 allValvesMask = 0;
};

vector<Valve> loadValves(const stringlist& input)
{
    vector<Valve> valves;
    valves.reserve(input.size());

    for (auto& line : input)
    {
        istringstream is(line);
        Valve v;
        is >> "Valve " >> v.name >> "has flow rate=" >> v.rate;

        while (!isupper(is.peek()))
            is.get();
        string next;
        for (;;)
        {
            is >> skipws >> setw(2) >> next;
            v.tunnelNames.push_back(next);
            while (is.peek() == ',')
                is.get();

            if (is.peek() == istringstream::traits_type::eof())
                break;
        }
        
        valves.push_back(move(v));
    }

    ranges::sort(valves, {}, &Valve::name);

    for (auto& v : valves)
    {
        v.neighbours.reserve(size(v.tunnelNames));
        for (auto& neighName : v.tunnelNames)
        {
            auto itNeigh = ranges::find(valves, neighName, &Valve::name);
            v.neighbours.push_back(ValveIx(distance(begin(valves), itNeigh)));
        }
    }

    return valves;
}

ValveIx findValveIx(const string& name, const vector<Valve>& valves)
{
    auto itValve = ranges::find(valves, name, &Valve::name);
    return ValveIx(distance(begin(valves), itValve));
};

u8 findMinCost(const string& aName, const string& bName, const vector<Valve>& valves)
{
    constexpr auto MaxCost = numeric_limits<u8>::max();

    ValveIx aIx = findValveIx(aName, valves);
    ValveIx bIx = findValveIx(bName, valves);

    vector<u8> best(size(valves), MaxCost);
    best[aIx] = 0;

    vector<ValveIx> prev(size(valves), numeric_limits<ValveIx>::max());
    vector<ValveIx> open(size(valves));
    ranges::iota(open, ValveIx(0));

    while(!empty(open))
    {
        auto itCurrIx = ranges::min_element(open, [&best](ValveIx a, ValveIx b) { return best[a] < best[b]; });
        ValveIx currIx = *itCurrIx;
        if (currIx == bIx)
            return best[currIx];

        erase_unsorted(open, itCurrIx);

        const Valve& curr = valves[currIx];

        for (ValveIx neighIx : curr.neighbours)
        {
            if (ranges::find(open, neighIx) == end(open))
                continue;

            u8 newCost = best[currIx] + 1;
            if (newCost < best[neighIx])
            {
                best[neighIx] = newCost;
                prev[neighIx] = currIx;
            }
        }
    }

    throw "no path";
}

ValveGraph optimiseTunnels(const vector<Valve>& valves)
{
    ValveGraph g;
    auto findNodeIx = [&g](const string& name) -> ValveIx { return ValveIx(distance(begin(g.nodes), ranges::find(g.nodes, name, &ValveNode::name))); };
    auto isRoom = [](const Valve& v) { return v.rate > 0 || v.name == "AA"; };

    for (auto& v : valves | views::filter(isRoom))
        g.nodes.push_back({ .rate = u16(v.rate), .name = v.name });

    for (auto& v : g.nodes)
        v.costToValve.resize(g.nodes.size(), 0);

    // find min cost btw each pair of rooms
    for (auto itA=begin(g.nodes); (itA+1) != end(g.nodes); ++itA)
    {
        for (auto itB = itA+1; itB != end(g.nodes); ++itB)
        {
            u8 cost = findMinCost(itA->name, itB->name, valves);
            itA->costToValve[findNodeIx(itB->name)] = cost;
            itB->costToValve[findNodeIx(itA->name)] = cost;
        }
    }

    g.maxFlowRate = accumulate(begin(g.nodes), end(g.nodes), 0u, [](u32 acc, auto& v) { return acc + v.rate; });
    g.allValvesMask = u16((1 << size(g.nodes)) - 1);

    return g;
}


struct GraphState
{
    const ValveGraph& graph;

    u32 totalReleased = 0;
    u16 currentFlowRate = 0;
    u16 valvesOpen = 1;
    i8 minute = 1;
    ValveIx location = 0;
};


ostream& operator<<(ostream& os, const GraphState& s)
{
    os << "== Minute " << int(s.minute) << " (" << s.graph.nodes[s.location].name << ") ==\n";
    os << "  Valves open ";
    if (s.valvesOpen)
    {
        for (u32 i=0, mask=1; i<16; ++i, mask <<= 1)
        {
            if (s.valvesOpen & mask)
                os << s.graph.nodes[i].name << " ";
        }
    }
    else
    {
        os << "none ";
    }

    os << ", releasing " << s.currentFlowRate << " pressure. " << s.totalReleased << " released before now.\n";

    return os;
}


bool tick(GraphState& state, int count, u32& mostReleased)
{
    if (count == 0) [[unlikely]]
        throw "hm";

    // is there any point?
    int remaining = 31 - state.minute;
    int thisFlowTime = min(count, remaining);
    u32 thisFlow = thisFlowTime * state.currentFlowRate;
    int finalFlowTime = max(0, 31 - (state.minute + thisFlowTime));
    int finalFlow = finalFlowTime * state.graph.maxFlowRate;
    if (state.totalReleased + thisFlow + finalFlow < mostReleased)
    {
        state.minute = 31;
        return false;
    }

    state.totalReleased += thisFlow;
    state.minute += i8(thisFlowTime);

    if (state.minute > 30)
    {
        if (state.totalReleased > mostReleased)
        {
            mostReleased = state.totalReleased;
//            cout << "found new max release of " << mostReleased << endl;
        }
        return false;
    }
    return true;
}


void takeNextAction(const GraphState& state, u32& mostReleased);

inline void tryOpeningValve(const GraphState& state, u32& mostReleased)
{
    const ValveNode& room = state.graph.nodes[state.location];

    const u16 roomMask = 1 << state.location;
    if (room.rate > 0 && !(state.valvesOpen & roomMask))
    {
        GraphState newState = state;
        if (tick(newState, 1, mostReleased))
        {
            newState.valvesOpen |= roomMask;
            newState.currentFlowRate += room.rate;
            takeNextAction(newState, mostReleased);
        }
    }
}

inline void tryMovingToRoom(const GraphState& state, ValveIx dest, u32& mostReleased)
{
    const ValveNode& room = state.graph.nodes[state.location];
    u8 cost = room.costToValve[dest];

    if (state.minute + cost > 30)
        return;

    GraphState newState = state;
    if (!tick(newState, cost, mostReleased))
        return; // this was a fruitless action

    newState.location = dest;
    takeNextAction(newState, mostReleased);
}

void takeNextAction(const GraphState& state, u32& mostReleased)
{
    tryOpeningValve(state, mostReleased);

    // try moving to all the other closed valves
    if (state.valvesOpen != state.graph.allValvesMask)
    {
        u16 mask = 1;
        for (ValveIx i=0; i<ValveIx(size(state.graph.nodes)); ++i, mask <<= 1)
        {
            if (i == state.location || state.valvesOpen & mask)
                continue;

            tryMovingToRoom(state, i, mostReleased);
        }
    }

    // try running out the clock in here
    GraphState newState = state;
    tick(newState, 100, mostReleased);
}


u32 findBestPath(const ValveGraph& graph)
{
    const GraphState state{ graph };

#if 0
    u32 mostReleased = 0;
    takeNextAction(state, mostReleased);
    return mostReleased;
#else
    auto numActions = size(graph.nodes);  // +1 for open, -1 for move to this room

    vector<u32> mostPerFirstAction(numActions, 0);
    vector<thread> threads;
    threads.reserve(numActions);

    threads.emplace_back([&state, &mostPerFirstAction]() { tryOpeningValve(state, mostPerFirstAction[0]); });

    // try moving to all the other closed valves
    for (ValveIx i = 1; i < ValveIx(size(graph.nodes)); ++i)
    {
        threads.emplace_back([i, &state, &mostPerFirstAction]() { tryMovingToRoom(state, i, mostPerFirstAction[i]); });
    }

    for (auto& t : threads)
        t.join();

    return ranges::max(mostPerFirstAction);
#endif
}



u32 day16(const stringlist& input)
{
    auto valves = loadValves(input);
    auto graph = optimiseTunnels(valves);

    return findBestPath(graph);
}

int day16_2(const stringlist& input)
{
    for (auto& line : input)
    {
        (void)line;
    }

    return -1;
}


void run_day16()
{
    string sample =
R"(Valve AA has flow rate=0; tunnels lead to valves DD, II, BB
Valve BB has flow rate=13; tunnels lead to valves CC, AA
Valve CC has flow rate=2; tunnels lead to valves DD, BB
Valve DD has flow rate=20; tunnels lead to valves CC, AA, EE
Valve EE has flow rate=3; tunnels lead to valves FF, DD
Valve FF has flow rate=0; tunnels lead to valves EE, GG
Valve GG has flow rate=0; tunnels lead to valves FF, HH
Valve HH has flow rate=22; tunnel leads to valve GG
Valve II has flow rate=0; tunnels lead to valves AA, JJ
Valve JJ has flow rate=21; tunnel leads to valve II)";

    test(1651u, day16(READ(sample)));
    nononoD(day16(LOAD(16)));

    //test(-100, day16_2(READ(sample)));
    //gogogo(day16_2(LOAD(16)));
}