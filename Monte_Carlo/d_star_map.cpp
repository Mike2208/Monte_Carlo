#include "d_star_map.h"

template<class T>
int DStarMap<T>::UpdateMap_SinglePos(const Map2D<T> &UpdatedOriginalMap, const T &OldValue,const POS_2D &UpdatePos, const bool IsUpdatedValueSmaller)
{
	// Start by getting new value at UpdatePos
	if(IsUpdatedValueSmaller)
	{
		return UpdateMap_SinglePos_SmallerValue(UpdatedOriginalMap, OldValue, UpdatePos);
	}
	else
	{
		return -1;
	}
}

template<class T>
int DStarMap<T>::UpdateMap_SinglePos_SmallerValue(const Map2D<T> &UpdatedOriginalMap, const T &OldValue,const POS_2D &UpdatePos)
{
	// Set position to update to new value by subtracting (OldValue-UpdatedValue)
	this->_DStarMap.GetPixelR(UpdatePos) = OldValue - UpdatedOriginalMap.GetPixel(UpdatePos);

	// Start by start val and update all necessary adjacent points
	std::queue<POS_2D> posToCheck;
	posToCheck.push(UpdatePos);

	do
	{
		const POS_2D &curPos = posToCheck.front();
		const T curDistVal = this->_DStarMap.GetPixel(curPos);

		for(const auto &curNavOption : NavigationOptions)
		{
			const POS_2D adjacentPos = curPos + curNavOption;
			T adjacentCompVal;

			// Check if adjacent pos is in map and get value
			if(UpdatedOriginalMap.GetPixel(adjacentPos, adjacentCompVal) < 0)
				continue;		// Skip if invalid pos

			adjacentCompVal += curDistVal;
			T &r_adjacentDistVal = this->_DStarMap.GetPixelR(adjacentPos);

			// Compare values
			if(adjacentCompVal < r_adjacentDistVal)
			{
				r_adjacentDistVal = adjacentCompVal;

				posToCheck.push(adjacentPos);
			}
		}

		posToCheck.pop();
	}
	while(posToCheck.size() > 0);

	return 1;
}

template<class T>
int DStarMap<T>::UpdateMap_SinglePos_LargerValue(const Map2D<T> &UpdatedOriginalMap, const T &OldValue,const POS_2D &UpdatePos)
{

}
