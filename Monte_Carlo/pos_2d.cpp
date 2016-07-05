#include "pos_2d.h"

#ifdef DEBUG	// DEBUG
std::ostream& operator<<(std::ostream& os, const POS_2D &obj)
{
	return (os << "X: " << obj.X << "\tY: " << obj.Y);
}
#endif			// ~DEBUG
