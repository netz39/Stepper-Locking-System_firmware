#pragma once
#include <cstdint>
#include <limits>

struct __attribute((packed)) BgrColor
{
    uint8_t blue = 0;
    uint8_t green = 0;
    uint8_t red = 0;

    constexpr BgrColor &operator*=(const double &val)
    {
        blue *= val;
        green *= val;
        red *= val;
        return *this;
    }
};

inline constexpr BgrColor operator*(const BgrColor lhs, const double val)
{
    BgrColor result;
    result.blue = lhs.blue * val;
    result.green = lhs.green * val;
    result.red = lhs.red * val;
    return result;
}

inline constexpr BgrColor operator*(const double val, const BgrColor rhs)
{
    return rhs * val;
}

inline constexpr BgrColor operator+(const BgrColor &lhs, const BgrColor &rhs)
{
    BgrColor result;
    result.blue = lhs.blue * rhs.blue;
    result.green = lhs.green * rhs.green;
    result.red = lhs.red * rhs.red;
    return result;
}

constexpr BgrColor ColorOff{0, 0, 0};

constexpr BgrColor Red{0, 0, 255};
constexpr BgrColor Green{0, 255, 0};
constexpr BgrColor Blue{255, 0, 0};

constexpr BgrColor Yellow{0, 190, 255};
constexpr BgrColor Orange{0, 100, 255};
constexpr BgrColor Magenta{150, 0, 255};
constexpr BgrColor Turquoise{140, 255, 0};
constexpr BgrColor NeutralWhite{255, 255, 255};
constexpr BgrColor ColdWhite{255, 250, 230};

struct __attribute((packed)) BgrColorDiff
{
    int16_t blue = 0;
    int16_t green = 0;
    int16_t red = 0;

    constexpr BgrColorDiff &operator*=(const double &val)
    {
        blue *= val;
        green *= val;
        red *= val;
        return *this;
    }
};

inline constexpr BgrColor operator+(const BgrColor &lhs, const BgrColorDiff &rhs)
{
    BgrColor result;
    result.blue = lhs.blue + rhs.blue;
    result.green = lhs.green + rhs.green;
    result.red = lhs.red + rhs.red;
    return result;
}

inline constexpr BgrColorDiff operator*(const BgrColorDiff lhs, const double val)
{
    BgrColorDiff result;
    result.blue = lhs.blue * val;
    result.green = lhs.green * val;
    result.red = lhs.red * val;
    return result;
}

inline constexpr BgrColorDiff operator*(const double val, const BgrColorDiff rhs)
{
    return rhs * val;
}
inline constexpr BgrColorDiff operator/(BgrColorDiff lhs, const double val)
{
    BgrColorDiff result;
    result.blue = lhs.blue / val;
    result.green = lhs.green / val;
    result.red = lhs.red / val;
    return result;
}

inline constexpr BgrColorDiff operator-(const BgrColor &lhs, const BgrColor &rhs)
{
    BgrColorDiff result;
    result.blue = lhs.blue - rhs.blue;
    result.green = lhs.green - rhs.green;
    result.red = lhs.red - rhs.red;
    return result;
}