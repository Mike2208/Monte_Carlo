#ifndef OGM_DIVIDER_H
#define OGM_DIVIDER_H

/*	class OGMDivider
 *		divides an occupancy grid map into districts
 */

#include <vector>

#include "occupancy_grid_map.h"

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
		OGMDivider() = default;

		static int DivideOGM(const OG_MAP &OGMap, const OGM_CELL_TYPE &DivisionValue, std::vector<OG_MAP> &Districts);

	private:
		const OG_MAP			*_MainMap;		// Map to be divided
		std::vector<OG_MAP>		_Districts;		// Storage of districts

};

#endif // OGM_DIVIDER_H
