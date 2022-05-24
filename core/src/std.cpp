#include "core/BuildConfiguration.hpp"
#if IS_EMBEDDED_BUILD()
namespace std
{
void __throw_bad_function_call()
{
}

void __throw_out_of_range_fmt(char const *, ...)
{
}
} // namespace std
#endif