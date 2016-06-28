#ifndef STANDARD_DEFINITIONS_H
#define STANDARD_DEFINITIONS_H

#ifndef DEBUG
//#define DEBUG
//#include <iostream>
#endif

#include <iostream>
#include <type_traits>
#include <limits>
#include <cmath>

template<class T>
static const inline T GetInfiniteVal()		// Returns maximum value possible for T
{
	if(std::numeric_limits<T>::has_infinity)
		return std::numeric_limits<T>::infinity();
	else
		return std::numeric_limits<T>::max();
}

template<class T>
inline T GetMinIncrementedValue(const T &Value)		// Returns next higher representable value of T
{
	// check type
	if(std::is_same<T, float>::value)
		return nextafterf(Value, GetInfiniteVal<T>());
	else if(std::is_same<T, double>::value)
		return nextafter(Value, GetInfiniteVal<T>());
	else if(std::is_same<T, long double>::value)
		return nextafterl(Value, GetInfiniteVal<T>());
	else if(std::is_integral<T>::value)
	{
		// Check if this value can be incremented
		if(Value == std::numeric_limits<T>::max())
			return Value;
		else
			return Value+1;
	}

	// Standard, return min increment of float type
	return nextafterf(Value, GetInfiniteVal<T>());
}

#ifdef DEBUG	// DEBUG
static std::string PrintTabs(const unsigned int &NumTabs)
{
	std::string curString;
	for(unsigned int i = 0; i<NumTabs; ++i)
	{
		curString.append("\t");
	}

	return curString;
}
#endif			// ~DEBUG

#endif // STANDARD_DEFINITIONS_H
