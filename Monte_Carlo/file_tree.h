#ifndef FILE_TREE_H
#define FILE_TREE_H

#include <fstream>
#include <limits>
#include "file_tree_node.h"

template<class T>
class FileTreeNode;

namespace FILE_TREE
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
class FileTree
{
		typedef FILE_TREE::FILE_STREAM FILE_STREAM;
		typedef FILE_TREE::FILE_OFFSET FILE_OFFSET;
	public:
		typedef FileTreeNode<T> TREE_NODE;

		FileTree() = default;
		FileTree(const char *FileName);

		int LoadFile(const char *FileName);

		int GetNode(const FILE_OFFSET &NodePos, FileTreeNode<T> &NodeData);

	private:

		FILE_STREAM _FileData;		// FStream to file
};

#include "file_tree_templates.cpp"

#endif // FILE_TREE_H
