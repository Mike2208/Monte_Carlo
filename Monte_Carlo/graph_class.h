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
class GraphNode;

template<class T, class U>
class GraphClass
{
		friend class GraphNode<T,U>;

		typedef typename GRAPH_DEFINITIONS::STORAGES<T,U>::VERTICE_STORAGE	VERTICE_STORAGE;
		typedef typename GRAPH_DEFINITIONS::STORAGES<T,U>::VERTICE_ID		VERTICE_ID;
		typedef typename GRAPH_DEFINITIONS::STORAGES<T,U>::EDGE_STORAGE		EDGE_STORAGE;
		typedef typename GRAPH_DEFINITIONS::STORAGES<T,U>::EDGE_ID			EDGE_ID;

	public:
		GraphClass() : _Vertices(), _Edges() { this->Init(); }
		GraphClass(const GraphClass<T,U> &S) = default;
		GraphClass(GraphClass<T,U> &&S) = default;
		GraphClass<T,U> &operator=(const GraphClass<T,U> &S) = default;
		GraphClass<T,U> &operator=(GraphClass<T,U> &&S) = default;

		void Init()	{ this->_Vertices.clear(); this->_Edges.clear(); this->_Vertices.push_back(GraphVertice<T,U>()); }		// Clear any previous data and add empty vertice

	private:

		VERTICE_STORAGE	_Vertices;		// Storage of nodes
		EDGE_STORAGE	_Edges;			// Storage of edges
};

#include "graph_node.h"

#endif // GRAPH_CLASS_H
