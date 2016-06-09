#ifndef GRAPH_DEFINITIONS_H
#define GRAPH_DEFINITIONS_H

#include <vector>

namespace GRAPH_CLASS
{
	template<class T, class U>
	class GraphEdge;

	template<class T, class U>
	class GraphVertice;
}

namespace GRAPH_DEFINITIONS
{
	template<class T, class U>
	struct STORAGES
	{
		typedef typename std::vector<GRAPH_CLASS::GraphVertice<T,U>> VERTICE_STORAGE;
		typedef typename VERTICE_STORAGE::size_type VERTICE_ID;

		typedef typename std::vector<GRAPH_CLASS::GraphEdge<T,U>> EDGE_STORAGE;
		typedef typename VERTICE_STORAGE::size_type EDGE_ID;
	};
}

#endif // GRAPH_DEFINITIONS_H
