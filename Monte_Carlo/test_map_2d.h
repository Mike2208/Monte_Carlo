#ifndef TEST_MAP_2D_H
#define TEST_MAP_2D_H

/*	class TestMap2D
 *		creates and stores a test map for testing policies
 */

#include "standard_definitions.h"
#include "map_2d.h"
#include "occupancy_grid_map.h"

class TestMap2D
{
	public:
		TestMap2D();

		void LoadMaps(const OccupancyGridMap &InitialMap, const Map2D_Discrete &RealMap);		// Loads maps for testing

		int CreateMapsFromProbabilityPNGFile(const char *FileName);		// Create maps from PNG file containing a probabilistic OGM

	private:
		OccupancyGridMap	_InitialMap;		// Initial map that bot receives before being deployed
		Map2D_Discrete		_RealMap;			// Real map of environment

		void EstimateRealMapFromOGM();
};

#endif // TEST_MAP_2D_H
