#ifndef ROBOT_NAVIGATION_H
#define ROBOT_NAVIGATION_H

#include "standard_definitions.h"
#include "pos_2d.h"

typedef int NAV_OPTION_TYPE;
typedef float MOVE_DIST_TYPE;

#ifndef ADVANCED_MOVE
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
#else
const NAV_OPTION_TYPE NumNavigationOptions = 8;					// Number of move options available
const POS_2D NavigationOptions[NumNavigationOptions] =			// Array with all possible move options
{
	POS_2D(static_cast<POS_2D_TYPE>( 0), static_cast<POS_2D_TYPE>( 1)),		// Move up
	POS_2D(static_cast<POS_2D_TYPE>(-1), static_cast<POS_2D_TYPE>( 1)),		// Move up right
	POS_2D(static_cast<POS_2D_TYPE>( 1), static_cast<POS_2D_TYPE>( 0)),		// Move right
	POS_2D(static_cast<POS_2D_TYPE>( 1), static_cast<POS_2D_TYPE>(-1)),		// Move down right
	POS_2D(static_cast<POS_2D_TYPE>( 0), static_cast<POS_2D_TYPE>(-1)),		// Move down
	POS_2D(static_cast<POS_2D_TYPE>(-1), static_cast<POS_2D_TYPE>(-1)),		// Move down left
	POS_2D(static_cast<POS_2D_TYPE>(-1), static_cast<POS_2D_TYPE>( 0)),		// Move left
	POS_2D(static_cast<POS_2D_TYPE>( 1), static_cast<POS_2D_TYPE>( 1)),		// Move up left
};

// Cost for movement
const MOVE_DIST_TYPE NavigationCost[NumNavigationOptions] =
{
	1,
	static_cast<MOVE_DIST_TYPE>(std::sqrt(2)),
	1,
	static_cast<MOVE_DIST_TYPE>(std::sqrt(2)),
	1,
	static_cast<MOVE_DIST_TYPE>(std::sqrt(2)),
	1,
	static_cast<MOVE_DIST_TYPE>(std::sqrt(2))
};
#endif

POS_2D GetAdjoiningPos(const POS_2D &CurPos, const NAV_OPTION_TYPE &OptionID);
MOVE_DIST_TYPE GetMovementCost(const POS_2D &CurPos, const POS_2D &MovePos);

class Robot_Navigation
{
	public:
};

#endif // ROBOT_NAVIGATION_H
