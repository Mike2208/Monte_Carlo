#ifndef DISTRICT_MAP_H
#define DISTRICT_MAP_H

/*	class DistrictMap
 *		map of one district
 */

#include "standard_definitions.h"
#include "district_map_definitions.h"

#include "map_2d.h"

#include <vector>
#include <array>

namespace DISTRICT_MAP
{
	typedef DISTRICT_MAP_DEFINITIONS::ID		ID;
	typedef DISTRICT_MAP_DEFINITIONS::ID_MAP	ID_MAP;

	typedef float	AVERAGE_TYPE;

	typedef bool		MAP_CELL_TYPE;
	typedef Map2D<MAP_CELL_TYPE>	MAP_TYPE;

	typedef std::vector<ID>		CONNECTED_ID_STORAGE;

	const MAP_CELL_TYPE IN_DISTRICT = true;
	const ID INVALID_DISTRICT_ID = DISTRICT_MAP_DEFINITIONS::INVALID_DISTRICT_ID;

	// Struct to temporarily store size of district (Min- and MaxPos form rectangle with all cells in them)
	struct DISTRICT_SIZE
	{
		ID DistrictID;			// ID of district
		POS_2D MinPos;			// Minimum position of cells
		POS_2D MaxPos;			// Maximum position of cells
		POS_2D PosInDistrict;	// Position with districtID

		DISTRICT_SIZE(const ID &NewID, const POS_2D &NewPos) : DistrictID(NewID), MinPos(NewPos), MaxPos(NewPos), PosInDistrict(NewPos) {}
		DISTRICT_SIZE() = delete;
		DISTRICT_SIZE(const DISTRICT_SIZE &S) = default;
		DISTRICT_SIZE(DISTRICT_SIZE &&S) = default;
		DISTRICT_SIZE &operator=(const DISTRICT_SIZE &S) = default;
		DISTRICT_SIZE &operator=(DISTRICT_SIZE &&S) = default;

		void ComparePos(const POS_2D &Pos);
		POS_2D_TYPE GetWidth()const;
		POS_2D_TYPE GetHeight()const;
	};

	typedef std::vector<DISTRICT_SIZE> DISTRICT_SIZE_VECTOR_STORAGE_TYPE;
	struct DISTRICT_SIZE_VECTOR : public DISTRICT_SIZE_VECTOR_STORAGE_TYPE
	{
		typedef DISTRICT_SIZE_VECTOR_STORAGE_TYPE STORAGE_TYPE;

		DISTRICT_SIZE *FindID(const ID &IDToFind);
		STORAGE_TYPE::size_type FindIDIterator(const ID &IDToFind);
	};

	typedef std::vector<POS_2D> POS_IN_DISTRICT;
	typedef std::array<POS_2D,2> CONNECTED_POS;

	struct ID_CONNECTION
	{
		std::array<ID,2> IDs;					// Array with the two connected IDs
		CONNECTED_POS ConnectionPos;	// Array with the connection positions

		ID_CONNECTION(const ID DistrictID1, const ID DistrictID2, const POS_2D &ConnectionPos1, const POS_2D &ConnectionPos2) : IDs({{DistrictID1, DistrictID2}}), ConnectionPos({{ConnectionPos1, ConnectionPos2}}) {}
	};

	struct ID_CONNECTION_VECTOR : public std::vector<ID_CONNECTION>
	{
		bool IsConnected(const ID DistrictID1, const ID DistrictID2) const;
		void GetAllConnectedIDs(const ID DistrictID, std::vector<ID> &ConnectedIDs) const;
		void AddConnection(const ID DistrictID1, const ID DistrictID2, const POS_2D &ConnectionPos1, const POS_2D &ConnectionPos2);
	};

	typedef std::vector<CONNECTED_POS> CONNECTED_POS_VECTOR;
	struct ADJACENT_DISTRICT
	{
		ID AdjacentDistrictID;			// Adjacent ID
		CONNECTED_POS_VECTOR ConnectionPositions;		// Position of connection (1st element is in original district, 2nd element is in adjacent district)

		void AddPosition(const POS_2D &OriginalDistrictPos, const POS_2D &AdjacentDistrictPos);
		bool IsConnectedToAdjacentDistrict(const POS_2D &ConnectionPos) const;

		ADJACENT_DISTRICT(const ID &_AdjacentDistrictID) : AdjacentDistrictID(_AdjacentDistrictID), ConnectionPositions() {}
		ADJACENT_DISTRICT(const ID &_AdjacentDistrictID, const CONNECTED_POS_VECTOR &_ConnectionPositions) : AdjacentDistrictID(_AdjacentDistrictID), ConnectionPositions(_ConnectionPositions) {}
	};

	struct ADJACENT_DISTRICT_VECTOR : public std::vector<ADJACENT_DISTRICT>
	{
		void AddConnection(const ID &AdjacentID, const POS_2D &OriginalDistrictPos, const POS_2D &AdjacentDistrictPos);
		void GetConnectionData(const ID &AdjacentID, CONNECTED_POS_VECTOR &Connections) const;

		bool IsConnectedToDistrict(const ID &AdjacentID) const;
	};
}

