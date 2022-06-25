#include <algorithm>
#include <core/SafeAssert.h>

namespace util
{
template <typename InputType, typename OutputType>
static OutputType mapValue(const InputType fromMin, const InputType fromMax, const OutputType toMin,
                           const OutputType toMax, const InputType value)
{
    // excluding everything not unit tested and not needed with devices
    SafeAssert(fromMin < fromMax);
    SafeAssert(toMin < toMax);
    SafeAssert(!(fromMax == 0 && fromMin < 0));
    SafeAssert(!(toMax == 0 && toMin < 0));

    const auto ToMin = static_cast<float>(toMin);
    const auto ToMax = static_cast<float>(toMax);
    const auto FromMin = static_cast<float>(fromMin);
    const auto FromMax = static_cast<float>(fromMax);

    const float v{std::min(std::max(static_cast<float>(value), FromMin), FromMax)};

    // https://stackoverflow.com/questions/5731863/mapping-a-numeric-range-onto-another
    return static_cast<OutputType>(ToMin + (ToMax - ToMin) * ((v - FromMin) / (FromMax - FromMin)));
}
} // namespace util