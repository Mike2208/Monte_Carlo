#ifndef GRAPH_DEFINITIONS_H
#define GRAPH_DEFINITIONS_H

#include <vector>

namespace GRAPH_DEFINITIONS
{
	template<class T, class U>
	struct STORAGES
	{
		typedef typename std::vector<T> VERTICE_STORAGE;
		typedef typename VERTICE_STORAGE::size_type VERTICE_ID;

		typedef typename std::vector<U> EDGE_STORAGE;
		typedef typename VERTICE_STORAGE::size_type EDGE_ID;
	};
}

#endif // GRAPH_DEFINITIONS_H
