#include "test_policy.h"

int TestPolicy::PerformTest(PolicyTree &BotPolicy, const POS_2D &StartPos, const POS_2D &Destination, const Map2D_Discrete &MapData)
{
	POS_2D botPosition;
	RobotAction nextActionToPerform;
	POS_2D		nextPosition;

	OGM_DISCRETE_TYPE cellOccupancy;
	RobotAction actionResult;

	if(StartPos == Destination)
		return 0;

	// Begin at start
	BotPolicy.ResetBotToStart();

	// Check that first action places robot at start
	if(!BotPolicy.GetNextRobotAction().IsMoveAction() ||
			BotPolicy.GetNextRobotPosition() != StartPos)
		return -1;

	// Place robot at start position
	actionResult.SetMoveAction();
	botPosition = StartPos;

	while(botPosition != Destination)
	{
		if(BotPolicy.MoveToNextBotAction(&actionResult) < 0)
			return -3;		// Error, dest not reached

		nextActionToPerform = BotPolicy.GetNextRobotAction();
		nextPosition = BotPolicy.GetNextRobotPosition();

		if(nextActionToPerform.IsMoveAction())
		{
			// If bot should move, check if move is valid
			if(MapData.GetPixel(nextPosition, cellOccupancy) < 0)
				return -1;

			if(cellOccupancy == OGM_DISCRETE_FULL)
				return -2;		// Error, trying to move to occupied cell

			// Move bot to next position and set result to move action
			botPosition = nextPosition;
			actionResult = nextActionToPerform;
		}
		else if(nextActionToPerform.IsObserveAction())
		{
			// Move to next node to check results of observation
			actionResult = nextActionToPerform;
			if(BotPolicy.MoveToNextBotAction(&actionResult) < 0)
				return -3;

			// Tell policy whether cell is occupied or not
			if(MapData.GetPixel(nextPosition, cellOccupancy) < 0)
				return -1;

			// save result of action
			if(cellOccupancy == OGM_DISCRETE_FULL)
				actionResult.SetOccupiedResult();
			else
				actionResult.SetFreeResult();
		}
		else
		{
			return -4;		// Error, policy tree inconsistent (this action shouldn't be reached)
		}
	}

	return 1;
}
