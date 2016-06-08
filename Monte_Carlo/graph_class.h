#ifndef GRAPH_CLASS_H
#define GRAPH_CLASS_H

/*	class Graph_Class
 *		stores a graph
 */

#include "graph_vertice.h"
#include <vector>

namespace GRAPH_CLASS
{
}

template<class T, class U>
class GraphClass
{
		typedef typename GRAPH_DEFINITIONS::STORAGES<T,U>::VERTICE_STORAGE	VERTICE_STORAGE;
		typedef typename GRAPH_DEFINITIONS::STORAGES<T,U>::VERTICE_ID		VERTICE_ID;
		typedef typename GRAPH_DEFINITIONS::STORAGES<T,U>::EDGE_STORAGE		EDGE_STORAGE;
		typedef typename GRAPH_DEFINITIONS::STORAGES<T,U>::EDGE_ID			EDGE_ID;

	public:



	private:

		VERTICE_STORAGE	_Vertices;		// Storage of nodes
		EDGE_STORAGE	_Edges;			// Storage of edges
};

#endif // GRAPH_CLASS_H
