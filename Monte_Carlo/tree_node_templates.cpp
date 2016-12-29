#ifndef TREE_NODE_TEMPLATES_H
#define TREE_NODE_TEMPLATES_H

#include "standard_definitions.h"
#include "tree_node.h"

template<class T>
TreeNode<T>::TreeNode(TreeNode<T> *const Parent, const T &NewData) noexcept : _Parent(Parent), _Children(), _Data(NewData)
{
	this->_Children.resize(0);

#ifdef DEBUG	// DEBUG
	if(Parent == nullptr)
		this->_NodeDepth = 0;
	else
		this->_NodeDepth = Parent->_NodeDepth+1;
#endif			// ~DEBUG
}

template<class T>
TreeNode<T>::TreeNode(TreeNode<T> *const Parent, T &&NewData) noexcept : _Parent(Parent), _Children(), _Data(std::move(NewData))
{
	this->_Children.resize(0);

#ifdef DEBUG	// DEBUG
	if(Parent == nullptr)
		this->_NodeDepth = 0;
	else
		this->_NodeDepth = Parent->_NodeDepth+1;
#endif			// ~DEBUG
}

template<class T>
TreeNode<T>::TreeNode(const TreeNode<T> &S) noexcept : _Parent(S._Parent), _Children(S._Children), _Data(S._Data)
{
	this->ResetChildPointersSingle();
	//this->ResetChildPointersRecursive();

#ifdef DEBUG	// DEBUG
	this->_NodeDepth = S._NodeDepth;
#endif			// ~DEBUG
}

template<class T>
TreeNode<T>::TreeNode(TreeNode<T> &&S) noexcept : _Parent(std::move(S._Parent)), _Children(std::move(S._Children)), _Data(std::move(S._Data))
{
	this->ResetChildPointersSingle();

#ifdef DEBUG	// DEBUG
	this->_NodeDepth = std::move(S._NodeDepth);
#endif			// ~DEBUG
}

template<class T>
TreeNode<T> &TreeNode<T>::operator=(const TreeNode<T> &S) noexcept
{
	this->_Parent = S._Parent;
	this->_Children = S._Children;
	this->_Data = S._Data;

#ifdef DEBUG	// DEBUG
	this->_NodeDepth = S._NodeDepth;
#endif			// ~DEBUG

	this->ResetChildPointersSingle();
	//this->ResetChildPointersRecursive();

	return *this;
}

template<class T>
TreeNode<T> &TreeNode<T>::operator=(TreeNode<T> &&S) noexcept
{
	this->_Parent = std::move(S._Parent);
	this->_Children = std::move(S._Children);
	this->_Data = std::move(S._Data);

#ifdef DEBUG	// DEBUG
	this->_NodeDepth = std::move(S._NodeDepth);
#endif			// ~DEBUG

	this->ResetChildPointersSingle();

	return *this;
}

template<class T>
TreeNode<T> *TreeNode<T>::AddChild(const T &NewData)
{
	this->_Children.push_back(std::move(TreeNode<T>(this, NewData)));

	this->ResetChildPointersSingle();

	return &(this->_Children.back());
}

template<class T>
TreeNode<T> *TreeNode<T>::AddChild(T &&NewData)
{
	this->_Children.push_back(std::move(TreeNode<T>(this, std::move(NewData))));

	this->ResetChildPointersSingle();

	return &(this->_Children.back());
}

template<class T>
TreeNode<T> *TreeNode<T>::InsertChild(const T &NewData)
{
	// Move children to temporary storage
	auto tmpChildStorage(std::move(this->_Children));

	// Current node's child storage cleared with move
	//this->_Children.clear();

	// Insert new child and move temporary storage
	TreeNode<T> *const retVal = this->AddChild(NewData);
	retVal->_Children = std::move(tmpChildStorage);

	// Reset pointers of moved storage
	this->_Children.front().ResetChildPointersSingle();

	return retVal;
}

template<class T>
void TreeNode<T>::ResetChildPointersSingle() noexcept
{
	// Go through all children once
	for(auto &curChild : this->_Children )
		curChild.SetParent(this);
}

template<class T>
void TreeNode<T>::ResetChildPointersRecursive() noexcept
{
	// Go through all children recursively
	for(auto &curChild : this->_Children)
	{
		curChild.SetParent(this);
		curChild.ResetChildPointersRecursive();
	}
}


#ifdef DEBUG	// DEBUG
template<class T>
void TreeNode<T>::PrintNode() const
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

#endif // TREE_NODE_TEMPLATES_H
