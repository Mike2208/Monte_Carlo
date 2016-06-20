#ifndef GRAPH_EDGE_H
#define GRAPH_EDGE_H

#include "graph_definitions.h"

#include <array>
#include <vector>

#ifndef GRAPH_CLASS_H
template<class T, class U>
class GraphClass;
#endif

namespace GRAPH_CLASS
{
	template<class T, class U>
	class GraphEdge
	{
			typedef typename GRAPH_DEFINITIONS::STORAGES<T,U>::EDGE_STORAGE		EDGE_STORAGE;
			typedef typename GRAPH_DEFINITIONS::STORAGES<T,U>::EDGE_ID			EDGE_ID;
			typedef typename GRAPH_DEFINITIONS::STORAGES<T,U>::VERTICE_STORAGE	VERTICE_STORAGE;
			typedef typename GRAPH_DEFINITIONS::STORAGES<T,U>::VERTICE_ID		VERTICE_ID;

		public:

			GraphEdge(const U &Data) : _Data(Data), _ConnectedVertices() {}

			GraphEdge() = default;
			GraphEdge(const GraphEdge &S) = default;
			GraphEdge(GraphEdge &&S) = default;
			GraphEdge &operator=(const GraphEdge &S) = default;
			GraphEdge &operator=(GraphEdge &&S) = default;

			VERTICE_ID GetConnectedVerticeID(const bool FirstVertice) const		{ return this->_ConnectedVertices[FirstVertice]; }

			const U &GetEdgeData() const	{ return this->_Data; }		// Returns data of this vertice
			U &GetEdgeDataR() { return this->_Data; }					// Returns reference to data of this vertice

			void SetEdgeData(const U &NewData) { this->_Data = NewData; }		// Sets data stored heregr

		private:

			U	_Data;		// Data stored in this node

			std::array<VERTICE_ID,2>		_ConnectedVertices;		// Stores IDs of vertices connected with this edge

			template<class V, class W>
			friend class GraphClass;
	};
}

#endif // GRAPH_EDGE_H
