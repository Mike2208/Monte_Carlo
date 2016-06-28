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
	typedef DISTRICT_MAP_DEFINITIONS::ID		ID;
	typedef DISTRICT_MAP_DEFINITIONS::ID_MAP	ID_MAP;

	typedef bool		MAP_CELL_TYPE;
	typedef Map2D<MAP_CELL_TYPE>	MAP_TYPE;

	typedef std::vector<ID>		CONNECTED_ID_STORAGE;

	const MAP_CELL_TYPE IN_DISTRICT = true;
	const ID INVALID_DISTRICT_ID = DISTRICT_MAP_DEFINITIONS::INVALID_DISTRICT_ID;

	// Struct to temporarily store size of district (Min- and MaxPos form rectangle with all cells in them)
	struct DISTRICT_SIZE
	{
		ID DistrictID;		// ID of district
		POS_2D MinPos;		// Minimum position of cells
		POS_2D MaxPos;		// Maximum position of cells

		DISTRICT_SIZE(const ID &NewID, const POS_2D &NewPos) : DistrictID(NewID), MinPos(NewPos), MaxPos(NewPos) {}
		DISTRICT_SIZE() = delete;

		void ComparePos(const POS_2D &Pos);
	};

	typedef std::vector<POS_2D> POS_IN_DISTRICT;
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

		bool IsGlobalCellInDistrict(const POS_2D &GlobalPosition) const;

		// Set District from IDMap
		void SetDistrictFromIDMap(const DISTRICT_MAP::ID_MAP &IDMap, const POS_2D &PosOfOneID, DISTRICT_MAP::POS_IN_DISTRICT *GlobalPosInDistrict = nullptr);

		// Global pixel data
		DISTRICT_MAP_CELL_TYPE GetGlobalPixel(const POS_2D &GlobalPosition) const { return this->GetPixel(GlobalPosition-this->_GlobalMapPosition); }
		int GetGlobalPixel(const POS_2D &GlobalPosition, DISTRICT_MAP_CELL_TYPE &Value) const { return this->GetPixel(GlobalPosition-this->_GlobalMapPosition, Value); }

		void SetGlobalPixel(const POS_2D &GlobalPosition, const DISTRICT_MAP_CELL_TYPE &Value) { this->SetPixel(GlobalPosition-this->_GlobalMapPosition, Value); }

		POS_2D ConvertToLocalPosition(const POS_2D &GlobalPosition) const { return GlobalPosition-this->_GlobalMapPosition; }
		POS_2D ConvertToGlobalPosition(const POS_2D &LocalPosition) const { return LocalPosition+this->_GlobalMapPosition; }

		// Extra Data
		void SetDistrictMap(const DISTRICT_MAP_TYPE &NewMapData, const POS_2D &NewMapPos);

		void SetID(const DISTRICT_ID &NewID);
		const DISTRICT_ID &GetID() const { return this->_ID; }

		const POS_2D &GetGlobalMapPosition() const { return this->_GlobalMapPosition; }
		void SetGlobalMapPosition(const POS_2D &GlobalMapPos) { this->_GlobalMapPosition = GlobalMapPos; }

		const POS_2D &GetLocalPosInDistrict() const { return this->_LocalPositionInDistrict; }
		void SetLocalPosInDistrict(const POS_2D &LocalPosition) { this->_LocalPositionInDistrict = LocalPosition; }
		const POS_2D &&GetGlobalPosInDistrict() const { return std::move(this->ConvertToGlobalPosition(this->_LocalPositionInDistrict)); }

	private:

		POS_2D					_GlobalMapPosition;			// Position of map in global map
		POS_2D					_LocalPositionInDistrict;	// Position of one cell that is in districts
		DISTRICT_ID				_ID;						// ID of this district
		CONNECTED_ID_STORAGE	_ConnectedIDs;				// IDs of all surrounding districts that can be reached from this one

		friend class DistrictMapStorage;
};

#endif // DISTRICT_MAP_H
