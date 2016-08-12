#include "monte_carlo_option2.h"
#include "algorithm_a_star.h"

#include <cstring>

namespace MONTE_CARLO_OPTION2
{
	const NODE_EXTRA_DATA_LEAF *NODE_EXTRA_DATA::GetExtraLeafData() const
	{
		if(this->DataType == DATA_LEAF)
			return static_cast<NODE_EXTRA_DATA_LEAF *>(this->ExtraData);

		return nullptr;
	}

	NODE_EXTRA_DATA_LEAF *NODE_EXTRA_DATA::GetExtraLeafData()
	{
		if(this->DataType == DATA_LEAF)
			return static_cast<NODE_EXTRA_DATA_LEAF *>(this->ExtraData);

		return nullptr;
	}

	void NODE_EXTRA_DATA::SetExtraData(const NODE_EXTRA_DATA_LEAF &ExtraLeafData)
	{
		if(this->DataType == DATA_LEAF)
			*(static_cast<NODE_EXTRA_DATA_LEAF *>(this->ExtraData)) = ExtraLeafData;
		else
		{
			// Wrong data type, delete it first
			this->DeleteExtraData();

			// Add new data
			this->DataType = DATA_LEAF;
			this->ExtraData = new NODE_EXTRA_DATA_LEAF(ExtraLeafData);
		}
	}

	void NODE_EXTRA_DATA::SetExtraData(NODE_EXTRA_DATA_LEAF &&ExtraLeafData)
	{
		if(this->DataType == DATA_LEAF)
			*(static_cast<NODE_EXTRA_DATA_LEAF *>(this->ExtraData)) = std::move(ExtraLeafData);
		else
		{
			// Wrong data type, delete it first
			this->DeleteExtraData();

			// Add new data
			this->DataType = DATA_LEAF;
			this->ExtraData = new NODE_EXTRA_DATA_LEAF(std::move(ExtraLeafData));
		}
	}

	const NODE_EXTRA_DATA_OBSTACLE *NODE_EXTRA_DATA::GetExtraObstacleData() const
	{
		if(this->DataType == DATA_OBSTACLE)
			return static_cast<NODE_EXTRA_DATA_OBSTACLE *>(this->ExtraData);

		return nullptr;
	}

	NODE_EXTRA_DATA_OBSTACLE *NODE_EXTRA_DATA::GetExtraObstacleData()
	{
		if(this->DataType == DATA_OBSTACLE)
			return static_cast<NODE_EXTRA_DATA_OBSTACLE *>(this->ExtraData);

		return nullptr;
	}

	void NODE_EXTRA_DATA::SetExtraData(const NODE_EXTRA_DATA_OBSTACLE &ExtraObstacleData)
	{
		if(this->DataType == DATA_OBSTACLE)
			*(static_cast<NODE_EXTRA_DATA_OBSTACLE *>(this->ExtraData)) = ExtraObstacleData;
		else
		{
			// Wrong data type, delete it first
			this->DeleteExtraData();

			// Add new data
			this->DataType = DATA_OBSTACLE;
			this->ExtraData = new NODE_EXTRA_DATA_OBSTACLE(ExtraObstacleData);
		}
	}

	void NODE_EXTRA_DATA::SetExtraData(NODE_EXTRA_DATA_OBSTACLE &&ExtraObstacleData)
	{
		if(this->DataType == DATA_OBSTACLE)
			*(static_cast<NODE_EXTRA_DATA_OBSTACLE *>(this->ExtraData)) = std::move(ExtraObstacleData);
		else
		{
			// Wrong data type, delete it first
			this->DeleteExtraData();

			// Add new data
			this->DataType = DATA_OBSTACLE;
			this->ExtraData = new NODE_EXTRA_DATA_OBSTACLE(std::move(ExtraObstacleData));
		}
	}

	const NODE_EXTRA_DATA_MOVE *NODE_EXTRA_DATA::GetExtraMoveData() const
	{
		if(this->DataType == DATA_MOVE)
			return static_cast<NODE_EXTRA_DATA_MOVE *>(this->ExtraData);

		return nullptr;
	}

	NODE_EXTRA_DATA_MOVE *NODE_EXTRA_DATA::GetExtraMoveData()
	{
		if(this->DataType == DATA_MOVE)
			return static_cast<NODE_EXTRA_DATA_MOVE *>(this->ExtraData);

		return nullptr;
	}

	void NODE_EXTRA_DATA::SetExtraData(const NODE_EXTRA_DATA_MOVE &ExtraMoveData)
	{
		if(this->DataType == DATA_MOVE)
			*(static_cast<NODE_EXTRA_DATA_MOVE *>(this->ExtraData)) = ExtraMoveData;
		else
		{
			// Wrong data type, delete it first
			this->DeleteExtraData();

			// Add new data
			this->DataType = DATA_MOVE;
			this->ExtraData = new NODE_EXTRA_DATA_MOVE(ExtraMoveData);
		}
	}

	void NODE_EXTRA_DATA::SetExtraData(NODE_EXTRA_DATA_MOVE &&ExtraMoveData)
	{
		if(this->DataType == DATA_MOVE)
			*(static_cast<NODE_EXTRA_DATA_MOVE *>(this->ExtraData)) = std::move(ExtraMoveData);
		else
		{
			// Different data type, delete it first
			this->DeleteExtraData();

			// Add new data
			this->DataType = DATA_MOVE;
			this->ExtraData = new NODE_EXTRA_DATA_MOVE(std::move(ExtraMoveData));
		}
	}

	void NODE_EXTRA_DATA::DeleteExtraData()
	{
		// Typesafe delete
		this->DeleteExtraLeafData();
		this->DeleteExtraObstacleData();
		this->DeleteExtraMoveData();
	}

	// Custom constructors due to NODE_EXTRA_DATA memory allocation
	NODE_EXTRA_DATA::NODE_EXTRA_DATA() : ExtraData(nullptr), DataType(DATA_EMPTY)
	{}

	NODE_EXTRA_DATA::NODE_EXTRA_DATA(NODE_EXTRA_DATA_LEAF &&_LeafData) noexcept : ExtraData(new NODE_EXTRA_DATA_LEAF(std::move(_LeafData))), DataType(DATA_LEAF)
	{}

	NODE_EXTRA_DATA::NODE_EXTRA_DATA(NODE_EXTRA_DATA_OBSTACLE &&_ObstacleData) : ExtraData(new NODE_EXTRA_DATA_OBSTACLE(std::move(_ObstacleData))), DataType(DATA_OBSTACLE)
	{}

	NODE_EXTRA_DATA::NODE_EXTRA_DATA(NODE_EXTRA_DATA_MOVE &&_MoveData) : ExtraData(new NODE_EXTRA_DATA_MOVE(std::move(_MoveData))), DataType(DATA_MOVE)
	{}

	NODE_EXTRA_DATA::NODE_EXTRA_DATA(const NODE_EXTRA_DATA &S) noexcept : ExtraData(nullptr), DataType(DATA_EMPTY)
	{
		if(S.DataType == DATA_EMPTY)
			this->ExtraData = nullptr;
		else if(S.DataType == DATA_OBSTACLE)
			this->ExtraData = new NODE_EXTRA_DATA_OBSTACLE(*(static_cast<NODE_EXTRA_DATA_OBSTACLE *>(S.ExtraData)));
		else if(S.DataType == DATA_LEAF)
			this->ExtraData = new NODE_EXTRA_DATA_LEAF(*(static_cast<NODE_EXTRA_DATA_LEAF *>(S.ExtraData)));

		this->DataType = S.DataType;
	}

