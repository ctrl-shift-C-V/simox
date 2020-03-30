#include "ColorMap.h"


namespace simox::color
{

    ColorMap::ColorMap()
    {
    }

    ColorMap::ColorMap(std::initializer_list<Color> init) : ColorMap("", init)
    {
    }

    ColorMap::ColorMap(std::initializer_list<std::pair<float, Color> > init) : ColorMap("", init)
    {
    }

    ColorMap::ColorMap(const std::string& name, std::initializer_list<Color> init) :
        _name(name)
    {
        if (init.size() == 1)
        {
            add_key(0, *init.begin());
        }
        else
        {
            size_t i = 0;
            float max = init.size() - 1;
            for (const auto& color : init)
            {
                add_key(i++ / max, color);
            }
        }
    }

    ColorMap::ColorMap(const std::string& name, std::initializer_list<std::pair<float, Color> > init) :
        _name(name)
    {
        for (const auto& [v, c] : init)
        {
            add_key(v, c);
        }
    }


    Color ColorMap::at(float value) const
    {
        if (empty())
        {
            return Color::black();
        }

        if (keys.size() == 1)
        {
            return keys.begin()->second;
        }

        if (_vmin || _vmax)
        {
            // Native: [1 .. 2]
            // Virtual: [100 .. 200]
            // => Scale 150 to 1.5

            value = (value - vmin()) / (vmax() - vmin());  // 150 -> 0.5
            value = value * (original_vmax() - original_vmin()) + original_vmin(); // 0.5 -> 1.5
        }

        // keys.size() >= 2

        if (value <= keys.begin()->first)
        {
            return keys.begin()->second;
        }

        // map::lower_bound(): lb not less than value (lb >= value)
        // map::upper_bound(): ub greater  than value (ub > value)
        auto upper = keys.upper_bound(value);
        if (upper == keys.end())
        {
            // No entry > value  =>  value >= all entries.
            --upper;
            return upper->second;
        }

        auto lower = upper;
        --lower;

        // Interpolate between lower and upper

        float t = (value - lower->first) / (upper->first - lower->first);
        // t = 0 => full lower,  t = 1 => full upper

        return interpol::linear(t, lower->second, upper->second);
    }


    float ColorMap::original_vmin() const
    {
        if (empty())
        {
            return 0;
        }
        return keys.begin()->first;
    }


    float ColorMap::original_vmax() const
    {
        if (empty())
        {
            return 1;
        }
        auto it = keys.end();
        --it;
        return it->first;
    }

}
