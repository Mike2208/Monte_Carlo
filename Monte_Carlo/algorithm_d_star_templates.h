#ifndef ALGORITHM_D_STAR_TEMPLATES_H
#define ALGORITHM_D_STAR_TEMPLATES_H

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
			else if(adjoiningDist == compDistVal)
			{
				// If two adjacent positions have the same cost to goal, select the closer one
			}
		}

		// Position checked, go to next pos
		posToCheck.pop();
	}
	while(posToCheck.size() > 0);
}

// code copied from CalculateDStarMap
template<class T>
void AlgorithmDStar<T>::CalculateDStarHomologyDistMap(const Map2D<T> &OriginalMap, const POS_2D &ZeroPos, const T &CutOffValue, D_STAR_DIST_MAP &DistMap)
{
	const D_STAR_MAP_DIST_TYPE maxVal = GetInfiniteVal<D_STAR_MAP_DIST_TYPE>();

	DistMap.ResetMap(OriginalMap.GetHeight(), OriginalMap.GetWidth(), maxVal);

	std::queue<POS_2D> posToCheck;

	// Set min position to 0
	DistMap.SetPixel(ZeroPos, 0);

	// Start at min pos
	posToCheck.push(ZeroPos);

	// Go through queue until no more adjacent positions are available
	do
	{
		const POS_2D &curPos = posToCheck.front();
		const D_STAR_MAP_DIST_TYPE &curDist = DistMap.GetPixel(curPos);

		// Check all adjoining positions
		for(auto navDirection : NavigationOptions)
		{
			const POS_2D adjoiningPos = curPos+navDirection;
			D_STAR_MAP_DIST_TYPE adjoiningDist;

			if(DistMap.GetPixel(adjoiningPos, adjoiningDist) < 0)
				continue;		// Check that adjoining pos is valid (not outside of map)

			const T &adjoiningVal = OriginalMap.GetPixel(adjoiningPos);

			// Check that adjoining pos is below CutOff
			if(adjoiningVal < CutOffValue)
			{
				// Is a faster path available
				if(adjoiningDist > curDist + 1)
				{
					// Set faster path
					DistMap.SetPixel(adjoiningPos, curDist + 1);

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

#endif // ALGORITHM_D_STAR_TEMPLATES_H
