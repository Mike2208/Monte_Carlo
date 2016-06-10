#ifndef TREE_CLASS_H
#define TREE_CLASS_H

#include "standard_definitions.h"
#include "tree_node.h"

template<class T>
class TreeClass
{
	public:
		typedef TreeNode<T> TREE_NODE;
		typedef T			TREE_NODE_DATA;
#ifdef DEBUG
		TreeClass() : _Root() { this->_Root._NodeDepth = 0; }
#else
		TreeClass() = default;
#endif

		TreeNode<T> &GetRoot() { return this->_Root; }
		const TreeNode<T> &GetRoot() const { return this->_Root; }

#ifdef DEBUG	// DEBUG
		void PrintTree_BreadthFirst();
		void PrintTree_DepthFirst();
		static void PrintTree_DepthFirst_Step(const TreeNode<T> &CurNode);
#endif			// ~DEBUG

	private:

		TreeNode<T> _Root;		// Root node of tree
};

#include "tree_class_templates.h"

#endif // TREE_CLASS_H
