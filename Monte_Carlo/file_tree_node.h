#ifndef FILE_TREE_NODE_H
#define FILE_TREE_NODE_H

/*	class FileTree
 *		contains a tree in a file
 */

#include "file_tree.h"
#include <vector>

template<class T>
class FileTree;

namespace FILE_TREE_NODE
{
	typedef std::fstream FILE_STREAM;
	typedef FILE_STREAM::off_type FILE_OFFSET;
	const FILE_OFFSET INVALID_ID = std::numeric_limits<FILE_OFFSET>::max();

	struct NODE_STRUCT
	{
		FILE_OFFSET ParentOffset = INVALID_ID;		// Offset from file start

		unsigned int NumChildren = 0;
	};
}

template<class T>
class FileTreeNode
{
	public:
		typedef FILE_TREE_NODE::NODE_STRUCT NODE_STRUCT;
		FileTreeNode() = default;
		FileTreeNode(const NODE_STRUCT &Data);

		unsigned int GetNumChildren() const { return this->_NodeData.NuChildren; }

		int MoveToChildNode(const unsigned int ChildID);
		int MoveToParent();

		bool IsLeaf() const { return (this->_NodeData.NumChildren == 0 ? true : false); }
		bool IsRoot() const { return (this->_NodeData.ParentOffset == FILE_TREE_NODE::INVALID_ID ? true : false); }

		// Conversion to access data
		operator const T&() const { return this->_Data; }
		operator T&() { return this->_Data; }

	private:

		T _Data;
		FILE_TREE_NODE::NODE_STRUCT _NodeData;
		std::vector<FILE_TREE_NODE::FILE_OFFSET> _ChildrenOffset;		// Offset from file start of children of this node

		FileTree<T> *_FileData = nullptr;

		friend class FileTree<T>;
};

#include "file_tree_node_templates.cpp"

#endif // FILE_TREE_H
