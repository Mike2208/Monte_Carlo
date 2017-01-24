#include "monte_carlo_option3.h"
#include "algorithm_a_star.h"

#include <cstring>
#include <time.h>

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

	bool NODE_EXTRA_DATA::operator==(const NODE_EXTRA_DATA &S) const
	{
		// If actions aren't the same, return false
		if(this->Action != S.Action)
			return false;

		if(this->Action.IsObserveAction())
		{
			// Return true if both observe same point
			if(dynamic_cast<const NODE_EXTRA_DATA_OBSERVE_ACTION&>(*this->_pExtraData).ObservationPoint == dynamic_cast<const NODE_EXTRA_DATA_OBSERVE_ACTION&>(*S._pExtraData).ObservationPoint)
				return true;
		}
		else if (this->Action.IsIncompleteMove())
		{
			// Return true if both move to same point next
			if(dynamic_cast<const NODE_EXTRA_DATA_JUMP&>(*this->_pExtraData).PathData.front() == dynamic_cast<const NODE_EXTRA_DATA_JUMP&>(*S._pExtraData).PathData.front())
				return true;
		}
		else if (this->Action.IsCompleteMove())
		{
			// Return true if both move to same point next
			if(dynamic_cast<const NODE_EXTRA_DATA_MOVE&>(*this->_pExtraData).NewPos == dynamic_cast<const NODE_EXTRA_DATA_MOVE&>(*S._pExtraData).NewPos)
				return true;
		}
		else if (this->Action.IsMovePath())
		{
			// Return true if both move to same point
			if(dynamic_cast<const NODE_EXTRA_DATA_MOVE_PATH&>(*this->_pExtraData).TargetPosition == dynamic_cast<const NODE_EXTRA_DATA_MOVE_PATH&>(*S._pExtraData).TargetPosition)
				return true;
		}

		return false;
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
	NODE_DATA::NODE_DATA(const NODE_ACTION _Action, const NODE_VALUE_TYPE &_Value, const EXPECTED_LENGTH_TYPE &_ExpectedLength, const CERTAINTY_TYPE &_Certainty, const COST_TYPE &_ExpectedCost, const NUM_VISIT_TYPE &_NumVisits, bool _IsDone, NODE_EXTRA_DATA &&_ExtraData) : NODE_EXTRA_DATA(std::move(_ExtraData)), Value(_Value), ExpectedLength(_ExpectedLength), Certainty(_Certainty), ExpectedCost(_ExpectedCost), NumVisits(_NumVisits), IsDone(_IsDone)
	{
		this->Action = _Action;
	}

	NODE_DATA::NODE_DATA(const NODE_ACTION _Action) : NODE_EXTRA_DATA()
	{
		this->Action = _Action;
	}

	NODE_DATA::NODE_DATA(const NODE_ACTION _Action, NODE_EXTRA_DATA &&_ExtraData) : NODE_EXTRA_DATA(std::move(_ExtraData))
	{
		this->Action = _Action;
	}

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

	std::unique_ptr<NODE_EXTRA_DATA_OBJECT> NODE_EXTRA_DATA_MOVE_PATH::Create() const
	{
		// Return with move to prevent destruction
		return std::move(std::unique_ptr<NODE_EXTRA_DATA_OBJECT>(new NODE_EXTRA_DATA_MOVE_PATH()));
	}

	std::unique_ptr<NODE_EXTRA_DATA_OBJECT> NODE_EXTRA_DATA_MOVE_PATH::Clone() const
	{
		// Return with move to prevent destruction
		return std::move(std::unique_ptr<NODE_EXTRA_DATA_OBJECT>(new NODE_EXTRA_DATA_MOVE_PATH(*this)));
	}

	OBSTACLE_DATA::OBSTACLE_DATA(const POS_2D &_Position, const CERTAINTY_TYPE &_OccupancyPercentage) : POS_2D(_Position), OccupancyPercentage(_OccupancyPercentage)
	{}

	std::unique_ptr<NODE_EXTRA_DATA_OBJECT> NODE_EXTRA_DATA_OBSERVE_ACTION::Create() const
	{
		// Return with move to prevent destruction
		return std::move(std::unique_ptr<NODE_EXTRA_DATA_OBJECT>(new NODE_EXTRA_DATA_OBSERVE_ACTION()));
	}

	std::unique_ptr<NODE_EXTRA_DATA_OBJECT> NODE_EXTRA_DATA_OBSERVE_ACTION::Clone() const
	{
		// Return with move to prevent destruction
		return std::move(std::unique_ptr<NODE_EXTRA_DATA_OBJECT>(new NODE_EXTRA_DATA_OBSERVE_ACTION(*this)));
	}

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
		return std::move(std::unique_ptr<NODE_EXTRA_DATA_OBJECT>(new NODE_EXTRA_DATA_MOVE(*this)));
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
				}

				// Clear previously visited quads to let them be visited again
				this->VisitedQuads.clear();
				this->PrevPath.clear();

				this->PrevNodeWasObserveAction = false;
			}

			// Divide quad by the requested number of times
			const auto &curData = this->pCurNode->GetData();
			for(size_t divideNum = 0; divideNum < curData.NumDivisions; ++divideNum)
			{
				this->DStarMaps.DivideQuad(this->CurCertaintyLogData, std::vector<POS_2D>(), this->DStarMaps.GetQuadIDAtPos(this->CurBotData.GetGlobalBotPosition()), 1 , 0);
			}

			if(nextData.Action.IsCompleteMove())
			{
				// If this node is a move action, update robot position
				const NODE_EXTRA_DATA_MOVE &moveData = dynamic_cast<const NODE_EXTRA_DATA_MOVE&>(nextData.GetExtraData());

				// Move bot to next position
				this->CurBotData.SetGlobalBotPosition(moveData.NewPos);

				// Add to quad vector
				const auto curQuad = this->DStarMaps.GetQuadIDAtPos(moveData.NewPos);
				if(this->VisitedQuads.empty() || this->VisitedQuads.back() != curQuad)
					this->VisitedQuads.push_back(curQuad);

				// Add to path
				this->PrevPath.push_back(moveData.NewPos);
			}
			else if (nextData.Action.IsMovePath())
			{
				// If this node is a move action along a path, update robot position
				const NODE_EXTRA_DATA_MOVE_PATH &moveData = dynamic_cast<const NODE_EXTRA_DATA_MOVE_PATH&>(nextData.GetExtraData());

				// Move bot to next position
				this->CurBotData.SetGlobalBotPosition(moveData.TargetPosition);

				// Add to quad vector
				const auto curQuad = this->DStarMaps.GetQuadIDAtPos(moveData.TargetPosition);
				if(this->VisitedQuads.empty() || this->VisitedQuads.back() != curQuad)
					this->VisitedQuads.push_back(curQuad);

				// Add to path
				this->PrevPath.push_back(moveData.TargetPosition);
			}
			else if(nextData.Action.IsIncompleteMove())
			{
				// If this node is a jump action, update robot position and map
				const NODE_EXTRA_DATA_JUMP &jumpData = dynamic_cast<const NODE_EXTRA_DATA_JUMP&>(nextData.GetExtraData());

				// Set all visited positions to free
				for(const auto &curPos : jumpData.PathData)
				{
					// Remove visited cells after observation
					if(this->CurCertaintyLogData.GetPixel(curPos) > OGM_LOG_MIN)
					{
						this->VisitedQuads.clear();
						this->PrevPath.clear();
					}

					this->SetMapPositionDiscrete(curPos, 0);

					// Add to visited quads
					const auto curQuad = this->DStarMaps.GetQuadIDAtPos(curPos);
					if(this->VisitedQuads.empty() || this->VisitedQuads.back() != curQuad)
						this->VisitedQuads.push_back(curQuad);

					// Add to path
					this->PrevPath.push_back(curPos);
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

			this->_StepDownSynced = true;
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

	void BRANCH_DATA::Init(const std::vector<OGM_MAP> &OriginalMaps, const POS_2D &Start, const POS_2D &Destination, TREE_CLASS &TreeData)
	{
		this->pOriginalMaps = &OriginalMaps;
		//this->VisitMap.ResetMap(OriginalMap.GetWidth(), OriginalMap.GetHeight(), 0);
		this->pCurNode = &(TreeData.GetRoot());
		//this->RemainingMapEntropy = OccupancyGridMap::CalculateEntropyFromMap(OriginalMap);
		this->CurBotData.SetGlobalBotPosition(Start);
		//this->PrevPath.clear();
		this->VisitedQuads.clear();
		this->PosToUpdate.clear();
		this->Destination = Destination;
		this->Start = Start;
		this->PrevNodeWasObserveAction = false;

		this->_StepDownPossible = true;
		this->_StepDownSynced = true;

		// Calculate total map certainties
		this->MapCertainties.resize(this->pOriginalMaps->size(), 1.0f);
		this->MapCertaintiesNormalized.resize(this->pOriginalMaps->size(), 1.0f/(this->pOriginalMaps->size()));

		// Calculate map data at start
		//OccupancyGridMap::CalculateLogMapFromCellMap(OriginalMap, this->CurLogData);
		this->CurCertaintyLogData.ResizeMap(OriginalMaps.at(0).GetWidth(), OriginalMaps.at(0).GetHeight());
		this->UpdateTotalMap();

		// Set start position to cleared
		this->SetMapPositionDiscrete(Start, OGM_DISCRETE_EMPTY);
		this->PosToUpdate.clear();

		// Calculate D* maps
		this->DStarMaps.ResetDestPos(this->CurCertaintyLogData, Destination);
	}

	BRANCH_DATA::BRANCH_DATA(const std::vector<OGM_MAP> &MapData, RobotData &_RobotData, MCPathStorage *const PathStorage) : pOriginalMaps(&MapData), CurBotData(_RobotData), pPathStorage(PathStorage), DStarMaps(MapData.at(0).GetWidth(), MapData.at(0).GetHeight())
	{}

	void BRANCH_DATA::SetMapPositionDiscrete(const POS_2D &Position, const bool &NewValue)
	{
		// Update map entropy of this cell and all cells around it with the robot radius
		auto &r_oldPosData = this->CurCertaintyLogData.GetPixelR(Position);
		if((NewValue && r_oldPosData < OGM_LOG_MAX)			// Only update at change
			|| (!NewValue && r_oldPosData > OGM_LOG_MIN))
		{
			this->RemainingMapEntropy += 0-OccupancyGridMap::CalculateEntropyFromProb(OccupancyGridMap::CalculateCertaintyProbValueFromCertaintyLog(r_oldPosData));

			// Set this cell and update all cells around it with the robot radius
			// NOTE: Use inverse for certainty!!!
			if(NewValue)
				r_oldPosData =  OGM_LOG_MAX;
			else
				r_oldPosData =  OGM_LOG_MIN;

			// Add to vector with updated positions
			this->PosToUpdate.push_back(Position);
		}
	}

	void BRANCH_DATA::UpdateMapCertainties(const CERTAINTY_TYPE PrevOccupancyProb, const bool PosOccupied)
	{
		CERTAINTY_TYPE totalCertainty = 0;
		for(auto &curCertainty : this->MapCertainties)
		{
			if(PosOccupied)
				curCertainty *= PrevOccupancyProb;
			else
				curCertainty *= (1.0f-PrevOccupancyProb);

			totalCertainty += curCertainty;
		}

		for(size_t curNormCertID = 0; curNormCertID < this->MapCertaintiesNormalized.size(); ++curNormCertID)
		{
			this->MapCertaintiesNormalized.at(curNormCertID) = this->MapCertainties.at(curNormCertID)/totalCertainty;
		}
	}

	void BRANCH_DATA::UpdateTotalMap()
	{
		POS_2D curPos;
		for(curPos.X = 0; curPos.X < this->CurCertaintyLogData.GetWidth(); ++curPos.X)
		{
			for(curPos.Y = 0; curPos.Y < this->CurCertaintyLogData.GetHeight(); ++curPos.Y)
			{
				auto &r_CurVal = this->CurCertaintyLogData.GetPixelR(curPos);
				if(r_CurVal > OGM_LOG_MIN && r_CurVal < OGM_LOG_MAX)
				{
					const auto prevVal = r_CurVal;

					r_CurVal = 0;
					for(size_t curMapID = 0; curMapID < this->pOriginalMaps->size(); ++curMapID)
					{
						r_CurVal += this->MapCertaintiesNormalized.at(curMapID)*this->pOriginalMaps->at(curMapID).GetPixel(curPos);
					}

					r_CurVal = OccupancyGridMap::CalculateCertaintyLogFromProb(r_CurVal/OGM_CELL_MAX);

					if(prevVal != r_CurVal)
						this->PosToUpdate.push_back(curPos);
				}
			}
		}
	}

	void BRANCH_DATA::SetMapPositionFromCertaintyLogValue(const POS_2D &Position, const OGM_LOG_TYPE &CertaintyValue)
	{
		// Update map entropy of this cell and all cells around it with the robot radius
		auto &r_oldPosData = this->CurCertaintyLogData.GetPixelR(Position);
		if(CertaintyValue != r_oldPosData)		// Only update at change
		{
			this->RemainingMapEntropy += OccupancyGridMap::CalculateEntropyFromProb(OccupancyGridMap::CalculateCertaintyProbValueFromCertaintyLog(CertaintyValue))-OccupancyGridMap::CalculateEntropyFromProb(OccupancyGridMap::CalculateCertaintyProbValueFromCertaintyLog(r_oldPosData));

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

MonteCarloOption3::MonteCarloOption3(const std::vector<OccupancyGridMap> &Maps, RobotData _RobotData, const MC_PATH_STORAGE::PATH_ID &MaxStoredPaths) : _MCTree(), _Branch(Maps, _RobotData, &(this->_PathStorage)), _StartBranch(Maps, _RobotData, &(this->_PathStorage)), _PathStorage(MaxStoredPaths)
{}

int MonteCarloOption3::PerformMonteCarlo(const POS_2D &StartPos, const POS_2D &Destination, const NODE_VALUE_TYPE &Constant, const CheckConditions &StopConditions, const char *const PolicyFileName, PolicyData &NewPolicy)
{
	// Set current map to use
	this->_Branch.Init(*this->_Branch.pOriginalMaps, StartPos, Destination, this->_MCTree);
	this->_Branch.Constant = Constant;		// Set explore parameter

	// Create tree
	this->_MCTree.Reset();
	this->_MCTree.GetRoot().GetDataR() = NODE_DATA(MONTE_CARLO_OPTION3::NODE_ACTION_NONE, NODE_EXTRA_DATA());		// Set start node as no action

	// Start at root
	this->_Branch.pCurNode = &(this->_MCTree.GetRoot());

	// Store root data for later
	this->_StartBranch = this->_Branch;

	// End Condition: Continue until all paths are checked
	const auto &rootNodeData = this->_StartBranch.pCurNode->GetData();
	auto startTime = time(nullptr);
	while(!StopConditions.AreConditionsMet(rootNodeData.IsDone, rootNodeData.NumVisits, time(nullptr)-startTime, rootNodeData.CertaintyPolicy, rootNodeData.Certainty, rootNodeData.MapCertainty))
	{
		// Perform one Monte Carlo step
		this->Selection();
		this->Expansion();
		this->Simulation();
		this->Backtrack();

		// Revert to start
		this->_Branch = this->_StartBranch;
	}

	NewPolicy = this->CreatePolicyFromTree(PolicyFileName);

	return 1;
}

int MonteCarloOption3::Selection()
{
	TREE_NODE *const &ppCurNode = this->_Branch.pCurNode;

	bool expandIncompleteMove;

	const auto &firstNodeData = ppCurNode->GetData();

	// If first node is incomplete move, select whether to move down or expand node
	if(firstNodeData.Action.IsIncompleteMove())
	{
		const NODE_EXTRA_DATA_JUMP &jumpData = dynamic_cast<const NODE_EXTRA_DATA_JUMP &>(firstNodeData.GetExtraData());

		if(this->CompareIncompleteMoveAndPath(OccupancyGridMap::CalculateCertaintyProbValueFromCertaintyLog(jumpData.RecentPathLogCertainty), jumpData.RecentPathLength, firstNodeData.Certainty, firstNodeData.ExpectedLength))
		{
			// If expansion better, stop
			expandIncompleteMove = true;

			return 2;	// Current node will be expanded
		}
	}

	expandIncompleteMove = false;

	while(!ppCurNode->IsLeaf())
	{
		// find first child that isn't done
		TREE_NODE::CHILD_ID bestChildID = ppCurNode->GetNumChildren();
		MONTE_CARLO_OPTION3::NODE_VALUE_TYPE bestValue;
		TREE_NODE::CHILD_ID curChildID = 0;
		for(;curChildID < ppCurNode->GetNumChildren(); ++curChildID)
		{
			// Stop if this child branch isn't fully explored
			const auto &curChildNodeData = ppCurNode->GetChild(curChildID)->GetData();
			if(!curChildNodeData.IsDone)
				break;
			else if(curChildNodeData.Action.IsIncompleteMove())
			{
				// break if path is uncertain
				if(dynamic_cast<const NODE_EXTRA_DATA_JUMP&>(curChildNodeData.GetExtraData()).RecentPathLogCertainty > OGM_LOG_MIN)
				{
					break;
				}
			}
		}

		// Node already fully explored, return finished
		if(curChildID == ppCurNode->GetNumChildren())
		{
			// If this node can be expanded, do it
			const auto &curNodeData = ppCurNode->GetData();
			if(curNodeData.Action.IsIncompleteMove() && dynamic_cast<const NODE_EXTRA_DATA_JUMP&>(curNodeData.GetExtraData()).RecentPathLogCertainty > OGM_LOG_MIN)
			{
				expandIncompleteMove = true;
				break;
			}
			else
				return 0;		// else Return that everything is done
		}

		bestValue = ppCurNode->GetChild(curChildID)->GetData().Value;
		bestChildID = curChildID;
		for(curChildID += 1; curChildID < ppCurNode->GetNumChildren(); ++curChildID)
		{
			// Skip if this node is already done
			const auto &childNodeData = ppCurNode->GetChild(curChildID)->GetData();
			if(childNodeData.IsDone)
			{
				if(childNodeData.Action.IsIncompleteMove())
				{
					// Skip if path is done
					if(dynamic_cast<const NODE_EXTRA_DATA_JUMP&>(childNodeData.GetExtraData()).RecentPathLogCertainty <= OGM_LOG_MIN)
						continue;
				}
				else
					continue;
			}

			const MONTE_CARLO_OPTION3::NODE_VALUE_TYPE curValue = ppCurNode->GetChild(curChildID)->GetData().Value;
			if(curValue > bestValue)
			{
				bestValue = curValue;
				bestChildID = curChildID;
			}
		}

		// check best child node
		const auto &bestChildData = ppCurNode->GetChild(bestChildID)->GetData();
		if(bestChildData.Action.IsIncompleteMove())
		{
			// break if path is uncertain
			if(dynamic_cast<const NODE_EXTRA_DATA_JUMP&>(bestChildData.GetExtraData()).RecentPathLogCertainty > OGM_LOG_MIN)
			{
				// update whether expansion is better
				const NODE_EXTRA_DATA_JUMP &jumpData = dynamic_cast<const NODE_EXTRA_DATA_JUMP &>(bestChildData.GetExtraData());

				if(this->CompareIncompleteMoveAndPath(OccupancyGridMap::CalculateCertaintyProbValueFromCertaintyLog(jumpData.RecentPathLogCertainty), jumpData.RecentPathLength, bestChildData.Certainty, bestChildData.ExpectedLength))
				{
					// If expansion better, stop
					expandIncompleteMove = true;
					this->_Branch.StepDownOneNodeSimple(bestChildID);

					break;
				}
//				else
//					expandIncompleteMove = false;
			}
//			else
//				expandIncompleteMove = false;
		}
//		else
//			expandIncompleteMove = false;

		expandIncompleteMove = false;

		// Move to next child
		this->_Branch.StepDownOneNodeSimple(bestChildID);
	}

	// Check selected node
	if(!expandIncompleteMove)
	{
		this->_Branch.StepDownOneNodeSync();		// Sync up if no expansion of incomplete move planned

		// Return that leaf node was selected
		return 1;
	}
	else
		return 2;	// return that expansion will be done
}

int MonteCarloOption3::Expansion()
{
	TREE_NODE *&ppCurNode = this->_Branch.pCurNode;
	NODE_DATA *pCurNodeData = &ppCurNode->GetDataR();

	// Check whether to expand or fill in skipped cells
	if(pCurNodeData->Action.IsIncompleteMove() && !this->_Branch.IsNodeSynced())
	{
		// Insert skipped cell
		this->InsertSkippedCell(this->_Branch);
	}
	else
	{
		// Perform expansion at leaf node
		this->PerformLeafExpansion(this->_Branch);
	}

	return 1;
}

int MonteCarloOption3::Simulation()
{
	// Sync up branch D* dest map
	this->_Branch.StepDownOneNodeSync();
	this->_Branch.SyncDStarMaps(1,0);

	// Check if children exist
	if(this->_Branch.pCurNode->IsLeaf())
	{
		// If none exist, only simulate this node
		this->SimulateCurrentLeaf();
	}
	else
	{
		// Simulate all children of this node
		const auto numChildren = this->_Branch.pCurNode->GetNumChildren();
		BRANCH_DATA tmpBranch(*this->_Branch.pOriginalMaps, this->_Branch.CurBotData, this->_Branch.pPathStorage);;
		for(TREE_NODE::CHILD_ID curChildID = 0; curChildID < numChildren; ++curChildID)
		{
			// Check if child node is leaf
			if(!this->_Branch.pCurNode->GetChild(curChildID)->IsLeaf())
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
			tmpBranch.SyncDStarMaps(1, 0);

			// Simulate this leaf
			this->SimulateBranch(tmpBranch);
		}
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
			this->Backtrack_Step(this->_Branch);		// Backtrack to child that isn't leaf
			this->_Branch.StepUpOneNodeSimple();
		}
	}

	// Continue backtracking until root is reached
	while(this->_Branch.pCurNode->GetParent() != nullptr)
	{
		this->Backtrack_Step(this->_Branch);
		this->_Branch.StepUpOneNodeSimple();
	}

	// Perform one last backtrack to calculate root node values
	this->Backtrack_Step(this->_Branch);

	return 1;
}

PolicyData MonteCarloOption3::CreatePolicyFromTree(const char *FileName)
{
	auto branch = this->_StartBranch;

	// Convert tree to policy
	PolicyData::TREE_NODE rootNode;

	// Create header
	const auto &rootData = branch.pCurNode->GetData();
	PolicyData::FILE_HEADER header;
	header.DestPos = branch.Destination;
	header.StartPos = branch.Start;
	header.GoalCertainty = rootData.Certainty;
	header.LengthToGoal = rootData.ExpectedLength;
	header.MapHeight = branch.CurCertaintyLogData.GetHeight();
	header.MapWidth = branch.CurCertaintyLogData.GetWidth();
	header.LengthToPolicyCompletion = rootData.ExpectedLengthPolicy;
	header.PolicyCertainty = rootData.CertaintyPolicy;

	// Convert current node to policy node
	this->CreatePolicyFromTreeStep(branch, rootNode);

	// Write policy to file and return it
	PolicyData newPolicy;
	newPolicy.WritePolicyToFile(FileName, header, rootNode);
	return newPolicy;
}

void MonteCarloOption3::CreatePolicyFromTreeStep( BRANCH_DATA &Branch, PolicyData::TREE_NODE &PolicyNode )
{
	TREE_NODE *&ppCurNode = Branch.pCurNode;
	const NODE_DATA *pCurLeafData = &ppCurNode->GetData();

	auto *pPolData = &PolicyNode.GetDataR();

	// Sync data
	Branch.StepDownOneNodeSync();

	// Get map state
	OccupancyGridMap::CalculateProbMapFromCertaintyLogMap(pPolData->MapState, Branch.CurCertaintyLogData);

	// Find next observe action or leaf
	while(!pCurLeafData->Action.IsObserveAction() && !ppCurNode->IsLeaf() && !pCurLeafData->Action.IsIncompleteMove())
	{
		bool tmp;
		auto *bestChildLeaf = this->FindBestChildNode(*ppCurNode, tmp);

		// Move down to best child
		Branch.StepDownOneNodeSimple(bestChildLeaf-&(ppCurNode->GetStorage().front()));
		pCurLeafData = &ppCurNode->GetData();
	}

	if(pCurLeafData->Action.IsObserveAction())
	{
		Branch.StepDownOneNodeSync();

		// At observe action, create observation
		pPolData->MovePos = Branch.CurBotData.GetGlobalBotPosition();
		pPolData->ScanPos = dynamic_cast<const NODE_EXTRA_DATA_OBSERVE_ACTION&>(pCurLeafData->GetExtraData()).ObservationPoint;

		// Create two children
		auto *pFreeNode = PolicyNode.AddChild(PolicyData::NODE_DATA());
		auto *pOccupiedNode = PolicyNode.AddChild(PolicyData::NODE_DATA());

		// Add two children
		auto occupiedBranch = Branch;

		// Free node
		Branch.StepDownOneNode(0);
		this->CreatePolicyFromTreeStep(Branch, *pFreeNode);

		// Occupied node
		occupiedBranch.StepDownOneNode(1);
		this->CreatePolicyFromTreeStep(occupiedBranch, *pOccupiedNode);
	}
	else if(pCurLeafData->Action.IsIncompleteMove())
	{
		auto *pPolNode = &PolicyNode;
		pPolData = &pPolNode->GetDataR();

		auto freeBranch = Branch;

		// Map Certainty
		size_t bestID = 0;
		float bestCert = 0;
		for(size_t curMapID = 0; curMapID < Branch.MapCertaintiesNormalized.size(); ++curMapID)
		{
			if(Branch.MapCertaintiesNormalized[curMapID] > bestCert)
			{
				bestCert = Branch.MapCertaintiesNormalized[curMapID];
				bestID = curMapID;
			}
		}

		// Create new orders for every cell
		auto prevPos = Branch.CurBotData.GetGlobalBotPosition();
		const auto &skippedPath = dynamic_cast<const NODE_EXTRA_DATA_JUMP&>(pCurLeafData->GetExtraData()).PathData;
		for(const auto &curPos : skippedPath)
		{
			pPolData->MovePos = prevPos;
			pPolData->ScanPos = curPos;

			auto occupiedBranch = Branch;

			// Free Node and occupied node
			auto *pFreeNode = pPolNode->AddChild(PolicyData::NODE_DATA());
			auto *pOccupiedNode = pPolNode->AddChild(PolicyData::NODE_DATA());

			// Occupied data
			occupiedBranch.SetMapPositionDiscrete(curPos, 1);
			auto &occupiedData = pOccupiedNode->GetDataR();
			OccupancyGridMap::CalculateProbMapFromCertaintyLogMap(occupiedData.MapState, occupiedBranch.CurCertaintyLogData);
			occupiedData.MovePos = prevPos;
			occupiedData.ScanPos = prevPos;
			occupiedData.PolicyState = POLICY_DATA::POLICY_UNKNOWN;		// Set to error result
			occupiedData.MapID = bestID;

			// Free data
			freeBranch.SetMapPositionDiscrete(curPos, 0);
			auto &freeData = pOccupiedNode->GetDataR();
			OccupancyGridMap::CalculateProbMapFromCertaintyLogMap(freeData.MapState, freeBranch.CurCertaintyLogData);
			freeData.ScanPos = curPos;
			freeData.PolicyState = POLICY_DATA::POLICY_UNKNOWN;		// Set to error result
			freeData.MapID = bestID;

			// Move to next free node
			pPolNode = pFreeNode;
			pPolData = &pPolNode->GetDataR();

			prevPos = curPos;
		}

		// Afterwards, sync up
		Branch.StepDownOneNodeSync();
		pCurLeafData = &ppCurNode->GetData();
		if(!ppCurNode->IsLeaf())
			this->CreatePolicyFromTreeStep(Branch, *pPolNode);		// Continue if not done yet
		else
		{
			// At leaf, compute policy output
			if(pCurLeafData->CertaintyPolicy >= OGM_PROB_MAX)
			{
				if(pCurLeafData->Certainty >= OGM_PROB_MAX)
					pPolData->PolicyState = POLICY_DATA::POLICY_SUCCESS;		// Return success
				else
					pPolData->PolicyState = POLICY_DATA::POLICY_BLOCKED;		// Return no path available
			}
			else
			{
				pPolData->PolicyState = POLICY_DATA::POLICY_UNKNOWN;		// Return error
			}

			// Map Certainty
			pPolData->MapID = 0;
			float bestCert = 0;
			for(size_t curMapID = 0; curMapID < Branch.MapCertaintiesNormalized.size(); ++curMapID)
			{
				if(Branch.MapCertaintiesNormalized[curMapID] > bestCert)
				{
					bestCert = Branch.MapCertaintiesNormalized[curMapID];
					pPolData->MapID = curMapID;
				}
			}
		}
	}
	else if(ppCurNode->IsLeaf())
	{
		Branch.StepDownOneNodeSync();

		// At leaf, store data
		pPolData->MovePos = Branch.CurBotData.GetGlobalBotPosition();
		pPolData->ScanPos = Branch.Destination;

		// Map Certainty
		pPolData->MapID = 0;
		float bestCert = 0;
		for(size_t curMapID = 0; curMapID < Branch.MapCertaintiesNormalized.size(); ++curMapID)
		{
			if(Branch.MapCertaintiesNormalized[curMapID] > bestCert)
			{
				bestCert = Branch.MapCertaintiesNormalized[curMapID];
				pPolData->MapID = curMapID;
			}
		}

		// At leaf, compute policy output
		if(pCurLeafData->CertaintyPolicy >= OGM_PROB_MAX)
		{
			if(pCurLeafData->Certainty >= OGM_PROB_MAX)
				pPolData->PolicyState = POLICY_DATA::POLICY_SUCCESS;		// Return success
			else
				pPolData->PolicyState = POLICY_DATA::POLICY_BLOCKED;		// Return no path available
		}
		else
		{
			pPolData->PolicyState = POLICY_DATA::POLICY_UNKNOWN;		// Return error
		}
	}
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

	// Map Certainty
	r_CurLeafData.MapCertainty = OGM_PROB_MIN;
	for(const auto curMapCert : BranchData.MapCertaintiesNormalized)
	{
		if(curMapCert > r_CurLeafData.MapCertainty)
			r_CurLeafData.MapCertainty = curMapCert;
	}

	// Policy length is expected length to goal
	r_CurLeafData.ExpectedLengthPolicy = r_CurLeafData.ExpectedLength;

	// Simulation is only done at first visit
	r_CurLeafData.NumVisits = 1;

	// Calculate node value
	const auto *const pParentNode = BranchData.pCurNode->GetParent();
	this->CalculateNodeValue( r_CurLeafData, BranchData.Constant,  (pParentNode == nullptr ? 1 : pParentNode->GetData().NumVisits), r_CurLeafData.Value);
}

void MonteCarloOption3::Backtrack_Step(BRANCH_DATA &BranchData)
{
	// Determine type of current node
	TREE_NODE * const&ppCurNode = BranchData.pCurNode;
	NODE_DATA &r_CurNodeData = ppCurNode->GetDataR();

	bool allChildNodesDone;
	const TREE_NODE *const pBestChildNode = this->FindBestChildNode(*ppCurNode, allChildNodesDone);
	if(pBestChildNode == nullptr)
		return;		// This is leaf, no backtrack possible to here

	const NODE_DATA &bestData = pBestChildNode->GetData();

	if(r_CurNodeData.Action.IsObserveAction())
	{
		// Combine both result options
		if(ppCurNode->GetNumChildren() != 2)
			return;				// ERROR

		// Get cell certainty data
		auto bestChoiceCertainty = dynamic_cast<const NODE_EXTRA_DATA_OBSERVE_ACTION&>(r_CurNodeData.GetExtraData()).ObservationFreeProbabilityLog;

		// Get other option
		const NODE_DATA *pOtherData;
		if(pBestChildNode == ppCurNode->GetChild(0))
		{
			bestChoiceCertainty = OccupancyGridMap::CalculateCertaintyProbValueFromCertaintyLog(bestChoiceCertainty);
			pOtherData = &(ppCurNode->GetChild(1)->GetData());
		}
		else
		{
			bestChoiceCertainty = OccupancyGridMap::CalculateProbValueFromCertaintyLog(bestChoiceCertainty);
			pOtherData = &(ppCurNode->GetChild(0)->GetData());
		}

		const auto bestCertainty = bestData.GetCertatintyBeforeNodeAction();
		const auto bestLength = bestData.GetLengthBeforeNodeAction();
		const auto otherCertainty = pOtherData->GetCertatintyBeforeNodeAction();
		const auto otherLength = pOtherData->GetLengthBeforeNodeAction();
		r_CurNodeData.Certainty = bestChoiceCertainty * bestCertainty+(MONTE_CARLO_OPTION3::MAX_CERTAINTY-bestChoiceCertainty)*otherCertainty;
		r_CurNodeData.ExpectedLength = bestChoiceCertainty * bestLength+(MONTE_CARLO_OPTION3::MAX_CERTAINTY-bestChoiceCertainty)*otherLength;
		r_CurNodeData.ExpectedCost = bestChoiceCertainty * bestData.ExpectedCost+(MONTE_CARLO_OPTION3::MAX_CERTAINTY-bestChoiceCertainty)*pOtherData->ExpectedCost + BranchData.ObserveActionCost;		// Add cost for this observation as well

		// Map Certainty
		r_CurNodeData.MapCertainty = bestChoiceCertainty * bestData.MapCertainty + (MONTE_CARLO_OPTION3::MAX_CERTAINTY-bestChoiceCertainty)*pOtherData->MapCertainty;

		// Adjust policy data
		r_CurNodeData.CertaintyPolicy = bestChoiceCertainty * bestData.CertaintyPolicy +(MONTE_CARLO_OPTION3::MAX_CERTAINTY-bestChoiceCertainty)*pOtherData->CertaintyPolicy;

		if(r_CurNodeData.CertaintyPolicy >= OGM_PROB_MAX)
			r_CurNodeData.IsDone = true;

		if(r_CurNodeData.Certainty <= OGM_PROB_MIN)
			r_CurNodeData.ExpectedLengthPolicy = 0;
		else
			r_CurNodeData.ExpectedLengthPolicy = r_CurNodeData.ExpectedLength;
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
				r_CurNodeData.ExpectedCost = bestData.ExpectedCost + BranchData.MoveActionCost*movePathData.PathLength;
				r_CurNodeData.ExpectedLength = bestData.ExpectedLength + movePathData.PathLength;
			}
			else
			{
				r_CurNodeData.ExpectedCost = bestData.ExpectedCost + BranchData.MoveActionCost*1;
				r_CurNodeData.ExpectedLength = bestData.ExpectedLength + 1;
			}
		}
		else if(bestData.Action.IsObserveAction())
		{
			// Copy best value and add uncertainty
			const auto &observationData = dynamic_cast<const NODE_EXTRA_DATA_OBSERVE_ACTION&>(bestData.GetExtraData());
			r_CurNodeData.Certainty = bestData.Certainty; //OccupancyGridMap::CalculateCertaintyProbValueFromCertaintyLog( OccupancyGridMap::CalculateCertaintyLogValueFromCertaintyProb(bestData.Certainty) + observationData.ObservationFreeProbabilityLog);				// Copy certainty, the uncertainty of this path is already considered in the following observation

			r_CurNodeData.ExpectedCost = bestData.ExpectedCost + BranchData.ObserveActionCost;
			r_CurNodeData.ExpectedLength = bestData.ExpectedLength;
		}

		// Map Certainty
		r_CurNodeData.MapCertainty = bestData.MapCertainty;

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

			r_CurNodeData.IsDone = allChildNodesDone;
		}
	}

	// Calculate node value
	const auto *const pParentNode = BranchData.pCurNode->GetParent();
	this->CalculateNodeValue( r_CurNodeData, BranchData.Constant,  (pParentNode == nullptr ? r_CurNodeData.NumVisits : pParentNode->GetData().NumVisits), r_CurNodeData.Value);

	// Increase number of visits
	r_CurNodeData.NumVisits++;
}

