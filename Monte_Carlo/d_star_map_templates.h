#ifndef D_STAR_MAP_TEMPLATES_H
#define D_STAR_MAP_TEMPLATES_H

#include "d_star_map.h"

#include "standard_definitions.h"
#include "robot_navigation.h"

template<class T>
int DStarMap<T>::CalculateMinCostPathLength(const DStarMap<T> &Map, const POS_2D &StartPos, PATH_LENGTH_TYPE &PathLength, const T *const CutOffCellVal)
{
	PATH_LENGTH_TYPE curLength = 0;

	// Dist Map, necessary when two adjacent dist costs are equal
	typename AlgorithmDStar<T>::DIST_MAP distMap;
	bool distMapCalculated = false;
	T distMapCutOffValue = 0;

	// Start at start pos
	POS_2D curPos = StartPos;
	T bestAdjacentVal;
	POS_2D bestAdjacentPos;

	while(curPos != Map.ZeroPos())
	{
		bestAdjacentPos = curPos;
		bestAdjacentVal = GetInfiniteVal<T>();
		for(const auto &moveOption : NavigationOptions)
		{
			const POS_2D adjacentPos = curPos+moveOption;

			T adjacentVal;
			if(Map.MapData().GetPixel(adjacentPos, adjacentVal) < 0)
				continue;

			// Skip if value greater than CutOffValue, if one is given
			if(CutOffCellVal != nullptr)
			{
				if(adjacentVal >= *CutOffCellVal)
					continue;
			}

			// Check if this position is better
			if(adjacentVal < bestAdjacentVal)
			{
				bestAdjacentVal = adjacentVal;
				bestAdjacentPos = adjacentPos;
			}
			else if(adjacentVal == bestAdjacentVal)
			{
				// if both values have equal cost distance, select the one that's closer (less cells) to destination
				if(!distMapCalculated || distMapCutOffValue > adjacentVal)
				{
					distMapCalculated = true;
					distMapCutOffValue = adjacentVal;

					AlgorithmDStar<T>::CalculateDStarHomologyDistMap(Map.MapData(), Map.ZeroPos(), GetMinIncrementedValue(adjacentVal), distMap);
				}

				// Check which position is closer
				if(distMap.GetPixel(adjacentPos) < distMap.GetPixel(bestAdjacentPos))
				{
					bestAdjacentVal = adjacentVal;
					bestAdjacentPos = adjacentPos;
				}
			}
		}

		// Return error if no better position was found
		if(bestAdjacentPos == curPos)
			return -1;

		// Move to adjacent pos
		curPos = bestAdjacentPos;

		// Update length
		curLength++;
	}

	// return answer
	PathLength = curLength;

	return 1;
}


#endif // D_STAR_MAP_TEMPLATES_H
