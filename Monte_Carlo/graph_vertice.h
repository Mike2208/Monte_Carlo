#ifndef GRAPH_VERTICE_H
#define GRAPH_VERTICE_H

/*	class GraphVertices
 *		stores one vertice, its data and its connections
 */

#include <vector>

#include "graph_definitions.h"

//#ifndef GRAPH_CLASS_H
template<class T, class U>
class GraphClass;
//#endif

namespace GRAPH_CLASS
{
	template<class T, class U>
	class GraphVertice
	{
			typedef typename GRAPH_DEFINITIONS::STORAGES<T,U>::VERTICE_STORAGE	VERTICE_STORAGE;
			typedef typename GRAPH_DEFINITIONS::STORAGES<T,U>::VERTICE_ID		VERTICE_ID;
			typedef typename GRAPH_DEFINITIONS::STORAGES<T,U>::EDGE_STORAGE		EDGE_STORAGE;
			typedef typename GRAPH_DEFINITIONS::STORAGES<T,U>::EDGE_ID			EDGE_ID;

		public:

			GraphVertice(const T &Data) : _Data(Data), _ConnectedVertices(), _ConnectedEdges() {}

			GraphVertice() = default;
			GraphVertice(const GraphVertice &S) = default;
			GraphVertice(GraphVertice &&S) = default;
			GraphVertice &operator=(const GraphVertice &S) = default;
			GraphVertice &operator=(GraphVertice &&S) = default;

			VERTICE_ID GetNumConnectedVertices() const	{ return this->_ConnectedVertices.size(); }
			VERTICE_ID GetConnectedVerticeID(const VERTICE_ID &ConnectedVerticeNum) const { return this->_ConnectedVertices.at(ConnectedVerticeNum); }

			EDGE_ID GetConnectedEdgeID(const VERTICE_ID &ConnectedVerticeNum) const { return this->_ConnectedEdges.at(ConnectedVerticeNum); }

			const T &GetVerticeData() const	{ return this->_Data; }	// Returns data of this vertice
			T		&GetVerticeDataR() { return this->_Data; }			// Returns reference to data of this vertice

			void SetVerticeData(const T &NewData)	{ this->_Data = NewData; }		// Sets data stored here

			EDGE_ID GetConnectionID(const VERTICE_ID &VerticeID)const;					// Checks whether these two vertice IDs are connected and returns the edge ID of the connection

		private:

			T	_Data;		// Data stored in this node

			std::vector<VERTICE_ID>		_ConnectedVertices;		// Stores IDs of connected vertices
			std::vector<EDGE_ID>		_ConnectedEdges;		// Stores IDs of connected edges

			template<class V, class W>
			friend class GraphClass;
	};
}

#include "graph_vertice_templates.h"

#endif // GRAPH_VERTICE_H
