#ifndef FILE_TREE_TEMPLATES_CPP
#define FILE_TREE_TEMPLATES_CPP

#include "file_tree.h"
#include "file_tree_node.h"

template<class T>
FileTree<T>::FileTree(const char *FileName)
{
	this->LoadFile(FileName);
}

template<class T>
int FileTree<T>::LoadFile(const char *FileName)
{
	if(this->_FileData.is_open())
		this->_FileData.close();

	this->_FileData.open(FileName, std::ios_base::out | std::ios_base::binary);

	if(this->_FileData.is_open())
		return 1;

	return 0;
}

template<class T>
int FileTree<T>::GetNode(const FILE_OFFSET &NodePos, FileTreeNode<T> &NodeData)
{
	if(!this->_FileData.is_open())
		return -1;

	// Read header data containing T and number of children
	this->_FileData.seekp(NodePos);
	this->_FileData.read(reinterpret_cast<void*>(&(NodeData._NodeData)), sizeof(FILE_TREE::NODE_STRUCT));

	// Reserve enough for children locations
	if(NodeData._NodeData.NumChildren > 0)
	{
		// Read children offsets to vector
		NodeData._ChildrenOffset.resize(NodeData._NodeData.NumChildren);
		this->_FileData.read(&(NodeData._ChildrenOffset.begin()), sizeof(FILE_TREE::FILE_OFFSET)*NodeData._NodeData.NumChildren);
	}
	else
		NodeData._ChildrenOffset.clear();

	// Read data
	NodeData._Data.ReadFromFile(this->_FileData);

	NodeData._FileData = this;

	return 1;
}

#endif
