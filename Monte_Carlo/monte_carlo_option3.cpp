#include "monte_carlo_option3.h"
#include "algorithm_a_star.h"

#include <cstring>

namespace MONTE_CARLO_OPTION3
{
	const NODE_EXTRA_DATA_OBJECT &NODE_EXTRA_DATA::GetExtraData() const
	{
		return *(this->_pExtraData);
	}

	NODE_EXTRA_DATA_OBJECT &NODE_EXTRA_DATA::GetExtraData()
	{
		return *(this->_pExtraData);
	}

	void NODE_EXTRA_DATA::SetExtraData(const NODE_EXTRA_DATA_OBJECT &ExtraData)
	{
		this->_pExtraData = ExtraData.Clone();
	}

	void NODE_EXTRA_DATA::SetExtraData(std::unique_ptr<NODE_EXTRA_DATA_OBJECT> &&ExtraData)
	{
		this->_pExtraData = std::move(ExtraData);
	}

	void NODE_EXTRA_DATA::DeleteExtraData()
	{
		// Typesafe delete
		this->_pExtraData.release();
	}

	NODE_EXTRA_DATA::NODE_EXTRA_DATA(const NODE_EXTRA_DATA_OBJECT &ExtraDataObject) : _pExtraData(ExtraDataObject.Clone())
	{}

	NODE_EXTRA_DATA::NODE_EXTRA_DATA(std::unique_ptr<NODE_EXTRA_DATA_OBJECT> &&ExtraDataObject) : _pExtraData(std::move(ExtraDataObject))
	{}

	NODE_EXTRA_DATA::NODE_EXTRA_DATA(const NODE_EXTRA_DATA &S) : _pExtraData((S._pExtraData == nullptr ? nullptr : S._pExtraData->Clone()))
	{}

	NODE_EXTRA_DATA &NODE_EXTRA_DATA::operator=(const NODE_EXTRA_DATA &S)
	{
		this->_pExtraData = (S._pExtraData == nullptr ? nullptr : S._pExtraData->Clone());

		return *this;
	}

	bool NODE_DATA::IsDeadEnd() const
	{
		if(this->Certainty <= OGM_PROB_MIN)
			return true;

		return false;
	}

	CERTAINTY_TYPE NODE_DATA::GetCertatintyBeforeNodeAction() const
	{
		// Remove incomplete move certainty from
		if(this->Action.IsIncompleteMove())
		{
			const auto &jumpData = dynamic_cast<const NODE_EXTRA_DATA_JUMP&>(this->GetExtraData());
			return OccupancyGridMap::CalculateCertaintyProbValueFromCertaintyLog(OccupancyGridMap::CalculateCertaintyLogValueFromCertaintyProb(this->Certainty) + jumpData.RecentPathLogCertainty);
		}
		else
			return this->Certainty;
	}

	EXPECTED_LENGTH_TYPE NODE_DATA::GetLengthBeforeNodeAction() const
	{
		// Remove incomplete move certainty from
		if(this->Action.IsIncompleteMove())
		{
			const auto &jumpData = dynamic_cast<const NODE_EXTRA_DATA_JUMP&>(this->GetExtraData());
			return this->ExpectedLength + jumpData.RecentPathLength;
		}
		else
			return this->ExpectedLength;
	}

	// Custom constructors due to NODE_EXTRA_DATA memory allocation
	NODE_DATA::NODE_DATA(const NODE_ACTION _Action, const NODE_VALUE_TYPE &_Value, const EXPECTED_LENGTH_TYPE &_ExpectedLength, const CERTAINTY_TYPE &_Certainty, const COST_TYPE &_ExpectedCost, const NUM_VISIT_TYPE &_NumVisits, bool _IsDone, NODE_EXTRA_DATA &&_ExtraData) : NODE_EXTRA_DATA(std::move(_ExtraData)), Action(_Action), Value(_Value), ExpectedLength(_ExpectedLength), Certainty(_Certainty), ExpectedCost(_ExpectedCost), NumVisits(_NumVisits), IsDone(_IsDone)
	{}

	NODE_DATA::NODE_DATA(const NODE_ACTION _Action) : NODE_EXTRA_DATA(), Action(_Action)
	{}

	NODE_DATA::NODE_DATA(const NODE_ACTION _Action, NODE_EXTRA_DATA &&_ExtraData) : NODE_EXTRA_DATA(std::move(_ExtraData)), Action(_Action)
	{}

	void NODE_EXTRA_DATA_JUMP::SetExtraData(const NODE_EXTRA_DATA &ExtraData)
	{
		this->_ExtraData = ExtraData;
	}

	void NODE_EXTRA_DATA_JUMP::SetExtraData(NODE_EXTRA_DATA &&ExtraData)
	{
		this->_ExtraData = std::move(ExtraData);
	}

	const NODE_EXTRA_DATA &NODE_EXTRA_DATA_JUMP::GetExtraData() const
	{
		return this->_ExtraData;
	}

	NODE_EXTRA_DATA &NODE_EXTRA_DATA_JUMP::GetExtraData()
	{
		return this->_ExtraData;
	}

//	NODE_EXTRA_DATA_JUMP::operator const NODE_EXTRA_DATA_OBJECT*() const
//	{
//		return &this->_ExtraData.GetExtraData();
//	}

//	NODE_EXTRA_DATA_JUMP::operator NODE_EXTRA_DATA_OBJECT*()
//	{
//		return &this->_ExtraData.GetExtraData();
//	}

	std::unique_ptr<NODE_EXTRA_DATA_OBJECT> NODE_EXTRA_DATA_JUMP::Create() const
	{
		// Return with move to prevent destruction
		return std::move(std::unique_ptr<NODE_EXTRA_DATA_OBJECT>(new NODE_EXTRA_DATA_JUMP()));
	}

	std::unique_ptr<NODE_EXTRA_DATA_OBJECT> NODE_EXTRA_DATA_JUMP::Clone() const
	{
		// Return with move to prevent destruction
		return std::move(std::unique_ptr<NODE_EXTRA_DATA_OBJECT>(new NODE_EXTRA_DATA_JUMP(*this)));
	}

	OBSTACLE_DATA::OBSTACLE_DATA(const POS_2D &_Position, const CERTAINTY_TYPE &_OccupancyPercentage) : POS_2D(_Position), OccupancyPercentage(_OccupancyPercentage)
	{}

	NODE_EXTRA_DATA_MOVE::NODE_EXTRA_DATA_MOVE(const POS_2D &MoveDestination)
	{
		this->NewPos = MoveDestination;
	}

	std::unique_ptr<NODE_EXTRA_DATA_OBJECT> NODE_EXTRA_DATA_MOVE::Create() const
	{
		// Return with move to prevent destruction
		return std::move(std::unique_ptr<NODE_EXTRA_DATA_OBJECT>(new NODE_EXTRA_DATA_MOVE()));
	}

