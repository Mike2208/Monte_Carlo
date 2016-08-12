#ifndef FILE_TREE_NODE_TEMPLATES_CPP
#define FILE_TREE_NODE_TEMPLATES_CPP

#include "file_tree_node.h"
#include "file_tree.h"

template<class T>
int FileTreeNode<T>::MoveToChildNode(const unsigned int ChildID)
{
	if(this->_NodeData.NumChildren <= ChildID)
		return -1;

	return this->_FileData.GetNode(this->_NodeData.ChildrenOffset.at(ChildID), this->_NodeData);
}

template<class T>
int FileTreeNode<T>::MoveToParent()
{
	if(this->_NodeData.ParentOffset == FILE_TREE_NODE::INVALID_ID)
		return -1;

	return this->_FileData.GetNode(this->_NodeData.ParentOffset, this->_NodeData);
}

#endif
