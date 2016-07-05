#include "robot_action.h"

#ifdef DEBUG	// DEBUG
std::ostream& operator<<(std::ostream& os, const RobotAction &obj)
{
	return obj.PrintAction(os);
}

std::ostream& RobotAction::PrintAction(std::ostream& os) const
{
	if(this->IsMoveAction())
		return (os << "Move Action");
	else if(this->IsObserveAction())
		return (os << "Observe Action");
	else if(this->IsCellFree())
		return (os << "Free Cell");
	else
		return (os << "Occupied Cell");
}
#endif			// ~DEBUG
