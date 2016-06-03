#include "monte_carlo_branch_data.h"

void MonteCarloBranchData::Reset()
{
	this->PathTaken.clear();
	this->FirstPosAfterObservation = 0;
	this->pOGMap = nullptr;
}

void MonteCarloBranchData::SetMonteCarloObjectives(const OccupancyGridMap &Map, const POS_2D &Destination)
{
	this->Reset();

	this->Destination = Destination;
	this->pOGMap = &Map;
}

void MonteCarloBranchData::ResetBranchDataToRoot(const MONTE_CARLO_TREE_CLASS &Tree)
{
	// Set paths to zero
	this->PathTaken.clear();
	this->FirstPosAfterObservation = 0;

	// Recalculate log map
	OccupancyGridMap::CalculateLogMapFromCellMap(*this->pOGMap, this->LogMap);

	// Recalculate D* Map
	this->DStarCostMap.CalculateDStarCostMap(this->LogMap, this->Destination, OGM_LOG_MIN, OGM_LOG_MAX);

	// Reset number of visits to zero for all cells
	this->NumVisitsMap.ResetMap(this->pOGMap->GetWidth(), this->pOGMap->GetHeight(), 0);

	// Set current node to root node
	this->pCurNode = &(const_cast<MONTE_CARLO_TREE_CLASS &>(Tree).GetRoot());

}

int	MonteCarloBranchData::MoveDownOneNode(const MONTE_CARLO_NODE::CHILD_ID &NextNodeID)
{
	// Check that next node exists
	if(NextNodeID >= this->pCurNode->GetNumChildren())
		return -1;

	// Update all data
	this->UpdateBranchData(*(this->pCurNode->GetChild(NextNodeID)));

	return 1;
}

int	MonteCarloBranchData::MoveUpOneNode()
{
	// Check that current node isn't root
	if(this->pCurNode->GetParent() == nullptr)
		return -1;

	// Revert node data
	this->RevertBranchData(*(this->pCurNode->GetParent()));

	return 1;
}

void MonteCarloBranchData::UpdateBranchData(const MONTE_CARLO_NODE &NextNode)
{
	// Set new current node
	this->pCurNode = const_cast<MONTE_CARLO_NODE *>(&NextNode);

	// Get Data
	const MonteCarloNodeData &nextData = NextNode.GetData();

	// Update maps with data
	if(nextData.Action.IsMoveAction())
	{
		// If this node represents a move action, update path and visits map
		this->NumVisitsMap.GetPixelR(nextData.NewCell)++;

		this->PathTaken.push_back(nextData.NewCell);
	}
	else if(nextData.Action.IsObserveAction())
	{
		// If this is observe action, reset first position after observation to this position
		this->FirstPosAfterObservation = this->PathTaken.size()-1;
		//if(this->FirstPosAfterObservation > 0)
		//	this->FirstPosAfterObservation--;		// Set last position as first
	}
	else
	{
		// If this is an observation result, update log data
		if(nextData.Action.IsCellFree())
		{
			// At free cell, set log map position to zero
			this->LogMap.GetPixelR(nextData.NewCell) = OGM_LOG_MIN;
		}
		else
		{
			// At occpupied cell, set log map position to infinity
			this->LogMap.GetPixelR(nextData.NewCell) = OGM_LOG_MAX;
		}

		this->DStarCostMap.CalculateDStarCostMap(this->LogMap, this->Destination, OGM_LOG_MIN, OGM_LOG_MAX);
	}
}

void MonteCarloBranchData::RevertBranchData(const MONTE_CARLO_NODE &PrevNode)
{
	// Check that given node is parent
	if(&PrevNode != this->pCurNode->GetParent())
		return;

	// Get current Data
	const MonteCarloNodeData &curData = this->pCurNode->GetData();

	// Revert maps with data
	if(curData.Action.IsMoveAction())
	{
		// If this node represents a move action, update path and visits map
		this->NumVisitsMap.GetPixelR(curData.NewCell)--;

		this->PathTaken.pop_back();

		// Just set first move after observation to invalid
		this->FirstPosAfterObservation = this->PathTaken.size();
	}
	else if(curData.Action.IsObserveAction())
	{
		// If this is observe action, reset first position after observation to next move
		this->FirstPosAfterObservation = this->PathTaken.size();
	}
	else
	{
		// Recalculate Log Map value from cell data
		this->LogMap.GetPixelR(curData.NewCell) = OccupancyGridMap::CalculateLogValFromCell(this->pOGMap->GetPixel(curData.NewCell));

		this->DStarCostMap.CalculateDStarCostMap(this->LogMap, this->Destination, OGM_LOG_MIN, OGM_LOG_MAX);
	}

	// Set new current node
	this->pCurNode = const_cast<MONTE_CARLO_NODE *>(&PrevNode);
}
