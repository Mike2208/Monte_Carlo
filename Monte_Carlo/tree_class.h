#ifndef TREE_CLASS_H
#define TREE_CLASS_H

#include "tree_node.h"

template<class T>
class TreeClass
{
	public:
		TreeClass() = default;

		TreeNode<T> &GetRoot() { return this->_Root; }

	private:

		TreeNode<T> _Root;		// Root node of tree
};

#endif // TREE_CLASS_H
