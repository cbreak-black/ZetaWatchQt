#ifndef ZETA_FORMAT_HELPERS_HPP
#define ZETA_FORMAT_HELPERS_HPP

#include <sstream>
#include <string>
#include <iomanip>
#include <chrono>

namespace Zeta
{

struct Prefix
{
	uint64_t factor;
	char const * prefix;
};

extern Prefix const metricPrefixes[];
extern size_t const metricPrefixCount;

extern Prefix const binaryPrefixes[];
extern size_t const binaryPrefixCount;

template <typename T>
std::string
formatPrefixedValue(T size, Prefix const * prefix, size_t prefixCount)
{
	for (size_t p = 0; p < prefixCount; ++p)
	{
		if (size >= prefix[p].factor)
		{
			double scaledSize = size / double(prefix[p].factor);
			std::stringstream ss;
			ss << std::setprecision(2) << std::fixed << scaledSize << " "
			   << prefix[p].prefix;
			return ss.str();
		}
	}
	return std::to_string(size) + " ";
}

template <typename T>
std::string formatInformationValue(T size)
{
	return formatPrefixedValue(size, binaryPrefixes, binaryPrefixCount);
}

template <typename T>
std::string formatNormalValue(T size)
{
	return formatPrefixedValue(size, metricPrefixes, metricPrefixCount);
}

template <typename T>
std::string formatBytes(T bytes)
{
	return formatInformationValue(bytes) + "B";
}

inline std::string formatRate(uint64_t bytes, std::chrono::seconds const & time)
{
	return formatBytes(bytes / time.count()) + "/s";
}

inline std::string trim(std::string const & s)
{
	size_t first = s.find_first_not_of(' ');
	size_t last = s.find_last_not_of(' ');
	if (first != std::string::npos)
		return s.substr(first, last - first + 1);
	return s;
}

} // namespace Zeta

#endif /* ZetaMenuHelpers_h */
