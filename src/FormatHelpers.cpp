#include "FormatHelpers.hpp"

namespace Zeta
{

Prefix const metricPrefixes[] = {
    {1000000000000000000, "E"},
    {1000000000000000, "P"},
    {1000000000000, "T"},
    {1000000000, "G"},
    {1000000, "M"},
    {1000, "k"},
};

size_t const metricPrefixCount = std::extent<decltype(metricPrefixes)>::value;

// https://en.wikipedia.org/wiki/Binary_prefix
Prefix const binaryPrefixes[] = {
    {1ull << 60, "Ei"},
    {1ull << 50, "Pi"},
    {1ull << 40, "Ti"},
    {1ull << 30, "Gi"},
    {1ull << 20, "Mi"},
    {1ull << 10, "ki"},
};

size_t const binaryPrefixCount = std::extent<decltype(binaryPrefixes)>::value;

}