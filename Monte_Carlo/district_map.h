#ifndef DISTRICT_MAP_H
#define DISTRICT_MAP_H

/*	class DistrictMap
 *		map of one district
 */

#include "standard_definitions.h"
#include "district_map_definitions.h"

#include "map_2d.h"

#include <vector>

namespace DISTRICT_MAP
{
	typedef DISTRICT_MAP_DEFINITIONS::ID	ID;
	typedef bool		MAP_CELL_TYPE;
	typedef Map2D<MAP_CELL_TYPE>	MAP_TYPE;

	typedef std::vector<ID>		CONNECTED_ID_STORAGE;

	const MAP_CELL_TYPE IN_DISTRICT = true;
	const ID INVALID_DISTRICT_ID = DISTRICT_MAP_DEFINITIONS::INVALID_DISTRICT_ID;
}

class DistrictMap : public DISTRICT_MAP::MAP_TYPE
{
	public:
		typedef DISTRICT_MAP::ID					DISTRICT_ID;
		typedef DISTRICT_MAP::MAP_CELL_TYPE			DISTRICT_MAP_CELL_TYPE;
		typedef DISTRICT_MAP::MAP_TYPE				DISTRICT_MAP_TYPE;
		typedef DISTRICT_MAP::CONNECTED_ID_STORAGE	CONNECTED_ID_STORAGE;

		DistrictMap(const DISTRICT_ID &DistrictID, const POS_2D &GlobalMapPosition, const POS_2D_TYPE &NewWidth, const POS_2D_TYPE &NewHeight, const DISTRICT_MAP_CELL_TYPE &DefaultCellValue) : DISTRICT_MAP::MAP_TYPE(NewWidth, NewHeight, DefaultCellValue), _GlobalMapPosition(GlobalMapPosition), _ID(DistrictID) {}

		DistrictMap() = default;
		DistrictMap(const DistrictMap &S) = default;
		DistrictMap(DistrictMap &&S) = default;
		DistrictMap &operator=(const DistrictMap &S) = default;
		DistrictMap &operator=(DistrictMap &&S) = default;

		bool IsCellInDistrict(const POS_2D &Position) const;

		// Global pixel data
		DISTRICT_MAP_CELL_TYPE GetGlobalPixel(const POS_2D &GlobalPosition) const { return this->GetPixel(GlobalPosition-this->_GlobalMapPosition); }
		bool GetGlobalPixel(const POS_2D &GlobalPosition, DISTRICT_MAP_CELL_TYPE &Value) const { return this->GetPixel(GlobalPosition-this->_GlobalMapPosition, Value); }

		void SetGlobalPixel(const POS_2D &GlobalPosition, const DISTRICT_MAP_CELL_TYPE &Value) { this->SetPixel(GlobalPosition-this->_GlobalMapPosition, Value); }

		POS_2D ConvertToLocalPosition(const POS_2D &GlobalPosition) const { return GlobalPosition-this->_GlobalMapPosition; }
		POS_2D ConvertToGlobalPosition(const POS_2D &LocalPosition) const { return LocalPosition+this->_GlobalMapPosition; }

		// Extra Data
		void SetDistrictMap(const DISTRICT_MAP_TYPE &NewMapData, const POS_2D &NewMapPos);

		void SetID(const DISTRICT_ID &NewID);
		const DISTRICT_ID &GetID() const { return this->_ID; }

		void SetParentID(const DISTRICT_ID &ParentID);

		const POS_2D &GetGlobalMapPosition() const { return this->_GlobalMapPosition; }
		void SetGlobalMapPosition(const POS_2D &GlobalMapPos) { this->_GlobalMapPosition = GlobalMapPos; }

	private:

		POS_2D					_GlobalMapPosition;	// Position of map in global map
		DISTRICT_ID				_ID;				// ID of this district
		CONNECTED_ID_STORAGE	_ConnectedIDs;		// IDs of all surrounding districts that can be reached from this one

		friend class DistrictMapStorage;
};

#endif // DISTRICT_MAP_H
