#ifndef ROBOT_DATA_H
#define ROBOT_DATA_H

#include "standard_definitions.h"
#include "pos_2d.h"

namespace ROBOT_DATA
{
	typedef POS_2D BOT_POS;
}

class RobotData
{
	public:
		RobotData();

		void SetGlobalBotPosition(const POS_2D &GlobalPos);
		POS_2D GetGlobalBotPosition() const;

	private:

		ROBOT_DATA::BOT_POS _PositionData;
};

#endif // ROBOT_DATA_H
