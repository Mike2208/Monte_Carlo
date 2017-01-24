#include "check_conditions.h"

CheckConditions::CheckConditions(bool WaitUntilDone, size_t NumRepetitions, size_t MaxSeconds, float MinPolCertainty, float MinGoalCertainty, float MinMapCertainty)
	: _WaitUntilDone(WaitUntilDone), _NumRepetitions(NumRepetitions), _MaxSeconds(MaxSeconds), _MinPolCertainty(MinPolCertainty), _MinGoalCertainty(MinGoalCertainty), _MinMapCertainty(MinMapCertainty)
{}

bool CheckConditions::AreConditionsMet(bool Done, size_t NumRepetitions, size_t Seconds, float PolCertainty, float GoalCertainty,	float MapCertainty) const
{
	if(this->_WaitUntilDone)
	{
		return Done;
	}

	if(Seconds >= this->_MaxSeconds)
		return true;

	if(NumRepetitions < this->_NumRepetitions)
		return false;

	if(PolCertainty < this->_MinPolCertainty)
		return false;

	if(GoalCertainty < this->_MinGoalCertainty)
		return false;

	if(MapCertainty < this->_MinMapCertainty)
		return false;

	return true;
}
