#ifndef GRAPH_CLASS_H
#define GRAPH_CLASS_H

/*	class Graph_Class
 *		stores a graph
 */

#include "graph_edge.h"
#include "graph_vertice.h"
#include <vector>

namespace GRAPH_CLASS
{
	// Struct with data to connect two graphs
	template<class T, class U>
	struct COMBINATION_DATA
	{
		typename GRAPH_DEFINITIONS::STORAGES<T,U>::VERTICE_ID	OtherVertice;		// Vertices to connect
		typename GRAPH_DEFINITIONS::STORAGES<T,U>::VERTICE_ID	VerticeToConnectTo;

		U NewEdgeData;
	};
}

template<class T, class U>
class GraphNode;

template<class T, class U>
class GraphClass
{
		//friend class GraphNode;

		typedef typename GRAPH_DEFINITIONS::STORAGES<T,U>::VERTICE_STORAGE	VERTICE_STORAGE;

		typedef typename GRAPH_DEFINITIONS::STORAGES<T,U>::EDGE_STORAGE		EDGE_STORAGE;
		typedef typename GRAPH_DEFINITIONS::STORAGES<T,U>::EDGE_ID			EDGE_ID;

	public:
		typedef typename GRAPH_DEFINITIONS::STORAGES<T,U>::VERTICE_ID		VERTICE_ID;
		typedef GRAPH_CLASS::COMBINATION_DATA<T,U>	COMBINATION_DATA;

		GraphClass() : _Vertices(), _Edges() { this->Init(); }
		GraphClass(const GraphClass<T,U> &S) = default;
		GraphClass(GraphClass<T,U> &&S) = default;
		GraphClass<T,U> &operator=(const GraphClass<T,U> &S) = default;
		GraphClass<T,U> &operator=(GraphClass<T,U> &&S) = default;

		void Init()	{ this->_Vertices.clear(); this->_Edges.clear(); this->_Vertices.push_back(GRAPH_CLASS::GraphVertice<T,U>()); }		// Clear any previous data and add empty vertice

		void CombineWithGraph(const GraphClass &OtherGraph, const std::vector<COMBINATION_DATA> &ConnectionPoints);		// Adds the given graph to this one using the connection points

		EDGE_ID AddConnection(const VERTICE_ID &Vertice1, const VERTICE_ID &Vertice2, const U &NewEdgeData);
		VERTICE_ID AddVertice(const T &NewVerticeData);

	private:

		VERTICE_STORAGE	_Vertices;		// Storage of nodes
		EDGE_STORAGE	_Edges;			// Storage of edges

		template<class V, class W>
		friend class GraphNode;
};

#include "graph_class_templates.cpp"

#include "graph_node.h"

#endif // GRAPH_CLASS_H
