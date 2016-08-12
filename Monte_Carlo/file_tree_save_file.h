#ifndef FILE_TREE_SAVE_FILE_H
#define FILE_TREE_SAVE_FILE_H

#include "file_tree.h"
#include "tree_class.h"

#include <queue>

namespace FILE_TREE_SAVE_FILE
{
	// void T::WriteToFile(std::fstream &FileData, FILE_OFFSET &SavedSize)	const	// Just writes the data in a way that it can be retrieved
	// T::ReadFromFile(std::fstream &FileData)		// Read T into NewData from FileData

	template<class T>
	struct NODES_TO_SAVE
	{
		const TreeNode<T> *pNode;
		FILE_TREE::FILE_OFFSET ParentOffset;
		typename TreeNode<T>::CHILD_ID NodeID;

		NODES_TO_SAVE(const TreeNode<T> *const _pNode, const FILE_TREE::FILE_OFFSET _ParentOffset, const typename TreeNode<T>::CHILD_ID _NodeID) : pNode(_pNode), ParentOffset(_ParentOffset), NodeID(_NodeID) {}
	};
}

template<class T>
class FileTreeSaveFile
{
		typedef FILE_TREE::FILE_OFFSET FILE_OFFSET;
		typedef FILE_TREE_SAVE_FILE::NODES_TO_SAVE<T> NODES_TO_SAVE;
	public:
		FileTreeSaveFile();

		static int SaveTreeToFile(const char *FileName, const TreeClass<T> &Tree);
};

template<class T>
int FileTreeSaveFile<T>::SaveTreeToFile(const char *FileName, const TreeClass<T> &Tree)
{
	std::fstream fileDat;
	fileDat.open(FileName, std::ios_base::in | std::ios_base::binary);

	if(!fileDat.is_open())
		return -1;

	// Write all data to file
	FILE_OFFSET nextFreePos = 0;
	std::queue<const NODES_TO_SAVE> nodesToSave;
	nodesToSave.push(NODES_TO_SAVE(&(Tree.GetRoot()), FILE_TREE::INVALID_ID, 0));
	do
	{
		const NODES_TO_SAVE &curNode = nodesToSave.front();

		// Update location in parent data
		const auto curNodePos = nextFreePos;
		if(curNode.ParentOffset != FILE_TREE::INVALID_ID)
		{
			fileDat.seekp(curNode.ParentOffset+sizeof(FILE_OFFSET)*curNode.NodeID);
			fileDat.write(reinterpret_cast<const char*>(curNodePos), sizeof(FILE_OFFSET));
		}

		FILE_TREE_NODE::NODE_STRUCT tmpStruct;
		tmpStruct.NumChildren = curNode.pNode->GetNumChildren();
		tmpStruct.ParentOffset = curNode.ParentOffset;


		// Write node data
		fileDat.seekp(nextFreePos);
		fileDat.write(reinterpret_cast<const char*>(&tmpStruct), sizeof(FILE_TREE_NODE::NODE_STRUCT));

		// Write child offsets and save new nodes to check
		for(typename TreeNode<T>::CHILD_ID curID = 0; curID < curNode.pNode->GetNumChildren(); ++curID)
		{
			fileDat.write(reinterpret_cast<const char*>(nextFreePos), sizeof(FILE_OFFSET));
			nodesToSave.push(NODES_TO_SAVE(curNode.pNode->GetChild(curID), curNodePos, curID));
		}

		// Write T
		FILE_OFFSET tSize;
		curNode.pNode->GetData().WriteToFile(fileDat, tSize);

		// Update free position
		nextFreePos += sizeof(FILE_TREE_NODE::NODE_STRUCT) + sizeof(FILE_OFFSET)*curNode.pNode->GetNumChildren() + tSize;

		// Finished with current element
		nodesToSave.pop();
	}
	while(!nodesToSave.empty());

	// Close file after write
	fileDat.close();

	return 1;
}

#endif // FILE_TREE_SAVE_FILE_H
