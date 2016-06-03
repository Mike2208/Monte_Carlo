#ifndef TREE_CLASS_TEMPLATES_H
#define TREE_CLASS_TEMPLATES_H

#include "standard_definitions.h"
#include "tree_class.h"

#ifdef DEBUG	// DEBUG
#include <queue>

template<class T>
void TreeClass<T>::PrintTree_BreadthFirst()
{
	std::queue<TreeNode<T> *> nextBranchToPrint;

	TreeNode<T> *pCurNode = &(this->GetRoot());

	nextBranchToPrint.push(pCurNode);

	while(nextBranchToPrint.size() > 0)
	{
		nextBranchToPrint.front()->PrintNode();

		for(typename TreeNode<T>::CHILD_ID i = 0; i<nextBranchToPrint.front()->GetNumChildren(); ++i)
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
void TreeClass<T>::PrintTree_DepthFirst_Step(const TreeNode<T> &CurNode)
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

#endif // TREE_CLASS_TEMPLATES_H
