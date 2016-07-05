#ifndef ALGORITHM_D_STAR_H
#define ALGORITHM_D_STAR_H

#include "standard_definitions.h"
#include "map_2d.h"

#include <queue>

typedef unsigned int D_STAR_DIST_MAP_TYPE;
typedef Map2D<D_STAR_DIST_MAP_TYPE> D_STAR_DIST_MAP;

template<class T>
class AlgorithmDStar
{
	public:
		typedef D_STAR_DIST_MAP DIST_MAP;
		typedef D_STAR_DIST_MAP_TYPE DIST_MAP_TYPE;
		typedef std::vector<POS_2D> PATH_VECTOR;

		//AlgorithmDStar() = default;

		//typedef float T;
		static void CalculateDStarCostMap(const Map2D<T> &OriginalMap, const POS_2D &ZeroPos, const T &MinVal, const T &MaxVal, Map2D<T> &CostMap);					// Calculates D Star distance map
		static void CalculateDStarHomologyDistMap(const Map2D<T> &OriginalMap, const POS_2D &ZeroPos, const T &CutOffValue, DIST_MAP &DistMap);			// Calculates distance to zero pos from any point on the map

		static void UpdateMap_SingleValue(const Map2D<T> &UpdatedMap, const POS_2D &UpdatedPos, Map2D<T> &DStarMapToUpdate);				// Updates D* Map with one new data position
		static void UpdateMap(const Map2D<T> &UpdatedOriginalMap, const std::vector<POS_2D> &UpdatedPos, Map2D<T> &DStarMapToUpdate);				// Update multiple positions at once

		static void FindDStarPath(const DIST_MAP &DistMap, const POS_2D &StartPos, PATH_VECTOR &Path);

	private:

		static void UpdateMap_Routine(const Map2D<T> &UpdatedOriginalMap, std::queue<POS_2D> &PosToCheck, Map2D<T> &DStarMapToUpdate);		// Routine that runs to update a map
};

#include "algorithm_d_star_templates.cpp"

#endif // ALGORITHM_D_STAR_H
