#ifndef TREE_CLASS_PERMANENT_H
#define TREE_CLASS_PERMANENT_H

#include "standard_definitions.h"
#include "tree_node_permanent.h"

template<class T>
class TreeClassPermanent
{
	public:
		typedef TreeNodePermanent<T> TREE_NODE;
		typedef T	TREE_NODE_DATA;
#ifdef DEBUG
		TreeClassPermanent() : _NodeStorage() { this->_NodeStorage.push_back(TreeNodePermanent<T>(0, TREE_NODE_PERMANENT::INVALID_ID, T(), *this)); }
#else
		TreeClassPermanent();
#endif

		void Reset();

		TreeNodePermanent<T> &GetRoot() { return this->_NodeStorage.at(0); }
		const TreeNodePermanent<T> &GetRoot() const { return this->_NodeStorage.at(0); }

		const typename TreeNodePermanent<T>::STORAGE &GetStorage() const { return this->_NodeStorage; }

#ifdef DEBUG	// DEBUG
		void PrintTree_BreadthFirst();
		void PrintTree_DepthFirst();
		void PrintTree_DepthFirst_Step(const TreeNodePermanent<T> &CurNode);
#endif			// ~DEBUG

	private:

		typename TreeNodePermanent<T>::STORAGE _NodeStorage;		// storage of all nodes of tree

		template<class U>
		friend class TreeNodePermanent;
};

#include "tree_class_permanent_templates.cpp"

#endif // TREE_CLASS_PERMANENT_H
