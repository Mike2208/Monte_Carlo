#include "monte_carlo_option2.h"
#include <cstring>

namespace MONTE_CARLO_OPTION2
{
	NODE_DATA::NODE_DATA() : Action(NODE_ACTION_ERROR), NodeData(nullptr), NodeDataSize(0)
	{
	}

	NODE_DATA::NODE_DATA(const NODE_DATA &S) noexcept : Action(NODE_ACTION_ERROR)
	{
		// Copy everything first
		std::memcpy(this, &S, sizeof(NODE_DATA));

		// Now allocate memory for data, then copy it
		this->NodeData = new char[S.NodeDataSize];
		std::memcpy(this->NodeData, S.NodeData, S.NodeDataSize);
	}

	NODE_DATA::NODE_DATA(NODE_DATA &&S) noexcept : Action(NODE_ACTION_ERROR)
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

	bool TRAVERSED_PATH_SINGLE::IsBidirectionalPath(const POS_2D &Pos1, const POS_2D &Pos2) const
	{
		if( ((*this)[0] == Pos1 && (*this)[1] == Pos2) ||
				((*this)[1] == Pos1 && (*this)[0] == Pos2))
			return true;

		return false;
	}

	bool TRAVERSED_PATH::PathContainsBidirectionalMove(const POS_2D &Pos1, const POS_2D &Pos2) const
	{
		for(const auto &curMove : (*this))
		{
			if(curMove.IsBidirectionalPath(Pos1, Pos2))
				return true;
		}

		return false;
	}

	const DISTRICT &BRANCH_DATA::GetCurDistrict() const
	{
		return this->pDistrictStorage->GetDistrict(this->GetCurDistrictID());
	}

	DISTRICT_ID BRANCH_DATA::GetCurDistrictID() const
	{
		return this->pDistrictStorage->GetDistrictIDAtGlobalPos(this->CurBotData.GetGlobalBotPosition());
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
//		this->_Branch.StepDownOneNode(bestChildID);
	}

	return 1;
}


void MonteCarloOption2::Expansion()
{
	TREE_NODE *&ppCurNode = this->_Branch.pCurNode;
	const DistrictMap &curDistrict = this->_Branch.GetCurDistrict();

	// Look at all connections around current node
	const POS_2D curBotPos = this->_Branch.CurBotData.GetGlobalBotPosition();
	for(const auto &curIDConnection : curDistrict.GetAdjacentConnections())
	{
		//if()
		for(const auto &curConnectionPos : curIDConnection.ConnectionPositions)
		{
			if(curBotPos == curConnectionPos[0])
				continue;			// Skip if we are at this position
		}
	}
}

void Simulation();
void Backtrack();
