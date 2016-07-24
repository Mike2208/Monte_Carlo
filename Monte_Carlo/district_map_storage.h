#ifndef DISTRICT_MAP_STORAGE_H
#define DISTRICT_MAP_STORAGE_H

/*	class DistrictMapStorage
 *		stores all district maps, trees, and graphs
 */

#include "standard_definitions.h"
#include "district_map_definitions.h"
#include "district_map.h"
#include "district_graph.h"
#include "district_tree.h"

namespace DISTRICT_MAP_STORAGE
{
	typedef DISTRICT_MAP::ID_MAP ID_MAP;
	typedef DISTRICT_TREE::TREE_NODE::ID TREE_ID;

	struct STORAGE_ELEMENT
	{
		DistrictMap Map;
		TREE_ID TreePosition;

		STORAGE_ELEMENT(const DistrictMap &MapData) : Map(MapData), TreePosition(DISTRICT_TREE::INVALID_ID) {}
	};
}

struct DISTRICT_MAP_DEFINITIONS::STORAGE : public std::vector<DISTRICT_MAP_STORAGE::STORAGE_ELEMENT>
{
	void push_back(const DistrictMap &val) { static_cast<std::vector<DISTRICT_MAP_STORAGE::STORAGE_ELEMENT>*>(this)->push_back(DISTRICT_MAP_STORAGE::STORAGE_ELEMENT(val)); }
	DistrictMap &at (size_type n) { return static_cast<std::vector<DISTRICT_MAP_STORAGE::STORAGE_ELEMENT>*>(this)->at(n).Map; }
	const DistrictMap &at (size_type n) const { return static_cast<const std::vector<DISTRICT_MAP_STORAGE::STORAGE_ELEMENT>*>(this)->at(n).Map; }

	const DISTRICT_MAP_STORAGE::TREE_ID &GetTreeID(const DISTRICT_MAP_DEFINITIONS::ID &DistrictID) const { return static_cast<const std::vector<DISTRICT_MAP_STORAGE::STORAGE_ELEMENT>*>(this)->at(DistrictID).TreePosition; }
	void SetTreeID(const DISTRICT_MAP_DEFINITIONS::ID &DistrictID, const DISTRICT_MAP_STORAGE::TREE_ID &NewTreeID) { static_cast<std::vector<DISTRICT_MAP_STORAGE::STORAGE_ELEMENT>*>(this)->at(DistrictID).TreePosition = NewTreeID; }
};

class DistrictMapStorage
{
	public:
		typedef DISTRICT_MAP::MAP_TYPE			DISTRICT_MAP_TYPE;
		typedef DISTRICT_MAP_DEFINITIONS::ID	DISTRICT_ID;
		typedef DISTRICT_MAP_STORAGE::ID_MAP	DISTRICT_ID_MAP;

		// Divide Map into districts
		template<class T>
		void ResetDistricts(const Map2D<T> &OriginalMap);		// Resets everything and resizes _TotalMap to size of OriginalMap

		template<class T>
		int DivideMap(const DISTRICT_ID &DistrictToDivide, const Map2D<T> &OriginalMap, const T &CutOffValue);		// Divide given map into smaller areas (use CutOffValue to separate district)

		// Access data
		void GetDistrictAtGlobalPos(const POS_2D &Position, const DistrictMap *District) const;
		DISTRICT_ID GetDistrictIDAtGlobalPos(const POS_2D &Position) const;

		const DistrictGraph &GetDistrictGraph() const { return this->_DistrictGraph; }

		// Total Map Navigation
		void ResetTotalMap();			// Resets total map

	private:

		DISTRICT_ID_MAP						_IDMap;					// Contains ID of currently used map
		DISTRICT_MAP_DEFINITIONS::STORAGE	_DistrictStorage;		// Contains all maps
		DistrictTree						_DistrictTree;			// Tree with data on which districts are split into smaller ones
		DistrictGraph						_DistrictGraph;			// Graph of all connections
};

#endif // DISTRICT_MAP_STORAGE_H
