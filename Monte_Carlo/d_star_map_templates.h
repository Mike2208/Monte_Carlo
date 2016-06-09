#ifndef D_STAR_MAP_TEMPLATES_H
#define D_STAR_MAP_TEMPLATES_H

#include "d_star_map.h"

#include "standard_definitions.h"
#include "robot_navigation.h"
#include <array>

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

template<class T>
int DStarMap<T>::UpdateMap_SinglePos(const Map2D<T> &UpdatedOriginalMap, const POS_2D &UpdatePos)
{
	return this->UpdateMap_SinglePos_AnyValue(UpdatedOriginalMap, UpdatePos);
}

template<class T>
int DStarMap<T>::UpdateMap_SinglePos_AnyValue(const Map2D<T> &UpdatedOriginalMap, const POS_2D &UpdatePos)
{
	std::queue<POS_2D> posToCheck;		// First value in array: Position to check, second value: Position that update originated from

	//Begin by checking start position
	posToCheck.push(UpdatePos);

	// Continue until all values have been updated
	do
	{
		const POS_2D &curPos = posToCheck.front();

		T bestAdjacentDistVal = GetInfiniteVal<T>();
		POS_2D bestPos;

		const T curVal = UpdatedOriginalMap.GetPixel(curPos);

		// Find best distance to zero position
		for(const auto &curNavOption : NavigationOptions)
		{
			const POS_2D adjacentPos = curPos + curNavOption;

			//Check if pos exists and get value
			T adjacentDistVal;
			if(this->_DStarMap.GetPixel(adjacentPos, adjacentDistVal) < 0)
				continue;		// Skip if pos is invalid

			//Compare to best value
			if(adjacentDistVal <= bestAdjacentDistVal)
			{
				bestPos = adjacentPos;
				bestAdjacentDistVal = adjacentDistVal;
			}
		}

		// Save best value if it is different, skip it and adjacent neighbors if nothing changes
		T *pCurDistVal = &(this->_DStarMap.GetPixelR(curPos));
		if(*pCurDistVal != bestAdjacentDistVal + curVal)
		{
			*pCurDistVal = bestAdjacentDistVal + curVal;

			// Add all adjacent positions except for best one
			for(const auto &curNavOption : NavigationOptions)
			{
				const POS_2D adjacentPos = curPos + curNavOption;

				if(adjacentPos != bestPos)
					posToCheck.push(adjacentPos);
			}
		}

		posToCheck.pop();
	}
	while(posToCheck.size() > 0);

	return 1;
}

//template<class T>
//int DStarMap<T>::UpdateMap_SinglePos_SmallerValue(const Map2D<T> &UpdatedOriginalMap, const T &OldValue,const POS_2D &UpdatePos)
//{
//	// Set position to update to new value by subtracting (OldValue-UpdatedValue)
//	this->_DStarMap.GetPixelR(UpdatePos) = OldValue - UpdatedOriginalMap.GetPixel(UpdatePos);

//	// Start by start val and update all necessary adjacent points
//	std::queue<POS_2D> posToCheck;
//	posToCheck.push(UpdatePos);

//	do
//	{
//		const POS_2D &curPos = posToCheck.front();
//		const T curDistVal = this->_DStarMap.GetPixel(curPos);

//		for(const auto &curNavOption : NavigationOptions)
//		{
//			const POS_2D adjacentPos = curPos + curNavOption;
//			T adjacentCompVal;

//			// Check if adjacent pos is in map and get value
//			if(UpdatedOriginalMap.GetPixel(adjacentPos, adjacentCompVal) < 0)
//				continue;		// Skip if invalid pos

//			adjacentCompVal += curDistVal;
//			T &r_adjacentDistVal = this->_DStarMap.GetPixelR(adjacentPos);

//			// Compare values
//			if(adjacentCompVal < r_adjacentDistVal)
//			{
//				r_adjacentDistVal = adjacentCompVal;

//				posToCheck.push(adjacentPos);
//			}
//		}

//		posToCheck.pop();
//	}
//	while(posToCheck.size() > 0);

//	return 1;
//}

#endif // D_STAR_MAP_TEMPLATES_H
