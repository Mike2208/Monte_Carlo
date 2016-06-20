#ifndef GRAPH_CLASS_TEMPLATES_H
#define GRAPH_CLASS_TEMPLATES_H

#include "graph_class.h"

template<class T, class U>
typename GRAPH_DEFINITIONS::STORAGES<T,U>::EDGE_ID GraphClass<T,U>::AddConnection(const VERTICE_ID &Vertice1, const VERTICE_ID &Vertice2, const U &NewEdgeData)
{
	// Check wehther this connection exists already
	EDGE_ID tmpID = this->_Vertices.at(Vertice1).GetConnectionID(Vertice2);

	if(tmpID == GRAPH_DEFINITIONS::STORAGES<T,U>::INVALID_EDGE_ID())
	{
		// Create new edge if this one doesn't exist yet
		this->_Vertices.at(Vertice1)._ConnectedVertices.push_back(Vertice2);		// Connect vertice 1 to vertice 2
		this->_Vertices.at(Vertice2)._ConnectedVertices.push_back(Vertice1);		// Connect vertice 2 to vertice 1

		const EDGE_ID newEdgeID = this->_Edges.size();
		this->_Edges.push_back(GRAPH_CLASS::GraphEdge<T,U>(NewEdgeData));			// Create new edge
		this->_Vertices.at(Vertice1)._ConnectedEdges.push_back(newEdgeID);
		this->_Vertices.at(Vertice2)._ConnectedEdges.push_back(newEdgeID);

		return newEdgeID;
	}
	else
	{
		this->_Edges.at(tmpID).SetEdgeData(NewEdgeData);		// If edge already exists, just overwrite connection data

		return tmpID;
	}
}

template<class T, class U>
typename GRAPH_DEFINITIONS::STORAGES<T,U>::VERTICE_ID GraphClass<T,U>::AddVertice(const T &NewVerticeData)
{
	// Save new edge position
	VERTICE_ID newVerticeID = this->_Vertices.size();

	// Add new vertice to storage
	this->_Vertices.push_back(GRAPH_CLASS::GraphVertice<T,U>(NewVerticeData));

	return newVerticeID;
}

template<class T, class U>
void GraphClass<T,U>::CombineWithGraph(const GraphClass<T,U> &OtherGraph, const std::vector<COMBINATION_DATA> &ConnectionPoints)
{
	// Save old edge size
	const VERTICE_ID oldVerticeSize = this->_Vertices.size();
	const VERTICE_ID oldEdgeSize = this->_Edges.size();

	// Add other graph vertices to this one
	this->_Vertices.resize(this->_Vertices.size()+OtherGraph._Vertices.size());
	for(VERTICE_ID curVerticeID = 0; curVerticeID < OtherGraph._Vertices.size(); ++curVerticeID)
	{
		GRAPH_CLASS::GraphVertice<T,U> tmpVertice(OtherGraph._Vertices.at(curVerticeID));
		for(auto &curID : tmpVertice._ConnectedVertices)
			curID += oldVerticeSize;
		for(auto &curID : tmpVertice._ConnectedEdges)
			curID += oldEdgeSize;

		this->_Vertices.at(oldVerticeSize + curVerticeID) = std::move(tmpVertice);		// Copy vertices individually
	}

	// Do the same for edges
	this->_Edges.resize(this->_Edges.size()+OtherGraph._Edges.size());
	for(EDGE_ID curEdgeID = 0; curEdgeID < OtherGraph._Edges.size(); ++curEdgeID)
	{
		GRAPH_CLASS::GraphEdge<T,U> tmpEdge(OtherGraph._Edges.at(curEdgeID));
		for(auto &curID : tmpEdge._ConnectedVertices)
			curID += oldEdgeSize;

		this->_Edges.at(curEdgeID+oldEdgeSize) = std::move(tmpEdge);		// Copy edges individually
	}

	// Now add connections between old and new graph
	for(const auto &curConnection : ConnectionPoints)
	{
		this->AddConnection(curConnection.OtherVertice+oldVerticeSize, curConnection.VerticeToConnectTo, curConnection.NewEdgeData);
	}
}

#endif // GRAPH_CLASS_TEMPLATES_H
