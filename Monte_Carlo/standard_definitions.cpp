#include "standard_definitions.h"

#ifdef DEBUG	// DEBUG
std::string PrintTabs(const unsigned int &NumTabs)
{
	std::string curString;
	for(unsigned int i = 0; i<NumTabs; ++i)
	{
		curString.append("\t");
	}

	return curString;
}
#endif			// ~DEBUG
