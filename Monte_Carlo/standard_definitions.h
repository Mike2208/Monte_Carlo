#ifndef STANDARD_DEFINITIONS_H
#define STANDARD_DEFINITIONS_H

#include <limits>

template<class T>
inline T GetInfiniteVal()		// Returns maximum value possible for T
{
	if(std::numeric_limits<T>::has_infinity)
		return std::numeric_limits<T>::infinity();
	else
		return std::numeric_limits<T>::max();
}

#endif // STANDARD_DEFINITIONS_H