	NODE_EXTRA_DATA::NODE_EXTRA_DATA(NODE_EXTRA_DATA &&S) noexcept : ExtraData(std::move(S.ExtraData)), DataType(std::move(S.DataType))
	{
		S.ExtraData = nullptr;
		S.DataType = DATA_EMPTY;

		// If this is a leaf node, copy back the data stored here
		if(this->DataType == DATA_LEAF)
		{
			NODE_EXTRA_DATA *const pExtraData = (static_cast<NODE_EXTRA_DATA_LEAF *>(this->ExtraData))->GetExtraData();
			if(pExtraData != nullptr)
			{
				S = std::move(*pExtraData);
			}
		}
	}

	NODE_EXTRA_DATA &NODE_EXTRA_DATA::operator=(const NODE_EXTRA_DATA &S) noexcept
	{
		// Delete any data that is currently stored
		this->DeleteExtraData();

		if(S.DataType == DATA_EMPTY)
			this->ExtraData = nullptr;
		else if(S.DataType == DATA_OBSTACLE)
			this->ExtraData = new NODE_EXTRA_DATA_OBSTACLE(*(static_cast<NODE_EXTRA_DATA_OBSTACLE *>(S.ExtraData)));
		else if(S.DataType == DATA_LEAF)
			this->ExtraData = new NODE_EXTRA_DATA_LEAF(*(static_cast<NODE_EXTRA_DATA_LEAF *>(S.ExtraData)));;

		this->DataType = S.DataType;

		return *this;
	}

	NODE_EXTRA_DATA &NODE_EXTRA_DATA::operator=(NODE_EXTRA_DATA &&S) noexcept
	{
		// Delete any data that is currently stored
		this->DeleteExtraData();

		this->ExtraData = std::move(S.ExtraData);
		this->DataType = std::move(S.DataType);

		// Remove data from S
		S.ExtraData = nullptr;
		S.DataType = DATA_EMPTY;

		return *this;
	}

	NODE_EXTRA_DATA::~NODE_EXTRA_DATA() noexcept
	{
		// Delete data in a typesafe way
		this->DeleteExtraData();
	}

	void NODE_EXTRA_DATA::ChangeDataType(const NODE_EXTRA_DATA_TYPE &NewType)
	{
		if(this->DataType == NewType)
			return;

		this->DeleteExtraData();

		if(NewType == DATA_LEAF)
			this->ExtraData = new NODE_EXTRA_DATA_LEAF();
		else if(NewType == DATA_OBSTACLE)
			this->ExtraData = new NODE_EXTRA_DATA_OBSTACLE();
		else if(NewType == DATA_MOVE)
			this->ExtraData = new NODE_EXTRA_DATA_MOVE();

		this->DataType = NewType;
	}

	void NODE_EXTRA_DATA::DeleteExtraObstacleData()
	{
		if(this->DataType == DATA_OBSTACLE)
		{
			// Delete data in a typesafe way
			delete static_cast<NODE_EXTRA_DATA_OBSTACLE*>(this->ExtraData);
			this->ExtraData = nullptr;
			this->DataType = DATA_EMPTY;
		}
	}

	void NODE_EXTRA_DATA::DeleteExtraLeafData()
	{
		if(this->DataType == DATA_LEAF)
		{
			// Delete data in a typesafe way
			delete static_cast<NODE_EXTRA_DATA_LEAF*>(this->ExtraData);
			this->ExtraData = nullptr;
			this->DataType = DATA_EMPTY;
		}
	}

	void NODE_EXTRA_DATA::DeleteExtraMoveData()
	{
		if(this->DataType == DATA_MOVE)
		{
			// Delete data in a typesafe way
			delete static_cast<NODE_EXTRA_DATA_MOVE*>(this->ExtraData);
			this->ExtraData = nullptr;
			this->DataType = DATA_EMPTY;
		}
	}

	NODE_DATA::NODE_DATA(const NODE_ACTION _Action, const NODE_VALUE_TYPE &_Value, const EXPECTED_LENGTH_TYPE &_ExpectedLength, const CERTAINTY_TYPE &_Certainty, const COST_TYPE &_ExpectedCost, const NUM_VISIT_TYPE &_NumVisits, const POS_2D &_Position, bool _IsDone, NODE_EXTRA_DATA &&_ExtraData) : NODE_EXTRA_DATA(std::move(_ExtraData)), Action(_Action), Value(_Value), ExpectedLength(_ExpectedLength), Certainty(_Certainty), ExpectedCost(_ExpectedCost), NumVisits(_NumVisits), Position(_Position), IsDone(_IsDone)
	{}

	NODE_DATA::NODE_DATA(const NODE_ACTION _Action, const POS_2D &_Position) : NODE_EXTRA_DATA(), Action(_Action), Position(_Position)
	{}

	NODE_DATA::NODE_DATA(const NODE_ACTION _Action, const POS_2D &_Position, NODE_EXTRA_DATA &&_ExtraData) : NODE_EXTRA_DATA(std::move(_ExtraData)), Action(_Action), Position(_Position)
	{}

	void NODE_DATA::WriteToFile(std::fstream &FileData, int &SavedSize) const
	{
		SavedSize = sizeof(NODE_DATA);

		// Write node
		FileData.write(reinterpret_cast<const char *>(this), sizeof(NODE_DATA));

		// Write extra data
		if(this->DataType == DATA_LEAF)
		{
			const NODE_EXTRA_DATA_LEAF *pExtraData = static_cast<NODE_EXTRA_DATA_LEAF*>(this->ExtraData);
			// TODO: Write data and add SavedSize
		}
		else if(this->DataType == DATA_MOVE)
		{
			// TODO: Write data and add SavedSize
		}
		else if(this->DataType == DATA_OBSTACLE)
		{
			// TODO: Write data and add SavedSize
		}
	}

	void NODE_DATA::ReadFromFile(std::fstream &FileData)
	{
		// Read node
		FileData.read(reinterpret_cast<char *>(this), sizeof(NODE_DATA));

		// Read extra data
		if(this->DataType == DATA_LEAF)
		{
			const NODE_EXTRA_DATA_LEAF *pExtraData = static_cast<NODE_EXTRA_DATA_LEAF*>(this->ExtraData);
			// TODO: Read data
		}
		else if(this->DataType == DATA_MOVE)
		{
			// TODO: Read data
		}
		else if(this->DataType == DATA_OBSTACLE)
		{
			// TODO: Read data
		}
	}

	NODE_EXTRA_DATA_LEAF::NODE_EXTRA_DATA_LEAF(const NODE_EXTRA_DATA_LEAF &S)
	{
		std::memcpy(this, &S, sizeof(NODE_EXTRA_DATA_LEAF));

		this->ExtraData = nullptr;
		if(S.ExtraData != nullptr)
			this->ExtraData = new NODE_EXTRA_DATA(*(S.ExtraData));
	}

