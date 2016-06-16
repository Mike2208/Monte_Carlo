#ifndef ALGORITHM_VORONOI_FIELDS_H
#define ALGORITHM_VORONOI_FIELDS_H

/*	class AlgorithmVoronoiFields
 *		Calculates Voronoi Field of a given map
 */

#include "standard_definitions.h"
#include "map_2d.h"

template<class T>
class AlgorithmVoronoiFields
{
	public:

		static void CalculateVoronoiField(const Map2D<T> &OriginalMap, int);
};

#endif // ALGORITHM_VORONOI_FIELDS_H
