#ifndef DISTRICT_MAP_H
#define DISTRICT_MAP_H

/*	class DistrictMap
 *		map of one district
 */

#include "standard_definitions.h"
#include "district_map_definitions.h"

#include "map_2d.h"

namespace DISTRICT_MAP
{
	typedef DISTRICT_MAP_DEFINITIONS::ID	ID;
	typedef Map2D<ID>						MAP_TYPE;
}

class DistrictMap : public DISTRICT_MAP::MAP_TYPE
{
	public:
		typedef DISTRICT_MAP::ID		DISTRICT_ID;
		typedef DISTRICT_MAP::MAP_TYPE	DISTRICT_MAP_TYPE;

		DistrictMap() = default;
		DistrictMap(const DistrictMap &S) = default;
		DistrictMap(DistrictMap &&S) = default;
		DistrictMap &operator=(const DistrictMap &S) = default;
		DistrictMap &operator=(DistrictMap &&S) = default;

		bool IsCellInDistrict(const POS_2D &Position) const;
		bool HasThisMapChildren() const;

	private:

		POS_2D				_GlobalMapPosition;	// Position of map in global map
		DISTRICT_ID			_ID;				// ID of this district
		DISTRICT_ID			_ParentID;			// ID of parent

		friend class DistrictMapStorage;
};

#endif // DISTRICT_MAP_H
