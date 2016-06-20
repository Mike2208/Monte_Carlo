#ifndef DISTRICT_MAP_DEFINITIONS_H
#define DISTRICT_MAP_DEFINITIONS_H

#include "standard_definitions.h"
#include "occupancy_grid_map.h"

#include <vector>

class DistrictMap;

namespace DISTRICT_MAP_DEFINITIONS
{
	typedef std::vector<DistrictMap> 	STORAGE;	// Storage of one district
	typedef STORAGE::size_type			ID;			// ID of one district

	const ID INVALID_DISTRICT_ID = GetInfiniteVal<ID>();		// ID in a cell that is not part of this district

	typedef unsigned int DEPTH_TYPE;
}

#endif // DISTRICT_MAP_DEFINITIONS_H
