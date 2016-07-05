#ifndef TREE_CLASS_PERMANENT_TEMPLATES_H
#define TREE_CLASS_PERMANENT_TEMPLATES_H

#include "standard_definitions.h"
#include "tree_class_permanent.h"

#ifndef DEBUG
template<class T>
TreeClassPermanent<T>::TreeClassPermanent()
{
	this->Reset();
}
#endif

template<class T>
void TreeClassPermanent<T>::Reset()
{
	this->_NodeStorage.clear();
	this->_NodeStorage.push_back(TreeNodePermanent<T>(0,TREE_NODE_PERMANENT::INVALID_ID,T(), *this));		// Add empty node
}

#ifdef DEBUG	// DEBUG
#include <queue>

template<class T>
void TreeClassPermanent<T>::PrintTree_BreadthFirst()
{
	std::queue<TreeNodePermanent<T> *> nextBranchToPrint;

	TreeNodePermanent<T> *pCurNode = &(this->GetRoot());

	nextBranchToPrint.push(pCurNode);

	while(nextBranchToPrint.size() > 0)
	{
		const TreeNodePermanent<T> *const pCurNode = nextBranchToPrint.front();
		pCurNode->PrintNode();

		for(typename TreeNodePermanent<T>::ID i = 0; i<pCurNode->GetNumChildren(); ++i)
		{
			const typename TreeNodePermanent<T>::ID childID = pCurNode->GetChildID(i);
			nextBranchToPrint.push(&(this->_NodeStorage.at(childID)));
		}

		nextBranchToPrint.pop();
	}
}

template<class T>
void TreeClassPermanent<T>::PrintTree_DepthFirst()
{
	TreeClassPermanent<T>::PrintTree_DepthFirst_Step(this->GetRoot());
}

template<class T>
void TreeClassPermanent<T>::PrintTree_DepthFirst_Step(const TreeNodePermanent<T> &CurNode)
{
	CurNode.PrintNode();

	for(const auto &childID : CurNode.GetStorage())
	{
		TreeClassPermanent<T>::PrintTree_DepthFirst_Step(this->_NodeStorage.at(childID));
		if(this->_NodeStorage.at(childID).GetParentID() != CurNode._NodeID)
			std::cout << "ERRRRRRRRRRRRRRRRRRRRRRRRROR";
	}
}

#endif			// ~DEBUG

#endif // TREE_CLASS_PERMANENT_TEMPLATES_H
