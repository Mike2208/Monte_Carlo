#ifndef GRAPH_EDGE_H
#define GRAPH_EDGE_H

#include "graph_definitions.h"

template<class T, class U>
class GraphEdge
{
		typedef typename GRAPH_DEFINITIONS::STORAGES<T,U>::EDGE_STORAGE		EDGE_STORAGE;
		typedef typename GRAPH_DEFINITIONS::STORAGES<T,U>::EDGE_ID			EDGE_ID;
		typedef typename GRAPH_DEFINITIONS::STORAGES<T,U>::VERTICE_STORAGE	VERTICE_STORAGE;
		typedef typename GRAPH_DEFINITIONS::STORAGES<T,U>::VERTICE_ID		VERTICE_ID;

	public:

		GraphEdge(EDGE_STORAGE &Storage) : _Storage(Storage) {}

		const U &GetEdgeData() const	{ return this->_Data; }	// Returns data of this vertice
		U &GetEdgeDataR() { return this->_Data; }			// Returns reference to data of this vertice

		void SetEdgeData(const U &NewData)	{ this->_Data = NewData; }		// Sets data stored here

	private:

		EDGE_STORAGE &_Storage;			// Reference to storage vector of all vertices
		VERTICE_ID		_VerticeID;		// ID of current vertice

		U	_Data;		// Data stored in this node

		std::vector<VERTICE_ID>		_ConnectedVertices;		// Stores IDs of connected vertices
		std::vector<EDGE_ID>		_ConnectedEdges;		// Stores IDs of connected edges
};

#endif // GRAPH_EDGE_H
