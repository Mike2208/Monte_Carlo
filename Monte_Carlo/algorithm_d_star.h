#ifndef ALGORITHM_D_STAR_H
#define ALGORITHM_D_STAR_H

#include "standard_definitions.h"
#include "map_2d.h"

typedef unsigned int D_STAR_DIST_MAP_TYPE;
typedef Map2D<D_STAR_DIST_MAP_TYPE> D_STAR_DIST_MAP;

template<class T>
class AlgorithmDStar
{
	public:
		typedef D_STAR_DIST_MAP DIST_MAP;
		typedef D_STAR_DIST_MAP_TYPE DIST_MAP_TYPE;

		//AlgorithmDStar() = default;

		//typedef float T;
		static void CalculateDStarCostMap(const Map2D<T> &OriginalMap, const POS_2D &ZeroPos, const T &MinVal, const T &MaxVal, Map2D<T> &CostMap);					// Calculates D Star distance map
		static void CalculateDStarHomologyDistMap(const Map2D<T> &OriginalMap, const POS_2D &ZeroPos, const T &CutOffValue, DIST_MAP &DistMap);			// Calculates distance to zero pos from any point on the map

	private:

		Map2D<T> _CostMap;
};

#include "algorithm_d_star_templates.h"

#endif // ALGORITHM_D_STAR_H
