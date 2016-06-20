#ifndef TREE_NODE_PERMANENT_TEMPLATES_H
#define TREE_NODE_PERMANENT_TEMPLATES_H

#include "standard_definitions.h"
#include "tree_node_permanent.h"

template<class T>
TreeNodePermanent<T>::TreeNodePermanent(const ID NodeID, const ID &ParentID, const T &NewData, TreeClassPermanent<T> &StorageClass) : _ParentID(ParentID), _ChildIDs(), _Data(NewData), _NodeID(NodeID), _StorageClass(StorageClass)
{
#ifdef DEBUG	// DEBUG
	if(Parent == nullptr)
		this->_NodeDepth = 0;
	else
		this->_NodeDepth = Parent->_NodeDepth+1;
#endif			// ~DEBUG
}

template<class T>
TreeNodePermanent<T>::TreeNodePermanent(const TreeNodePermanent<T> &S) : _ParentID(S._ParentID), _ChildIDs(S._ChildIDs), _Data(S._Data), _NodeID(S._NodeID), _StorageClass(S._StorageClass)
{
#ifdef DEBUG	// DEBUG
	this->_NodeDepth = S._NodeDepth;
#endif			// ~DEBUG
}

template<class T>
TreeNodePermanent<T>::TreeNodePermanent(TreeNodePermanent<T> &&S) : _ParentID(std::move(S._ParentID)), _ChildIDs(std::move(S._ChildIDs)), _Data(std::move(S._Data)), _NodeID(S._NodeID), _StorageClass(S._StorageClass)
{
#ifdef DEBUG	// DEBUG
	this->_NodeDepth = std::move(S._NodeDepth);
#endif			// ~DEBUG
}

template<class T>
TREE_NODE_PERMANENT::ID TreeNodePermanent<T>::AddChild(const T &NewData)
{
	const ID newChildID = this->_StorageClass._NodeStorage.size();
	this->_StorageClass._NodeStorage.push_back(std::move(TreeNodePermanent<T>(this->_NodeID, this->_ParentID, NewData, this->_StorageClass)));
	this->_ChildrenIDs.push_back(newChildID);

	return newChildID;
}

#ifdef DEBUG	// DEBUG
template<class T>
void TreeNodePermanent<T>::PrintNode() const
{
	std::cout << PrintTabs(this->_NodeDepth) << "Node Address: " << this << std::endl;
	std::cout << PrintTabs(this->_NodeDepth) << "Node Parent: " << this->_Parent << std::endl;
	std::cout << PrintTabs(this->_NodeDepth) << "Num Children: " << this->_Children.size() << std::endl;
	std::cout << PrintTabs(this->_NodeDepth) << "Node Depth: " << this->_NodeDepth <<std::endl;
	std::cout << PrintTabs(this->_NodeDepth) << "Node Data: " << std::endl;
	this->_Data.PrintNodeData(this->_NodeDepth);
	std::cout << std::endl;
}

#endif			// ~DEBUG

#endif // TREE_NODE_PERMANENT_TEMPLATES_H
