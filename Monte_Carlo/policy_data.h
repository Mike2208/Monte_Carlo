#ifndef POLICY_DATA_H
#define POLICY_DATA_H

#include "occupancy_grid_map.h"
#include "pos_2d.h"
#include "tree_class.h"

#include <fstream>

class PolicyData;

namespace POLICY_DATA
{
	typedef float CERTAINTY_TYPE;
	typedef float EXPECTED_LENGTH_TYPE;

	enum POLICY_RESULT
	{
		POLICY_SUCCESS, POLICY_BLOCKED, POLICY_UNKNOWN
	};

	typedef OGM_PROB_TYPE MAP_CELL_TYPE;
	struct NODE_DATA_SIMPLE
	{
			POS_2D MovePos;							// Position to move to
			POS_2D ScanPos;							// Position to scan next
			size_t MapID;							// Map ID chosen as most correct
			POLICY_RESULT PolicyState = POLICY_SUCCESS;		// Policy State
	};

	struct NODE_DATA : public NODE_DATA_SIMPLE
	{
		Map2D<MAP_CELL_TYPE>	MapState;		// Map state to compare to true map
	};

	typedef TreeNode<NODE_DATA>		TREE_NODE;
	typedef TreeClass<NODE_DATA>	TREE_CLASS;

	typedef long FILE_POS;
	struct FILE_HEADER
	{
		POS_2D StartPos;
		POS_2D DestPos;

		size_t NumNodes = 0;
		FILE_POS NextFreePos = 0;

		size_t MapWidth = 0, MapHeight = 0;

		CERTAINTY_TYPE GoalCertainty;		// Certainty of reaching goal
		EXPECTED_LENGTH_TYPE LengthToGoal;	// Expected Length until goal is reached

		CERTAINTY_TYPE PolicyCertainty;					// Certainty of policy finishing
		EXPECTED_LENGTH_TYPE LengthToPolicyCompletion;	// Expected Length until policy is finished
	};

	struct FILE_NODE_DATA : public NODE_DATA
	{
		size_t GetNumChildren() const {return this->NumChildren; }
		const std::vector<FILE_POS> &GetChildren() const { return this->ChildrenPos; }

		FILE_NODE_DATA() = default;
		FILE_NODE_DATA(const FILE_NODE_DATA &S) = default;
		FILE_NODE_DATA(FILE_NODE_DATA &&S) = default;
		FILE_NODE_DATA &operator=(const FILE_NODE_DATA &S)	= default;
		FILE_NODE_DATA &operator=(FILE_NODE_DATA &&S)			= default;

		FILE_NODE_DATA &operator=(const NODE_DATA &S)	{ *static_cast<NODE_DATA*>(this) = S; return *this; }
		FILE_NODE_DATA &operator=(NODE_DATA &&S)			{ *static_cast<NODE_DATA*>(this) = std::move(S); return *this; }

	private:

		FILE_POS NodePosition = 0;

		size_t NumChildren = 0;
		std::vector<FILE_POS> ChildrenPos;
		FILE_POS				ParentPos = 0;

		friend PolicyData;
	};
}

class PolicyData
{
	public:

	typedef	POLICY_DATA::NODE_DATA_SIMPLE	NODE_DATA_SIMPLE;
	typedef	POLICY_DATA::NODE_DATA		NODE_DATA;
	typedef	POLICY_DATA::TREE_NODE		TREE_NODE;
	typedef	POLICY_DATA::TREE_CLASS		TREE_CLASS;
	typedef	POLICY_DATA::FILE_HEADER	FILE_HEADER;
	typedef	POLICY_DATA::FILE_NODE_DATA		FILE_NODE_DATA;
	typedef POLICY_DATA::FILE_POS		FILE_POS;
	typedef POLICY_DATA::MAP_CELL_TYPE	MAP_CELL_TYPE;

		PolicyData();

		int ReadPolicyFromFile(const char *const FileName, FILE_HEADER &Header, FILE_NODE_DATA &RootNode);
		int WritePolicyToFile(const char *const FileName, const FILE_HEADER &HeaderData, const TREE_NODE &RootNode);

		FILE_HEADER GetFileHeader();
		void SetFileHeader(const FILE_HEADER &Header);

		FILE_NODE_DATA GetFileNode(const FILE_POS Position, const FILE_HEADER &Header);
		void WriteFileNode(const FILE_NODE_DATA &Node);

	private:

		std::fstream _PolicyFile;

		// Read and write
		static FILE_POS WriteNodeToFile(std::fstream &File, const FILE_NODE_DATA &CurNode);
		static FILE_POS ReadNodeFromFile(std::fstream &File, const FILE_POS NodePos, const FILE_HEADER &FileHeader, FILE_NODE_DATA &NodeData);

		static void UpdateChildPositions(std::fstream &File, const FILE_NODE_DATA &CurNode);

		static FILE_POS WriteNodeStep(std::fstream &File, const TREE_NODE &CurNode, const FILE_POS ParentPos, const FILE_POS &CurPos, size_t &NumNodes);
		static FILE_POS ReadNodeStep(std::fstream &File, const FILE_POS &NodePos, FILE_NODE_DATA &NodeData);
};

#endif // POLICY_DATA_H
