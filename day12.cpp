#include "pch.h"
#include "harness.h"
#include "pt2.h"

#include <queue>

#define CHECKED_VECTOR2Dx

template<typename T>
class vector2d
{
public:
    using value_type = T;
    using coord = Pt2<i16>;

    vector2d(u32 width, u32 height, const value_type& defaultVal = {})
        : m_width(width)
        , m_height(height)
        , m_buf(width* height, defaultVal)
    { /**/ }

    vector2d(const stringlist& lines)
        : m_width(u16(lines.front().length()))
        , m_height(u16(lines.size()))
    {
        m_buf.reserve(m_width * m_height);
        for (const string& line : lines)
            ranges::copy(line, back_inserter(m_buf));
    }

    [[nodiscard]] u32 width() const { return m_width; }
    [[nodiscard]] u32 height() const { return m_height; }
    [[nodiscard]] u32 size() const { return m_width * m_height; }

    bool isInMap(const coord& c) const
    {
        return (c.x < int(m_width) && c.y < int(m_height) && c.x >= 0 && c.y >= 0);
    }

    value_type& operator[](const coord& c)
    {
#ifdef CHECKED_VECTOR2D
        if (!isInMap(c))
            throw "oob";
#endif
        return m_buf[c.x + c.y * m_width];
    }
    const value_type& operator[](const coord& c) const
    {
#ifdef CHECKED_VECTOR2D
        if (!isInMap(c))
            throw "oob";
#endif
        return m_buf[c.x + c.y * m_width];
    }

    coord find_first(const value_type& needle) const
    {
        auto foundIt = ranges::find(m_buf, needle);
        if (foundIt == end(m_buf))
            throw "doesn't exist";

        int ix = int(distance(begin(m_buf), foundIt));
        auto y = i16(ix / m_width);
        auto x = i16(ix % m_width);
        return { x, y };
    }

    // o_O VC 16.11.21 can't codegen the non-hardcoded version!
    //void foreach_matching(const value_type& match, auto&& fn)
    void foreach_a(auto&& fn)
    {
        auto it = begin(m_buf);
        for (u32 y = 0; y < m_height; ++y)
        {
            for (u32 x = 0; x < m_width; ++x, ++it)
            {
                if (*it == 'a')
                    fn(coord{ coord::el_type(x), coord::el_type(y) });
            }
        }
    }

private:
    u32 m_width;
    u32 m_height;
    vector<value_type> m_buf;
};


namespace
{
using MapCoord = vector2d<char>::coord;
using MapIndex = MapCoord;
constexpr MapIndex InvalidMapIx(std::numeric_limits<MapCoord::el_type>::max(), std::numeric_limits<MapCoord::el_type>::max());

using Distance = u16;
constexpr Distance MaxDistance = numeric_limits<Distance>::max();

constexpr MapCoord Directions[] = {{1,0}, {0,1}, {-1,0}, {0,-1}};
}

void erase_unsorted(auto& container, auto it)
{
    swap(*it, *container.rbegin());
    container.pop_back();
}


int findShortestPath(const vector2d<char>& map, MapCoord start, MapCoord dest)
{
    vector2d<MapIndex> bestPath(map.width(), map.height(), InvalidMapIx);
    vector2d<u8> visited(map.width(), map.height(), false);

    using DistCoordPair = pair<Distance, MapCoord>;
    using OpenSet = vector<DistCoordPair>;
    OpenSet open;

    visited[start] = true;
    open.emplace_back(Distance(0), start);

    bool found = false;
    while (!open.empty() && !found)
    {
        auto nextIt = ranges::min_element(open, less<Distance>(), &DistCoordPair::first);
        auto [nodeDist, nodePos] = *nextIt;
        char node = map[nodePos];
        erase_unsorted(open, nextIt);
        visited[nodePos] = true;

        for (const auto& dir : Directions)
        {
            MapCoord neighbourPos = nodePos + dir;
            if (!map.isInMap(neighbourPos))
                continue;

            if (visited[neighbourPos])
                continue;

            char neighbour = map[neighbourPos];
            if (neighbour > node + 1)
                continue;

            Distance newDist = nodeDist + 1;

            if (neighbourPos == dest)
            {
                return int(newDist);
            }

            auto neighbourIt = ranges::find_if(open, [neighbourPos](auto& dcpair) { return dcpair.second == neighbourPos; });
            if (neighbourIt != end(open))
            {
                if (neighbourIt->first > newDist)
                {
                    neighbourIt->first = newDist;
                    bestPath[neighbourPos] = nodePos;
                }
            }
            else
            {
                bestPath[neighbourPos] = nodePos;
                open.emplace_back(newDist, neighbourPos);
            }
        }
    }

    // no path
    return INT_MAX;
}


int day12(const stringlist& input)
{
    vector2d<char> map(input);
    if ((map.width()+1 >= u32(InvalidMapIx.x)) || (map.height() + 1 >= u32(InvalidMapIx.y)))
        throw "too many cells!";

    auto start = map.find_first('S');
    auto dest = map.find_first('E');

    map[start] = 'a';
    map[dest] = 'z';

    return findShortestPath(map, start, dest);
}

int day12_2(const stringlist& input)
{
    vector2d<char> hill(input);
    if ((hill.width() + 1 >= u32(InvalidMapIx.x)) || (hill.height() + 1 >= u32(InvalidMapIx.y)))
        throw "too many cells!";

    // all start points are equal
    auto start = hill.find_first('S');
    hill[start] = 'a';

    vector<MapCoord> startPoints;
    hill.foreach_a([&startPoints](const MapCoord& c) { startPoints.push_back(c); });

    auto dest = hill.find_first('E');
    hill[dest] = 'z';

    int shortest = INT_MAX;
    for (auto& startPoint : startPoints)
        shortest = min(shortest, findShortestPath(hill, startPoint, dest));

    return shortest;
}


void run_day12()
{
    string sample =
R"(Sabqponm
abcryxxl
accszExk
acctuvwj
abdefghi)";

    test(31, day12(READ(sample)));
    gogogo(day12(LOAD(12)));

    test(29, day12_2(READ(sample)));
    nononoD(day12_2(LOAD(12)));
}