class DistrictMap : public DISTRICT_MAP::MAP_TYPE
{
	public:
		typedef DISTRICT_MAP::ID						DISTRICT_ID;
		typedef DISTRICT_MAP::MAP_CELL_TYPE				DISTRICT_MAP_CELL_TYPE;
		typedef DISTRICT_MAP::MAP_TYPE					DISTRICT_MAP_TYPE;
		typedef DISTRICT_MAP::CONNECTED_ID_STORAGE		CONNECTED_ID_STORAGE;
		typedef DISTRICT_MAP::ADJACENT_DISTRICT_VECTOR	ADJACENT_DISTRICT_VECTOR;

		DistrictMap(const DISTRICT_ID &DistrictID, const POS_2D &GlobalMapPosition, const POS_2D_TYPE &NewWidth, const POS_2D_TYPE &NewHeight, const DISTRICT_MAP_CELL_TYPE &DefaultCellValue) : DISTRICT_MAP::MAP_TYPE(NewWidth, NewHeight, DefaultCellValue), _GlobalMapPosition(GlobalMapPosition), _ID(DistrictID) {}
		DistrictMap(const DISTRICT_MAP::DISTRICT_SIZE &Size) : DISTRICT_MAP::MAP_TYPE(Size.MaxPos.X-Size.MinPos.X+1, Size.MaxPos.Y-Size.MinPos.Y+1, !DISTRICT_MAP::IN_DISTRICT), _GlobalMapPosition(Size.MinPos), _ID(Size.DistrictID) {}

		DistrictMap() = default;
		DistrictMap(const DistrictMap &S) = default;
		DistrictMap(DistrictMap &&S) = default;
		DistrictMap &operator=(const DistrictMap &S) = default;
		DistrictMap &operator=(DistrictMap &&S) = default;

		// Get sizes of all IDs in Map
		static void GetAllDistrictSizesOfIDMap(const DISTRICT_MAP::ID_MAP &IDMap, DISTRICT_MAP::DISTRICT_SIZE_VECTOR &DistrictSizes);

		bool IsGlobalCellInDistrict(const POS_2D &GlobalPosition) const;

		// Set District from IDMap
		void SetDistrictFromIDMap(const DISTRICT_MAP::ID_MAP &IDMap, const POS_2D &PosOfOneID, DISTRICT_MAP::POS_IN_DISTRICT *GlobalPosInDistrict = nullptr);
		void SetFreeDistrict(const POS_2D_TYPE &Width, const POS_2D_TYPE &Height, const DISTRICT_ID &ID);

		// Global pixel data
		DISTRICT_MAP_CELL_TYPE GetGlobalPixel(const POS_2D &GlobalPosition) const { return this->GetPixel(GlobalPosition-this->_GlobalMapPosition); }
		int GetGlobalPixel(const POS_2D &GlobalPosition, DISTRICT_MAP_CELL_TYPE &Value) const { return this->GetPixel(GlobalPosition-this->_GlobalMapPosition, Value); }

		POS_2D ConvertToLocalPosition(const POS_2D &GlobalPosition) const { return GlobalPosition-this->_GlobalMapPosition; }
		POS_2D ConvertToGlobalPosition(const POS_2D &LocalPosition) const { return LocalPosition+this->_GlobalMapPosition; }

		// Adjacent District Data
		void GetConnectedIDs(std::vector<DISTRICT_ID> &AdjacentIDs) const;
		void GetGlobalConnectionPositions(const DISTRICT_ID &AdjacentID, std::vector<POS_2D> ConnectionPositions) const;		// Returns connection positions in this district

		bool IsConnectedToDistrict(const DISTRICT_ID &AdjacentDistrictID) const { return this->_AdjacentDistricts.IsConnectedToDistrict(AdjacentDistrictID); }

		// Extra Data
		const DISTRICT_ID &GetID() const { return this->_ID; }
		void ChangeID(const DISTRICT_ID &NewID, DISTRICT_MAP::ID_MAP &GlobalIDMap);

		const POS_2D &GetGlobalMapPosition() const { return this->_GlobalMapPosition; }
		const POS_2D &GetLocalPosInDistrict() const { return this->_LocalPositionInDistrict; }
		POS_2D GetGlobalPosInDistrict() const { return this->ConvertToGlobalPosition(this->_LocalPositionInDistrict); }

		const ADJACENT_DISTRICT_VECTOR &GetAdjacentConnections() const { return this->_AdjacentDistricts; }

	private:

		POS_2D						_GlobalMapPosition;			// Position of map in global map
		POS_2D						_LocalPositionInDistrict;	// Position of one cell that is in districts
		DISTRICT_ID					_ID;						// ID of this district
		CONNECTED_ID_STORAGE		_ConnectedIDs;				// IDs of all surrounding districts that can be reached from this one
		ADJACENT_DISTRICT_VECTOR	_AdjacentDistricts;			// Connections to adjacent districts

		void SetDistrictMap(const DISTRICT_MAP_TYPE &NewMapData, const POS_2D &NewMapPos);
		void SetGlobalPixel(const POS_2D &GlobalPosition, const DISTRICT_MAP_CELL_TYPE &Value) { this->SetPixel(GlobalPosition-this->_GlobalMapPosition, Value); }
		void SetID(const DISTRICT_ID &NewID);

		void SetGlobalMapPosition(const POS_2D &GlobalMapPos) { this->_GlobalMapPosition = GlobalMapPos; }
		void SetLocalPosInDistrict(const POS_2D &LocalPosition) { this->_LocalPositionInDistrict = LocalPosition; }

		friend class DistrictMapStorage;
};

#endif // DISTRICT_MAP_H
