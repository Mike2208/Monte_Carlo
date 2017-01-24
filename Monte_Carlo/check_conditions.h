#ifndef CHECK_CONDITIONS_H
#define CHECK_CONDITIONS_H

#include "standard_definitions.h"

#include <iostream>

class CheckConditions
{
	public:
		CheckConditions(bool WaitUntilDone = false, size_t NumRepetitions = 0, size_t MaxSeconds = GetInfiniteVal<size_t>(), float MinPolCertainty = 0, float MinGoalCertainty = 0, float MinMapCertainty = 0);

		bool AreConditionsMet(bool Done, size_t NumRepetitions, size_t Seconds, float PolCertainty, float GoalCertainty, float MapCertainty) const;

		bool	_WaitUntilDone = false;
		size_t	_NumRepetitions = 0;

		size_t	_MaxSeconds	= GetInfiniteVal<size_t>();
		float	_MinPolCertainty = 0;
		float	_MinGoalCertainty = 0;
		float	_MinMapCertainty = 0;
};

#endif // CHECK_CONDITIONS_H