int MonteCarloOption3::PerformLeafExpansion(BRANCH_DATA &BranchData)
{
	TREE_NODE * const&ppCurNode = BranchData.pCurNode;
	NODE_DATA *pCurNodeData = &ppCurNode->GetDataR();

	// Sanity Check that we are at leaf
	if(!ppCurNode->IsLeaf())
		return -1;

	// Make sure data is synced
	BranchData.StepDownOneNodeSync();

	// Calculate move costs to these quads
	BranchData.SyncDStarMaps(1,1);

	const auto curPos = BranchData.CurBotData.GetGlobalBotPosition();
	auto curID = BranchData.DStarMaps.GetQuadIDAtPos(curPos);

	// Create new paths out of quad
	bool pathFound = false;
	do
	{
		// Get all adjacent quads
		auto adjacentQuads = BranchData.DStarMaps.GetAdjacentQuadIDs(curID);
		for(const auto adjacentID : adjacentQuads)
		{
			// Make sure this quad hasn't been traversed recently
			bool prevVisited = false;
			for(const auto prevID : BranchData.VisitedQuads)
			{
				if(prevID == adjacentID)
				{
					prevVisited = true;
					break;
				}
			}
			if(prevVisited)
				continue;		// Skip if previously visited

			// Find best edge position to this quad
			const auto edgeRectangle = BranchData.DStarMaps.GetAdjacentRectangle(curID, adjacentID);
			auto bestPosVal = GetInfiniteVal<EXPECTED_LENGTH_TYPE>();
			POS_2D bestEdgePos = curPos;
			POS_2D edgePos;
			for(edgePos.X = edgeRectangle.BottomLeftPos.X; edgePos.X-edgeRectangle.BottomLeftPos.X < edgeRectangle.Width; ++edgePos.X)
			{
				for(edgePos.Y = edgeRectangle.BottomLeftPos.Y; edgePos.Y-edgeRectangle.BottomLeftPos.Y < edgeRectangle.Height; ++edgePos.Y)
				{
					auto curVal = BranchData.DStarMaps.GetDStarRatioMapToStart().GetPixel(edgePos)+BranchData.DStarMaps.GetDStarRatioDistMapToGoal().GetPixel(edgePos);
					if(curVal < bestPosVal)
					{
						bestPosVal = curVal;
						bestEdgePos = edgePos;
					}
				}
			}

			if(bestEdgePos == curPos)
				continue;				// No position found that wasn't occupied, skip

			// Check that position wasn't previously visited
			for(const auto &prevPos : BranchData.PrevPath)
			{
				if(prevPos == bestEdgePos)
				{
					prevVisited = true;
					break;
				}
			}
			if(prevVisited)
				continue;		// Skip if previously visited

			pathFound = true;

			// Create a jump to this position
			std::unique_ptr<NODE_EXTRA_DATA_JUMP> pNewJump(new NODE_EXTRA_DATA_JUMP());
			pNewJump->PathData = BranchData.DStarMaps.CalculateRatioPathFromStart(bestEdgePos);
			pNewJump->RecentPathLength = BranchData.DStarMaps.GetDStarRatioDistMapToStart().GetPixel(bestEdgePos);
			pNewJump->RecentPathLogCertainty = BranchData.DStarMaps.GetDStarRatioCertaintyMapToStart().GetPixel(bestEdgePos);

			// Add jump to children
			ppCurNode->AddChild(NODE_DATA(MONTE_CARLO_OPTION3::NODE_ACTION_JUMP, NODE_EXTRA_DATA(std::move(pNewJump))));
		}

		// If no path was found, divide quad
		if(!pathFound)
		{
			// Divide quad
			if(!BranchData.DStarMaps.DivideQuad(BranchData.CurCertaintyLogData, BranchData.PosToUpdate, curID, 1, 1))
				continue;		// Division failed, stop

			BranchData.PosToUpdate.clear();
			pCurNodeData->NumDivisions++;

			curID = BranchData.DStarMaps.GetQuadIDAtPos(curPos);
		}
	}
	while(!pathFound);

	// If both dest and bot are in same quad, create path
	bool destPathCreated = false;
	if(curID == BranchData.DStarMaps.GetQuadIDAtPos(BranchData.Destination))
	{
		// Check that dest is reachable and that bot isn't already at destination
		if(curPos != BranchData.Destination && BranchData.DStarMaps.GetDStarRatioMapToStart().GetPixel(BranchData.Destination) < OGM_LOG_MAX)
		{
			// Create a jump to destination
			std::unique_ptr<NODE_EXTRA_DATA_JUMP> pNewJump(new NODE_EXTRA_DATA_JUMP());
			pNewJump->PathData = BranchData.DStarMaps.CalculateRatioPathFromStart(BranchData.Destination);
			pNewJump->RecentPathLength = BranchData.DStarMaps.GetDStarRatioDistMapToStart().GetPixel(BranchData.Destination);
			pNewJump->RecentPathLogCertainty = BranchData.DStarMaps.GetDStarRatioCertaintyMapToStart().GetPixel(BranchData.Destination);

			// Add jump to children
			ppCurNode->AddChild(NODE_DATA(MONTE_CARLO_OPTION3::NODE_ACTION_JUMP, NODE_EXTRA_DATA(std::move(pNewJump))));

			destPathCreated = true;
		}
	}

	// If no new path was created, set this node as finished
	if(!destPathCreated && !pathFound)
	{
		pCurNodeData->IsDone = true;
	}

	return 1;
}

