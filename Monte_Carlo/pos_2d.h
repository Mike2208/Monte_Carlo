#ifndef POS_2D_H
#define POS_2D_H

#include "standard_definitions.h"

typedef unsigned int POS_2D_TYPE;
const POS_2D_TYPE POS_2D_MIN = 0;

struct POS_2D
{
	POS_2D_TYPE X;
	POS_2D_TYPE Y;

	POS_2D(const POS_2D_TYPE &_X, const POS_2D_TYPE &_Y) : X(_X), Y(_Y) {}
	POS_2D() = default;
	POS_2D(const POS_2D &S) = default;
	POS_2D(POS_2D &&S) = default;
	POS_2D &operator=(const POS_2D &S) = default;
	POS_2D &operator=(POS_2D &&S) = default;

	POS_2D operator-(const POS_2D &S) const { return POS_2D(this->X-S.X, this->Y-S.Y); }
	POS_2D operator+(const POS_2D &S) const { return POS_2D(this->X+S.X, this->Y+S.Y); }
	bool operator==(const POS_2D &S) const { return ((this->X == S.X && this->Y == S.Y)? 1:0); }
	bool operator!=(const POS_2D &S) const { return ((this->X == S.X && this->Y == S.Y)? 0:1); }
};

#ifdef DEBUG	// DEBUG
static std::ostream& operator<<(std::ostream& os, const POS_2D &obj)
{
	return (os << "X: " << obj.X << "\tY: " << obj.Y);
}
#endif			// ~DEBUG

#endif // POS_2D_H