	NODE_EXTRA_DATA_LEAF &NODE_EXTRA_DATA_LEAF::operator=(const NODE_EXTRA_DATA_LEAF &S)
	{
		// Delete any previous data
		if(this->PathStorage != nullptr)
			this->PathStorage->ClearPath(this->StoredPathID);
		if(this->ExtraData != nullptr)
			this->ExtraData->DeleteExtraData();

		// Copy all data, then create new container for extraData
		std::memcpy(this, &S, sizeof(NODE_EXTRA_DATA_LEAF));
		this->ExtraData = nullptr;
		if(S.ExtraData != nullptr)
			this->ExtraData = new NODE_EXTRA_DATA(*(S.ExtraData));

		return *this;
	}

	void NODE_EXTRA_DATA_LEAF::SetExtraData(const NODE_EXTRA_DATA &ExtraData)
	{
		if(this->ExtraData == nullptr)
			this->ExtraData = new NODE_EXTRA_DATA(ExtraData);
		else
			*(this->ExtraData) = ExtraData;
	}

	void NODE_EXTRA_DATA_LEAF::SetExtraData(NODE_EXTRA_DATA &&ExtraData)
	{
		if(this->ExtraData == nullptr)
			this->ExtraData = new NODE_EXTRA_DATA(std::move(ExtraData));
		else
			*(this->ExtraData) = std::move(ExtraData);
	}

	const NODE_EXTRA_DATA *NODE_EXTRA_DATA_LEAF::GetExtraData() const
	{
		return this->ExtraData;
	}

	NODE_EXTRA_DATA *NODE_EXTRA_DATA_LEAF::GetExtraData()
	{
		return this->ExtraData;
	}

	NODE_EXTRA_DATA_LEAF::operator const NODE_EXTRA_DATA_MOVE*() const
	{
		if(this->ExtraData != nullptr)
			return this->ExtraData->GetExtraMoveData();

		return nullptr;
	}

	NODE_EXTRA_DATA_LEAF::operator NODE_EXTRA_DATA_MOVE*()
	{
		if(this->ExtraData != nullptr)
			return this->ExtraData->GetExtraMoveData();

		return nullptr;
	}

	NODE_EXTRA_DATA_LEAF::operator const NODE_EXTRA_DATA_OBSTACLE*() const
	{
		if(this->ExtraData != nullptr)
			return this->ExtraData->GetExtraObstacleData();

		return nullptr;
	}

	NODE_EXTRA_DATA_LEAF::operator NODE_EXTRA_DATA_OBSTACLE*()
	{
		if(this->ExtraData != nullptr)
			return this->ExtraData->GetExtraObstacleData();

		return nullptr;
	}

	NODE_EXTRA_DATA_LEAF::NODE_EXTRA_DATA_LEAF(NODE_EXTRA_DATA_LEAF &&S)
	{
		std::memcpy(this, &S, sizeof(NODE_EXTRA_DATA_LEAF));

		S.PathStorage = nullptr;
		S.ExtraData = nullptr;
	}

	NODE_EXTRA_DATA_LEAF &NODE_EXTRA_DATA_LEAF::operator=(NODE_EXTRA_DATA_LEAF &&S)
	{
		// Delete any data
		if(this->PathStorage != nullptr)
			this->PathStorage->ClearPath(this->StoredPathID);
		if(this->ExtraData != nullptr)
			delete this->ExtraData;

		memcpy(this, &S, sizeof(NODE_EXTRA_DATA_LEAF));

		// Remove data from S
		S.ExtraData = nullptr;
		S.PathStorage = nullptr;

		return *this;
	}

	NODE_EXTRA_DATA_LEAF::~NODE_EXTRA_DATA_LEAF()
	{
		if(this->PathStorage != nullptr)
			this->PathStorage->ClearPath(this->StoredPathID);
		if(this->ExtraData != nullptr)
		{
			delete this->ExtraData;
			this->ExtraData = nullptr;
		}
	}

	OBSTACLE_DATA::OBSTACLE_DATA(const POS_2D &_Position, const CERTAINTY_TYPE &_OccupancyPercentage) : POS_2D(_Position), OccupancyPercentage(_OccupancyPercentage)
	{}

