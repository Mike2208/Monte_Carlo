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
	typedef std::vector<DistrictGraph>	GRAPH_STORAGE;			// Storage of all different graphs of different tree depths
}

class DistrictMapStorage
{
	public:
		typedef DISTRICT_MAP::MAP_TYPE			DISTRICT_MAP_TYPE;
		typedef DISTRICT_MAP_DEFINITIONS::ID	DISTRICT_ID;

		//
		template<class T>
		void ResetDistricts(const Map2D<T> &OriginalMap);		// Resets everything and resizes _TotalMap to size of OriginalMap

		template<class T>
		void DivideMap(const DISTRICT_ID &DistrictToDivide, const Map2D<T> &OriginalMap, const T &CutOffValue);		// Divide given map into smaller areas (use CutOffValue to separate district)

		// Total Map Navigation
		void ResetTotalMap();			// Resets total map

		int TotalMapMoveUp();			// Move total map up one depth
		int TotalMapMoveDown();		// Move total map down one depth

		int TotalMapMoveIDDown(const DISTRICT_ID &IDToMoveDown);
		int TotalMapMoveIDUp(const DISTRICT_ID &IDToMoveUp);

		// Get Navigation data
		const DISTRICT_MAP_TYPE &GetTotalMapData() const { return this->_TotalMap; }
		const DistrictGraph		&GetTotalDistrictGraph() const { return this->_TotalGraph; }

	private:

		DISTRICT_MAP_TYPE						_TotalMap;				// Total map that can be created through dividing districts
		DistrictGraph							_TotalGraph;			// Total graph of connections
		DISTRICT_MAP_DEFINITIONS::STORAGE		_DistrictStorage;		// Contains all maps
		DistrictTree							_DistrictTree;			// Tree with data on which districts are split into smaller ones
		DISTRICT_MAP_STORAGE::GRAPH_STORAGE		_GraphStorage;			// Contains connection graphs of all depths of a tree
};

#endif // DISTRICT_MAP_STORAGE_H