int MonteCarloOption3::InsertSkippedCell(BRANCH_DATA &BranchData)
{
	// Check where to insert cell
	TREE_NODE * const&ppCurNode = BranchData.pCurNode;
	NODE_DATA *pCurNodeData = &ppCurNode->GetDataR();

	NODE_EXTRA_DATA_JUMP &r_CurJumpData = dynamic_cast<NODE_EXTRA_DATA_JUMP &>(pCurNodeData->GetExtraData());

	// Return error if jump is empty
	if(r_CurJumpData.PathData.empty())
		return -1;

	// Find division cell
	POS_2D curPos = BranchData.CurBotData.GetGlobalBotPosition();
	CERTAINTY_TYPE firstPathLogCert = OGM_LOG_MIN;
	EXPECTED_LENGTH_TYPE firstPathLength = 0;
	CERTAINTY_TYPE secondPathLogCert = r_CurJumpData.RecentPathLogCertainty;
	EXPECTED_LENGTH_TYPE secondPathLength = r_CurJumpData.RecentPathLength;
	auto curPathPosIterator = r_CurJumpData.PathData.begin();
	for(; curPathPosIterator != r_CurJumpData.PathData.end(); ++curPathPosIterator)
	{
		auto curLogCert = BranchData.CurCertaintyLogData.GetPixel(*curPathPosIterator);

		// Update certainty and length
		firstPathLogCert = r_CurJumpData.RecentPathLogCertainty - secondPathLogCert;
		firstPathLength = r_CurJumpData.RecentPathLength - secondPathLength;

		secondPathLogCert -= curLogCert;
		secondPathLength -= GetMovementCost(curPos, *curPathPosIterator);

		if(firstPathLogCert >= secondPathLogCert)
			break;

		// Update current pos
		curPos = *curPathPosIterator;
	}

	const auto newPos = *curPathPosIterator;
	std::unique_ptr<TREE_NODE> pChildNode(nullptr);

	// Divide jump into three parts (first jump, inserted cell, second jump)
	if(curPathPosIterator == r_CurJumpData.PathData.begin())
	{
		// If it should be divided at start, don't insert first jump, just alter it to second one
		r_CurJumpData.PathData.erase(r_CurJumpData.PathData.begin(), curPathPosIterator+1);
		r_CurJumpData.RecentPathLength = secondPathLength;
		r_CurJumpData.RecentPathLogCertainty = secondPathLogCert;

		// Move back up to parent node
		pChildNode.reset(ppCurNode);
		BranchData.StepUpOneNodeSimple();
		pCurNodeData = &ppCurNode->GetDataR();

		// Remove this branch from parent (will be done later)
		//pChildNode = pChildNode->RemoveAndSetAsRoot();
	}
	else
	{
		// Save data for second node
		std::unique_ptr<NODE_EXTRA_DATA_JUMP> pOldData(new NODE_EXTRA_DATA_JUMP(r_CurJumpData));

		// Update next node data
		const auto curPathPosID = curPathPosIterator-r_CurJumpData.PathData.begin();
		pOldData->PathData.erase(pOldData->PathData.begin(), pOldData->PathData.begin()+curPathPosID+1);
		pOldData->RecentPathLength = secondPathLength;
		pOldData->RecentPathLogCertainty = secondPathLogCert;

		// Update current node data
		r_CurJumpData.PathData.erase(curPathPosIterator, r_CurJumpData.PathData.end());
		r_CurJumpData.RecentPathLength = firstPathLength;
		r_CurJumpData.RecentPathLogCertainty = firstPathLogCert;

		// Sync and move down
		BranchData.StepDownOneNodeSync();

		// Create second path step
		pChildNode.reset(ppCurNode->InsertChild(NODE_DATA(MONTE_CARLO_OPTION3::NODE_ACTION_JUMP, NODE_EXTRA_DATA(std::move(pOldData)))));
	}

	// Expand node
	this->ExpandSkippedCell(BranchData, pChildNode, newPos, 1);

	// Remove cell if second stage is empty
	if(pChildNode != nullptr)
	{
		if(secondPathLength <= 0.125f)
			ppCurNode->RemoveChildOutOfTree(&(*pChildNode) - &ppCurNode->GetStorage().front());

		// Release pointer if parent available (else it will be deleted at end)
		if(pChildNode->GetParent() != nullptr)
			pChildNode.release();
	}

	return 1;
}

