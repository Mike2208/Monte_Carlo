#ifndef DISTRICT_GRAPH_H
#define DISTRICT_GRAPH_H

/*	class DistrictGraph
 *		graph of ceonnections between districts
 */

#include "standard_definitions.h"
#include "district_map_definitions.h"
#include "graph_class.h"

class DistrictMapStorage;

namespace DISTRICT_GRAPH
{
	struct EDGE_DATA
	{

	};

	struct VERTICE_DATA
	{
		DISTRICT_MAP_DEFINITIONS::ID	ID;		// ID of map
	};
}

class DistrictGraph : public GraphClass<DISTRICT_GRAPH::VERTICE_DATA, DISTRICT_GRAPH::EDGE_DATA>
{
	public:

	private:

		friend class DistrictMapStorage;
};

#endif // DISTRICT_GRAPH_H
