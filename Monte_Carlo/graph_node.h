#ifndef GRAPH_NODE_H
#define GRAPH_NODE_H

#include "graph_definitions.h"
#include "graph_class.h"
#include "graph_vertice.h"
#include "graph_edge.h"

template<class T, class U>
class GraphNode
{
		typedef typename GRAPH_DEFINITIONS::STORAGES<T,U>::VERTICE_STORAGE	VERTICE_STORAGE;
		typedef typename GRAPH_DEFINITIONS::STORAGES<T,U>::EDGE_STORAGE		EDGE_STORAGE;

	public:
		typedef typename GRAPH_DEFINITIONS::STORAGES<T,U>::VERTICE_ID		VERTICE_ID;
		typedef typename GRAPH_DEFINITIONS::STORAGES<T,U>::EDGE_ID			EDGE_ID;

		GraphNode(GraphClass<T,U> &GraphData) : _VerticeStorage(GraphData._Vertices), _EdgeStorage(GraphData._Edges), _CurVertice(&(GraphData._Vertices.at(0))) {}
		GraphNode(const GraphNode<T,U> &S) = default;
		GraphNode(GraphNode<T,U> &&S) = default;
		GraphNode<T,U> &operator=(const GraphNode<T,U> &S) = default;
		GraphNode<T,U> &operator=(GraphNode<T,U> &&S) = default;

		VERTICE_ID GetNumConnectedVertices() const	{ return this->_CurVertice->GetNumConnectedVertices(); }
		GRAPH_CLASS::GraphVertice<T,U> *GetConnectedVertice(const VERTICE_ID &ConnectedVerticeNum) const { return &(this->_VerticeStorage.at(this->_CurVertice->GetConnectedVerticeID(ConnectedVerticeNum))); }

		const U &GetEdgeData(const EDGE_ID &ConnectedEdgeNum) const { return this->_EdgeStorage.at(this->_CurVertice->GetConnectedEdgeID(ConnectedEdgeNum)).GetEdgeData(); }
		U &GetEdgeDataR(const EDGE_ID &ConnectedEdgeNum) { return this->_EdgeStorage.at(this->_CurVertice->GetConnectedEdgeID(ConnectedEdgeNum)).GetEdgeDataR(); }

		void SetEdgeData(const EDGE_ID &ConnectedEdgeNum, const U &NewData) { return this->_EdgeStorage.at(this->_CurVertice->GetConnectedEdgeID(ConnectedEdgeNum)).SetEdgeData(NewData); }

		const T &GetVerticeData() const { return this->_CurVertice->GetVerticeData(); }
		T &GetVerticeDataR() { return this->_CurVertice->GetVerticeDataR(); }

		void SetVerticeData(const T &NewData) { this->_CurVertice->SetVerticeData(NewData); }

	private:

		VERTICE_STORAGE		&_VerticeStorage;		// Reference to vertice storage
		EDGE_STORAGE		&_EdgeStorage;			// Reference to edge storage

		GRAPH_CLASS::GraphVertice<T,U>	*_CurVertice;			// Pointer to current vertice
};

#endif // GRAPH_NODE_H
