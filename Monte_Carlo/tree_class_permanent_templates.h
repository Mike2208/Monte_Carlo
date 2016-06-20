#ifndef TREE_CLASS_PERMANENT_TEMPLATES_H
#define TREE_CLASS_PERMANENT_TEMPLATES_H

#include "standard_definitions.h"
#include "tree_class_permanent.h"

template<class T>
TreeClassPermanent<T>::TreeClassPermanent()
{
	this->Reset();
}

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
		nextBranchToPrint.front()->PrintNode();

		for(typename TreeNodePermanent<T>::CHILD_ID i = 0; i<nextBranchToPrint.front()->GetNumChildren(); ++i)
		{
			nextBranchToPrint.push(nextBranchToPrint.front()->GetChild(i));
		}

		nextBranchToPrint.pop();
	}
}

template<class T>
void TreeClass<T>::PrintTree_DepthFirst()
{
	TreeClass<T>::PrintTree_DepthFirst_Step(this->_Root);
}

template<class T>
void TreeClass<T>::PrintTree_DepthFirst_Step(const TreeNodePermanent<T> &CurNode)
{
	CurNode.PrintNode();

	for(const auto &childNode : CurNode.GetStorage())
	{
		TreeClass<T>::PrintTree_DepthFirst_Step(childNode);
		if(childNode.GetParent() != &CurNode)
			std::cout << "ERRRRRRRRRRRRRRRRRRRRRRRRROR";
	}
}

#endif			// ~DEBUG

#endif // TREE_CLASS_PERMANENT_TEMPLATES_H
