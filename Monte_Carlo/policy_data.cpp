#include "policy_data.h"

PolicyData::PolicyData()
{

}

int PolicyData::ReadPolicyFromFile(const char *const FileName, FILE_HEADER &Header, FILE_NODE_DATA &RootNode)
{
	this->_PolicyFile.close();

	this->_PolicyFile.open(FileName, std::fstream::binary | std::fstream::in | std::fstream::out | std::fstream::ate);

	// Read header
	Header = this->GetFileHeader();

	// Read root node
	PolicyData::ReadNodeFromFile(this->_PolicyFile, sizeof(FILE_HEADER), Header, RootNode);

	if(this->_PolicyFile.is_open())
		return 1;

	return -1;
}

int PolicyData::WritePolicyToFile(const char *const FileName, const FILE_HEADER &HeaderData, const TREE_NODE &RootNode)
{
	this->_PolicyFile.close();

	// Erase prev data
	this->_PolicyFile.open(FileName, std::fstream::binary | std::fstream::out | std::fstream::in);

	auto curHeader = HeaderData;

	// Write header
	this->SetFileHeader(curHeader);

	FILE_NODE_DATA curFileNode;
	curFileNode = RootNode.GetData();

	curHeader.NextFreePos =	PolicyData::WriteNodeStep(this->_PolicyFile, RootNode, 0, sizeof(FILE_HEADER), curHeader.NumNodes);

	// Update header with new data
	this->SetFileHeader(curHeader);

	if(this->_PolicyFile.is_open())
		return 1;

	return -1;
}

PolicyData::FILE_HEADER PolicyData::GetFileHeader()
{
	FILE_HEADER header;

	// Read header
	this->_PolicyFile.seekg(0);
	this->_PolicyFile.read(reinterpret_cast<char*>(&header), sizeof(FILE_HEADER));

	return header;
}

void PolicyData::SetFileHeader(const FILE_HEADER &Header)
{
	// Write header
	this->_PolicyFile.seekp(0);
	this->_PolicyFile.write(reinterpret_cast<const char*>(&Header), sizeof(FILE_HEADER));
}

PolicyData::FILE_NODE_DATA PolicyData::GetFileNode(const FILE_POS Position, const FILE_HEADER &Header)
{
	FILE_NODE_DATA nodeData;
	PolicyData::ReadNodeFromFile(this->_PolicyFile, sizeof(FILE_HEADER), Header, nodeData);

	return nodeData;
}

void PolicyData::WriteFileNode(const FILE_NODE_DATA &Node)
{
	PolicyData::WriteNodeToFile(this->_PolicyFile, Node);
}

PolicyData::FILE_POS PolicyData::WriteNodeToFile(std::fstream &File, const FILE_NODE_DATA &CurNode)
{
	File.seekp(CurNode.NodePosition);

	// Write node data
	File.write(reinterpret_cast<const char*>(&static_cast<const NODE_DATA_SIMPLE&>(CurNode)), sizeof(NODE_DATA_SIMPLE));

	// Write number of children
	File.write(reinterpret_cast<const char*>(&CurNode.NumChildren), sizeof(size_t));

	// Write new positions
	PolicyData::UpdateChildPositions(File, CurNode);

	// Write node position
	File.write(reinterpret_cast<const char*>(&CurNode.NodePosition), sizeof(FILE_POS));

	// Write parent position
	File.write(reinterpret_cast<const char*>(&CurNode.ParentPos), sizeof(FILE_POS));

	// Write map state
	const auto &mapState = CurNode.MapState;
	POS_2D curPos;
	for(curPos.X = 0; curPos.X < mapState.GetWidth(); ++curPos.X)
	{
		for(curPos.Y = 0; curPos.Y < mapState.GetHeight(); ++curPos.Y)
		{
			File.write(reinterpret_cast<const char*>(&mapState.GetPixel(curPos)), sizeof(MAP_CELL_TYPE));
		}
	}

	// Return next position
	return CurNode.NodePosition + sizeof(NODE_DATA_SIMPLE)+sizeof(size_t)+sizeof(FILE_POS)*CurNode.ChildrenPos.size()+sizeof(FILE_POS)*2+mapState.GetWidth()*mapState.GetHeight()*sizeof(MAP_CELL_TYPE);
}