	NODE_EXTRA_DATA_MOVE::NODE_EXTRA_DATA_MOVE(const PATH_DATA &TotalPath, const ID &NextPosInPathID, const ID &EndPosInPathID) : PATH_DATA()
	{
		// Go through robot pos from next position (NextPosInPath) to destination (EndPosInPath)
		for(PATH_DATA::const_iterator curPosInPath = TotalPath.begin()+static_cast<difference_type>(NextPosInPathID); curPosInPath != TotalPath.begin()+static_cast<difference_type>(EndPosInPathID); ++curPosInPath)
			this->push_back(*curPosInPath);
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

	const DISTRICT &BRANCH_DATA::GetCurDistrict() const
	{
		return this->pDistrictStorage->GetDistrict(this->GetCurDistrictID());
	}

	DISTRICT_ID BRANCH_DATA::GetCurDistrictID() const
	{
		return this->pDistrictStorage->GetDistrictIDAtGlobalPos(this->CurBotData.GetGlobalBotPosition());
	}

	DISTRICT_ID BRANCH_DATA::GetDistrictIDAtPos(const POS_2D &Position) const
	{
		return this->pDistrictStorage->GetDistrictIDAtGlobalPos(Position);
	}

	void BRANCH_DATA::StepDownOneNode(const TREE_NODE::CHILD_ID &ChildID)
	{
		this->UpdateBranchData(*(this->pCurNode->GetChild(ChildID)));
	}

	void BRANCH_DATA::StepUpOneNode()
	{
		if(this->pCurNode->GetParent() != nullptr)
			this->RevertBranchData(*(this->pCurNode->GetParent()));
	}

	void BRANCH_DATA::Init(const OGM_MAP &OriginalMap, const POS_2D &Start, const POS_2D &Destination, TREE_CLASS &TreeData, const DISTRICT_STORAGE &DistrictStorage)
	{
		this->pOriginalMap = &OriginalMap;
		this->pCurNode = &(TreeData.GetRoot());
		this->Destination = Destination;
		this->FailedPaths.clear();
		this->PathsAfterObservation = 0;
		this->PrevPath.clear();
		this->RemainingMapEntropy = OccupancyGridMap::CalculateEntropyFromMap(OriginalMap);
		this->SuccessPaths.clear();
		this->VisitMap.ResetMap(OriginalMap.GetWidth(), OriginalMap.GetHeight(), 0);
		this->pCurJumpPath = nullptr;
		this->pDistrictStorage = &DistrictStorage;

		// Calculate map data at start
		OccupancyGridMap::CalculateLogMapFromCellMap(OriginalMap, this->CurLogData);
		OccupancyGridMap::CalculateProbMapFromCellMap(OriginalMap, this->CurMapData);
	}

	BRANCH_DATA::BRANCH_DATA(RobotData &_RobotData, MCPathStorage *const PathStorage) : CurBotData(_RobotData), pStoredPathData(PathStorage)
	{}

	void BRANCH_DATA::SetMapPosition(const POS_2D &Position, const OGM_MAP::CELL_TYPE &NewValue)
	{
		// Update map entropy of this cell and all cells around it with the robot radius
		auto &r_oldPosData = this->CurMapData.GetPixelR(Position);
		this->RemainingMapEntropy += OccupancyGridMap::CalculateEntropyFromCell(NewValue)-OccupancyGridMap::CalculateEntropyFromProb(r_oldPosData);

		// Set this cell and update all cells around it with the robot radius
		r_oldPosData =  OccupancyGridMap::CalculateProbValFromCell(NewValue);

		this->CurLogData.SetPixel(Position, OGM_MAP::CalculateLogValueFromProb(r_oldPosData));
	}

	void BRANCH_DATA::SetMapPosition(const POS_2D &Position, const CERTAINTY_TYPE &NewValue)
	{
		// Update map entropy of this cell and all cells around it with the robot radius
		auto &r_oldPosData = this->CurMapData.GetPixelR(Position);
		this->RemainingMapEntropy += OccupancyGridMap::CalculateEntropyFromProb(NewValue)-OccupancyGridMap::CalculateEntropyFromProb(r_oldPosData);

		// Set this cell and update all cells around it with the robot radius
		r_oldPosData = NewValue;

		this->CurLogData.SetPixel(Position, OGM_MAP::CalculateLogValueFromProb(NewValue));
	}

	void BRANCH_DATA::UpdateBranchData(const TREE_NODE &NextNode)
	{
		const NODE_DATA &nextData = NextNode.GetData();
		if(nextData.Action.IsCompleteMove())
		{
			// If this node is a move action, update robot position
			const NODE_EXTRA_DATA_MOVE *pMoveData = nextData.GetExtraMoveData();
			if(pMoveData != nullptr)
			{
				for(const auto &curPos : *pMoveData)
				{
					// Set all positions that bot visited as free
					this->SetMapPosition(curPos, MONTE_CARLO_OPTION2::CELL_FREE);
				}
			}

			// Set all positions that bot visited as free
			this->SetMapPosition(nextData.Position, MONTE_CARLO_OPTION2::CELL_FREE);

			// Move bot to next position
			this->CurBotData.SetGlobalBotPosition(nextData.Position);

			// Check if bot has left current jump successfully
			if(this->pCurJumpPath != nullptr && this->CurBotData.GetGlobalBotPosition() == this->pCurJumpPath->back())
			{
				this->SuccessPaths.push_back(*(this->pCurJumpPath));

				delete this->pCurJumpPath;
				this->pCurJumpPath = nullptr;
			}
		}
		else if(nextData.Action.IsIncompleteMove())
		{
			// At jump, save jump path
			if(this->pCurJumpPath != nullptr)
				delete this->pCurJumpPath;
			this->pCurJumpPath = new TRAVERSED_PATH_SINGLE(this->CurBotData.GetGlobalBotPosition(), nextData.Position);

			this->PrevPath.push_back(*(this->pCurJumpPath));
		}
		else if(nextData.Action.IsObserveResult())
		{
			// At observe result, record observation result
			const NODE_EXTRA_DATA_OBSTACLE *pObstacleData = nextData.GetExtraObstacleData();
			if(pObstacleData != nullptr)
			{
				for(const auto &curPosData : *pObstacleData)
				{
					this->SetMapPosition(static_cast<POS_2D>(curPosData), curPosData.OccupancyPercentage);
				}
			}
			else
			{
				// Set position to free or occupied
				if(nextData.Action.IsFreeResult())
					this->SetMapPosition(nextData.Position, MONTE_CARLO_OPTION2::CELL_FREE);
				else
					this->SetMapPosition(nextData.Position, MONTE_CARLO_OPTION2::CELL_OCCUPIED);
			}

			if(nextData.Action.IsOccupiedResult())
			{
				// If this is a occupied result, add this path to failed paths and move out of jump path
				if(this->pCurJumpPath != nullptr)
				{
					this->FailedPaths.push_back(*(this->pCurJumpPath));
					delete this->pCurJumpPath;
					this->pCurJumpPath = nullptr;
				}
			}

			// Store that we performed an observation
			this->PathsAfterObservation = this->PrevPath.size();
		}
		else if(nextData.Action.IsObserveAction())
		{
			// At observe action, do nothing
		}

		// Update current node
		this->pCurNode = const_cast<TREE_NODE*>(&NextNode);
	}

	void BRANCH_DATA::RevertBranchData(const TREE_NODE &PrevNode)
	{
		// Revert data of current node
		const NODE_DATA &prevData = PrevNode.GetData();
		const NODE_DATA &curData = this->pCurNode->GetData();
		if(curData.Action.IsCompleteMove())
		{
			// If this node is a move action, update robot position
			const NODE_EXTRA_DATA_MOVE *pMoveData = curData.GetExtraMoveData();
			if(pMoveData != nullptr)
			{
				for(const auto &curPos : *pMoveData)
				{
					// Set all positions that bot visited as free
					this->SetMapPosition(curPos, this->pOriginalMap->GetPixel(curPos));
				}
			}

			// Set all positions that bot visited as free
			this->SetMapPosition(curData.Position, MONTE_CARLO_OPTION2::CELL_FREE);

			// Move bot to previous position
			const TREE_NODE *pLastMoveNode = &PrevNode;
			while(!pLastMoveNode->GetData().Action.IsCompleteMove())
			{
				if(pLastMoveNode->GetParent() == nullptr)
					return;			// ERROR

				pLastMoveNode = pLastMoveNode->GetParent();
			}
			this->CurBotData.SetGlobalBotPosition(pLastMoveNode->GetData().Position);
		}
		else if(curData.Action.IsIncompleteMove())
		{
			// Remove old jump data
			this->PrevPath.pop_back();
		}
		else if(curData.Action.IsObserveResult())
		{
			// At observe result, revert observation result
			const NODE_EXTRA_DATA_OBSTACLE *pObstacleData = curData.GetExtraObstacleData();
			if(pObstacleData != nullptr)
			{
				for(const auto &curPosData : *pObstacleData)
				{
					this->SetMapPosition(static_cast<POS_2D>(curPosData), this->pOriginalMap->GetPixel(curPosData));
				}
			}
			else
				this->SetMapPosition(prevData.Position, this->pOriginalMap->GetPixel(prevData.Position));

			// Store that we performed an observation
			this->PathsAfterObservation = this->PrevPath.size();
		}
		else if(prevData.Action.IsObserveAction())
		{
			// At observe action, do nothing
		}

		if(prevData.Action.IsIncompleteMove())
		{
			// At jump, delete old jump path
			if(this->pCurJumpPath != nullptr)
				*(this->pCurJumpPath) = TRAVERSED_PATH_SINGLE(this->CurBotData.GetGlobalBotPosition(), prevData.Position);
			else
				this->pCurJumpPath = new TRAVERSED_PATH_SINGLE(this->CurBotData.GetGlobalBotPosition(), prevData.Position);
		}

		// Revert current node
		this->pCurNode = const_cast<TREE_NODE*>(&PrevNode);
	}
}

MonteCarloOption2::MonteCarloOption2(RobotData _RobotData , const MC_PATH_STORAGE::PATH_ID &MaxStoredPaths) : _MCTree(), _Branch(_RobotData, &(this->_PathStorage)), _PathStorage(MaxStoredPaths)
{}

int MonteCarloOption2::PerformMonteCarlo(const OccupancyGridMap &Map, const DistrictMapStorage &DistrictData, const POS_2D &StartPos, const POS_2D &Destination)
{
	// Set current map to use
	this->_Branch.Init(Map, StartPos, Destination, this->_MCTree, DistrictData);

	// Create tree
	this->_MCTree.Reset();
	TREE_NODE *pCurNode = &(this->_MCTree.GetRoot());
	pCurNode->GetDataR() = NODE_DATA(MONTE_CARLO_OPTION2::NODE_ACTION_OBSERVE, StartPos);		// Observe start pos
	pCurNode = pCurNode->AddChild(NODE_DATA(MONTE_CARLO_OPTION2::NODE_ACTION_RESULT_FREE, StartPos));		// Mark start pos as free
	pCurNode->AddChild(NODE_DATA(MONTE_CARLO_OPTION2::NODE_ACTION_MOVE, StartPos));				// Move robot to start pos

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


void MonteCarloOption2::Expansion()
{
	TREE_NODE *&ppCurNode = this->_Branch.pCurNode;
	const DistrictMap &curDistrict = this->_Branch.GetCurDistrict();

	const POS_2D curBotPos = this->_Branch.CurBotData.GetGlobalBotPosition();

	// Is there already a path stored that this bot should take?
	bool expansionFinished = false;
	const NODE_EXTRA_DATA_LEAF *pLeafData = ppCurNode->GetData().GetExtraLeafData();
	if(pLeafData != nullptr)
	{
		// Have we reached the destination?
		if(curBotPos == pLeafData->TargetPosition)
		{
			// Add node that leads to follow-up position if it exists
			if(pLeafData->TargetPosition != pLeafData->FollowUpPosition)
			{
				// Decide whether to observe status of FollowUp Position first, or move there
				const BRANCH_DATA::PROB_CELL_TYPE cellProbability = this->_Branch.GetCurMapData(pLeafData->FollowUpPosition);
				if(cellProbability <=  MONTE_CARLO_OPTION2::CELL_FREE)
				{
					ppCurNode->AddChild(NODE_DATA(MONTE_CARLO_OPTION2::NODE_ACTION_MOVE, pLeafData->FollowUpPosition));		// If cell is free, move here
					expansionFinished = true;

					ppCurNode->GetDataR().DeleteExtraData();		// Leaf data no longer needed, delete it
				}
				else if (cellProbability >= MONTE_CARLO_OPTION2::CELL_OCCUPIED)
				{
					expansionFinished = false;				// Moving here is impossible, find new move orders

					ppCurNode->GetDataR().DeleteExtraData();		// Leaf data no longer needed, delete it
				}
				else
				{
					// If cell status is unknown, observe cell
					ppCurNode->AddChild(NODE_DATA(MONTE_CARLO_OPTION2::NODE_ACTION_OBSERVE, pLeafData->FollowUpPosition));
					expansionFinished = true;
				}
			}
		}
		else if(curBotPos == pLeafData->FollowUpPosition)
		{
			ppCurNode->GetDataR().DeleteExtraData();			// Leaf data no longer needed, delete it

			// continue looking for new positions
			expansionFinished = false;
		}
		else
		{
			// If we are neither at the target nor the follow up position, follow the stored path instead of finding new one
			// Check if this path is already stored or whether we need to calculate it
			const PATH_DATA *pPath;
			if(!this->_Branch.pStoredPathData->IsPathStored(pLeafData->StoredPathID))
			{
				// Path not stored, calculate it and store it in temporary location
				MCPathStorage::PATH_ID freeID;
				if(this->_Branch.pStoredPathData->IsFreeSpaceAvailable(&freeID))
				{
					PATH_DATA tmpPath;
					CalculatePath(this->_Branch, curBotPos, pLeafData->TargetPosition, &tmpPath, nullptr, nullptr, nullptr);
					this->_Branch.pStoredPathData->SetPath(freeID, std::move(tmpPath));
					pPath = &(this->_Branch.pStoredPathData->GetPath(freeID));
				}
				else
				{
					CalculatePath(this->_Branch, curBotPos, pLeafData->TargetPosition, &(this->_Branch.pStoredPathData->GetTempPath()), nullptr, nullptr, nullptr);
					pPath = &(this->_Branch.pStoredPathData->GetTempPath());			// Point to temp path
				}
			}
			else
			{
				ppCurNode->GetDataR().GetExtraLeafData()->StoredPathID = this->_Branch.pStoredPathData->GetMaxStoredPaths();		// Set to invalid ID
				pPath = &(this->_Branch.pStoredPathData->GetPath(pLeafData->StoredPathID));		// Point to stored path
			}

			// Follow the path until an obstacle should be simulated
			EXPECTED_LENGTH_TYPE pathLength;
			CERTAINTY_TYPE pathCertainty;
			PATH_DATA::ID posReachedID;
			POS_2D obstaclePos;
			POS_2D posReached;
			TREE_NODE *pIntermediateNode;
			int tmpResult;
			if(pPath->size() > 2 && this->_Branch.CurMapData.GetPixel(pPath->back()) > MONTE_CARLO_OPTION2::MIN_CERTAINTY)
			{
				// Create an obstacle at end if it's possible
				PATH_DATA::const_iterator forcedObstacle = pPath->end()-1;
				tmpResult = this->FollowPathUntilObstacle(this->_Branch, *pPath, this->_Branch.MinPathCertainty, this->_Branch.MinPathLength, this->_Branch.CurBotData.GetScanRange(), this->_Branch.CurBotData.GetGlobalBotPosition(), MONTE_CARLO_OPTION2::MAX_CERTAINTY, 0, &(forcedObstacle), &pathCertainty, &pathLength, &posReachedID, &obstaclePos);
			}
			else
				tmpResult = this->FollowPathUntilObstacle(this->_Branch, *pPath, this->_Branch.MinPathCertainty, this->_Branch.MinPathLength, this->_Branch.CurBotData.GetScanRange(), this->_Branch.CurBotData.GetGlobalBotPosition(), MONTE_CARLO_OPTION2::MAX_CERTAINTY, 0, nullptr, &pathCertainty, &pathLength, &posReachedID, &obstaclePos);
			if(tmpResult < 0)
				return;			//ERROR
			else if(tmpResult == 0)
			{
				// This result indicates that bot encountered an obstacle before moving, thus skip the move order and add observation immediately
				pIntermediateNode = ppCurNode;
				posReached = curBotPos;
			}
			else
			{
				// Move to new position, then add branch
				posReached = pPath->at(posReachedID);

				if(posReachedID == 0)
					pIntermediateNode = ppCurNode->AddChild(NODE_DATA(MONTE_CARLO_OPTION2::NODE_ACTION_MOVE, pPath->at(posReachedID)));			// If one move necessary, add simple move order
				else
					pIntermediateNode = ppCurNode->AddChild(NODE_DATA(MONTE_CARLO_OPTION2::NODE_ACTION_MOVE, pPath->at(posReachedID), NODE_EXTRA_DATA(NODE_EXTRA_DATA_MOVE(*pPath, 0, posReachedID))));				

				// Update path by removing traversed position
				if(posReachedID == pPath->size()-1)
					pLeafData->PathStorage->ClearPath(pLeafData->StoredPathID);
				else
					pLeafData->PathStorage->GetPathR(pLeafData->StoredPathID).erase(pPath->begin(), pPath->begin()+posReachedID);

				// Calculate move data
				//this->CalculateMoveNodeData(this->_Branch, this->_Branch.pCurNode->GetDataR());
			}

			// Now add an observation node and move leaf data to this node
			NODE_EXTRA_DATA_OBSTACLE obstacleData = this->CreateObstacleAtPos(this->_Branch, obstaclePos, posReached, this->_Branch.ObstacleLength, this->_Branch.ObstacleHeight, this->_Branch.MinObstacleCertainty, this->_Branch.MaxObstacleCertainty);
			pIntermediateNode = pIntermediateNode->AddChild(NODE_DATA(MONTE_CARLO_OPTION2::NODE_ACTION_OBSERVE, obstaclePos, NODE_EXTRA_DATA(std::move(ppCurNode->GetDataR()))));

			// Now append obstacle data to this leaf's data
			pIntermediateNode->GetDataR().GetExtraLeafData()->SetExtraData(std::move(obstacleData));

			expansionFinished = true;
		}
	}

	if(!expansionFinished)
	{
		// No path already stored
		// Look at all connections around current node and add those that haven't been followed recently
		for(const auto &curIDConnection : curDistrict.GetAdjacentConnections())
		{
			for(const auto &curConnectionPos : curIDConnection.ConnectionPositions)
			{
				if(curBotPos == curConnectionPos[0])
					continue;			// Skip if we are at this position

				// Check if a position has already been traversed recently
				if(this->_Branch.PrevPath.PartialPathContainsBidirectionalMove(curConnectionPos[0], curConnectionPos[1], this->_Branch.PathsAfterObservation))
					continue;			// Skip if we already tried this decision

				// If this position hasn't been traversed yet, add branch that moves here
				// Add child with node data
				NODE_EXTRA_DATA_LEAF tmpLeafData;
				PATH_DATA &tmpPath = this->_Branch.pStoredPathData->ReservePathWithTemp(&tmpLeafData.StoredPathID);		// Get a new free path pos

				// Attempt to find path to destination
				tmpLeafData.TargetPosition = curConnectionPos.at(0);
				if(CalculatePath(this->_Branch, curBotPos, tmpLeafData.TargetPosition, &tmpPath, nullptr, nullptr, nullptr) >= 0)
				{
					// If a path was found, add exploration node
					tmpLeafData.FollowUpPosition = curConnectionPos.at(1);
					tmpLeafData.RecentPathCertainty = MONTE_CARLO_OPTION2::MAX_CERTAINTY;
					tmpLeafData.RecentPathLength = 0;
					tmpLeafData.PathStorage = this->_Branch.pStoredPathData;

					// Create new child
					TREE_NODE *pNewNode = ppCurNode->AddChild(NODE_DATA(MONTE_CARLO_OPTION2::NODE_ACTION_JUMP, curConnectionPos.at(1), std::move(tmpLeafData)));
				}
			}
		}

		// Check if we are in same district as destination
		if(this->_Branch.pDistrictStorage->GetDistrictIDAtGlobalPos(this->_Branch.Destination) == curDistrict.GetID())
		{
			// Add child with node data leading to destination
			NODE_EXTRA_DATA_LEAF tmpLeafData;
			PATH_DATA &tmpPath = this->_Branch.pStoredPathData->ReservePathWithTemp(&tmpLeafData.StoredPathID);		// Get a new free path pos

			// Attempt to find path to destination
			tmpLeafData.TargetPosition = this->_Branch.Destination;
			if(CalculatePath(this->_Branch, curBotPos, tmpLeafData.TargetPosition, &tmpPath, nullptr, nullptr, nullptr) >= 0)
			{
				// If a path was found, add exploration node
				tmpLeafData.FollowUpPosition = this->_Branch.Destination;
				tmpLeafData.RecentPathCertainty = MONTE_CARLO_OPTION2::MAX_CERTAINTY;
				tmpLeafData.RecentPathLength = 0;
				tmpLeafData.PathStorage = this->_Branch.pStoredPathData;

				// Create new child
				TREE_NODE *pNewNode = ppCurNode->AddChild(NODE_DATA(MONTE_CARLO_OPTION2::NODE_ACTION_JUMP, this->_Branch.Destination, std::move(tmpLeafData)));
			}
		}
	}

	if(ppCurNode->GetNumChildren() == 0)
		ppCurNode->GetDataR().IsDone = true;		// Set leaf to done if no children could be created
}

void MonteCarloOption2::Simulation()
{
	// Simulate all children of this node
	const auto numChildren = this->_Branch.pCurNode->GetNumChildren();
	for(TREE_NODE::CHILD_ID curChildID = 0; curChildID < numChildren; ++curChildID)
	{
		// Find leaf of this node
		unsigned int numStepsDown = 1;
		this->_Branch.StepDownOneNode(curChildID);

		while(!this->_Branch.pCurNode->IsLeaf())
		{
			numStepsDown++;
			this->_Branch.StepDownOneNode(0);		// There should be only one child
		}

		// Simulate this leaf
		this->SimulateCurrentLeaf();

		// Move back up to parent node of all simulations
		while(numStepsDown > 1)
		{
			numStepsDown--;
			this->_Branch.StepUpOneNode();

			this->Backtrack_Step();			// Perform backtrack from this node
		}

		// Skip last backtrack, that will be done after all children have been simulated
		this->_Branch.StepUpOneNode();
	}
}

void MonteCarloOption2::Backtrack()
{
	// Continue backtracking until root is reached
	while(this->_Branch.pCurNode->GetParent() != nullptr)
	{
		this->Backtrack_Step();
		this->_Branch.StepUpOneNode();
	}

	// Perform one last backtrack to calculate root node values
	this->Backtrack_Step();
}

void MonteCarloOption2::SimulateCurrentLeaf()
{
	// Check leaf type
	TREE_NODE *&ppCurNode = this->_Branch.pCurNode;
	NODE_DATA &r_CurLeafData = ppCurNode->GetDataR();
	if(r_CurLeafData.Action.IsObserveAction())
	{
		// Create two child leafs, one for free result and one for occupied

		// Free result should copy old leaf data
		ppCurNode->AddChild(NODE_DATA(MONTE_CARLO_OPTION2::NODE_ACTION_RESULT_FREE, r_CurLeafData.Position, NODE_EXTRA_DATA(std::move(r_CurLeafData))));			// This moves leaf data, but copies the obstacle data and leaves it behind
		ppCurNode->AddChild(NODE_DATA(MONTE_CARLO_OPTION2::NODE_ACTION_RESULT_FREE, r_CurLeafData.Position, NODE_EXTRA_DATA(std::move(r_CurLeafData))));		// This moves the remaining obstacle data to this child node

		this->Simulation();		// Run the simulation on these two created leaves
	}
	else if(r_CurLeafData.Action.IsIncompleteMove())
	{
		NODE_DATA intermediateData;

		// Run two simulations, one from bot position to the jump point, the other from jump point to destination
		this->RunSimulation(this->_Branch.CurBotData.GetGlobalBotPosition(), r_CurLeafData.Position, this->_Branch, ppCurNode->GetParent()->GetData().NumVisits, intermediateData);
		this->RunSimulation(r_CurLeafData.Position, this->_Branch.Destination, this->_Branch, ppCurNode->GetParent()->GetData().NumVisits, r_CurLeafData);

		// Combine the two results
		r_CurLeafData.ExpectedLength += intermediateData.ExpectedLength;
		r_CurLeafData.ExpectedCost += intermediateData.ExpectedCost;
		r_CurLeafData.Certainty *= intermediateData.Certainty;

		r_CurLeafData.IsDone &= intermediateData.IsDone;			// Only done if both paths are done
		this->CalculateNodeValue(r_CurLeafData, this->_Branch.Constant, ppCurNode->GetParent()->GetData().NumVisits, r_CurLeafData.Value);			// Calculate node value
	}
	else if(r_CurLeafData.Action.IsObserveResult() || r_CurLeafData.Action.IsMovementAction())
	{
		// Run the simulation
		this->RunSimulation(this->_Branch.CurBotData.GetGlobalBotPosition(), this->_Branch.Destination, this->_Branch, ppCurNode->GetParent()->GetData().NumVisits, r_CurLeafData);
	}
}

void MonteCarloOption2::Backtrack_Step()
{
	// Determine type of current node
	TREE_NODE * const&ppCurNode = this->_Branch.pCurNode;
	NODE_DATA &r_CurNodeData = ppCurNode->GetDataR();

	const TREE_NODE *const pBestChildNode = this->FindBestChildNode(*ppCurNode, r_CurNodeData.IsDone);

	if(r_CurNodeData.Action.IsObserveAction())
	{
		// Combine both result options
		if(ppCurNode->GetNumChildren() != 2)
			return;				// ERROR

		const NODE_DATA &bestData = pBestChildNode->GetData();

		// Get other option
		const NODE_DATA *pOtherData;
		if(pBestChildNode == ppCurNode->GetChild(0))
			pOtherData = &(ppCurNode->GetChild(1)->GetData());
		else
			pOtherData = &(ppCurNode->GetChild(0)->GetData());

		r_CurNodeData.Certainty = bestData.Certainty+(MONTE_CARLO_OPTION2::MAX_CERTAINTY-bestData.Certainty)*pOtherData->Certainty;
		r_CurNodeData.ExpectedLength = bestData.ExpectedLength+(MONTE_CARLO_OPTION2::MAX_CERTAINTY-bestData.Certainty)*pOtherData->ExpectedLength;
		r_CurNodeData.ExpectedCost = bestData.ExpectedCost+(MONTE_CARLO_OPTION2::MAX_CERTAINTY-bestData.Certainty)*pOtherData->ExpectedCost + this->_Branch.ObserveActionCost;		// Add cost for this observation as well
	}
	else if(r_CurNodeData.Action.IsIncompleteMove())
	{
		// Just copy best value
		const NODE_DATA &bestData = pBestChildNode->GetData();

		r_CurNodeData.Certainty = bestData.Certainty;
		r_CurNodeData.ExpectedCost = bestData.ExpectedCost + this->_Branch.ObserveActionCost;
		r_CurNodeData.ExpectedLength = bestData.ExpectedLength;
	}
	else if(r_CurNodeData.Action.IsMovementAction())
	{
		// Copy best value and add move length of this node
		const NODE_DATA &bestData = pBestChildNode->GetData();
		r_CurNodeData.Certainty = bestData.Certainty;				// Copy certainty, the uncertainty of this path is already considered in the following observation

		const NODE_EXTRA_DATA_MOVE *pMoveData = bestData.GetExtraMoveData();
		if(pMoveData != nullptr)
		{
			r_CurNodeData.ExpectedCost = bestData.ExpectedCost + this->_Branch.MoveActionCost*pMoveData->size();
			r_CurNodeData.ExpectedLength = bestData.ExpectedLength + pMoveData->size();
		}
		else
		{
			r_CurNodeData.ExpectedCost = bestData.ExpectedCost + this->_Branch.MoveActionCost*1;
			r_CurNodeData.ExpectedLength = bestData.ExpectedLength + 1;
		}
	}
	else // if(r_CurNodeData.Action.IsObserveResult())
	{
		// Copy best value
		const NODE_DATA &bestData = pBestChildNode->GetData();
		r_CurNodeData.Certainty = bestData.Certainty;				// Copy certainty, the uncertainty of this path is already considered in the following observation

		r_CurNodeData.Certainty = bestData.Certainty;
		r_CurNodeData.ExpectedCost = bestData.ExpectedCost + this->_Branch.ObserveActionCost;
		r_CurNodeData.ExpectedLength = bestData.ExpectedLength;
	}

	r_CurNodeData.NumVisits++;
}

int MonteCarloOption2::CalculatePath(const BRANCH_DATA &BranchData, const POS_2D &StartPos, const POS_2D &Destination, PATH_DATA *const PathTaken, EXPECTED_LENGTH_TYPE *const ExpectedLength, CERTAINTY_TYPE *const Certainty, COST_TYPE *const Cost)
{
	PATH_DATA tmpPath;
	if(!const_cast<BRANCH_DATA &>(BranchData).AStarMap.CalculatePath(BranchData.CurMapData, MONTE_CARLO_OPTION2::MAX_CERTAINTY, StartPos, Destination, &(BranchData.pDistrictStorage->GetDistrict(BranchData.GetDistrictIDAtPos(Destination))), &tmpPath, ExpectedLength))
		return -1;		// return error

	// Calculate Cost or certainty if requested
	if(Cost != nullptr || Certainty != nullptr)
	{
		CERTAINTY_TYPE invertedCertainty = MONTE_CARLO_OPTION2::MIN_CERTAINTY;

		for(const auto &curPos : tmpPath)
		{
			invertedCertainty += (MONTE_CARLO_OPTION2::MAX_CERTAINTY-invertedCertainty)*(BranchData.CurMapData.GetPixel(curPos));
		}

		if(Certainty != nullptr)
			*Certainty = MONTE_CARLO_OPTION2::MAX_CERTAINTY-invertedCertainty;

		if(Cost != nullptr)
			*Cost = BranchData.MoveActionCost*tmpPath.size();
	}

	// Save path if requested
	if(PathTaken != nullptr)
		*PathTaken = std::move(tmpPath);

	return 1;
}

int MonteCarloOption2::FollowPathUntilObstacle(const BRANCH_DATA &BranchData, const PATH_DATA &PathToTake, const CERTAINTY_TYPE &MinCertainty, const EXPECTED_LENGTH_TYPE &MinLength, const EXPECTED_LENGTH_TYPE &ScanRange, const POS_2D &StartPos, const CERTAINTY_TYPE &StartCertainty, const EXPECTED_LENGTH_TYPE &StartLength, const PATH_DATA::const_iterator *const NextForcedObstacleIterator, CERTAINTY_TYPE *const PathCertainty, EXPECTED_LENGTH_TYPE *const PathLength, PATH_DATA::ID *const EndPosID, POS_2D *const ObstaclePos)
{
	// Set start parameters
	const CERTAINTY_TYPE maxInvertedLogCertainty = OccupancyGridMap::CalculateLogValueFromProb(MONTE_CARLO_OPTION2::MAX_CERTAINTY - MinCertainty);
	CERTAINTY_TYPE curInvertedLogCertainty = OccupancyGridMap::CalculateLogValueFromProb(StartCertainty);
	EXPECTED_LENGTH_TYPE curLength = StartLength;
	EXPECTED_LENGTH_TYPE curDistBetweenBotAndObstacle = GetMovementCost(StartPos, PathToTake.at(0));		// Keep track of distance between robot and obstacle
	PATH_DATA::ID obstacleID = GetInfiniteVal<PATH_DATA::ID>();
	PATH_DATA::ID botID = GetInfiniteVal<PATH_DATA::ID>();
	const POS_2D *pCurBotPos = &StartPos;
	const POS_2D *pCurObstaclePos = &StartPos;

	// Go through path until length reaches minimum AND Certainty reaches minimum
	for(auto curPathPos = PathToTake.begin(); curPathPos != PathToTake.end(); ++curPathPos)
	{
		// Update certainty and length
		curInvertedLogCertainty += BranchData.CurLogData.GetPixel(*curPathPos);
		const auto movementCost = GetMovementCost(*curPathPos, *pCurObstaclePos);
		curLength += movementCost;
		curDistBetweenBotAndObstacle += movementCost;

		// Move bot until next obstacel pos is back in scan range
		while(curDistBetweenBotAndObstacle > ScanRange)
		{
			// Move bot up along path
			botID++;

			// Update distance
			curDistBetweenBotAndObstacle -= GetMovementCost(*pCurBotPos, PathToTake.at(botID));

			// Update robot position
			pCurBotPos = &(PathToTake.at(botID));
		}

		// Update obstacle position
		pCurObstaclePos = &(*curPathPos);

		// If we reach a forced obstacle position, create an obstacle here
		if(NextForcedObstacleIterator != nullptr)
		{
			if(*NextForcedObstacleIterator == curPathPos)
			{
				obstacleID = static_cast<PATH_DATA::ID>((*NextForcedObstacleIterator)-PathToTake.begin());
				break;
			}
		}

		// Check if both the certainty and length is in correct area
		if(curLength >= MinLength && curInvertedLogCertainty >= maxInvertedLogCertainty)
		{
			// Stop if both conditions are met
			obstacleID = static_cast<PATH_DATA::ID>((*NextForcedObstacleIterator)-PathToTake.begin());
			break;
		}
	}

	// Save the requested data
	if(PathCertainty != nullptr)
		*PathCertainty = MONTE_CARLO_OPTION2::MAX_CERTAINTY - OccupancyGridMap::CalculateProbValueFromLog(curInvertedLogCertainty);
	if(PathLength != nullptr)
		*PathLength = std::move(curLength);
	if(EndPosID != nullptr)
		*EndPosID = std::move(botID);
	if(ObstaclePos != nullptr)
		*ObstaclePos = *pCurObstaclePos;

	// Check if a path was found
	if(obstacleID == GetInfiniteVal<PATH_DATA::ID>())
		return -1;

	// Check if bot moved
	if(botID == GetInfiniteVal<PATH_DATA::ID>())
		return 0;

	return 1;
}

MonteCarloOption2::NODE_EXTRA_DATA_OBSTACLE &&MonteCarloOption2::CreateObstacleAtPos(const BRANCH_DATA &BranchData, const POS_2D &ObstaclePosition, const POS_2D &BotPosition, const EXPECTED_LENGTH_TYPE &ObstacleLength, const EXPECTED_LENGTH_TYPE &ObstacleHeight, const CERTAINTY_TYPE &MinObstacleCertainty, const CERTAINTY_TYPE &MaxObstacleCertainty)
{
	NODE_EXTRA_DATA_OBSTACLE newObstacle;

	if(ObstacleHeight == 1 && ObstacleLength == 1)
	{
		newObstacle.push_back(MONTE_CARLO_OPTION2::OBSTACLE_DATA(ObstaclePosition, MONTE_CARLO_OPTION2::MAX_CERTAINTY));

		return std::move(newObstacle);
	}

	// Calculate height and width angle
	float heightAngle = std::atan2(ObstaclePosition.Y-BotPosition.Y, ObstaclePosition.X - BotPosition.X);
	float widthAngle = (heightAngle < -M_PI_2 ? -heightAngle+M_PI_2 : heightAngle-M_PI_2);

	// Get start position and increment
	const float incrementAlongWidthX = 1;
	const float incrementAlongWidthY = std::sin(widthAngle)/std::cos(widthAngle);
	const float incrementAlongHeightX = std::cos(heightAngle)/std::sin(heightAngle);
	const float incrementAlongHeightY = 1;
	const POS_2D_TYPE startPosX = std::round(std::cos(-widthAngle)*ObstacleLength/2+ObstaclePosition.X);
	const POS_2D_TYPE startPosY = std::round(std::sin(-widthAngle)*ObstacleLength/2+ObstaclePosition.Y);

	// Move obstacle data
	for(float curIncrementAlongWidthX = 0, curIncrementAlongWidthY = 0; curIncrementAlongWidthX <= ObstacleLength; curIncrementAlongWidthX += incrementAlongWidthX, curIncrementAlongWidthY += incrementAlongWidthY)
	{
		for(float curIncrementAlongHeightX = 0, curIncrementAlongHeightY = 0; curIncrementAlongHeightY <= ObstacleHeight; curIncrementAlongHeightX += incrementAlongHeightX, curIncrementAlongHeightY += incrementAlongHeightY)
		{
			// Check if current position is in map
			const MONTE_CARLO_OPTION2::OBSTACLE_DATA curPos = MONTE_CARLO_OPTION2::OBSTACLE_DATA(POS_2D(std::round(curIncrementAlongWidthX+curIncrementAlongHeightX+startPosX), std::round(curIncrementAlongWidthY+curIncrementAlongHeightY+startPosY)), MONTE_CARLO_OPTION2::MAX_CERTAINTY);

			if(BranchData.CurMapData.IsInMap(static_cast<POS_2D>(curPos)))
				newObstacle.push_back(curPos);
		}
	}

	return std::move(newObstacle);
}

void MonteCarloOption2::RunSimulation(const POS_2D &BotPos, const POS_2D &Destination, const BRANCH_DATA &MapData, const NUM_VISITS_TYPE &NumParentsVisit, NODE_DATA &Result)
{
	if(BotPos == Destination)
	{
		// If we reached destination, set node as done
		Result.IsDone = true;

		// As we are at dest, the cost, length, and certainty are trivial
		Result.ExpectedCost = 0;
		Result.ExpectedLength = 0;
		Result.Certainty = 1;
	}
	else
	{
		// Calculate one path to destination
		this->CalculatePath(MapData, BotPos, Destination, nullptr, &(Result.ExpectedLength), &(Result.Certainty), &(Result.ExpectedCost));

		if(Result.Certainty <= MONTE_CARLO_OPTION2::MIN_CERTAINTY)
		{
			Result.IsDone = true;
			Result.ExpectedCost = GetInfiniteVal<EXPECTED_LENGTH_TYPE>();
			Result.ExpectedLength = GetInfiniteVal<EXPECTED_LENGTH_TYPE>();
		}
	}

	// Set visits to this node to one
	Result.NumVisits = 1;

	// Calculate node value from this data
	this->CalculateNodeValue(Result, this->_Branch.Constant, NumParentsVisit, Result.Value);
}

void MonteCarloOption2::CalculateNodeValue(const NODE_DATA &Data, const NODE_VALUE_TYPE &Constant, const NUM_VISITS_TYPE &NumParentsVisit, NODE_VALUE_TYPE &Value)
{
	// Node is done, don't select it
	if(Data.IsDone)
		Value = MONTE_CARLO_OPTION2::MIN_NODE_VALUE;
	else
		Value = Data.Certainty/Data.ExpectedLength + Constant*static_cast<NODE_VALUE_TYPE>(std::sqrt(static_cast<double>(NumParentsVisit)/Data.NumVisits));
}

const MonteCarloOption2::TREE_NODE *MonteCarloOption2::FindBestChildNode(const TREE_NODE &CurNode, bool &AllDone) const
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

MonteCarloOption2::NODE_VALUE_TYPE MonteCarloOption2::CalculateComparableChildValue(const NODE_DATA &ChildNodeData) const
{
	return ChildNodeData.ExpectedLength;
}
