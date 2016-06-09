#ifndef GRAPH_VERTICE_H
#define GRAPH_VERTICE_H

/*	class GraphVertices
 *		stores one vertice, its data and its connections
 */

#include <vector>

#include "graph_definitions.h"

namespace GRAPH_VERTICE
{
}

template<class T, class U>
class GraphVertice
{
		typedef typename GRAPH_DEFINITIONS::STORAGES<T,U>::VERTICE_STORAGE	VERTICE_STORAGE;
		typedef typename GRAPH_DEFINITIONS::STORAGES<T,U>::VERTICE_ID		VERTICE_ID;
		typedef typename GRAPH_DEFINITIONS::STORAGES<T,U>::EDGE_STORAGE		EDGE_STORAGE;
		typedef typename GRAPH_DEFINITIONS::STORAGES<T,U>::EDGE_ID			EDGE_ID;

	public:

		VERTICE_ID GetNumConnectedVertices() const	{ return this->_ConnectedVertices.size(); }
		VERTICE_ID GetConnectedVerticeID(const VERTICE_ID &ConnectedVerticeNum) const { return this->_ConnectedVertices.at(ConnectedVerticeNum); }

		EDGE_ID GetConnectedEdgeID(const VERTICE_ID &ConnectedVerticeNum) const { return this->_ConnectedEdges.at(ConnectedVerticeNum); }

		const T &GetVerticeData() const	{ return this->_Data; }	// Returns data of this vertice
		T		&GetVerticeDataR() { return this->_Data; }			// Returns reference to data of this vertice

		void SetVerticeData(const T &NewData)	{ this->_Data = NewData; }		// Sets data stored here

	private:

		T	_Data;		// Data stored in this node

		std::vector<VERTICE_ID>		_ConnectedVertices;		// Stores IDs of connected vertices
		std::vector<EDGE_ID>		_ConnectedEdges;		// Stores IDs of connected edges
};

#endif // GRAPH_VERTICE_H
