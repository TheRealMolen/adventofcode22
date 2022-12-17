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
    int rate;
    bool open = false;
    string name;
    vector<ValveEdge> outgoing;
};
struct ValveGraph
{
    vector<ValveNode> nodes;
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
        u32 cost = 1;

        const string& start() const { return rooms.front(); }
        const string& end() const { return rooms.back(); }
    };
    vector<Tunnel> tunnels;

    ValveGraph g;

    auto isRoom = [](const Valve& v) { return v.rate > 0 || v.name == "AA"; };
    auto findValve = [&valves](const string& name) -> const Valve* { return &*ranges::find(valves, name, &Valve::name); };
    auto findNodeIx = [&g](const string& name) -> u8 { return u8(distance(begin(g.nodes), ranges::find(g.nodes, name, &ValveNode::name))); };
    for (auto& v : valves)
    {
        if (usedRooms.contains(v.name))
            continue;

        if (isRoom(v))
        {
            cout << "** adding room " << v.name << endl;
            ValveNode n;
            n.rate = v.rate;
            n.name = v.name;
            g.nodes.push_back(move(n));

            usedRooms.insert(v.name);
        }
        else
        {
            cout << "** adding tunnel starting at " << v.name << endl;

            if (v.tunnelNames.size() != 2)
                throw "ohno";

            Tunnel t{ { v.tunnelNames[0], v.name, v.tunnelNames[1] } };
            usedRooms.insert(v.name);

            // follow the tunnel backwards
            cout << "  scan back from " << v.name << " via " << t.start() << endl;
            for (const Valve* prev = findValve(t.start()); !isRoom(*prev); prev = findValve(t.start()))
            {
                if (prev->tunnelNames.size() != 2)
                    throw "ohno";;

                usedRooms.insert(t.start());
                t.rooms.push_front((t.rooms[1] == prev->tunnelNames[0]) ? prev->tunnelNames[1] : prev->tunnelNames[0]);
                cout << "      ... -> " << t.start() << endl;
                t.cost++;
            }
            // follow the tunnel forwards
            cout << "  scan fwd from " << v.name << " via " << t.end() << endl;
            for (const Valve* next = findValve(t.end()); !isRoom(*next); next = findValve(t.end()))
            {
                if (next->tunnelNames.size() != 2)
                    throw "ohno";

                usedRooms.insert(t.end());
                t.rooms.push_back((t.rooms[t.rooms.size()-2] == next->tunnelNames[0]) ? next->tunnelNames[1] : next->tunnelNames[0]);
                cout << "      ... -> " << t.end() << endl;
                t.cost++;
            }

            tunnels.push_back(move(t));
        }
    }

    for (const auto& t : tunnels)
    {
        u8 startIx = findNodeIx(t.start());
        u8 endIx = findNodeIx(t.end());

        g.nodes[startIx].outgoing.emplace_back(endIx, u8(t.cost));
        g.nodes[endIx].outgoing.emplace_back(startIx, u8(t.cost));
    }

    return g;
}



int day16(const stringlist& input)
{
    auto valves = loadValves(input);
    auto graph = optimiseTunnels(valves);

    return -1;
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

    test(1651, day16(READ(sample)));
    gogogo(day16(LOAD(16)));

    //test(-100, day16_2(READ(sample)));
    //gogogo(day16_2(LOAD(16)));
}