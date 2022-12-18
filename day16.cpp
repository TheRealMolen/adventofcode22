#include "pch.h"
#include "harness.h"


struct Valve
{
    int rate;
    bool open = false;
    string name;
    vector<string> tunnelNames;
};

using ValveIx = u8;

struct ValveEdge    // just one way; a tunnel is a pair of these
{
    ValveIx end;
    u8 cost;
};
struct ValveNode
{
    u16 rate;
    bool open = false;
    string name;
    vector<ValveEdge> outgoing;
};
struct ValveGraph
{
    vector<ValveNode> nodes;
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

    return valves;
}

ValveGraph optimiseTunnels(const vector<Valve>& valves)
{
    set<string> usedRooms;
    struct Tunnel
    {
        deque<string> rooms;
        u32 cost = ~0u;

        const string& start() const { return rooms.front(); }
        const string& end() const { return rooms.back(); }
    };
    vector<Tunnel> tunnels;
    vector<pair<string, string>> doors;

    ValveGraph g;

    auto isRoom = [](const Valve& v) { return v.rate > 0 || v.name == "AA"; };
    auto findValve = [&valves](const string& name) -> const Valve* { return &*ranges::find(valves, name, &Valve::name); };
    auto isNameARoom = [&](const string& name) { return isRoom(*findValve(name)); };
    auto findNodeIx = [&g](const string& name) -> u8 { return u8(distance(begin(g.nodes), ranges::find(g.nodes, name, &ValveNode::name))); };
    for (auto& v : valves)
    {
        if (usedRooms.contains(v.name))
            continue;
        usedRooms.insert(v.name);

        if (isRoom(v))
        {
            g.nodes.push_back({ .rate = u16(v.rate), .name = v.name });
            for (const string& tunnel : v.tunnelNames | views::filter(isNameARoom))
                doors.emplace_back(v.name, tunnel);
        }
        else
        {
            Tunnel t{ { v.tunnelNames[0], v.name, v.tunnelNames[1] }, 2 };

            // follow the tunnel backwards
            for (const Valve* prev = findValve(t.start()); !isRoom(*prev); prev = findValve(t.start()))
            {
                usedRooms.insert(t.start());
                t.rooms.push_front((t.rooms[1] == prev->tunnelNames[0]) ? prev->tunnelNames[1] : prev->tunnelNames[0]);
                t.cost++;
            }
            // follow the tunnel forwards
            for (const Valve* next = findValve(t.end()); !isRoom(*next); next = findValve(t.end()))
            {
                usedRooms.insert(t.end());
                t.rooms.push_back((t.rooms[t.rooms.size()-2] == next->tunnelNames[0]) ? next->tunnelNames[1] : next->tunnelNames[0]);
                t.cost++;
            }

            tunnels.push_back(move(t));
        }
    }

    for (const auto& d : doors)
    {
        u8 startIx = findNodeIx(d.first);
        u8 endIx = findNodeIx(d.second);
        g.nodes[startIx].outgoing.emplace_back(endIx, u8(1));
    }

    for (const auto& t : tunnels)
    {
        u8 startIx = findNodeIx(t.start());
        u8 endIx = findNodeIx(t.end());

        g.nodes[startIx].outgoing.emplace_back(endIx, u8(t.cost));
        g.nodes[endIx].outgoing.emplace_back(startIx, u8(t.cost));
    }

    {
        g.allValvesMask = 0;
        u16 mask = 1;
        for (auto itNode=begin(g.nodes); itNode!=end(g.nodes); ++itNode, mask <<= 1)
        {
            if (itNode->rate > 0)
                g.allValvesMask |= mask;
        }
    }

    return g;

}


struct GraphState
{
    const ValveGraph& graph;

    u32 totalReleased = 0;
    u16 currentFlowRate = 0;
    u16 valvesOpen = 0;
    i8 minute = 1;
    ValveIx location = 0;

    u8 nextExits[16] = { 0 };
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
   // cout << state << endl;

    // todo multiply
    for (/**/; count > 0; --count)
    {
        state.totalReleased += state.currentFlowRate;
        ++state.minute;

        if (state.minute > 30)
        {
            if (state.totalReleased > mostReleased)
            {
                mostReleased = state.totalReleased;
                cout << "found new max release of " << mostReleased << endl;
            }

            return false;
        }
    }

    return true;
}

void takeNextAction(const GraphState& state, u32& mostReleased)
{
    const ValveNode& room = state.graph.nodes[state.location];
    const u16 roomMask = 1 << state.location;

    // try turning the valve on
    if (room.rate > 10 && !(state.valvesOpen & roomMask))
    {
        GraphState newState = state;
        if (tick(newState, 1, mostReleased))
        {
            newState.valvesOpen |= roomMask;
            newState.currentFlowRate += room.rate;
            takeNextAction(newState, mostReleased);
        }
    }

    // try all tunnels off this room
    if (state.valvesOpen != state.graph.allValvesMask)
    {
        for (u8 i = 0; i < size(room.outgoing); ++i)
        {
            u8 exitIx = (i + state.nextExits[state.location]) % size(room.outgoing);
            auto& tunnel = room.outgoing[exitIx];

            GraphState newState = state;
            newState.nextExits[state.location] = exitIx + 1;
            if (!tick(newState, tunnel.cost, mostReleased))
                continue;

            newState.location = tunnel.end;
            takeNextAction(newState, mostReleased);
        }
    }

    // try turning the valve on
    if (room.rate > 0 && room.rate <= 10 && !(state.valvesOpen & roomMask))
    {
        GraphState newState = state;
        if (tick(newState, 1, mostReleased))
        {
            newState.valvesOpen |= roomMask;
            newState.currentFlowRate += room.rate;
            takeNextAction(newState, mostReleased);
        }
    }

    // try running out the clock in here
    GraphState newState = state;
    tick(newState, 100, mostReleased);
}



u32 day16(const stringlist& input)
{
    auto valves = loadValves(input);
    auto graph = optimiseTunnels(valves);

    u32 mostReleased = 0;
    GraphState initialState {graph};
    takeNextAction(initialState, mostReleased);

    return mostReleased;
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
    //gogogo(day16(LOAD(16)));

    //test(-100, day16_2(READ(sample)));
    //gogogo(day16_2(LOAD(16)));
}