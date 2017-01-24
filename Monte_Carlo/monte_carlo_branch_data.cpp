#include "monte_carlo_branch_data.h"

void MonteCarloBranchData::Reset()
{
	this->PrevBranches->clear();

	this->PathTaken.clear();
	this->FirstPosAfterObservation = 0;
	this->pOGMaps = nullptr;
}

void MonteCarloBranchData::SetMonteCarloObjectives(const std::vector<OccupancyGridMap> &Maps, const POS_2D &Destination)
{
	this->Reset();

	this->Destination = Destination;
	this->pOGMaps = &Maps;
}

void MonteCarloBranchData::ResetBranchDataToRoot(const MONTE_CARLO_TREE_CLASS &Tree)
{
	if(this->PrevBranches->empty())
	{
		// Set paths to zero
		this->PathTaken.clear();
		this->FirstPosAfterObservation = 0;

		// Calculate total map certainties
		this->MapCertainties.resize(this->pOGMaps->size(), 1.0f);
		this->MapCertaintiesNormalized.resize(this->pOGMaps->size(), 1.0f/(this->pOGMaps->size()));

		// Recalculate total map
		this->CertaintyLogMap.ResizeMap(this->pOGMaps->at(0).GetWidth(), this->pOGMaps->at(0).GetHeight());
		this->UpdateTotalMap();

		// Recalculate D* Map
		this->DStarCostMap.ResetDestPos(this->CertaintyLogMap, this->Destination);

		// Reset number of visits to zero for all cells
		this->NumVisitsMap.ResetMap(this->pOGMaps->at(0).GetWidth(), this->pOGMaps->at(0).GetHeight(), 0);

		// Set current node to root node
		this->pCurNode = &(const_cast<MONTE_CARLO_TREE_CLASS &>(Tree).GetRoot());
	}
	else
	{
		*this = std::move(this->PrevBranches->at(0));
		this->PrevBranches->clear();
	}
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

void MonteCarloBranchData::UpdateCostMap()
{
	// Update and clear update pos vector
	this->DStarCostMap.UpdateDStarMaps(this->CertaintyLogMap, this->PosToUpdate, this->GetBotPos(), 1, 0);
	this->PosToUpdate.clear();
}

void MonteCarloBranchData::UpdateMapCertainties(const float PrevOccupancyProb, const bool PosOccupied)
{
	float totalCertainty = 0;
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

void MonteCarloBranchData::UpdateTotalMap()
{
	POS_2D curPos;
	for(curPos.X = 0; curPos.X < this->CertaintyLogMap.GetWidth(); ++curPos.X)
	{
		for(curPos.Y = 0; curPos.Y < this->CertaintyLogMap.GetHeight(); ++curPos.Y)
		{
			auto &r_CurVal = this->CertaintyLogMap.GetPixelR(curPos);
			if(r_CurVal > OGM_LOG_MIN && r_CurVal < OGM_LOG_MAX)
			{
				const auto prevVal = r_CurVal;

				r_CurVal = 0;
				for(size_t curMapID = 0; curMapID < this->pOGMaps->size(); ++curMapID)
				{
					r_CurVal += this->MapCertaintiesNormalized.at(curMapID)*this->pOGMaps->at(curMapID).GetPixel(curPos);
				}

				r_CurVal = OccupancyGridMap::CalculateCertaintyLogFromProb(r_CurVal/OGM_CELL_MAX);

				if(prevVal != r_CurVal)
					this->PosToUpdate.push_back(curPos);
			}
		}
	}
}

void MonteCarloBranchData::UpdateBranchData(const MONTE_CARLO_NODE &NextNode)
{
	// Save current branch
	this->PrevBranches->push_back(*this);

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
		this->PosToUpdate.push_back(nextData.NewCell);

		// If this is an observation result, update log data
		if(nextData.Action.IsCellFree())
		{
			// At free cell, set log map position to zero
			auto &curLogCert = this->CertaintyLogMap.GetPixelR(nextData.NewCell);

			this->UpdateMapCertainties(OccupancyGridMap::CalculateCertaintyProbFromLog(curLogCert), 0);

			curLogCert = OGM_LOG_MIN;
		}
		else
		{
			// At occpupied cell, set log map position to infinity
			auto &curLogCert = this->CertaintyLogMap.GetPixelR(nextData.NewCell);

			this->UpdateMapCertainties(OccupancyGridMap::CalculateCertaintyProbFromLog(curLogCert), 1);

			curLogCert = OGM_LOG_MAX;
		}

		if(this->pOGMaps->size() > 1)
			this->UpdateTotalMap();

		//this->DStarCostMap.ResetDestPos(this->LogMap, this->Destination);
	}
}

void MonteCarloBranchData::RevertBranchData(const MONTE_CARLO_NODE &PrevNode)
{
	// Check that given node is parent
	if(&PrevNode != this->pCurNode->GetParent())
		return;

	// Move up one node
	*this = std::move(this->PrevBranches->back());

	// Remove last element
	this->PrevBranches->pop_back();
}