int MonteCarloOption3::ExpandSkippedCell(BRANCH_DATA &BranchData, std::unique_ptr<TREE_NODE> &ChildNode, const POS_2D &ChildPos, bool SkipPreviousPath)
{
	TREE_NODE *&ppCurNode = BranchData.pCurNode;
	NODE_DATA *pCurNodeData = &ppCurNode->GetDataR();

	// Remove child node from any nodes (this prevents errors when arrays are adjusted)
	ChildNode = ChildNode.release()->RemoveAndSetAsRoot();

	// Synchronize data
	BranchData.StepDownOneNodeSync();
	BranchData.SyncDStarMaps(1,0);

	bool childNodeInserted = false;

	// Go through all adjacent positions
	const auto curPos = BranchData.CurBotData.GetGlobalBotPosition();
	for(const auto moveOrder : NavigationOptions)
	{
		const auto adjacentPos = curPos+moveOrder;

		CERTAINTY_TYPE adjacentCert;
		if(BranchData.CurCertaintyLogData.GetPixel(adjacentPos, adjacentCert) < 0)
			continue;		// Skip if outside of map

		// Skip if already traversed
		bool skipPos = false;
		if(!SkipPreviousPath)
		{
			for(const auto prevPos : BranchData.PrevPath)
			{
				if(adjacentPos == prevPos)
				{
					skipPos = true;
					continue;
				}
			}
			if(skipPos)
				continue;
		}

		// Skip if already a node of this cell
		for(auto curChildNodeIterator = ppCurNode->GetStorage().begin(); curChildNodeIterator != ppCurNode->GetStorage().end(); ++curChildNodeIterator)
		{
			if(!curChildNodeIterator->GetData().Action.IsIncompleteMove() && *this->NextPosOfNode(*curChildNodeIterator) == adjacentPos)
			{
				skipPos = true;
				continue;
			}
		}
		if(skipPos)
			continue;

		// Create new node
		if(adjacentCert <= OGM_LOG_MIN)
		{
			// Create move order
			auto *const pNewNode = ppCurNode->AddChild(NODE_DATA(MONTE_CARLO_OPTION3::NODE_ACTION_MOVE, NODE_EXTRA_DATA_MOVE(adjacentPos)));

			if(ChildPos != adjacentPos)
			{
				// Simulate data if it won't be appended later
				auto tmpBranch = BranchData;
				tmpBranch.StepDownOneNode(ppCurNode->GetNumChildren()-1);
				tmpBranch.SyncDStarMaps(1,0);

				this->SimulateBranch(tmpBranch);
			}
		}
		else if(adjacentCert < OGM_LOG_MAX)
		{
			// Create observe order
			auto *const pNewNode = ppCurNode->AddChild(NODE_DATA(MONTE_CARLO_OPTION3::NODE_ACTION_OBSERVE, NODE_EXTRA_DATA_OBSERVE_ACTION(adjacentPos, adjacentCert)));

			auto occupiedBranch = BranchData;
			occupiedBranch.StepDownOneNode(occupiedBranch.pCurNode->GetNumChildren()-1);

			// Create a free node
			pNewNode->AddChild(NODE_DATA(MONTE_CARLO_OPTION3::NODE_ACTION_NONE));
			if(ChildPos != adjacentPos)
			{
				// Simulate data if it won't be appended later
				auto tmpBranch = occupiedBranch;
				tmpBranch.StepDownOneNode(tmpBranch.pCurNode->GetNumChildren()-1);
				tmpBranch.SyncDStarMaps(1,0);

				this->SimulateBranch(tmpBranch);
			}

			// Create an occupied node
			pNewNode->AddChild(NODE_DATA(MONTE_CARLO_OPTION3::NODE_ACTION_NONE));
			occupiedBranch.StepDownOneNode(occupiedBranch.pCurNode->GetNumChildren()-1);
			occupiedBranch.SyncDStarMaps(1,0);

			this->SimulateBranch(occupiedBranch);

			// Backtrack to decision
			if(ChildPos != adjacentPos)
			{
				occupiedBranch.StepUpOneNodeSimple();
				this->Backtrack_Step(occupiedBranch);
			}
		}
	}

	if(!childNodeInserted)
	{
		// Insert child node if not yet done
		for(auto &curChild : ppCurNode->GetStorageR())
		{
			if(*this->NextPosOfNode(curChild) == ChildPos)
			{
				childNodeInserted = true;

				BranchData.StepDownOneNode(&curChild - &ppCurNode->GetStorageR().front());

				// Append to proper place
				if(curChild.GetData().Action.IsObserveAction())
					BranchData.StepDownOneNodeSimple(0);

				// Release child node
				ChildNode.reset(ppCurNode->AddChild(std::move(*ChildNode.release())));

				break;
			}
		}
	}

	if(childNodeInserted)
		return 1;

	// Delete node if no insertion possible
	ChildNode.reset(nullptr);

	return 0;
}

