#include "robot_navigation.h"

POS_2D GetAdjoiningPos(const POS_2D &CurPos, const NAV_OPTION_TYPE &OptionID)
{
	return (CurPos+NavigationOptions[OptionID]);
}

Robot_Navigation::Robot_Navigation()
{

}

