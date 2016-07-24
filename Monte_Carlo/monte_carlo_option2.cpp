#include "monte_carlo_option2.h"
#include <cstring>

namespace MONTE_CARLO_OPTION2
{
	NODE_DATA::NODE_DATA() : NodeData(nullptr), NodeDataSize(0)
	{
	}

	NODE_DATA::NODE_DATA(const NODE_DATA &S) noexcept
	{
		// Copy everything first
		std::memcpy(this, &S, sizeof(NODE_DATA));

		// Now allocate memory for data, then copy it
		this->NodeData = new char[S.NodeDataSize];
		std::memcpy(this->NodeData, S.NodeData, S.NodeDataSize);
	}

	NODE_DATA::NODE_DATA(NODE_DATA &&S) noexcept
	{
		// Copy everything first
		std::memcpy(this, &S, sizeof(NODE_DATA));

		// Remove pointers from S
		S.NodeData = nullptr;
		S.NodeDataSize = 0;
	}

	NODE_DATA &NODE_DATA::operator=(const NODE_DATA &S) noexcept
	{
		// Copy everything first
		std::memcpy(this, &S, sizeof(NODE_DATA));

		// Now allocate memory for data, then copy it
		this->NodeData = new char[S.NodeDataSize];
		std::memcpy(this->NodeData, S.NodeData, S.NodeDataSize);

		return *this;
	}

	NODE_DATA &NODE_DATA::operator=(NODE_DATA &&S) noexcept
	{
		// Copy everything first
		std::memcpy(this, &S, sizeof(NODE_DATA));

		// Remove pointers from S
		S.NodeData = nullptr;
		S.NodeDataSize = 0;

		return *this;
	}

	NODE_DATA::~NODE_DATA() noexcept
	{
		if(this->NodeData != nullptr)
			delete[] static_cast<char*>(this->NodeData);
	}
}

int MonteCarloOption2::Selection()
{
	TREE_NODE *const&ppCurNode = this->_Branch.pCurNode;

	// Go through Tree, always selecting child with highest node value, until leaf is reached
	while(!ppCurNode->IsLeaf())
	{
		// find first child that isn't done
		TREE_NODE::CHILD_ID bestChildID = ppCurNode->GetNumChildren();
		MONTE_CARLO_OPTION2::NODE_VALUE_TYPE bestValue;
		TREE_NODE::CHILD_ID curChildID = 0;
		for(;curChildID < ppCurNode->GetNumChildren(); ++curChildID)
		{
			// Stop if this child branch isn't fully explored
			if(!ppCurNode->GetChild(curChildID)->GetData().IsDone)
				break;
		}

		// Node already fully explored, return finished
		if(curChildID == ppCurNode->GetNumChildren())
			return 0;

		bestValue = ppCurNode->GetChild(curChildID)->GetData().Value;
		bestChildID = curChildID;
		for(curChildID += 1; curChildID < ppCurNode->GetNumChildren(); ++curChildID)
		{
			// Skip if this node is already done
			if(ppCurNode->GetChild(curChildID)->GetData().IsDone)
				continue;

			const MONTE_CARLO_OPTION2::NODE_VALUE_TYPE curValue = ppCurNode->GetChild(curChildID)->GetData().Value;

			if(curValue > bestValue)
			{
				bestValue = curValue;
				bestChildID = curChildID;
			}
		}

		// Move down to selected branch
		this->_Branch.StepDownOneNode(bestChildID);
	}

	return 1;
}


void Expansion()
{

}

void Simulation();
void Backtrack();