const POS_2D *MonteCarloOption3::NextPosOfNode(const TREE_NODE &Node)
{
	const auto &nodeData = Node.GetData();
	if(nodeData.Action.IsIncompleteMove())
	{
		return &(dynamic_cast<const NODE_EXTRA_DATA_JUMP&>(nodeData.GetExtraData()).PathData.front());
	}
	else if (nodeData.Action.IsCompleteMove())
	{
		return &(dynamic_cast<const NODE_EXTRA_DATA_MOVE&>(nodeData.GetExtraData()).NewPos);
	}
	else if (nodeData.Action.IsObserveAction())
	{
		return &(dynamic_cast<const NODE_EXTRA_DATA_OBSERVE_ACTION&>(nodeData.GetExtraData()).ObservationPoint);
	}
	else if(nodeData.Action.IsMovePath())
	{
		return &(dynamic_cast<const NODE_EXTRA_DATA_MOVE_PATH&>(nodeData.GetExtraData()).TargetPosition);
	}

	return nullptr;
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

std::vector<MONTE_CARLO_OPTION3::NODE_DATA> MonteCarloOption3::ExpandCurrentNode(const BRANCH_DATA &BranchData, const POS_2D &CurPos, const POS_2D &NextPos, bool &NextPosReached)
{
	std::vector<MONTE_CARLO_OPTION3::NODE_DATA> newNodes;
	NextPosReached = false;

	// Go through all adjacent nodes
	for(const auto moveData : NavigationOptions)
	{
		const POS_2D adjacentPos = CurPos+moveData;

		// Skip if outside of map
		CERTAINTY_TYPE adjacentLogCert;
		if(!BranchData.CurCertaintyLogData.GetPixel(adjacentPos, adjacentLogCert))
			continue;

		if(adjacentLogCert >= OGM_LOG_MAX)
			continue;				// Skip if cell occupied

		// Skip if previously visited
		bool previouslyVisited = false;
		for(const auto prevPathPos : BranchData.PrevPath)
		{
			if(adjacentPos == prevPathPos)
			{
				previouslyVisited = true;
				break;
			}
		}
		if(previouslyVisited)
			continue;				// Skip if visited since last observe action

		// Create new empty node
		newNodes.push_back(NODE_DATA());
		auto &curNode = newNodes.back();
		if(adjacentLogCert > OGM_LOG_MIN)
		{
			// Create scan node
			curNode.Action.SetAction(MONTE_CARLO_OPTION3::NODE_ACTION_OBSERVE);
			curNode.SetExtraData(NODE_EXTRA_DATA_OBSERVE_ACTION(adjacentPos, adjacentLogCert));
		}
		else
		{
			// Change to complete move (move to next quad)
			curNode.Action.SetAction(MONTE_CARLO_OPTION3::NODE_ACTION_MOVE);			// Set action type
			curNode.SetExtraData(NODE_EXTRA_DATA_MOVE(adjacentPos));					// Set extra data to new value
		}

		// Check if this cell covers the next position
		if(adjacentPos == NextPos)
		{
			std::swap(newNodes.front(), curNode);		// If it does, move it to first vector location
			NextPosReached = true;
		}
	}

	return newNodes;
}

std::vector<MONTE_CARLO_OPTION3::NODE_DATA> MonteCarloOption3::ExpandCurrentNode(const BRANCH_DATA &BranchData, const POS_2D &CurPos)
{
	std::vector<MONTE_CARLO_OPTION3::NODE_DATA> newNodes;

	// Go through all adjacent nodes
	for(const auto moveData : NavigationOptions)
	{
		const POS_2D adjacentPos = CurPos+moveData;

		// Skip if outside of map
		CERTAINTY_TYPE adjacentLogCert;
		if(!BranchData.CurCertaintyLogData.GetPixel(adjacentPos, adjacentLogCert))
			continue;

		if(adjacentLogCert >= OGM_LOG_MAX)
			continue;				// Skip if cell occupied

		// Skip if previously visited
		bool previouslyVisited = false;
		for(const auto prevPathPos : BranchData.PrevPath)
		{
			if(adjacentPos == prevPathPos)
			{
				previouslyVisited = true;
				break;
			}
		}
		if(previouslyVisited)
			continue;				// Skip if visited since last observe action

		// Create new empty node
		newNodes.push_back(NODE_DATA());
		auto &curNode = newNodes.back();
		if(adjacentLogCert > OGM_LOG_MIN)
		{
			// Create scan node
			curNode.Action.SetAction(MONTE_CARLO_OPTION3::NODE_ACTION_OBSERVE);
			curNode.SetExtraData(NODE_EXTRA_DATA_OBSERVE_ACTION(adjacentPos, adjacentLogCert));
		}
		else
		{
			// Change to complete move (move to next quad)
			curNode.Action.SetAction(MONTE_CARLO_OPTION3::NODE_ACTION_MOVE);			// Set action type
			curNode.SetExtraData(NODE_EXTRA_DATA_MOVE(adjacentPos));					// Set extra data to new value
		}
	}

	return newNodes;
}

void MonteCarloOption3::ExpandScanNode(TREE_NODE &ScanNode)
{
	const auto &scanNodeData = ScanNode.GetData();

	// Do nothing if not scan action
	if(scanNodeData.Action.IsObserveAction())
		return;

	// Insert node indicator for observe result free
	ScanNode.InsertChild(NODE_DATA(MONTE_CARLO_OPTION3::NODE_ACTION_NONE));

	// Insert node indicator for observe result occupied
	ScanNode.InsertChild(NODE_DATA(MONTE_CARLO_OPTION3::NODE_ACTION_NONE));
}

inline MONTE_CARLO_OPTION3::NODE_VALUE_TYPE CalculateNodeValueFromCert(const MONTE_CARLO_OPTION3::CERTAINTY_TYPE Certainty, const MONTE_CARLO_OPTION3::EXPECTED_LENGTH_TYPE ExpLength, const MONTE_CARLO_OPTION3::CERTAINTY_TYPE Constant, const MONTE_CARLO_OPTION3::NUM_VISIT_TYPE NumVisits, const MONTE_CARLO_OPTION3::NUM_VISIT_TYPE NumParentsVisits)
{
	return Certainty/ExpLength + Constant*static_cast<MONTE_CARLO_OPTION3::NODE_VALUE_TYPE>(std::sqrt(static_cast<double>(NumVisits)/NumParentsVisits));
}

void MonteCarloOption3::CalculateNodeValue(const NODE_DATA &Data, const NODE_VALUE_TYPE &Constant, const NUM_VISITS_TYPE &NumParentsVisit, NODE_VALUE_TYPE &Value)
{
	// Node is done, don't select it
	if(Data.Action.IsIncompleteMove())
	{
		const auto &jumpData = dynamic_cast<const NODE_EXTRA_DATA_JUMP&>(Data.GetExtraData());
		Value = CalculateNodeValueFromCert(OccupancyGridMap::CalculateCertaintyProbValueFromCertaintyLog(OccupancyGridMap::CalculateCertaintyLogValueFromCertaintyProb(Data.Certainty)+jumpData.RecentPathLogCertainty), (Data.ExpectedLength+jumpData.RecentPathLength), Constant, Data.NumVisits, NumParentsVisit);
	}
	else
	{
		if(Data.IsDone)
			Value = MONTE_CARLO_OPTION3::MIN_NODE_VALUE;
		else
			Value = CalculateNodeValueFromCert(Data.Certainty, Data.ExpectedLength, Constant, Data.NumVisits, NumParentsVisit);
	}
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
	AllDone = pBestNode->GetData().IsDone;
	for(auto curChildIterator = CurNode.GetStorage().begin()+1; curChildIterator != CurNode.GetStorage().end(); ++curChildIterator)
	{
		const auto &curChildData = curChildIterator->GetData();
		if(!curChildData.IsDone)
			AllDone = false;
		else if(curChildData.Action.IsIncompleteMove())
		{
			if(dynamic_cast<const NODE_EXTRA_DATA_JUMP&>(curChildData.GetExtraData()).RecentPathLogCertainty > OGM_LOG_MIN)
				AllDone = false;
		}

		const auto curValue = this->CalculateComparableChildValue(curChildData);
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
	if(ChildNodeData.Action.IsIncompleteMove())
	{
		return ChildNodeData.ExpectedLength+dynamic_cast<const NODE_EXTRA_DATA_JUMP&>(ChildNodeData.GetExtraData()).RecentPathLength;
	}

	return ChildNodeData.ExpectedLength;
}
