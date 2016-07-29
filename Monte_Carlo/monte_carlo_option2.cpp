#include "monte_carlo_option2.h"
#include <cstring>

namespace MONTE_CARLO_OPTION2
{
	NODE_DATA::NODE_DATA() : NodeData(nullptr), NodeDataSize(0), Action(NODE_ACTION_ERROR)
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
		this->DeleteExtraData();
	}

	void NODE_DATA::SetAction(const NODE_ACTION _Action)
	{
		// Check if changes need to happen
		if(this->Action != _Action)
		{
			this->Action = _Action;

			// Allocate enough memory for extra data
			unsigned int newDataSize;
			void *pNewData;
			switch(this->Action.GetAction())		// Check how much memory is required
			{
				case NODE_ACTION_MOVE:
					newDataSize = sizeof(NODE_EXTRA_DATA_MOVE);
					pNewData = new char(sizeof(NODE_EXTRA_DATA_MOVE));
					break;

				case NODE_ACTION_JUMP:
					newDataSize = sizeof(NODE_EXTRA_DATA_JUMP);
					pNewData = new char(sizeof(NODE_EXTRA_DATA_JUMP));
					break;

				case NODE_ACTION_OBSERVE:
					newDataSize = sizeof(NODE_EXTRA_DATA_OBSERVATION);
					pNewData = new char(sizeof(NODE_EXTRA_DATA_OBSERVATION));
					break;

				case NODE_ACTION_RESULT_FREE:
				case NODE_ACTION_RESULT_OCCUPIED:
					newDataSize = sizeof(NODE_EXTRA_DATA_RESULT);
					pNewData = new char(sizeof(NODE_EXTRA_DATA_RESULT));
					break;

				default:
					pNewData = nullptr;
					newDataSize = 0;
			}

			// delete old data
			this->DeleteExtraData();

			// Save new location
			this->NodeData = pNewData;
			this->NodeDataSize = newDataSize;
		}
	}

	NODE_EXTRA_DATA_MOVE *NODE_DATA::GetExtraMoveData()
	{
		// Check if this node has correct action, then return pointer if correct
		if(this->Action.IsCompleteMove() && this->NodeDataSize == sizeof(NODE_EXTRA_DATA_MOVE))
			return static_cast<NODE_EXTRA_DATA_MOVE *>(this->NodeData);

		return nullptr;
	}

	NODE_EXTRA_DATA_JUMP *NODE_DATA::GetExtraJumpData()
	{
		// Check if this node has correct action, then return pointer if correct
		if(this->Action.IsIncompleteMove() && this->NodeDataSize == sizeof(NODE_EXTRA_DATA_JUMP))
			return static_cast<NODE_EXTRA_DATA_JUMP *>(this->NodeData);

		return nullptr;
	}

	NODE_EXTRA_DATA_OBSERVATION *NODE_DATA::GetExtraObservationData()
	{
		// Check if this node has correct action, then return pointer if correct
		if(this->Action.IsObserveAction() && this->NodeDataSize == sizeof(NODE_EXTRA_DATA_OBSERVATION))
			return static_cast<NODE_EXTRA_DATA_OBSERVATION *>(this->NodeData);

		return nullptr;
	}

	NODE_EXTRA_DATA_RESULT *NODE_DATA::GetExtraResultData()
	{
		// Check if this node has correct action, then return pointer if correct
		if(this->Action.IsObserveResult() && this->NodeDataSize == sizeof(NODE_EXTRA_DATA_RESULT))
			return static_cast<NODE_EXTRA_DATA_RESULT *>(this->NodeData);

		return nullptr;
	}

	int NODE_DATA::SetExtraMoveData(const NODE_EXTRA_DATA_MOVE &ExtraData)
	{
		// Check if this node has correct action
		if(this->Action.IsCompleteMove())
		{
			if( this->NodeDataSize != sizeof(NODE_EXTRA_DATA_MOVE))
			{
				this->DeleteExtraData();
				this->NodeDataSize = sizeof(NODE_EXTRA_DATA_MOVE);
				this->NodeData = new char[this->NodeDataSize];
			}

			*(static_cast<NODE_EXTRA_DATA_MOVE*>(this->NodeData)) = ExtraData;

			return 1;
		}

		return -1;
	}

	int NODE_DATA::SetExtraJumpData(const NODE_EXTRA_DATA_JUMP &ExtraData)
	{
		// Check if this node has correct action
		if(this->Action.IsIncompleteMove())
		{
			if( this->NodeDataSize != sizeof(NODE_EXTRA_DATA_JUMP))
			{
				this->DeleteExtraData();
				this->NodeDataSize = sizeof(NODE_EXTRA_DATA_JUMP);
				this->NodeData = new char[this->NodeDataSize];
			}

			*(static_cast<NODE_EXTRA_DATA_JUMP*>(this->NodeData)) = ExtraData;

			return 1;
		}

		return -1;
	}

	int NODE_DATA::SetExtraObservationData(const NODE_EXTRA_DATA_OBSERVATION &ExtraData)
	{
		// Check if this node has correct action
		if(this->Action.IsObserveAction())
		{
			if( this->NodeDataSize != sizeof(NODE_EXTRA_DATA_OBSERVATION))
			{
				this->DeleteExtraData();
				this->NodeDataSize = sizeof(NODE_EXTRA_DATA_OBSERVATION);
				this->NodeData = new char[this->NodeDataSize];
			}

			*(static_cast<NODE_EXTRA_DATA_OBSERVATION*>(this->NodeData)) = ExtraData;

			return 1;
		}

		return -1;
	}

	int NODE_DATA::SetExtraResultData(const NODE_EXTRA_DATA_RESULT &ExtraData)
	{
		// Check if this node has correct action
		if(this->Action.IsObserveResult())
		{
			if( this->NodeDataSize != sizeof(NODE_EXTRA_DATA_RESULT))
			{
				this->DeleteExtraData();
				this->NodeDataSize = sizeof(NODE_EXTRA_DATA_RESULT);
				this->NodeData = new char[this->NodeDataSize];
			}

			*(static_cast<NODE_EXTRA_DATA_RESULT*>(this->NodeData)) = ExtraData;

			return 1;
		}

		return -1;
	}

	void NODE_DATA::DeleteExtraData()
	{
		if(this->NodeData != nullptr)
			delete[] static_cast<char*>(this->NodeData);

		this->NodeDataSize = 0;
		this->NodeData = nullptr;
	}

	NODE_EXTRA_DATA_JUMP::NODE_EXTRA_DATA_JUMP() : JumpPolicy(nullptr)
	{
	}

	NODE_EXTRA_DATA_JUMP::NODE_EXTRA_DATA_JUMP(const POS_2D &_NextStep) : NODE_EXTRA_DATA_MOVE(_NextStep), JumpPolicy(nullptr)
	{
	}

	NODE_EXTRA_DATA_JUMP::NODE_EXTRA_DATA_JUMP(const NODE_EXTRA_DATA_JUMP &S) noexcept : NODE_EXTRA_DATA_MOVE(S)
	{
		// Copy everything first
		std::memcpy(this, &S, sizeof(NODE_DATA));

		// Now copy policy data if it exists
		if(S.JumpPolicy != nullptr)
		{
			this->JumpPolicy = new POLICY_DATA_TYPE(*(S.JumpPolicy));
		}
	}

	NODE_EXTRA_DATA_JUMP::NODE_EXTRA_DATA_JUMP(NODE_EXTRA_DATA_JUMP &&S) noexcept
	{
		// Copy everything first
		std::memcpy(this, &S, sizeof(NODE_DATA));

		// Now move policy data if it exists
		if(S.JumpPolicy != nullptr)
		{
			this->JumpPolicy = new POLICY_DATA_TYPE(std::move(*(S.JumpPolicy)));

			S.JumpPolicy = nullptr;		// Remove data from S
		}
	}

	NODE_EXTRA_DATA_JUMP &NODE_EXTRA_DATA_JUMP::operator=(const NODE_EXTRA_DATA_JUMP &S) noexcept
	{
		// Copy everything first
		std::memcpy(this, &S, sizeof(NODE_DATA));

		// Now copy policy data if it exists
		if(S.JumpPolicy != nullptr)
		{
			this->JumpPolicy = new POLICY_DATA_TYPE(*(S.JumpPolicy));
		}

		return *this;
	}

	NODE_EXTRA_DATA_JUMP &NODE_EXTRA_DATA_JUMP::operator=(NODE_EXTRA_DATA_JUMP &&S) noexcept
	{
		// Copy everything first
		std::memcpy(this, &S, sizeof(NODE_DATA));

		// Now move policy data if it exists
		if(S.JumpPolicy != nullptr)
		{
			this->JumpPolicy = new POLICY_DATA_TYPE(std::move(*(S.JumpPolicy)));

			S.JumpPolicy = nullptr;		// Remove data from S
		}

		return *this;
	}

	NODE_EXTRA_DATA_JUMP::~NODE_EXTRA_DATA_JUMP() noexcept
	{
		// Remove policy data
		if(this->JumpPolicy != nullptr)
			delete this->JumpPolicy;
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
		bool posRecentlyTraversed = false;
		for(const auto &curConnectionPos : curIDConnection.ConnectionPositions)
		{
			if(curBotPos == curConnectionPos[0])
				continue;			// Skip if we are at this position

			// Compare connection position with previous path
			for(MONTE_CARLO_OPTION2::TRAVERSED_PATH::iterator curPrevPath = this->_Branch.PrevPath.begin()+(this->_Branch.PathsAfterObservation); curPrevPath != this->_Branch.PrevPath.end(); ++curPrevPath)
			{
				if(curPrevPath.base()->at(0) != curConnectionPos.at(0) || curPrevPath.base()->at(1) != curConnectionPos.at(0))
				{
					// If this position hasn't been traversed yet, add branch that moves here move here
					NODE_DATA tmpNode;
					tmpNode.NumVisits = 0;
					if(GetMovementCost(curBotPos, curConnectionPos.at(0)) == 0)
					{
						// If a jump is necessary, set action
						tmpNode.SetAction(MONTE_CARLO_OPTION2::NODE_ACTION_JUMP);
						tmpNode.SetExtraJumpData(MONTE_CARLO_OPTION2::NODE_EXTRA_DATA_JUMP(curConnectionPos.at(1)));		// Store next move data as well
					}
					else
					{
						// If move, set action
						tmpNode.SetAction(MONTE_CARLO_OPTION2::NODE_ACTION_MOVE);
						tmpNode.SetExtraMoveData(MONTE_CARLO_OPTION2::NODE_EXTRA_DATA_MOVE(curConnectionPos.at(1)));		// Store next move data as well
					}

					tmpNode.Position = curConnectionPos.at(0);
					tmpNode.NumVisits = 0;

					// Add child with node data
					ppCurNode->AddChild(std::move(tmpNode));
				}
			}
		}
	}
}

void Simulation();
void Backtrack();
