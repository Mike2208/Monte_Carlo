#ifndef GRAPH_VERTICE_TEMPLATES_H
#define GRAPH_VERTICE_TEMPLATES_H

#include "graph_vertice.h"

namespace GRAPH_CLASS
{
	template<class T, class U>
	typename GRAPH_DEFINITIONS::STORAGES<T,U>::EDGE_ID GraphVertice<T,U>::GetConnectionID(const VERTICE_ID &VerticeID) const
	{
		for(VERTICE_ID i = 0; i<this->_ConnectedVertices.size(); ++i)
		{
			if(this->_ConnectedVertices.at(i) == VerticeID)
				return this->_ConnectedEdges.at(i);
		}

		return GRAPH_DEFINITIONS::STORAGES<T,U>::INVALID_EDGE_ID();		// Return invalid ID if not found
	}
}

#endif // GRAPH_VERTICE_TEMPLATES_H
