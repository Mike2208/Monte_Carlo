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
		typedef unsigned int	SCALING_FACTOR_TYPE;
		typedef OGM_CELL_TYPE	RANDOMIZE_FACTOR_TYPE;

		TestMap2D(const OGM_CELL_TYPE MIN_FLOOR_VAL = OGM_CELL_MIN, const OGM_CELL_TYPE MAX_CEIL_VAL = OGM_CELL_MAX, const OGM_CELL_TYPE UNKNOWN_VAL = OGM_CELL_MAX);

		void LoadMaps(const OccupancyGridMap &InitialMap, const Map2D_Discrete &RealMap);		// Loads maps for testing

		int CreateMapsFromProbabilityPNGFile(const char *FileName);			// Create maps from PNG file containing a probabilistic OGM

		void RandomizeData(const RANDOMIZE_FACTOR_TYPE &RandomizeFactor, const OGM_CELL_TYPE MinValueToRandomize, const OGM_CELL_TYPE MaxValueToRandomize);		// Randomizes map values a little

		void ScaleMapsDownByFactor(const SCALING_FACTOR_TYPE &DownScaleFactor);	// Scale the given maps down by a given factor

		const Map2D_Discrete &GetRealMap() const { return this->_RealMap; }
		const OccupancyGridMap &GetOGMap() const { return this->_InitialMap; }

		Map2D_Discrete &GetRealMapR() { return this->_RealMap; }
		OccupancyGridMap &GetOGMapR() { return this->_InitialMap; }

	private:
		OccupancyGridMap	_InitialMap;		// Initial map that bot receives before being deployed
		Map2D_Discrete		_RealMap;			// Real map of environment

		OGM_CELL_TYPE		_MIN_FLOOR_VAL;		// Value under which cell is set to zero
		OGM_CELL_TYPE		_MAX_CEIL_VAL;		// Value over which cell is set to 100%
		OGM_CELL_TYPE		_UNKNOWN_VAL;		// Value of unscanned data

		void EstimateRealMapFromOGM();

		OGM_CELL_TYPE CalculateOGMScaledDownCell(const POS_2D &OldCellPos, const SCALING_FACTOR_TYPE &Scale) const;
		OGM_DISCRETE_TYPE CalculateRealMapScaledDownCell(const POS_2D &OldCellPos, const SCALING_FACTOR_TYPE &Scale) const;
};

#endif // TEST_MAP_2D_H
