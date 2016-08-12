#ifndef ROBOT_DATA_H
#define ROBOT_DATA_H

#include "standard_definitions.h"
#include "pos_2d.h"

namespace ROBOT_DATA
{
	typedef POS_2D BOT_POS;
	typedef float SCAN_RANGE_TYPE;
}

class RobotData
{
	public:
		RobotData() = default;

		void SetGlobalBotPosition(const POS_2D &GlobalPos);
		POS_2D GetGlobalBotPosition() const;

		ROBOT_DATA::SCAN_RANGE_TYPE GetScanRange() const;

	private:

		ROBOT_DATA::BOT_POS _PositionData;
		ROBOT_DATA::SCAN_RANGE_TYPE _ScanRange = 1.5;
};

#endif // ROBOT_DATA_H
