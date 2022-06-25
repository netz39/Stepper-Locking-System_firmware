#pragma once

#include <cstdint>

namespace units
{
template <int M, int Kg, int S, int A, int K, int Mo, int C>
struct SiUnit
{
    enum
    {
        metre = M,
        kilogram = Kg,
        second = S,
        ampere = A,
        kelvin = K,
        mole = Mo,
        candela = C
    };
};
} // namespace units
