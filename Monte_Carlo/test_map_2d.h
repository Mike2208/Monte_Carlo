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
		typedef unsigned int SCALING_FACTOR;

		TestMap2D() = default;

		void LoadMaps(const OccupancyGridMap &InitialMap, const Map2D_Discrete &RealMap);		// Loads maps for testing

		int CreateMapsFromProbabilityPNGFile(const char *FileName);		// Create maps from PNG file containing a probabilistic OGM

		void ScaleMapsDownByFactor(const SCALING_FACTOR &DownScaleFactor);	// Scale the given maps down by a given factor

		const Map2D_Discrete &GetRealMap() const { return this->_RealMap; }
		const OccupancyGridMap &GetOGMap() const { return this->_InitialMap; }

	private:
		OccupancyGridMap	_InitialMap;		// Initial map that bot receives before being deployed
		Map2D_Discrete		_RealMap;			// Real map of environment

		void EstimateRealMapFromOGM();

		OGM_CELL_TYPE CalculateOGMScaledDownCell(const POS_2D &OldCellPos, const SCALING_FACTOR &Scale) const;
		OGM_DISCRETE_TYPE CalculateRealMapScaledDownCell(const POS_2D &OldCellPos, const SCALING_FACTOR &Scale) const;
};

#endif // TEST_MAP_2D_H
