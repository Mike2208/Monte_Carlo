#ifndef TREE_NODE_TEMPLATES_H
#define TREE_NODE_TEMPLATES_H

#include "tree_node.h"

template<class T>
TreeNode<T>::TreeNode(const TreeNode<T> &S) noexcept : _Parent(S._Parent), _Children(S._Children), _Data(S._Data)
{
	this->ResetChildPointersRecursive();
}

template<class T>
TreeNode<T>::TreeNode(TreeNode<T> &&S) noexcept : _Parent(std::move(S._Parent)), _Children(std::move(S._Children)), _Data(std::move(S._Data))
{
	this->ResetChildPointersSingle();
}

template<class T>
TreeNode<T> &TreeNode<T>::operator=(const TreeNode<T> &S) noexcept
{
	this->_Parent = S._Parent;
	this->_Children = S->_Children;
	this->_Data = S->_Data;

	this->ResetChildPointersRecursive();
}

template<class T>
TreeNode<T> &TreeNode<T>::operator=(TreeNode<T> &&S) noexcept
{
	this->_Parent = std::move(S._Parent);
	this->_Children = std::move(S->_Children);
	this->_Data = std::move(S->_Data);

	this->ResetChildPointersSingle();
}


template<class T>
void TreeNode<T>::ResetChildPointersSingle() noexcept
{
	// Go through all children once
	for(auto curChild : this->_Children )
		curChild.SetParent(this);
}

template<class T>
TreeNode<T> *TreeNode<T>::AddChild(const T &NewData)
{
	this->_Children.push_back(std::move(TreeNode<T>(this, NewData)));

	this->ResetChildPointersSingle();

	return &(this->_Children.back());
}

template<class T>
void TreeNode<T>::ResetChildPointersRecursive() noexcept
{
	// Go through all children recursively
	for(auto curChild : this->_Children)
	{
		curChild.SetParent(this);
		curChild.ResetChildPointersRecursive();
	}
}

#endif // TREE_NODE_TEMPLATES_H
