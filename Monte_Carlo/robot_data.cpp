#include "robot_data.h"

void RobotData::SetGlobalBotPosition(const POS_2D &GlobalPos)
{
	this->_PositionData = GlobalPos;
}

POS_2D RobotData::GetGlobalBotPosition() const
{
	return this->_PositionData;
}
