#include "robot_navigation.h"

POS_2D GetAdjoiningPos(const POS_2D &CurPos, const NAV_OPTION_TYPE &OptionID)
{
	return (CurPos+NavigationOptions[OptionID]);
}

MOVE_DIST_TYPE GetMovementCost(const POS_2D &CurPos, const POS_2D &MovePos)
{
	const POS_2D MoveVector = MovePos-CurPos;
	for(NAV_OPTION_TYPE i = 0; i<NumNavigationOptions; ++i)
	{
		if(MoveVector == NavigationOptions[i])
			return NavigationCost[i];
	}

	return 0;
}
