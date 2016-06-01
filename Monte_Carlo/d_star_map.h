#ifndef D_STAR_MAP_H
#define D_STAR_MAP_H

#include "algorithm_d_star.h"
#include "map_2d.h"

template<class T>
class DStarMap
{
	public:
		DStarMap();

		const Map2D<T> &MapData() { return this->_DStarMap; }		// Allows for read only access to map

		void CalculateDStarCostMap(const Map2D<T> &OriginalMap, const POS_2D &ZeroPos, const T &MinVal, const T &MaxVal)
		{ return AlgorithmDStar<T>::CalculateDStarCostMap(OriginalMap, ZeroPos, MinVal, MaxVal, this->_DStarMap); }

	private:

		Map2D<T>	_DStarMap;
};

#endif // D_STAR_MAP_H
