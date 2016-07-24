#ifndef TREE_NODE_PERMANENT_H
#define TREE_NODE_PERMANENT_H

#include "standard_definitions.h"
#include <vector>
#include <algorithm>	// std::sort

//#ifndef TREE_CLASS_PERMANENT_H
template<class T>
class TreeClassPermanent;
//#endif

template<class T>
class TreeNodePermanent;

namespace TREE_NODE_PERMANENT
{
	typedef std::vector<TreeNodePermanent<int>>::size_type ID;
	const ID INVALID_ID = GetInfiniteVal<ID>();
}

template<class T>
class TreeNodePermanent
{
		typedef std::vector<TreeNodePermanent<T>> STORAGE;

	public:
		typedef T DATA_TYPE;
		typedef typename TREE_NODE_PERMANENT::ID ID;

		TreeNodePermanent(const ID NodeID, const ID &ParentID, const T &NewData, TreeClassPermanent<T> &StorageClass);

		TreeNodePermanent() = delete;
		TreeNodePermanent(const TreeNodePermanent<T> &S);		// Copy Constructor
		TreeNodePermanent(TreeNodePermanent<T> &&S);			// Move Constructor
		TreeNodePermanent<T> &operator=(const TreeNodePermanent<T> &S) = delete;	// copy assignment operator
		TreeNodePermanent<T> &operator=(TreeNodePermanent<T> &&S) = delete;			// move assignment operator

		void Reset()	{ this->_ChildIDs.clear(); this->_ParentID = TREE_NODE_PERMANENT::INVALID_ID; }

		ID		GetParentID() const { return this->_ParentID; }
		void	SetParentID(const ID &NewParentID) { this->_ParentID = NewParentID; }

		T &GetDataR() { return this->_Data; }			// Get Reference to data ( allows changed)
		const T &GetData() const { return this->_Data; }		// Get Reference to data ( no changes)

		void SetData(const T &NewData) { this->_Data = NewData; }

		ID GetNumChildren() const  { return this->_ChildIDs.size(); }
		ID GetChildID(const ID &ChildNumber) const { return this->_ChildIDs.at(ChildNumber); }

		ID AddChild(const T &NewChildData);

		bool IsLeaf() const;		// Returns whether node is leaf ( no children )

		const std::vector<ID> &GetStorage() const { return this->_ChildIDs; }
		std::vector<ID> &GetStorage() { return this->_ChildIDs; }

#ifdef DEBUG	// DEBUG
		unsigned int _NodeDepth;
		void PrintNode() const;
#endif			// ~DEBUG

	private:
		ID				_ParentID;		// ID of parent
		std::vector<ID>	_ChildIDs;		// ID of child
		T				_Data;			// Data in node

		const ID		_NodeID;
		TreeClassPermanent<T>	&_StorageClass;

		template<class U>
		friend class TreeClassPermanent;
};

#include "tree_node_permanent_templates.cpp"

#endif // TREE_NODE_PERMANENT_H
