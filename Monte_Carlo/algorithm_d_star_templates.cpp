#ifndef ALGORITHM_D_STAR_TEMPLATES_H
#define ALGORITHM_D_STAR_TEMPLATES_H

#include "standard_definitions.h"
#include "robot_navigation.h"
#include "algorithm_d_star.h"

#include <queue>

// code copied from CalculateDStarHomologyDistMap
template<class T>
void AlgorithmDStar<T>::CalculateDStarCostMap(const Map2D<T> &OriginalMap, const POS_2D &ZeroPos, const T &MinVal, const T &MaxVal, Map2D<T> &CostMap)
{
	CostMap.ResetMap(OriginalMap.GetWidth(), OriginalMap.GetHeight(), MaxVal);

	std::queue<POS_2D> posToCheck;

	// Set min position to minVal
	CostMap.SetPixel(ZeroPos, MinVal);

	// Start at min pos
	posToCheck.push(ZeroPos);

	// Go through queue until no more adjacent positions are available
	do
	{
		const POS_2D &curPos = posToCheck.front();
		const T &curDist = CostMap.GetPixel(curPos);

		// Check all adjoining positions
		for(auto navDirection : NavigationOptions)
		{
			const POS_2D adjoiningPos = curPos+navDirection;
			T adjoiningDist;

			if(CostMap.GetPixel(adjoiningPos, adjoiningDist) < 0)
				continue;		// Check that adjoining pos is valid

			const T &adjoiningVal = OriginalMap.GetPixel(adjoiningPos);
			const T compDistVal = curDist + adjoiningVal;

			// Is a faster path available
			if(adjoiningDist > compDistVal)
			{
				// Set faster path
				CostMap.SetPixel(adjoiningPos, compDistVal);

				// Add to queue to be checked
				posToCheck.push(adjoiningPos);
			}
		}

		// Position checked, go to next pos
		posToCheck.pop();
	}
	while(posToCheck.size() > 0);
}

// code copied from CalculateDStarMap
template<class T>
void AlgorithmDStar<T>::CalculateDStarHomologyDistMap(const Map2D<T> &OriginalMap, const POS_2D &ZeroPos, const T &CutOffValue, DIST_MAP &DistMap)
{
	const DIST_MAP_TYPE maxVal = GetInfiniteVal<DIST_MAP_TYPE>();

	DistMap.ResetMap(OriginalMap.GetWidth(), OriginalMap.GetHeight(), maxVal);

	std::queue<POS_2D> posToCheck;

	// Set min position to 0
	DistMap.SetPixel(ZeroPos, 0);

	// Start at min pos
	posToCheck.push(ZeroPos);

	// Go through queue until no more adjacent positions are available
	do
	{
		const POS_2D &curPos = posToCheck.front();
		const DIST_MAP_TYPE &curDist = DistMap.GetPixel(curPos);

		// Check all adjoining positions
		for(auto navDirection : NavigationOptions)
		{
			const POS_2D adjoiningPos = curPos+navDirection;
			const DIST_MAP_TYPE navDist = GetMovementCost(curPos, adjoiningPos);
			DIST_MAP_TYPE adjoiningDist;

			if(DistMap.GetPixel(adjoiningPos, adjoiningDist) < 0)
				continue;		// Check that adjoining pos is valid (not outside of map)

			const T &adjoiningVal = OriginalMap.GetPixel(adjoiningPos);

			// Check that adjoining pos is below CutOff
			if(adjoiningVal < CutOffValue)
			{
				// Is a faster path available
				if(adjoiningDist > curDist + navDist)
				{
					// Set faster path
					DistMap.SetPixel(adjoiningPos, curDist + navDist);

					// Add to queue to be checked
					posToCheck.push(adjoiningPos);
				}
			}
		}

		// Position checked, go to next pos
		posToCheck.pop();
	}
	while(posToCheck.size() > 0);
}