	std::unique_ptr<NODE_EXTRA_DATA_OBJECT> NODE_EXTRA_DATA_MOVE::Clone() const
	{
		// Return with move to prevent destruction
		return std::move(std::unique_ptr<NODE_EXTRA_DATA_OBJECT>(new NODE_EXTRA_DATA_MOVE()));
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

	bool TRAVERSED_PATH::PartialPathContainsBidirectionalMove(const POS_2D &Pos1, const POS_2D &Pos2, const ID &StartPathID) const
	{
		for(auto curMove = StartPathID; curMove < this->size(); ++curMove)
		{
			if(this->at(curMove).IsBidirectionalPath(Pos1, Pos2))
				return true;
		}

		return false;
	}

	void BRANCH_DATA::StepDownOneNodeSimple(const TREE_NODE::CHILD_ID &ChildID)
	{
		// Stop if step down not possible
		if(!this->_StepDownPossible)
			return;

		// Resync if out of sync
		if(!this->_StepDownSynced)
			this->StepDownOneNodeSync();

		// Save ID
		this->ObservationID = ChildID;

		// Step down one node
		this->pCurNode = this->pCurNode->GetChild(ChildID);

		// Mark that sync is necessary
		this->_StepDownSynced = false;
	}

	void BRANCH_DATA::StepDownOneNodeSync()
	{
		if(!this->_StepDownSynced)
		{
			const NODE_DATA &nextData = this->pCurNode->GetData();

			// Divide quad by the requested number of times
			for(size_t divideNum = 0; divideNum < nextData.NumDivisions; ++divideNum)
			{
				this->DStarMaps.DivideQuad(this->CurCertaintyLogData, std::vector<POS_2D>(), this->DStarMaps.GetQuadIDAtPos(this->CurBotData.GetGlobalBotPosition()), 1 , 0);
			}

			// Check whether previous node was an observation action
			if(this->PrevNodeWasObserveAction)
			{
				// Set either to occupied or free, depending on child ID
				if(this->ObservationID == MONTE_CARLO_OPTION3::OBSERVE_RESULT_FREE_ID)
				{
					this->SetMapPositionDiscrete(this->ObservationPoint, OGM_DISCRETE_EMPTY);
				}
				else
				{
					this->SetMapPositionDiscrete(this->ObservationPoint, OGM_DISCRETE_FULL);

					// Clear previously visited quads to let them be visited again
					this->VisitedQuads.clear();
				}

				this->PrevNodeWasObserveAction = false;
			}

			if(nextData.Action.IsCompleteMove())
			{
				// If this node is a move action, update robot position
				const NODE_EXTRA_DATA_MOVE &moveData = dynamic_cast<const NODE_EXTRA_DATA_MOVE&>(nextData.GetExtraData());

				// Move bot to next position
				this->CurBotData.SetGlobalBotPosition(moveData.NewPos);

				// Add to quad vector
				const auto curQuad = this->DStarMaps.GetQuadIDAtPos(moveData.NewPos);
				if(this->VisitedQuads.back() != curQuad)
					this->VisitedQuads.push_back(curQuad);
			}
			else if (nextData.Action.IsMovePath())
			{
				// If this node is a move action along a path, update robot position
				const NODE_EXTRA_DATA_MOVE_PATH &moveData = dynamic_cast<const NODE_EXTRA_DATA_MOVE_PATH&>(nextData.GetExtraData());

				// Move bot to next position
				this->CurBotData.SetGlobalBotPosition(moveData.TargetPosition);

				// Add to quad vector
				const auto curQuad = this->DStarMaps.GetQuadIDAtPos(moveData.TargetPosition);
				if(this->VisitedQuads.back() != curQuad)
					this->VisitedQuads.push_back(curQuad);
			}
			else if(nextData.Action.IsIncompleteMove())
			{
				// If this node is a jump action, update robot position and map
				const NODE_EXTRA_DATA_JUMP &jumpData = dynamic_cast<const NODE_EXTRA_DATA_JUMP&>(nextData.GetExtraData());

				// Set all visited positions to free
				for(const auto &curPos : jumpData.PathData)
				{
					this->SetMapPositionDiscrete(curPos, 0);

					// Add to visited quads
					const auto curQuad = this->DStarMaps.GetQuadIDAtPos(curPos);
					if(this->VisitedQuads.back() != curQuad)
						this->VisitedQuads.push_back(curQuad);
				}

				// Update bot position
				this->CurBotData.SetGlobalBotPosition(jumpData.PathData.back());
			}
//			else if(nextData.Action.IsObserveResult())
//			{
//				// At observe result, record observation result
//				const NODE_EXTRA_DATA_OBSTACLE_SINGLE &obstacleData = dynamic_cast<const NODE_EXTRA_DATA_OBSTACLE_SINGLE&>(nextData.GetExtraData());

//				this->SetMapPositionDiscrete(obstacleData.ObstacleCell, obstacleData.IsOccupied);

//				// Clear previously visited quads to let them be visited again
//				if(nextData.Action.IsOccupiedResult())
//					this->VisitedQuads.clear();
//			}
			else if(nextData.Action.IsObserveAction())
			{
				// At observe action, save observe point
				this->ObservationPoint = dynamic_cast<const NODE_EXTRA_DATA_OBSERVE_ACTION&>(nextData.GetExtraData()).ObservationPoint;
				this->PrevNodeWasObserveAction = true;
			}

			this->_StepDownPossible = true;
		}
	}

	void BRANCH_DATA::StepDownOneNode(const TREE_NODE::CHILD_ID &ChildID)
	{
		// Step down one node ans sync up
		this->StepDownOneNodeSimple(ChildID);
		this->StepDownOneNodeSync();
	}

//	void BRANCH_DATA::SetStepDownToUnsynced()
//	{
//		this->_StepDownSynced = false;
//	}

	void BRANCH_DATA::StepUpOneNodeSimple()
	{
		// Check if step up possible
		auto *const pCurParent = this->pCurNode->GetParent();
		if(pCurParent != nullptr)
		{
			// Step up
			pCurNode = pCurParent;

			// Check if step down will still be allowed
			if(this->_StepDownSynced)
			{
				// If step down was synced before, no more step downs will be possible
				this->_StepDownPossible = false;
			}
			else
				this->_StepDownSynced = true;	// Now synced again
		}
	}

	bool BRANCH_DATA::IsNodeSynced() const
	{
		return this->_StepDownSynced;
	}

	void BRANCH_DATA::SyncDStarMaps(const bool SyncDestMaps, const bool SyncStartMaps)
	{
		// Sync requested data
		this->DStarMaps.UpdateDStarMaps(this->CurCertaintyLogData, this->PosToUpdate, this->CurBotData.GetGlobalBotPosition(), SyncDestMaps, SyncStartMaps);

		// Remove pos to update from queue
		this->PosToUpdate.clear();
	}

	void BRANCH_DATA::SimulateObserveObservation(const POS_2D &Position, const OGM_DISCRETE_TYPE CellOccupied)
	{
		this->SetMapPositionDiscrete(Position, CellOccupied);
	}

	void BRANCH_DATA::Init(const OGM_MAP &OriginalMap, const POS_2D &Start, const POS_2D &Destination, TREE_CLASS &TreeData)
	{
		this->pOriginalMap = &OriginalMap;
		//this->VisitMap.ResetMap(OriginalMap.GetWidth(), OriginalMap.GetHeight(), 0);
		this->pCurNode = &(TreeData.GetRoot());
		this->RemainingMapEntropy = OccupancyGridMap::CalculateEntropyFromMap(OriginalMap);
		this->CurBotData.SetGlobalBotPosition(Start);
		//this->PrevPath.clear();
		this->VisitedQuads.clear();
		this->PosToUpdate.clear();
		this->Destination = Destination;
		this->Start = Start;
		this->PrevNodeWasObserveAction = false;

		this->_StepDownPossible = true;
		this->_StepDownSynced = true;

		// Calculate map data at start
		//OccupancyGridMap::CalculateLogMapFromCellMap(OriginalMap, this->CurLogData);
		OccupancyGridMap::CalculateCertaintyLogMapFromCellMap(OriginalMap, this->CurCertaintyLogData);

		// Calculate D* maps
		this->DStarMaps.ResetDestPos(this->CurCertaintyLogData, Destination);
	}

	BRANCH_DATA::BRANCH_DATA(const OGM_MAP &MapData, RobotData &_RobotData, MCPathStorage *const PathStorage) : pOriginalMap(&MapData), CurBotData(_RobotData), pPathStorage(PathStorage), DStarMaps(MapData.GetWidth(), MapData.GetHeight())
	{}

	void BRANCH_DATA::SetMapPositionDiscrete(const POS_2D &Position, const bool &NewValue)
	{
		// Update map entropy of this cell and all cells around it with the robot radius
		auto &r_oldPosData = this->CurCertaintyLogData.GetPixelR(Position);
		if((NewValue && r_oldPosData >= OGM_LOG_MAX)			// Only update at change
			|| (!NewValue && r_oldPosData <= OGM_LOG_MIN))
		{
			this->RemainingMapEntropy += 0-OccupancyGridMap::CalculateEntropyFromProb(OccupancyGridMap::CalculateProbValueFromCertaintyLog((r_oldPosData)));

			// Set this cell and update all cells around it with the robot radius
			// NOTE: Use inverse for certainty!!!
			if(NewValue)
				r_oldPosData =  OGM_LOG_MIN;
			else
				r_oldPosData =  OGM_LOG_MAX;

			// Add to vector with updated positions
			this->PosToUpdate.push_back(Position);
		}
	}

	void BRANCH_DATA::SetMapPositionFromCertaintyLogValue(const POS_2D &Position, const OGM_LOG_TYPE &CertaintyValue)
	{
		// Update map entropy of this cell and all cells around it with the robot radius
		auto &r_oldPosData = this->CurCertaintyLogData.GetPixelR(Position);
		if(CertaintyValue != r_oldPosData)		// Only update at change
		{
			this->RemainingMapEntropy += OccupancyGridMap::CalculateEntropyFromProb(OccupancyGridMap::CalculateProbValueFromCertaintyLog(CertaintyValue))-OccupancyGridMap::CalculateEntropyFromProb(OccupancyGridMap::CalculateProbValueFromCertaintyLog(r_oldPosData));

			// Set this cell and update all cells around it with the robot radius
			r_oldPosData = CertaintyValue;

			// Add to vector with updated positions
			this->PosToUpdate.push_back(Position);
		}
	}

//	void BRANCH_DATA::UpdateBranchData(const TREE_NODE &NextNode)
//	{
//		const NODE_DATA &nextData = NextNode.GetData();
//		if(nextData.Action.IsCompleteMove())
//		{
//			// If this node is a move action, update robot position
//			const NODE_EXTRA_DATA_MOVE &moveData = dynamic_cast<const NODE_EXTRA_DATA_MOVE&>(nextData.GetExtraData());

//			// Move bot to next position
//			this->CurBotData.SetGlobalBotPosition(moveData.NewPos);

//			// Add to quad vector
//			const auto curQuad = this->DStarMaps.GetQuadIDAtPos(moveData.NewPos);
//			if(this->VisitedQuads.back() != curQuad)
//				this->VisitedQuads.push_back(curQuad);
//		}
//		else if (nextData.Action.IsMovePath())
//		{
//			// If this node is a move action along a path, update robot position
//			const NODE_EXTRA_DATA_MOVE_PATH &moveData = dynamic_cast<const NODE_EXTRA_DATA_MOVE_PATH&>(nextData.GetExtraData());

//			// Move bot to next position
//			this->CurBotData.SetGlobalBotPosition(moveData.TargetPosition);

//			// Add to quad vector
//			const auto curQuad = this->DStarMaps.GetQuadIDAtPos(moveData.TargetPosition);
//			if(this->VisitedQuads.back() != curQuad)
//				this->VisitedQuads.push_back(curQuad);
//		}
//		else if(nextData.Action.IsIncompleteMove())
//		{
//			// If this node is a jump action, update robot position and map
//			const NODE_EXTRA_DATA_JUMP &jumpData = dynamic_cast<const NODE_EXTRA_DATA_JUMP&>(nextData.GetExtraData());

//			// Set all visited positions to free
//			for(const auto &curPos : jumpData.PathData)
//			{
//				this->SetMapPositionDiscrete(curPos, 0);

//				// Add to visited quads
//				const auto curQuad = this->DStarMaps.GetQuadIDAtPos(curPos);
//				if(this->VisitedQuads.back() != curQuad)
//					this->VisitedQuads.push_back(curQuad);
//			}

//			// Update bot position
//			this->CurBotData.SetGlobalBotPosition(jumpData.PathData.back());
//		}
//		else if(nextData.Action.IsObserveResult())
//		{
//			// At observe result, record observation result
//			const NODE_EXTRA_DATA_OBSTACLE_SINGLE &obstacleData = dynamic_cast<const NODE_EXTRA_DATA_OBSTACLE_SINGLE&>(nextData.GetExtraData());

//			this->SetMapPositionDiscrete(obstacleData.ObstacleCell, obstacleData.IsOccupied);

//			// Clear previously visited quads to let them be visited again
//			if(nextData.Action.IsOccupiedResult())
//				this->VisitedQuads.clear();
//		}
//		else if(nextData.Action.IsObserveAction())
//		{
//			// At observe action, do nothing
//		}

//		// Update current node
//		this->pCurNode = const_cast<TREE_NODE*>(&NextNode);
//	}

	void BRANCH_DATA::RevertBranchDataSimple()
	{
		// Check is already at root node
		if(this->pCurNode->GetParent() == nullptr)
			return;

		// Revert current node
		this->pCurNode = this->pCurNode->GetParent();
	}
}

MonteCarloOption3::MonteCarloOption3(const OccupancyGridMap &Map, RobotData _RobotData, const MC_PATH_STORAGE::PATH_ID &MaxStoredPaths) : _MCTree(), _Branch(Map, _RobotData, &(this->_PathStorage)), _PathStorage(MaxStoredPaths)
{}

int MonteCarloOption3::PerformMonteCarlo(const POS_2D &StartPos, const POS_2D &Destination)
{
	// Set current map to use
	this->_Branch.Init(*this->_Branch.pOriginalMap, StartPos, Destination, this->_MCTree);

	// Create tree
	this->_MCTree.Reset();
	TREE_NODE *pCurNode = &(this->_MCTree.GetRoot());
	pCurNode->GetDataR() = NODE_DATA(MONTE_CARLO_OPTION3::NODE_ACTION_OBSERVE, NODE_EXTRA_DATA());		// Observe start pos
	//pCurNode = pCurNode->AddChild(NODE_DATA(MONTE_CARLO_OPTION3::NODE_ACTION_RESULT_FREE, NODE_EXTRA_DATA_OBSTACLE_SINGLE(StartPos, OGM_DISCRETE_EMPTY)));		// Mark start pos as free (now implied with being 0th child)
	pCurNode->AddChild(NODE_DATA(MONTE_CARLO_OPTION3::NODE_ACTION_MOVE, NODE_EXTRA_DATA_MOVE(StartPos)));				// Move robot to start pos

	this->_Branch.pCurNode = &(this->_MCTree.GetRoot());

	BRANCH_DATA rootData = this->_Branch;		// Store root data for later

	// End Condition: Continue until all paths are checked
	while(!this->_MCTree.GetRoot().GetData().IsDone)
	{
		// Perform one Monte Carlo step
		this->Selection();
		this->Expansion();
		this->Simulation();
		this->Backtrack();

		// Revert to start
		this->_Branch = rootData;
	}

	return 1;
}

int MonteCarloOption3::Selection()
{
	TREE_NODE *const &ppCurNode = this->_Branch.pCurNode;

	// Go through Tree, always selecting child with highest node value, until leaf is reached
	while(!ppCurNode->IsLeaf())
	{
		// find first child that isn't done
		TREE_NODE::CHILD_ID bestChildID = ppCurNode->GetNumChildren();
		MONTE_CARLO_OPTION3::NODE_VALUE_TYPE bestValue;
		TREE_NODE::CHILD_ID curChildID = 0;
		for(;curChildID < ppCurNode->GetNumChildren(); ++curChildID)
		{
			// Stop if this child branch isn't fully explored
			if(!ppCurNode->GetChild(curChildID)->GetData().IsDone)
				break;
		}

		// Node already fully explored, return finished
		if(curChildID == ppCurNode->GetNumChildren())
		{
			// If this node can be expanded, do it
			if(ppCurNode->GetData().Action.IsIncompleteMove())
				return 2;
			else
				return 0;		// Return that everything is done
		}

		bestValue = ppCurNode->GetChild(curChildID)->GetData().Value;
		bestChildID = curChildID;
		for(curChildID += 1; curChildID < ppCurNode->GetNumChildren(); ++curChildID)
		{
			// Skip if this node is already done
			if(ppCurNode->GetChild(curChildID)->GetData().IsDone)
				continue;

			const MONTE_CARLO_OPTION3::NODE_VALUE_TYPE curValue = ppCurNode->GetChild(curChildID)->GetData().Value;

			if(curValue > bestValue)
			{
				bestValue = curValue;
				bestChildID = curChildID;
			}
		}

		const NODE_DATA &curNodeData = ppCurNode->GetData();

		// Check if this node is a jump
		if(curNodeData.Action.IsIncompleteMove())
		{
			// If this node can be expanded, check what makes more sense
			const NODE_EXTRA_DATA_JUMP &jumpData = dynamic_cast<const NODE_EXTRA_DATA_JUMP &>(curNodeData.GetExtraData());

			if(this->CompareIncompleteMoveAndPath(OccupancyGridMap::CalculateCertaintyProbValueFromCertaintyLog(jumpData.RecentPathLogCertainty), jumpData.RecentPathLength, curNodeData.Certainty, curNodeData.ExpectedLength))
			{
				// Don't Sync back up, first
				//this->_Branch.StepDownOneNodeSync();

				// Return that incomplete node was selected
				return 2;
			}
		}

		// If no jump or child node is better, Move down to selected branch
		this->_Branch.StepDownOneNodeSimple(bestChildID);
	}

	// Sync everything back up
	this->_Branch.StepDownOneNodeSync();

	// Return that leaf node was selected
	return 1;
}


int MonteCarloOption3::Expansion()
{
	TREE_NODE *&ppCurNode = this->_Branch.pCurNode;
	NODE_DATA *pCurNodeData = &ppCurNode->GetDataR();

	// Add node incomplete move to destination
//	if(pCurNodeData->Action.IsNoAction())
//	{
//		// Sync up node
//		this->_Branch.StepDownOneNodeSync();

//		// Get distance and certainty to goal
//		const POS_2D curBotPos = this->_Branch.CurBotData.GetGlobalBotPosition();
//		const auto distToGoal = this->_Branch.DStarMaps.GetDStarRatioDistMapToGoal().GetPixel(curBotPos)+this->_Branch.DStarMaps.GetDStarRatioDistMapToStart().GetPixel(curBotPos);
//		const auto certLogToGoal = this->_Branch.DStarMaps.GetDStarRatioCertaintyMapToGoal().GetPixel(curBotPos)+this->_Branch.DStarMaps.GetDStarRatioCertaintyMapToStart().GetPixel(curBotPos);

//		// Change no action to movement to goal
//		// Select correct node type (incomplete move, move path, or single move)
//		if(certLogToGoal >= OGM_LOG_MAX)
//		{
//			// Mark as dead end
//			pCurNodeData->IsDone = true;
//		}
//		else if(certLogToGoal > OGM_LOG_MIN)
//		{
//			// Incomplete move
//			pCurNodeData->Action.SetAction(MONTE_CARLO_OPTION3::NODE_ACTION_JUMP);
//			pCurNodeData->SetExtraData(NODE_EXTRA_DATA_JUMP());

//			// Update jump data
//			auto &jumpData = dynamic_cast<NODE_EXTRA_DATA_JUMP&>(pCurNodeData->GetExtraData());

//			// Calculate path and jump data
//			jumpData.PathData = this->_Branch.DStarMaps.CalculateRatioPathToGoal(curBotPos);
//			jumpData.RecentPathLength = distToGoal;
//			jumpData.RecentPathLogCertainty = certLogToGoal;
//		}
//		else
//		{
//			if(distToGoal > MAX_MOVE_DIST)
//			{
//				// Complete move path
//				pCurNodeData->Action.SetAction(MONTE_CARLO_OPTION3::NODE_ACTION_MOVE_PATH);
//				pCurNodeData->SetExtraData(NODE_EXTRA_DATA_MOVE_PATH(this->_Branch.Destination, distToGoal));
//			}
//			else
//			{
//				// Complete single move
//				pCurNodeData->Action.SetAction(MONTE_CARLO_OPTION3::NODE_ACTION_MOVE);
//				pCurNodeData->SetExtraData(NODE_EXTRA_DATA_MOVE(this->_Branch.Destination));
//			}
//		}

//		// Run simulation to get node data
//		this->SimulateCurrentLeaf();

//		// If this node is now done, stop
//		if(pCurNodeData->IsDone)
//			return 0;

//		// Node changed, set to unsynced(only possible if node was no action)
//		this->_Branch.SetStepDownToUnsynced();
//	}

	// If this is an incomplete move that should be expanded, check whether quad should be divided or simply next quad on map should be analyzed
	if(pCurNodeData->Action.IsIncompleteMove() && !this->_Branch.IsNodeSynced())
	{
		QUAD_MAP::ID nextQuadID;
		bool alterAction = false;

		// Check whether a division is necessary
		auto &curJumpData = dynamic_cast<NODE_EXTRA_DATA_JUMP&>(pCurNodeData->GetExtraData());
		size_t nextQuadPathPos;
		if(curJumpData.PathData.empty())
			nextQuadPathPos = 0;
		else
			nextQuadPathPos = curJumpData.PathData.size()-1;

		QUAD_MAP::ID curQuadID;
		do
		{
			curQuadID = this->_Branch.DStarMaps.GetQuadIDAtPos(this->_Branch.CurBotData.GetGlobalBotPosition());
			nextQuadID = curQuadID;

			// Go through path and find next quad
			for(auto pathPosIterator = curJumpData.PathData.begin(); pathPosIterator != curJumpData.PathData.end(); ++pathPosIterator)
			{
				// Check if destination reached
				if(*pathPosIterator == this->_Branch.Destination)
				{
					// Destination found, save it
					nextQuadID = MONTE_CARLO_OPTION3::DESTINATION_ID;
					nextQuadPathPos = pathPosIterator - curJumpData.PathData.begin();
					break;
				}

				// Compare quad IDs
				const auto pathQuadID = this->_Branch.DStarMaps.GetQuadIDAtPos(*pathPosIterator);
				if(pathQuadID != curQuadID)
				{
					// Next quad found, save it
					nextQuadID = pathQuadID;
					nextQuadPathPos = pathPosIterator - curJumpData.PathData.begin();
					break;
				}
			}

			// Check if this move action should be changed to a scan (done when next path position is in different quad)
			if(nextQuadPathPos == 0)
			{
				alterAction = true;
				break;
			}

			// If next quad is destination, divide current quad
			if(nextQuadID == MONTE_CARLO_OPTION3::DESTINATION_ID)
			{
				// Divide quad and keep D* Destination map up to date
				if(!this->_Branch.DStarMaps.DivideQuad(this->_Branch.CurCertaintyLogData, std::vector<POS_2D>(), curQuadID, true, false))
					break;		// Stop if no division possible
				pCurNodeData->NumDivisions++;
			}

			// Continue checking until a next quad is found that isn't the destination
		}
		while(nextQuadID == MONTE_CARLO_OPTION3::DESTINATION_ID || nextQuadID == curQuadID);

		// Save current path data
		std::unique_ptr<NODE_EXTRA_DATA_JUMP> pOldData(new NODE_EXTRA_DATA_JUMP(curJumpData));

		// Check whether current node needs to be changed to an observation or move action
		if(alterAction)
		{
			// Check certainty of next node
			const auto curPosCertainty = this->_Branch.CurCertaintyLogData.GetPixel(pOldData->PathData.front());
			if(curPosCertainty > OGM_LOG_MIN)
			{
				// Change to scan
				pCurNodeData->Action.SetAction(MONTE_CARLO_OPTION3::NODE_ACTION_OBSERVE);
				pCurNodeData->SetExtraData(NODE_EXTRA_DATA_OBSERVE_ACTION(pOldData->PathData.front(), curPosCertainty));

				// Sync everything back up
				this->_Branch.StepDownOneNodeSync();
				this->_Branch.SyncDStarMaps(true, false);

				// Insert node indicator for observe result free
				auto *const pFreeResultNode = ppCurNode->InsertChild(NODE_DATA(MONTE_CARLO_OPTION3::NODE_ACTION_NONE));

				// Insert move to next position at free result
				pFreeResultNode->InsertChild(NODE_DATA(MONTE_CARLO_OPTION3::NODE_ACTION_MOVE, NODE_EXTRA_DATA_MOVE(pOldData->PathData.front())));

				// Create new child node that simulates an observe result occupied
				TREE_NODE *pOccupiedNode = ppCurNode->AddChild(NODE_DATA(MONTE_CARLO_OPTION3::NODE_ACTION_NONE));

				// Create a copy of current branch to simulate the occupied result
				auto tmpBranch = this->_Branch;

				// Simulate an occupied result
				tmpBranch.StepDownOneNode(1);

				// Sync up D* maps
				tmpBranch.DStarMaps.UpdateDStarMaps(tmpBranch.CurCertaintyLogData, tmpBranch.PosToUpdate, tmpBranch.CurBotData.GetGlobalBotPosition(), 1, 0);

				// Calculate node data
				this->SimulateBranch(tmpBranch);

//				pNewData->Certainty = certaintyToGoal;
//				pNewData->ExpectedLength = distToGoal;
//				pNewData->IsDone = false;
//				pNewData->NumVisits = 1;
//				this->CalculateNodeValue(*pNewData, tmpBranch.Constant, pCurNodeData->NumVisits, pNewData->Value);

//				if(certaintyToGoal > OGM_LOG_MIN)
//				{
//					// Incomplete move
//					pNewNode = ppCurNode->AddChild(NODE_DATA(MONTE_CARLO_OPTION3::NODE_ACTION_JUMP, NODE_EXTRA_DATA_JUMP()));
//					pNewData = &pNewNode->GetDataR();
//					NODE_EXTRA_DATA_JUMP &newExtraData = dynamic_cast<NODE_EXTRA_DATA_JUMP&>(pNewData->GetExtraData());
//					newExtraData.PathData = tmpBranch.DStarMaps.CalculateRatioPathToGoal(curBotPos);
//					newExtraData.RecentPathLength = distToGoal;
//					newExtraData.RecentPathLogCertainty = certaintyToGoal;

//					pNewData->Certainty = OGM_LOG_MIN;
//					pNewData->ExpectedLength = 0;
//					pNewData->IsDone = false;
//					pNewData->NumVisits = 1;
//					this->CalculateNodeValue(*pNewData, tmpBranch.Constant, curNodeData.NumVisits, pNewData->Value);
//				}
//				else
//				{
//					// Check if single move or larger one
//					if(distToGoal > MAX_MOVE_DIST)
//					{
//						// Move path
//						pNewNode = ppCurNode->AddChild(NODE_DATA(MONTE_CARLO_OPTION3::NODE_ACTION_MOVE_PATH, NODE_EXTRA_DATA_MOVE_PATH(tmpBranch.Destination, distToGoal)));
//						pNewData = &pNewNode->GetDataR();

//						pNewData->Certainty = certaintyToGoal;
//						pNewData->ExpectedLength = distToGoal;
//						pNewData->IsDone = true;			// Complete path found, done
//						pNewData->NumVisits = 1;
//						this->CalculateNodeValue(*pNewData, tmpBranch.Constant, curNodeData.NumVisits, pNewData->Value);
//					}
//					else
//					{
//						// Single move
//						pNewNode = ppCurNode->AddChild(NODE_DATA(MONTE_CARLO_OPTION3::NODE_ACTION_MOVE, NODE_EXTRA_DATA_MOVE(tmpBranch.Destination)));
//						pNewData = &pNewNode->GetDataR();

//						pNewData->Certainty = certaintyToGoal;
//						pNewData->ExpectedLength = distToGoal;
//						pNewData->IsDone = true;			// Complete path found, done
//						pNewData->NumVisits = 1;
//						this->CalculateNodeValue(*pNewData, tmpBranch.Constant, curNodeData.NumVisits, pNewData->Value);
//					}
//				}

				// Move down to inserted child (moves to observe result free)
				this->_Branch.StepDownOneNodeSimple(0);
				pCurNodeData = &ppCurNode->GetDataR();

				// Update Path Length and Certainty
				pOldData->RecentPathLogCertainty -= this->_Branch.CurCertaintyLogData.GetPixel(pOldData->PathData.front());
				pOldData->RecentPathLength -= GetMovementCost(this->_Branch.CurBotData.GetGlobalBotPosition(), pOldData->PathData.front());
			}
			else
			{
				// Change to complete move (move to next quad)
				pCurNodeData->Action.SetAction(MONTE_CARLO_OPTION3::NODE_ACTION_MOVE);			// Set action type
				pCurNodeData->SetExtraData(NODE_EXTRA_DATA_MOVE(pOldData->PathData.front()));	// Set extra data to new value

				// Update Path Length (certainty remains the same)
				pOldData->RecentPathLength -= GetMovementCost(this->_Branch.CurBotData.GetGlobalBotPosition(), pOldData->PathData.front());
			}
		}
		else
		{
			// Simply remove last part of vector (will now move to next quad)
			curJumpData.PathData.resize(nextQuadPathPos);

			// Update path length and certainty
			POS_2D prevPos = this->_Branch.CurBotData.GetGlobalBotPosition();
			for(auto curPathPos = pOldData->PathData.begin(), endPathPos = pOldData->PathData.begin()+nextQuadPathPos; curPathPos != endPathPos; ++curPathPos)
			{
				// Update Path Length and Certainty for each point bot crossed
				pOldData->RecentPathLogCertainty -= this->_Branch.CurCertaintyLogData.GetPixel(*curPathPos);
				pOldData->RecentPathLength -= GetMovementCost(prevPos, *curPathPos);

				prevPos = *curPathPos;
			}
		}

		// Sync everything back up (now we added first half of incomplete move path to branch)
		this->_Branch.StepDownOneNodeSync();

		// Remove first part of pOldData (already checked previously, in first half of path)
		if(nextQuadPathPos > 0)
			pOldData->PathData.erase(pOldData->PathData.begin(), pOldData->PathData.begin()+nextQuadPathPos-1);

		// Check if something is left
		if(pOldData->PathData.empty())
		{
			// Return that no simulation is necessary and don't append new possible moves
			return 0;
		}
		else
		{
			// If Path is left, create a node between both halves
			TREE_NODE *pNewNode = ppCurNode;

			// Determine type of node that must be created
			const auto oldPos = this->_Branch.CurBotData.GetGlobalBotPosition();
			const auto curPos = pOldData->PathData.front();
			const auto posLogCertainty = this->_Branch.CurCertaintyLogData.GetPixel(curPos);
			if(posLogCertainty > OGM_LOG_MIN)
			{
				// Add an observation action
				pNewNode = ppCurNode->InsertChild(NODE_DATA(MONTE_CARLO_OPTION3::NODE_ACTION_OBSERVE, NODE_EXTRA_DATA_OBSERVE_ACTION(curPos, posLogCertainty)));

				// Add free result
				pNewNode->InsertChild(NODE_DATA(MONTE_CARLO_OPTION3::NODE_ACTION_NONE));

				// Add occupied result
				auto *const pOccupiedNode = pNewNode->AddChild(NODE_DATA(MONTE_CARLO_OPTION3::NODE_ACTION_NONE));

				// Simulate occupied result to populate node data
				this->_Branch.StepDownOneNodeSync();		// Make sure everything is synced
				this->_Branch.SyncDStarMaps(1,0);
				auto tmpBranch = this->_Branch;
				tmpBranch.SimulateObserveObservation(tmpBranch.ObservationPoint, OGM_DISCRETE_FULL);
				this->SimulateBranch(tmpBranch);
				//this->RunSimulation(tmpBranch.CurBotData.GetGlobalBotPosition(), tmpBranch.Destination, tmpBranch, pCurNodeData->NumVisits, pOccupiedNode->GetDataR());

				// Move down to newly created free node
				this->_Branch.StepDownOneNodeSimple(0);
				pCurNodeData = &ppCurNode->GetDataR();
			}
			else
			{
				// Add a move action
				pNewNode = ppCurNode->InsertChild(NODE_DATA(MONTE_CARLO_OPTION3::NODE_ACTION_MOVE, NODE_EXTRA_DATA_MOVE(curPos)));

				// Move down to newly created move node
				this->_Branch.StepDownOneNodeSimple(0);
				pCurNodeData = &ppCurNode->GetDataR();
			}

			// Remove checked node from vector
			pOldData->RecentPathLogCertainty -= posLogCertainty;
			pOldData->RecentPathLength -= GetMovementCost(oldPos, curPos);
			pOldData->PathData.erase(pOldData->PathData.begin());

			// Check if path is left
			if(pOldData->PathData.empty())
			{
				// Save remaining path as incomplete move
				pNewNode = pNewNode->InsertChild(NODE_DATA(MONTE_CARLO_OPTION3::NODE_ACTION_JUMP, NODE_EXTRA_DATA(std::move(pOldData))));

//				// Adjust node data
//				if(pNewNode->IsLeaf())
//					this->SimulateCurrentLeaf();		// Only simulate if nothing behind it
//				else
//					this->PerformBacktrackStep(*pNewNode);
			}
			else
			{
				// If no path left, check whether destination was reached
				if(this->_Branch.CurBotData.GetGlobalBotPosition() == this->_Branch.Destination)
				{
					return 0;		// Nothing left to do
				}
			}
		}
	}

	NODE_DATA newJumpData(MONTE_CARLO_OPTION3::NODE_ACTION_JUMP, NODE_EXTRA_DATA(std::move(std::unique_ptr<NODE_EXTRA_DATA_JUMP>(new NODE_EXTRA_DATA_JUMP()))));

	// Sync everything back up if not yet done
	this->_Branch.StepDownOneNodeSync();
	this->_Branch.SyncDStarMaps(true, true);

	// Get all adjacent Quads
	const auto curQuadID = this->_Branch.DStarMaps.GetQuadIDAtPos(this->_Branch.CurBotData.GetGlobalBotPosition());
	const auto &curAdjacentQuadIDs = this->_Branch.DStarMaps.GetAdjacentQuadIDs(curQuadID);

	QUAD_MAP::ID pathNextID = curQuadID;
	bool destReached = false;
	if(pCurNodeData->Action.IsIncompleteMove())
	{
		// Find next quad ID
		const auto &jumpData(dynamic_cast<const NODE_EXTRA_DATA_JUMP&>(pCurNodeData->GetExtraData()));
		for(auto curPathPos = jumpData.PathData.begin(); curPathPos != jumpData.PathData.end(); ++curPathPos)
		{
			const auto pathQuadID = this->_Branch.DStarMaps.GetQuadIDAtPos(*curPathPos);
			if(pathQuadID != curQuadID)
			{
				// ID found, stop
				pathNextID = pathQuadID;
				break;
			}
		}
	}

	bool pathToDestCreated = false;
	bool newPathCreated = false;
	do
	{
		for(const auto &curAdjacentQuadID : curAdjacentQuadIDs)
		{
			// Check if this quad is next in incomplete move
			if(pathNextID == curAdjacentQuadID)
			{
				continue;			// Skip if it will already be checked
			}

			// Check if previously visited after last observation action
			bool previouslyVisited = false;
			for(const auto &prevQuadID : this->_Branch.VisitedQuads)
			{
				if(prevQuadID == curAdjacentQuadID)
				{
					previouslyVisited = true;
					break;		// don't visit now
				}
			}

			if(previouslyVisited)
				continue;			// Skip if previously visited

			// Get connection rectangle
			const auto rectangle = this->_Branch.DStarMaps.GetAdjacentRectangle(curQuadID, curAdjacentQuadID);
			const POS_2D rectangleTop = POS_2D(rectangle.BottomLeftPos.X+rectangle.Width, rectangle.BottomLeftPos.Y+rectangle.Height);
			POS_2D bestPos;
			auto bestVal = GetInfiniteVal<OGM_LOG_TYPE>();
			POS_2D curPos;
			for(curPos.X = rectangle.BottomLeftPos.X ; curPos.X < rectangleTop.X ; ++curPos.X)
			{
				for(curPos.Y = rectangle.BottomLeftPos.Y ; curPos.Y < rectangleTop.Y ; ++curPos.Y)
				{
					// Check which position is better
					const auto curVal = this->_Branch.DStarMaps.GetDStarRatioMapToGoal().GetPixel(curPos)+this->_Branch.DStarMaps.GetDStarRatioMapToStart().GetPixel(curPos);
					if(curVal <= bestVal)
					{
						bestVal = curVal;
						bestPos = curPos;
					}
				}
			}

			// Check if a position exists that isn't a dead end
			if(bestVal == GetInfiniteVal<OGM_LOG_TYPE>())
				continue;		// Skip if all positions are dead ends

			// Create new child that visits selected position
			const auto bestValDist = this->_Branch.DStarMaps.GetDStarRatioDistMapToGoal().GetPixel(bestPos)+this->_Branch.DStarMaps.GetDStarRatioDistMapToStart().GetPixel(bestPos);
			const auto bestValCertLog = this->_Branch.DStarMaps.GetDStarRatioCertaintyMapToGoal().GetPixel(bestPos)+this->_Branch.DStarMaps.GetDStarRatioCertaintyMapToStart().GetPixel(bestPos);

			// Select correct node type
			TREE_NODE *pNewNode = nullptr;
			NODE_DATA *pNewData = nullptr;

			if(bestValCertLog > OGM_LOG_MIN)
			{
				// Incomplete move
				pNewNode = ppCurNode->AddChild(NODE_DATA(MONTE_CARLO_OPTION3::NODE_ACTION_JUMP, NODE_EXTRA_DATA_JUMP()));
				pNewData = &pNewNode->GetDataR();
				auto &jumpData = dynamic_cast<NODE_EXTRA_DATA_JUMP&>(pNewData->GetExtraData());

				// Calculate path and jump data
				jumpData.PathData = this->_Branch.DStarMaps.CalculateRatioPathOverPos(bestPos);
				jumpData.RecentPathLength = bestValDist;
				jumpData.RecentPathLogCertainty = bestValCertLog;
			}
			else
			{
				if(bestValDist > MAX_MOVE_DIST)
				{
					// Complete move path
					pNewNode = ppCurNode->AddChild(NODE_DATA(MONTE_CARLO_OPTION3::NODE_ACTION_MOVE_PATH, NODE_EXTRA_DATA_MOVE_PATH(bestPos, bestValDist)));
				}
				else
				{
					// Complete single move
					pNewNode = ppCurNode->AddChild(NODE_DATA(MONTE_CARLO_OPTION3::NODE_ACTION_MOVE, NODE_EXTRA_DATA_MOVE(bestPos)));
				}
			}

			// Simulation done in next step
		}

		// Check if destination is in quad an not yet reached
		if(!pathToDestCreated && pathNextID != MONTE_CARLO_OPTION3::DESTINATION_ID)
		{
			if(this->_Branch.DStarMaps.GetQuadIDAtPos(this->_Branch.Destination) == curQuadID)
			{
				// Create new child that attempts to reach destination
				const auto destValDist = this->_Branch.DStarMaps.GetDStarRatioDistMapToGoal().GetPixel(this->_Branch.CurBotData.GetGlobalBotPosition());
				const auto destValCertLog = this->_Branch.DStarMaps.GetDStarRatioCertaintyMapToGoal().GetPixel(this->_Branch.CurBotData.GetGlobalBotPosition());

				// Select correct node type
				TREE_NODE *pNewNode = nullptr;
				NODE_DATA *pNewData = nullptr;

				if(destValCertLog > OGM_LOG_MIN)
				{
					// Incomplete move
					pNewNode = ppCurNode->AddChild(NODE_DATA(MONTE_CARLO_OPTION3::NODE_ACTION_JUMP, NODE_EXTRA_DATA_JUMP()));
					pNewData = &pNewNode->GetDataR();
					auto &jumpData = dynamic_cast<NODE_EXTRA_DATA_JUMP&>(pNewData->GetExtraData());

					// Calculate path and jump data
					jumpData.PathData = this->_Branch.DStarMaps.CalculateRatioPathOverPos(this->_Branch.CurBotData.GetGlobalBotPosition());
					jumpData.RecentPathLength = destValDist;
					jumpData.RecentPathLogCertainty = destValCertLog;
				}
				else
				{
					if(destValDist > MAX_MOVE_DIST)
					{
						// Complete move path
						pNewNode = ppCurNode->AddChild(NODE_DATA(MONTE_CARLO_OPTION3::NODE_ACTION_MOVE_PATH, NODE_EXTRA_DATA_MOVE_PATH(this->_Branch.Destination, destValDist)));
					}
					else
					{
						// Complete single move
						pNewNode = ppCurNode->AddChild(NODE_DATA(MONTE_CARLO_OPTION3::NODE_ACTION_MOVE, NODE_EXTRA_DATA_MOVE(this->_Branch.Destination)));
					}
				}

				// Simulation done in next step

				newPathCreated = true;
				pathToDestCreated = true;		// Prevent path being created twice
			}
		}

		// If no new path was created, divide quad and retry
		if(!newPathCreated)
		{
			// If no division possible, stop
			if(!this->_Branch.DStarMaps.DivideQuad(this->_Branch.CurCertaintyLogData, std::vector<POS_2D>(), this->_Branch.DStarMaps.GetQuadIDAtPos(this->_Branch.CurBotData.GetGlobalBotPosition()), 1, 1))
				break;

			pCurNodeData->NumDivisions++;
		}
	}
	while(!newPathCreated);
}

int MonteCarloOption3::Simulation()
{
	// Sync up branch D* dest map
	this->_Branch.StepDownOneNodeSync();
	this->_Branch.SyncDStarMaps(1,0);

	// Simulate all children of this node
	const auto numChildren = this->_Branch.pCurNode->GetNumChildren();
	BRANCH_DATA tmpBranch(*this->_Branch.pOriginalMap, this->_Branch.CurBotData, this->_Branch.pPathStorage);;
	for(TREE_NODE::CHILD_ID curChildID = 0; curChildID < numChildren; ++curChildID)
	{
		// Check if child node is leaf
		if(this->_Branch.pCurNode->GetChild(curChildID)->IsLeaf())
			continue;		// Skip if not leaf, backtrack will take care of this node later

//		// Find leaf of this node
//		unsigned int numStepsDown = 1;
//		this->_Branch.StepDownOneNode(curChildID);

//		while(!this->_Branch.pCurNode->IsLeaf())
//		{
//			numStepsDown++;
//			this->_Branch.StepDownOneNode(0);		// There should be only one child
//		}

		// Copy data for simulation
		tmpBranch = this->_Branch;

		// Step down to node to be simulated
		tmpBranch.StepDownOneNode(curChildID);

		// Simulate this leaf
		this->SimulateBranch(tmpBranch);
	}
}

int MonteCarloOption3::Backtrack()
{
	// Check whether any child node isn't a leaf
	auto &childStorage = this->_Branch.pCurNode->GetStorage();
	for(auto curChild = childStorage.begin(); curChild != childStorage.end(); ++curChild )
	{
		if(!curChild->IsLeaf())
		{
			this->_Branch.StepDownOneNodeSimple(curChild - childStorage.begin());
			this->Backtrack_Step();		// Backtrack to child that isn't leaf
			this->_Branch.StepUpOneNodeSimple();
		}
	}

	// Continue backtracking until root is reached
	while(this->_Branch.pCurNode->GetParent() != nullptr)
	{
		this->Backtrack_Step();
		this->_Branch.StepUpOneNodeSimple();
	}

	// Perform one last backtrack to calculate root node values
	this->Backtrack_Step();
}

void MonteCarloOption3::SimulateCurrentLeaf()
{
	this->SimulateBranch(this->_Branch);
}

void MonteCarloOption3::SimulateBranch(BRANCH_DATA &BranchData)
{
	// Check leaf type
	TREE_NODE *&ppCurNode = BranchData.pCurNode;
	NODE_DATA &r_CurLeafData = ppCurNode->GetDataR();
	auto curBotPos = BranchData.CurBotData.GetGlobalBotPosition();
	// Get certainty of rreaching goal
	r_CurLeafData.Certainty = OccupancyGridMap::CalculateCertaintyProbValueFromCertaintyLog(BranchData.DStarMaps.GetDStarRatioCertaintyMapToGoal().GetPixel(curBotPos));

	// Differentiate certainty
	if(r_CurLeafData.Certainty <= OGM_PROB_MIN)
	{
		// If goal is unreachable, set this branch as done, set policy certainty to 1, and path length to 0
		r_CurLeafData.IsDone = true;
		r_CurLeafData.CertaintyPolicy = OGM_PROB_MAX;

		r_CurLeafData.ExpectedLength = 0;
	}
	else if(r_CurLeafData.Certainty >= OGM_PROB_MAX)
	{
		// If goal is definitely reachable, set branch as done, policy certainty to 1, and path length to length to goal
		r_CurLeafData.IsDone = true;
		r_CurLeafData.CertaintyPolicy = OGM_PROB_MAX;

		r_CurLeafData.ExpectedLength = BranchData.DStarMaps.GetDStarRatioDistMapToGoal().GetPixel(curBotPos);
	}
	else
	{
		// If goal might be reachable, set branch as unfinished, policy certainty to path to goal certainty (minimum), and path length to length to destination
		r_CurLeafData.IsDone = false;
		r_CurLeafData.CertaintyPolicy = r_CurLeafData.Certainty;

		r_CurLeafData.ExpectedLength = BranchData.DStarMaps.GetDStarRatioDistMapToGoal().GetPixel(curBotPos);
	}

	// Policy length is expected length to goal
	r_CurLeafData.ExpectedLengthPolicy = r_CurLeafData.ExpectedLength;

	// Simulation is only done at first visit
	r_CurLeafData.NumVisits = 1;

	// Calculate node value
	const auto *const pParentNode = BranchData.pCurNode->GetParent();
	this->CalculateNodeValue( r_CurLeafData, BranchData.Constant,  (pParentNode == nullptr ? 1 : pParentNode->GetData().NumVisits), r_CurLeafData.Value);
}

void MonteCarloOption3::Backtrack_Step()
{
	// Determine type of current node
	TREE_NODE * const&ppCurNode = this->_Branch.pCurNode;
	NODE_DATA &r_CurNodeData = ppCurNode->GetDataR();

	const TREE_NODE *const pBestChildNode = this->FindBestChildNode(*ppCurNode, r_CurNodeData.IsDone);
	if(pBestChildNode == nullptr)
		return;		// This is leaf, no backtrack possible to here

	const NODE_DATA &bestData = pBestChildNode->GetData();

	if(r_CurNodeData.Action.IsObserveAction())
	{
		// Combine both result options
		if(ppCurNode->GetNumChildren() != 2)
			return;				// ERROR

		// Get other option
		const NODE_DATA *pOtherData;
		if(pBestChildNode == ppCurNode->GetChild(0))
			pOtherData = &(ppCurNode->GetChild(1)->GetData());
		else
			pOtherData = &(ppCurNode->GetChild(0)->GetData());

		const auto bestCertainty = bestData.GetCertatintyBeforeNodeAction();
		const auto bestLength = bestData.GetLengthBeforeNodeAction();
		const auto otherCertainty = pOtherData->GetCertatintyBeforeNodeAction();
		const auto otherLength = pOtherData->GetLengthBeforeNodeAction();
		r_CurNodeData.Certainty = bestCertainty+(MONTE_CARLO_OPTION3::MAX_CERTAINTY-bestCertainty)*otherCertainty;
		r_CurNodeData.ExpectedLength = bestLength+(MONTE_CARLO_OPTION3::MAX_CERTAINTY-bestCertainty)*otherLength;
		r_CurNodeData.ExpectedCost = bestData.ExpectedCost+(MONTE_CARLO_OPTION3::MAX_CERTAINTY-bestData.Certainty)*pOtherData->ExpectedCost + this->_Branch.ObserveActionCost;		// Add cost for this observation as well
	}
	else
	{
		if(bestData.Action.IsIncompleteMove())
		{
			// Calculate certainty of reaching destination before incomplete move
			r_CurNodeData.Certainty = bestData.GetCertatintyBeforeNodeAction();
			r_CurNodeData.ExpectedLength = bestData.GetLengthBeforeNodeAction();
			r_CurNodeData.ExpectedCost = bestData.ExpectedCost;
		}
		else if(bestData.Action.IsMovementAction())
		{
			// Copy best value and add move length of this node
			const NODE_DATA &bestData = pBestChildNode->GetData();
			r_CurNodeData.Certainty = bestData.GetCertatintyBeforeNodeAction();				// Copy certainty, the uncertainty of this path is already considered in the following observation

			// Add path length of child node to this node path length
			if(bestData.Action.IsMovePath())
			{
				const auto &movePathData = dynamic_cast<const NODE_EXTRA_DATA_MOVE_PATH&>(bestData.GetExtraData());
				r_CurNodeData.ExpectedCost = bestData.ExpectedCost + this->_Branch.MoveActionCost*movePathData.PathLength;
				r_CurNodeData.ExpectedLength = bestData.ExpectedLength + movePathData.PathLength;
			}
			else
			{
				r_CurNodeData.ExpectedCost = bestData.ExpectedCost + this->_Branch.MoveActionCost*1;
				r_CurNodeData.ExpectedLength = bestData.ExpectedLength + 1;
			}
		}
		else // if(bestData.Action.IsObserveaction())
		{
			// Copy best value and add uncertainty
			const auto &observationData = dynamic_cast<const NODE_EXTRA_DATA_OBSERVE_ACTION&>(bestData.GetExtraData());
			r_CurNodeData.Certainty = OccupancyGridMap::CalculateCertaintyProbValueFromCertaintyLog( OccupancyGridMap::CalculateCertaintyLogValueFromCertaintyProb(bestData.Certainty) + observationData.ObservationFreeProbabilityLog);				// Copy certainty, the uncertainty of this path is already considered in the following observation

			r_CurNodeData.ExpectedCost = bestData.ExpectedCost + this->_Branch.ObserveActionCost;
			r_CurNodeData.ExpectedLength = bestData.ExpectedLength;
		}
	}

	// Adjust policy data
	if(r_CurNodeData.Certainty <= OGM_PROB_MIN)
	{
		r_CurNodeData.CertaintyPolicy = OGM_PROB_MAX;

		r_CurNodeData.ExpectedLength = 0;
		r_CurNodeData.ExpectedLengthPolicy = 0;

		r_CurNodeData.IsDone = true;
	}
	else if(r_CurNodeData.Certainty >= OGM_PROB_MAX)
	{
		r_CurNodeData.CertaintyPolicy = OGM_PROB_MAX;

		r_CurNodeData.ExpectedLengthPolicy = r_CurNodeData.ExpectedLength;

		r_CurNodeData.IsDone = true;
	}
	else
	{
		r_CurNodeData.CertaintyPolicy = r_CurNodeData.Certainty;
		r_CurNodeData.ExpectedLengthPolicy = r_CurNodeData.ExpectedLength;

		r_CurNodeData.IsDone = false;
	}

	// Calculate node value
	const auto *const pParentNode = this->_Branch.pCurNode->GetParent();
	this->CalculateNodeValue( r_CurNodeData, this->_Branch.Constant,  (pParentNode == nullptr ? r_CurNodeData.NumVisits : pParentNode->GetData().NumVisits), r_CurNodeData.Value);

	// Increase number of visits
	r_CurNodeData.NumVisits++;
}

//int MonteCarloOption3::CalculatePath(const BRANCH_DATA &BranchData, const POS_2D &StartPos, const POS_2D &Destination, PATH_DATA *const PathTaken, EXPECTED_LENGTH_TYPE *const ExpectedLength, CERTAINTY_TYPE *const Certainty, COST_TYPE *const Cost)
//{
//	PATH_DATA tmpPath;
//	if(!const_cast<BRANCH_DATA &>(BranchData).AStarMap.CalculatePath(BranchData.CurMapData, MONTE_CARLO_OPTION3::MAX_CERTAINTY, StartPos, Destination, &(BranchData.pDistrictStorage->GetDistrict(BranchData.GetDistrictIDAtPos(Destination))), &tmpPath, ExpectedLength))
//		return -1;		// return error

//	// Calculate Cost or certainty if requested
//	if(Cost != nullptr || Certainty != nullptr)
//	{
//		CERTAINTY_TYPE invertedCertainty = MONTE_CARLO_OPTION3::MIN_CERTAINTY;

//		for(const auto &curPos : tmpPath)
//		{
//			invertedCertainty += (MONTE_CARLO_OPTION3::MAX_CERTAINTY-invertedCertainty)*(BranchData.CurMapData.GetPixel(curPos));
//		}

//		if(Certainty != nullptr)
//			*Certainty = MONTE_CARLO_OPTION3::MAX_CERTAINTY-invertedCertainty;

//		if(Cost != nullptr)
//			*Cost = BranchData.MoveActionCost*tmpPath.size();
//	}

//	// Save path if requested
//	if(PathTaken != nullptr)
//		*PathTaken = std::move(tmpPath);

//	return 1;
//}

//int MonteCarloOption3::FollowPathUntilObstacle(const BRANCH_DATA &BranchData, const PATH_DATA &PathToTake, const CERTAINTY_TYPE &MinCertainty, const EXPECTED_LENGTH_TYPE &MinLength, const EXPECTED_LENGTH_TYPE &ScanRange, const POS_2D &StartPos, const CERTAINTY_TYPE &StartCertainty, const EXPECTED_LENGTH_TYPE &StartLength, const PATH_DATA::const_iterator *const NextForcedObstacleIterator, CERTAINTY_TYPE *const PathCertainty, EXPECTED_LENGTH_TYPE *const PathLength, PATH_DATA::ID *const EndPosID, POS_2D *const ObstaclePos)
//{
//	// Set start parameters
//	const CERTAINTY_TYPE maxInvertedLogCertainty = OccupancyGridMap::CalculateLogValueFromProb(MONTE_CARLO_OPTION3::MAX_CERTAINTY - MinCertainty);
//	CERTAINTY_TYPE curInvertedLogCertainty = OccupancyGridMap::CalculateLogValueFromProb(StartCertainty);
//	EXPECTED_LENGTH_TYPE curLength = StartLength;
//	EXPECTED_LENGTH_TYPE curDistBetweenBotAndObstacle = GetMovementCost(StartPos, PathToTake.at(0));		// Keep track of distance between robot and obstacle
//	PATH_DATA::ID obstacleID = GetInfiniteVal<PATH_DATA::ID>();
//	PATH_DATA::ID botID = GetInfiniteVal<PATH_DATA::ID>();
//	const POS_2D *pCurBotPos = &StartPos;
//	const POS_2D *pCurObstaclePos = &StartPos;

//	// Go through path until length reaches minimum AND Certainty reaches minimum
//	for(auto curPathPos = PathToTake.begin(); curPathPos != PathToTake.end(); ++curPathPos)
//	{
//		// Update certainty and length
//		curInvertedLogCertainty += BranchData.CurLogData.GetPixel(*curPathPos);
//		const auto movementCost = GetMovementCost(*curPathPos, *pCurObstaclePos);
//		curLength += movementCost;
//		curDistBetweenBotAndObstacle += movementCost;

//		// Move bot until next obstacel pos is back in scan range
//		while(curDistBetweenBotAndObstacle > ScanRange)
//		{
//			// Move bot up along path
//			botID++;

//			// Update distance
//			curDistBetweenBotAndObstacle -= GetMovementCost(*pCurBotPos, PathToTake.at(botID));

//			// Update robot position
//			pCurBotPos = &(PathToTake.at(botID));
//		}

//		// Update obstacle position
//		pCurObstaclePos = &(*curPathPos);

//		// If we reach a forced obstacle position, create an obstacle here
//		if(NextForcedObstacleIterator != nullptr)
//		{
//			if(*NextForcedObstacleIterator == curPathPos)
//			{
//				obstacleID = static_cast<PATH_DATA::ID>((*NextForcedObstacleIterator)-PathToTake.begin());
//				break;
//			}
//		}

//		// Check if both the certainty and length is in correct area
//		if(curLength >= MinLength && curInvertedLogCertainty >= maxInvertedLogCertainty)
//		{
//			// Stop if both conditions are met
//			obstacleID = static_cast<PATH_DATA::ID>((*NextForcedObstacleIterator)-PathToTake.begin());
//			break;
//		}
//	}

//	// Save the requested data
//	if(PathCertainty != nullptr)
//		*PathCertainty = MONTE_CARLO_OPTION3::MAX_CERTAINTY - OccupancyGridMap::CalculateProbValueFromLog(curInvertedLogCertainty);
//	if(PathLength != nullptr)
//		*PathLength = std::move(curLength);
//	if(EndPosID != nullptr)
//		*EndPosID = std::move(botID);
//	if(ObstaclePos != nullptr)
//		*ObstaclePos = *pCurObstaclePos;

//	// Check if a path was found
//	if(obstacleID == GetInfiniteVal<PATH_DATA::ID>())
//		return -1;

//	// Check if bot moved
//	if(botID == GetInfiniteVal<PATH_DATA::ID>())
//		return 0;

//	return 1;
//}

//MonteCarloOption3::NODE_EXTRA_DATA_OBSTACLE &&MonteCarloOption3::CreateObstacleAtPos(const BRANCH_DATA &BranchData, const POS_2D &ObstaclePosition, const POS_2D &BotPosition, const EXPECTED_LENGTH_TYPE &ObstacleLength, const EXPECTED_LENGTH_TYPE &ObstacleHeight, const CERTAINTY_TYPE &MinObstacleCertainty, const CERTAINTY_TYPE &MaxObstacleCertainty)
//{
//	NODE_EXTRA_DATA_OBSTACLE newObstacle;

//	if(ObstacleHeight == 1 && ObstacleLength == 1)
//	{
//		newObstacle.push_back(MONTE_CARLO_OPTION3::OBSTACLE_DATA(ObstaclePosition, MONTE_CARLO_OPTION3::MAX_CERTAINTY));

//		return std::move(newObstacle);
//	}

//	// Calculate height and width angle
//	float heightAngle = std::atan2(ObstaclePosition.Y-BotPosition.Y, ObstaclePosition.X - BotPosition.X);
//	float widthAngle = (heightAngle < -M_PI_2 ? -heightAngle+M_PI_2 : heightAngle-M_PI_2);

//	// Get start position and increment
//	const float incrementAlongWidthX = 1;
//	const float incrementAlongWidthY = std::sin(widthAngle)/std::cos(widthAngle);
//	const float incrementAlongHeightX = std::cos(heightAngle)/std::sin(heightAngle);
//	const float incrementAlongHeightY = 1;
//	const POS_2D_TYPE startPosX = std::round(std::cos(-widthAngle)*ObstacleLength/2+ObstaclePosition.X);
//	const POS_2D_TYPE startPosY = std::round(std::sin(-widthAngle)*ObstacleLength/2+ObstaclePosition.Y);

//	// Move obstacle data
//	for(float curIncrementAlongWidthX = 0, curIncrementAlongWidthY = 0; curIncrementAlongWidthX <= ObstacleLength; curIncrementAlongWidthX += incrementAlongWidthX, curIncrementAlongWidthY += incrementAlongWidthY)
//	{
//		for(float curIncrementAlongHeightX = 0, curIncrementAlongHeightY = 0; curIncrementAlongHeightY <= ObstacleHeight; curIncrementAlongHeightX += incrementAlongHeightX, curIncrementAlongHeightY += incrementAlongHeightY)
//		{
//			// Check if current position is in map
//			const MONTE_CARLO_OPTION3::OBSTACLE_DATA curPos = MONTE_CARLO_OPTION3::OBSTACLE_DATA(POS_2D(std::round(curIncrementAlongWidthX+curIncrementAlongHeightX+startPosX), std::round(curIncrementAlongWidthY+curIncrementAlongHeightY+startPosY)), MONTE_CARLO_OPTION3::MAX_CERTAINTY);

//			if(BranchData.CurMapData.IsInMap(static_cast<POS_2D>(curPos)))
//				newObstacle.push_back(curPos);
//		}
//	}

//	return std::move(newObstacle);
//}

//void MonteCarloOption3::RunSimulation(const POS_2D &BotPos, const POS_2D &Destination, const BRANCH_DATA &MapData, const NUM_VISITS_TYPE &NumParentsVisit, NODE_DATA &Result)
//{
//	if(BotPos == Destination)
//	{
//		// If we reached destination, set node as done
//		Result.IsDone = true;

//		// As we are at dest, the cost, length, and certainty are trivial
//		Result.ExpectedCost = 0;
//		Result.ExpectedLength = 0;
//		Result.Certainty = 1;
//	}
//	else
//	{
//		// Calculate one path to destination
//		this->CalculatePath(MapData, BotPos, Destination, nullptr, &(Result.ExpectedLength), &(Result.Certainty), &(Result.ExpectedCost));

//		if(Result.Certainty <= MONTE_CARLO_OPTION3::MIN_CERTAINTY)
//		{
//			Result.IsDone = true;
//			Result.ExpectedCost = GetInfiniteVal<EXPECTED_LENGTH_TYPE>();
//			Result.ExpectedLength = GetInfiniteVal<EXPECTED_LENGTH_TYPE>();
//		}
//	}

//	// Set visits to this node to one
//	Result.NumVisits = 1;

//	// Calculate node value from this data
//	this->CalculateNodeValue(Result, this->_Branch.Constant, NumParentsVisit, Result.Value);
//}

void MonteCarloOption3::CalculateNodeValue(const NODE_DATA &Data, const NODE_VALUE_TYPE &Constant, const NUM_VISITS_TYPE &NumParentsVisit, NODE_VALUE_TYPE &Value)
{
	// Node is done, don't select it
	if(Data.IsDone)
		Value = MONTE_CARLO_OPTION3::MIN_NODE_VALUE;
	else
		Value = Data.Certainty/Data.ExpectedLength + Constant*static_cast<NODE_VALUE_TYPE>(std::sqrt(static_cast<double>(NumParentsVisit)/Data.NumVisits));
}

bool MonteCarloOption3::CompareIncompleteMoveAndPath(const CERTAINTY_TYPE MoveCertainty, const EXPECTED_LENGTH_TYPE MoveLength, const CERTAINTY_TYPE PathCertainty, const EXPECTED_LENGTH_TYPE PathLength)
{
	// Return 1 if incomplete move is larger than certain fraction of entire path
	if(MoveCertainty*MoveLength >= this->_Branch.MinMoveFraction * PathCertainty * PathLength)
		return 1;

	return 0;
}

const MonteCarloOption3::TREE_NODE *MonteCarloOption3::FindBestChildNode(const TREE_NODE &CurNode, bool &AllDone) const
{
	if(CurNode.GetNumChildren() == 0)
		return nullptr;

	const TREE_NODE *pBestNode = CurNode.GetChild(0);
	NODE_VALUE_TYPE bestValue = this->CalculateComparableChildValue(pBestNode->GetData());
	AllDone = true;
	for(auto curChildIterator = CurNode.GetStorage().begin()+1; curChildIterator != CurNode.GetStorage().end(); ++curChildIterator)
	{
		if(!curChildIterator->GetData().IsDone)
			AllDone = false;

		const auto curValue = this->CalculateComparableChildValue(curChildIterator->GetData());
		if(curValue < bestValue)
		{
			bestValue = curValue;
			pBestNode = &(*curChildIterator);
		}
	}

	return pBestNode;
}

MonteCarloOption3::NODE_VALUE_TYPE MonteCarloOption3::CalculateComparableChildValue(const NODE_DATA &ChildNodeData) const
{
	return ChildNodeData.ExpectedLength;
}