PolicyData::FILE_POS PolicyData::ReadNodeFromFile(std::fstream &File, const FILE_POS NodePos, const FILE_HEADER &FileHeader, FILE_NODE_DATA &NodeData)
{
	File.seekg(NodePos);

	// Read node data
	File.read(reinterpret_cast<char*>(&static_cast<NODE_DATA_SIMPLE&>(NodeData)), sizeof(NODE_DATA_SIMPLE));

	// Read number of children
	File.read(reinterpret_cast<char*>(&NodeData.NumChildren), sizeof(size_t));

	// Read position of children
	NodeData.ChildrenPos.reserve(NodeData.NumChildren);
	for(auto &curChildPos : NodeData.ChildrenPos)
	{
		File.read(reinterpret_cast<char*>(&curChildPos), sizeof(FILE_POS));
	}

	// Read node position
	File.read(reinterpret_cast<char*>(&NodeData.NodePosition), sizeof(FILE_POS));

	// Read parent position
	File.read(reinterpret_cast<char*>(&NodeData.ParentPos), sizeof(FILE_POS));

	// Read map state
	auto &mapState = NodeData.MapState;
	mapState.ResizeMap(FileHeader.MapWidth, FileHeader.MapWidth);
	POS_2D curPos;
	for(curPos.X = 0; curPos.X < mapState.GetWidth(); ++curPos.X)
	{
		for(curPos.Y = 0; curPos.Y < mapState.GetHeight(); ++curPos.Y)
		{
			File.read(reinterpret_cast<char*>(&mapState.GetPixelR(curPos)), sizeof(MAP_CELL_TYPE));
		}
	}

	// Return next position
	return NodeData.NodePosition + sizeof(NODE_DATA_SIMPLE)+sizeof(size_t)+sizeof(FILE_POS)*NodeData.ChildrenPos.size()+sizeof(FILE_POS)*2+mapState.GetWidth()*mapState.GetHeight()*sizeof(MAP_CELL_TYPE);
}

void PolicyData::UpdateChildPositions(std::fstream &File, const FILE_NODE_DATA &CurNode)
{
	File.seekp(CurNode.NodePosition+sizeof(NODE_DATA_SIMPLE)+sizeof(size_t));

	for(const auto curChildPos : CurNode.ChildrenPos )
		File.write(reinterpret_cast<const char*>(&curChildPos), sizeof(FILE_POS));
}

PolicyData::FILE_POS PolicyData::WriteNodeStep(std::fstream &File, const TREE_NODE &CurNode, const FILE_POS ParentPos, const FILE_POS &CurPos, size_t &NumNodes)
{
	File.seekp(CurPos);

	// Create file node
	FILE_NODE_DATA curFileNode;
	curFileNode = CurNode.GetData();

	// Convert node to file node
	curFileNode.NumChildren = CurNode.GetNumChildren();
	curFileNode.ChildrenPos.resize(curFileNode.NumChildren);
	curFileNode.NodePosition = CurPos;
	curFileNode.ParentPos = ParentPos;

	// Write data
	auto nextFreePos = PolicyData::WriteNodeToFile(File, curFileNode);

	// write all child positions
	for(size_t curChildNum = 0; curChildNum < CurNode.GetNumChildren(); ++curChildNum)
	{
		// Save child position and continue
		curFileNode.ChildrenPos.at(curChildNum) = nextFreePos;
		nextFreePos = PolicyData::WriteNodeStep(File, *CurNode.GetChild(curChildNum), curFileNode.NodePosition, nextFreePos, NumNodes);
	}

	// Update child node positions
	PolicyData::UpdateChildPositions(File, curFileNode);

	// Increment NumNodes
	NumNodes++;

	// Return next free position
	return nextFreePos;
}
