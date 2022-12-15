#pragma once

#include <ostream>
#include <vector>


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
    { /**/
    }

    vector2d(const stringlist& lines)
        : m_width(u16(lines.front().length()))
        , m_height(u16(lines.size()))
    {
        m_buf.reserve(m_width * m_height);
        for (const string& line : lines)
            ranges::copy(line, back_inserter(m_buf));
    }


    void setOffset(const coord& offset)
    {
        m_offset = offset;
    }


    [[nodiscard]] u32 width() const { return m_width; }
    [[nodiscard]] u32 height() const { return m_height; }
    [[nodiscard]] u32 size() const { return m_width * m_height; }

    bool isInMap(const coord& c) const
    {
        auto local = c - m_offset;
        return (local.x < int(m_width) && local.y < int(m_height) && local.x >= 0 && local.y >= 0);
    }

    value_type& operator[](const coord& c)                  { return getLocal(c - m_offset); }
    const value_type& operator[](const coord& c) const      { return getLocal(c - m_offset); }

    value_type& getLocal(const coord& c)
    {
#ifdef CHECKED_VECTOR2D
        if (!isInMap(c + m_offset))
            throw "oob";
#endif
        return m_buf[c.x + c.y * m_width];
    }
    const value_type& getLocal(const coord& c) const
    {
#ifdef CHECKED_VECTOR2D
        if (!isInMap(c + m_offset))
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
        return m_offset + coord{ x, y };
    }

    // o_O VC 16.11.21 can't codegen the non-hardcoded version!
    void foreach_matching(const value_type& match, auto&& fn)
    //void foreach_a(auto&& fn)
    {
        auto it = begin(m_buf);
        for (u32 y = 0; y < m_height; ++y)
        {
            for (u32 x = 0; x < m_width; ++x, ++it)
            {
                if (*it == match)
                    fn(m_offset + coord{ coord::el_type(x), coord::el_type(y) });
            }
        }
    }

private:
    u32 m_width;
    u32 m_height;
    coord m_offset{0,0};
    vector<value_type> m_buf;
};


inline ostream& operator<<(ostream& os, const vector2d<char> map)
{
    using coord = decltype(map)::coord;
    using ctype = coord::el_type;

    for (u32 y=0; y<map.height(); ++y)
    {
        for (u32 x=0; x<map.width(); ++x)
            os << map.getLocal(coord{ ctype(x), ctype(y) });

        os << "\n";
    }

    os << flush;
    return os;
}

