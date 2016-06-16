#ifndef OGM_DIVIDER_H
#define OGM_DIVIDER_H

/*	class OGMDivider
 *		divides an occupancy grid map into districts
 */

#include <vector>

#include "occupancy_grid_map.h"
#include "graph_class.h"

namespace OGM_DIVIDER
{
	typedef OccupancyGridMap OG_MAP;

	struct DISTRICT_DATA
	{
		OG_MAP DistrictMap;		// Map with District Data
		POS_2D Position;		// Position of top left cell in global map
	};
}

class OGMDivider
{
		typedef OGM_DIVIDER::OG_MAP OG_MAP;
	public:
		typedef std::vector<OG_MAP> DISTRICT_STORAGE;
		OGMDivider() = default;

		static int DivideOGM(const OG_MAP &OGMap, const OGM_CELL_TYPE &DivisionValue, DISTRICT_STORAGE &Districts);

	private:
		const OG_MAP			*_MainMap;			// Map to be divided
		DISTRICT_STORAGE		_Districts;			// Storage of districts
};

#endif // OGM_DIVIDER_H
