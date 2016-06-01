#ifndef TREE_NODE_H
#define TREE_NODE_H

#include <vector>


template<class T>
class TreeNode
{
		typedef std::vector<TreeNode<T>> CHILD_STORAGE;

	public:
		typedef typename CHILD_STORAGE::size_type CHILD_ID;

		TreeNode() = default;
		TreeNode(const TreeNode<T> &S) noexcept;			// Copy Constructor
		TreeNode(TreeNode<T> &&S) noexcept;		// Move Constructor
		TreeNode(TreeNode<T> *const Parent, const T &NewData) noexcept : _Parent(Parent), _Children(), _Data(NewData) { this->_Children.resize(0); }

		TreeNode<T> &operator=(const TreeNode<T> &S) noexcept;		// copy assignment operator
		TreeNode<T> &operator=(TreeNode<T> &&S) noexcept;	// move assignment operator

		void Reset()	{ this->_Children.clear(); this->_Parent = nullptr; }

		TreeNode<T> *GetParent() const { return this->_Parent; }
		void		SetParent(TreeNode<T> *NewParent) { this->_Parent = NewParent; }

		T &GetDataR() { return this->_Data; }			// Get Reference to data ( allows changed)
		const T &GetData() const { return this->_Data; }		// Get Reference to data ( no changes)

		void SetData(const T &NewData) { this->_Data = NewData; }

		CHILD_ID GetNumChildren() const  { return this->_Children.size(); }
		TreeNode<T> *GetChild(const CHILD_ID &ID)  { return &(this->_Children.at(ID)); }

		TreeNode<T> *AddChild(const T &NewData);

		bool IsLeaf() const { return (this->_Children.size() > 0 ? 0:1); }		// Returns whether node is leaf ( no children )

		const CHILD_STORAGE &GetStorage() const	{ return this->_Children; }

	private:
		TreeNode<T>		*_Parent;
		CHILD_STORAGE	_Children;
		T				_Data;

		void ResetChildPointersSingle() noexcept;		// Resets child pointers to parent ( done after move )
		void ResetChildPointersRecursive() noexcept;		// Resets child pointers to parent ( done after copy )
};

#include "tree_node_templates.h"

#endif // TREE_NODE_H
