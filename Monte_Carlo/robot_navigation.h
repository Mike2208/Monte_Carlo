#ifndef ROBOT_NAVIGATION_H
#define ROBOT_NAVIGATION_H

#include "standard_definitions.h"
#include "pos_2d.h"

typedef int NAV_OPTION_TYPE;
typedef float MOVE_DIST_TYPE;
const NAV_OPTION_TYPE NumNavigationOptions = 4;					// Number of move options available
const POS_2D NavigationOptions[NumNavigationOptions] =			// Array with all possible move options
{
	POS_2D(static_cast<POS_2D_TYPE>( 0), static_cast<POS_2D_TYPE>( 1)),		// Move up
	POS_2D(static_cast<POS_2D_TYPE>( 1), static_cast<POS_2D_TYPE>( 0)),		// Move right
	POS_2D(static_cast<POS_2D_TYPE>(-1), static_cast<POS_2D_TYPE>( 0)),		// Move left
	POS_2D(static_cast<POS_2D_TYPE>( 0), static_cast<POS_2D_TYPE>(-1))		// Move right
};

// Cost for movement
const MOVE_DIST_TYPE NavigationCost[NumNavigationOptions] =
{
	1,
	1,
	1,
	1
};

POS_2D GetAdjoiningPos(const POS_2D &CurPos, const NAV_OPTION_TYPE &OptionID);
MOVE_DIST_TYPE GetMovementCost(const POS_2D &CurPos, const POS_2D &MovePos);

class Robot_Navigation
{
	public:
};

#endif // ROBOT_NAVIGATION_H
