#pragma once

#include <algorithm>
#include <initializer_list>
#include <map>
#include <string>

#include <SimoxUtility/algorithm/apply.hpp>

#include "Color.h"
#include "interpolation.h"


namespace simox::color
{

    /**
     * @brief A color map, mapping scalar values to colors.
     */
    class ColorMap
    {
    public:

        /// Construct an unnamed empty color map. Will always return black.
        ColorMap();

        /// Construct an unnamed color map with equidistant values from 0 to 1 (inclusive).
        ColorMap(std::initializer_list<Color> init);
        /// Construct an unnamed color map with given values.
        ColorMap(std::initializer_list<std::pair<float, Color>> init);

        /// Construct a named color map with equidistant values from 0 to 1 (inclusive).
        ColorMap(const std::string& name, std::initializer_list<Color> init);
        /// Construct a named color map with given values.
        ColorMap(const std::string& name, std::initializer_list<std::pair<float, Color>> init);


        bool empty() const { return keys.empty(); }
        /// Get the number of keys.
        size_t size() const { return keys.size(); }

        void clear() { keys.clear(); }
        /// Add a key color at the given value.
        void add_key(float value, const Color& color) { keys[value] = color; }


        /**
         * @brief Get the color for the given scalar value.
         *
         * Empty color maps always return black.
         *
         * @param value The scalar value.
         * @return
         */
        Color at(float value) const;

        /// @see `ColorMap::at()`
        Color operator()(float value) const { return this->at(value); }

        /// Apply this colormap to a vector.
        template <typename V>
        std::vector<Color> operator()(const std::vector<V>& vector) const { return simox::alg::apply(*this, vector); }
        /// Apply this colormap to a map's values.
        template <typename K, typename V>
        std::map<K, Color> operator()(const std::map<K, V>& map) const { return simox::alg::apply(*this, map); }


        std::string name() const { return _name; }
        void setName(const std::string& name) { this->_name = name; }


        /// The value corresponding to the bottom color.
        float vmin() const { return _vmin ? *_vmin : original_vmin(); }
        /// The value corresponding to the top color.
        float vmax() const { return _vmax ? *_vmax : original_vmax(); }

        void set_vmin(float vmin) { this->_vmin = vmin; }
        void set_vmax(float vmax) { this->_vmax = vmax; }
        void set_vmin(const std::vector<float>& values) { set_vmin(*std::max_element(values.begin(), values.end())); }
        void set_vmax(const std::vector<float>& values) { set_vmax(*std::max_element(values.begin(), values.end())); }

        /// Sets the value limits, i.e. scales the color map to the range [vmin, vmax].
        void set_vlimits(float vmin, float vmax)
        {
            set_vmin(vmin);
            set_vmax(vmax);
        }
        void set_vlimits(const std::vector<float>& values)
        {
            const auto [min, max] = std::minmax_element(values.begin(), values.end());
            set_vmin(*min);
            set_vmax(*max);
        }


        /// Get this colormap reversed (but defined in the same value range as this).
        ColorMap reversed() const;


    private:

        /// Return the lowest key value in `keys` (0 if empty).
        float original_vmin() const;
        /// Return the highest key value in `keys` (1 if empty).
        float original_vmax() const;


    private:

        /// Map of value to color at that value.
        std::map<float, Color> keys;

        /// The name.
        std::string _name = "";


        /// The "virtual" minimal value.
        std::optional<float> _vmin = std::nullopt;
        /// The "virtual" maximal value.
        std::optional<float> _vmax = std::nullopt;

    };

}


namespace simox
{
    using ColorMap = color::ColorMap;
}