template<class T>
void AlgorithmDStar<T>::UpdateMap_SingleValue(const Map2D<T> &UpdatedOriginalMap, const POS_2D &UpdatedPos, Map2D<T> &DStarMapToUpdate)
{
	std::queue<POS_2D> posToCheck;		// First value in array: Position to check, second value: Position that update originated from

	//Begin by checking start position
	posToCheck.push(UpdatedPos);

	// Continue until all values have been updated
	AlgorithmDStar<T>::UpdateMap_Routine(UpdatedOriginalMap, posToCheck, DStarMapToUpdate);
}

template<class T>
void AlgorithmDStar<T>::UpdateMap(const Map2D<T> &UpdatedOriginalMap, const std::vector<POS_2D> &UpdatedPos, Map2D<T> &DStarMapToUpdate)
{
	std::queue<POS_2D> posToCheck;		// First value in array: Position to check, second value: Position that update originated from

	//Begin by checking all updated positions
	for(const auto &curPos : UpdatedPos)
	{
		posToCheck.push(curPos);
	}

	// Continue until all values have been updated
	AlgorithmDStar<T>::UpdateMap_Routine(UpdatedOriginalMap, posToCheck, DStarMapToUpdate);
}

template<class T>
void AlgorithmDStar<T>::FindDStarPath(const DIST_MAP &DistMap, const POS_2D &StartPos, PATH_VECTOR &Path)
{
	// Clear path
	Path.clear();


	// Start at beginning
	POS_2D curPos = StartPos;
	DIST_MAP_TYPE curDist = DistMap.GetPixel(curPos);

	Path.push_back(curPos);

	// Continue along path until destination is reached
	while(curDist != 0)
	{
		DIST_MAP_TYPE bestDist = GetInfiniteVal<DIST_MAP_TYPE>();
		POS_2D bestPos;

		for(const auto &navOption : NavigationOptions)
		{
			const POS_2D adjacentPos = curPos+navOption;
			DIST_MAP_TYPE adjacentDist;

			if(DistMap.GetPixel(adjacentPos, adjacentDist) < 0)
				continue;		// Skip if not on map

			if(adjacentDist <= bestDist)
			{
				bestDist = adjacentDist;
				bestPos = adjacentPos;
			}
		}

		// Move to next position and add it to path
		curPos = bestPos;

		Path.push_back(curPos);
	}
}

template<class T>
void AlgorithmDStar<T>::UpdateMap_Routine(const Map2D<T> &UpdatedOriginalMap, std::queue<POS_2D> &PosToCheck, Map2D<T> &DStarMapToUpdate)
{
	// Continue until all values have been updated
	do
	{
		const POS_2D &curPos = PosToCheck.front();

		T bestAdjacentDistVal = GetInfiniteVal<T>();
		POS_2D bestPos;

		const T curVal = UpdatedOriginalMap.GetPixel(curPos);

		// Find best distance to zero position
		for(const auto &curNavOption : NavigationOptions)
		{
			const POS_2D adjacentPos = curPos + curNavOption;

			//Check if pos exists and get value
			T adjacentDistVal;
			if(DStarMapToUpdate.GetPixel(adjacentPos, adjacentDistVal) < 0)
				continue;		// Skip if pos is invalid

			//Compare to best value
			if(adjacentDistVal <= bestAdjacentDistVal)
			{
				bestPos = adjacentPos;
				bestAdjacentDistVal = adjacentDistVal;
			}
		}

		// Save best value if it is different, skip it and adjacent neighbors if nothing changes
		T *pCurDistVal = &(DStarMapToUpdate.GetPixelR(curPos));
		if(*pCurDistVal != bestAdjacentDistVal + curVal)
		{
			*pCurDistVal = bestAdjacentDistVal + curVal;

			// Add all adjacent positions except for best one
			for(const auto &curNavOption : NavigationOptions)
			{
				const POS_2D adjacentPos = curPos + curNavOption;

				if(adjacentPos != bestPos)
					PosToCheck.push(adjacentPos);
			}
		}

		PosToCheck.pop();
	}
	while(PosToCheck.size() > 0);
}

#endif // ALGORITHM_D_STAR_TEMPLATES_H
