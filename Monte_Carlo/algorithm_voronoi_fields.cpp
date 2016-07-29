#include "algorithm_voronoi_fields.h"

bool ALGORITHM_VORONOI_FIELDS::DISTRICT_CHANGE_VECTOR::AreChangedIDs(const ID &DistrictID1, const ID &DistrictID2) const
{
	for(const auto &curChange : *this)
	{
		if((curChange.OldID == DistrictID1 && curChange.NewID == DistrictID2) ||
				(curChange.OldID == DistrictID2 && curChange.NewID == DistrictID1))
			return true;
	}

	return false;
}

ALGORITHM_VORONOI_FIELDS::SHORTEST_DIST_POS ALGORITHM_VORONOI_FIELDS::SKEL_MAP_SHORTEST_DIST_POS::ConvertToShortestDistPos(const ID_MAP &IDMap, const DIST_MAP &DistMap) const
{
	ID curID = IDMap.GetPixel(this->Position);			// The two connected districts
	ID adjacentID;

	// Find second position
	for(const auto navOption : NavigationOptions)
	{
		const POS_2D adjacentPos = this->Position+navOption;

		if(IDMap.GetPixel(adjacentPos, adjacentID) < 0)
			continue; // Skip if not in map

		if(adjacentID == curID)
			continue; // Same district, skip

		const DIST_MAP::CELL_TYPE adjacentDist = DistMap.GetPixel(adjacentPos);
		const MOVE_DIST_TYPE moveCost = GetMovementCost(this->Position, adjacentPos);
		if(adjacentDist <= this->Distance+moveCost && adjacentDist+moveCost >= this->Distance)
		{
			// Found adjacent position, create shortestDistPos
			return ALGORITHM_VORONOI_FIELDS::SHORTEST_DIST_POS(curID, adjacentID, this->Distance, this->Position, adjacentPos);
		}
	}

	// Return error value (shouldn't happen)
	return ALGORITHM_VORONOI_FIELDS::SHORTEST_DIST_POS(curID, curID, this->Distance, this->Position, this->Position);
}

ALGORITHM_VORONOI_FIELDS::SKEL_MAP_DATA::SKEL_MAP_DATA(const DIST_MAP::CELL_TYPE &_DistToPrevElement, const SKEL_MAP_SHORTEST_DIST_POS_ID &_PrevElementID) : DistToPrevElement(_DistToPrevElement), PrevElementID(_PrevElementID)
{
}

ALGORITHM_VORONOI_FIELDS::SKEL_MAP_POS_DATA::SKEL_MAP_POS_DATA(const POS_2D &_Pos, const SKEL_MAP_DATA &_SkelMapData) : POS_2D(_Pos), SKEL_MAP_DATA(_SkelMapData)
{
}

ALGORITHM_VORONOI_FIELDS::SKEL_MAP_POS_DATA::SKEL_MAP_POS_DATA(const POS_2D &_Pos, const DIST_MAP::CELL_TYPE &_DistToPrevElement, const SKEL_MAP_SHORTEST_DIST_POS_ID &_PrevElementID) : POS_2D(_Pos), SKEL_MAP_DATA(_DistToPrevElement, _PrevElementID)
{
}

void ALGORITHM_VORONOI_FIELDS::SKEL_MAP_SHORTEST_DIST_POS_VECTOR::push_back(const value_type &element)
{
	// If a previous element is listed, add this to list of next elements
	if(element.PrevElementID != ALGORITHM_VORONOI_FIELDS::SKEL_MAP_SHORTEST_DIST_POS_INVALID_ID)
	{
		const size_type newElementID = this->size();
		this->at(element.PrevElementID).NextElementIDs.push_back(newElementID);
	}

	// Add element regularly
	static_cast<std::vector<SKEL_MAP_SHORTEST_DIST_POS>*>(this)->push_back(element);
}

void ALGORITHM_VORONOI_FIELDS::SKEL_MAP_SHORTEST_DIST_POS_VECTOR::push_back(value_type &&element)
{
	// If a previous element is listed, add this to list of next elements
	if(element.PrevElementID != ALGORITHM_VORONOI_FIELDS::SKEL_MAP_SHORTEST_DIST_POS_INVALID_ID)
	{
		const size_type newElementID = this->size();
		this->at(element.PrevElementID).NextElementIDs.push_back(newElementID);
	}

	// Add element regularly
	static_cast<std::vector<SKEL_MAP_SHORTEST_DIST_POS>*>(this)->push_back(element);
}

ALGORITHM_VORONOI_FIELDS::SKEL_MAP_SHORTEST_DIST_POS *ALGORITHM_VORONOI_FIELDS::SKEL_MAP_SHORTEST_DIST_POS_VECTOR::FindClosestDist(const SKEL_MAP_SHORTEST_DIST_POS &CurDist, ALGORITHM_VORONOI_FIELDS::DIST_MAP::CELL_TYPE &Distance) const
{
	ALGORITHM_VORONOI_FIELDS::DIST_MAP::CELL_TYPE backDist = 0;
	const ALGORITHM_VORONOI_FIELDS::SKEL_MAP_SHORTEST_DIST_POS *pBackDist = nullptr;
	if(CurDist.PrevElementID != ALGORITHM_VORONOI_FIELDS::SKEL_MAP_SHORTEST_DIST_POS_INVALID_ID)
	{
		// Continue backtracking until a valid position is found
		pBackDist = &CurDist;

		do
		{
			backDist += pBackDist->DistToPrevElement;
			pBackDist = &(this->at(pBackDist->PrevElementID));
		}
		while(pBackDist->PrevElementID != ALGORITHM_VORONOI_FIELDS::SKEL_MAP_SHORTEST_DIST_POS_INVALID_ID
			  && !pBackDist->ValidElement);

		if(pBackDist == nullptr)
			backDist = ALGORITHM_VORONOI_FIELDS::MAX_DIST;
	}
	else
	{
		backDist = ALGORITHM_VORONOI_FIELDS::MAX_DIST;
	}

	ALGORITHM_VORONOI_FIELDS::DIST_MAP::CELL_TYPE frontDist;
	const ALGORITHM_VORONOI_FIELDS::SKEL_MAP_SHORTEST_DIST_POS *pFrontDist = this->FindClosestDistForward(CurDist, frontDist);
	if(pFrontDist == nullptr)
		frontDist = ALGORITHM_VORONOI_FIELDS::MAX_DIST;		// Resize to prevent errors if no front distance was found

	// If no other pos was found, just return nothing
	if(pBackDist == nullptr && pFrontDist == nullptr)
	{
		Distance = ALGORITHM_VORONOI_FIELDS::MAX_DIST;
		return nullptr;
	}

	// Return smaller distance
	if(backDist <= frontDist)
	{
		Distance = backDist;
		return const_cast<ALGORITHM_VORONOI_FIELDS::SKEL_MAP_SHORTEST_DIST_POS *>(pBackDist);
	}

	Distance = frontDist;
	return const_cast<ALGORITHM_VORONOI_FIELDS::SKEL_MAP_SHORTEST_DIST_POS *>(pFrontDist);
}

ALGORITHM_VORONOI_FIELDS::SKEL_MAP_SHORTEST_DIST_POS *ALGORITHM_VORONOI_FIELDS::SKEL_MAP_SHORTEST_DIST_POS_VECTOR::FindClosestDistForward(const SKEL_MAP_SHORTEST_DIST_POS &CurDist, ALGORITHM_VORONOI_FIELDS::DIST_MAP::CELL_TYPE &Distance) const
{
	ALGORITHM_VORONOI_FIELDS::DIST_MAP::CELL_TYPE bestDist = ALGORITHM_VORONOI_FIELDS::MAX_DIST;
	const ALGORITHM_VORONOI_FIELDS::SKEL_MAP_SHORTEST_DIST_POS *pBestDist = nullptr;

	if(CurDist.NextElementIDs.size() == 0)
	{
		Distance = ALGORITHM_VORONOI_FIELDS::MAX_DIST;
		return nullptr;
	}

	// Go through all following distances
	for(const auto &curForwardDistID : CurDist.NextElementIDs)
	{
		const ALGORITHM_VORONOI_FIELDS::SKEL_MAP_SHORTEST_DIST_POS *pCurForwardDist = &(this->at(curForwardDistID));

		// If this element was not erased
		if(pCurForwardDist->ValidElement)
		{
			// Check if this is best position
			if(pCurForwardDist->DistToPrevElement <= bestDist)
			{
				bestDist = pCurForwardDist->DistToPrevElement;
				pBestDist = pCurForwardDist;
			}
		}
		else
		{
			// Recursvely calculate shortest distance if this element was erased
			ALGORITHM_VORONOI_FIELDS::DIST_MAP::CELL_TYPE curDist = 0;
			const ALGORITHM_VORONOI_FIELDS::SKEL_MAP_SHORTEST_DIST_POS *pClosestDist = this->FindClosestDistForward(*pCurForwardDist, curDist);
			curDist += pCurForwardDist->DistToPrevElement;

			// Check if this is best position
			if(curDist <= bestDist && pClosestDist != nullptr)
			{
				bestDist = curDist;
				pBestDist = pClosestDist;
			}
		}
	}

	// Return results
	Distance = bestDist;
	return const_cast<ALGORITHM_VORONOI_FIELDS::SKEL_MAP_SHORTEST_DIST_POS *>(pBestDist);
}

DistrictMap *ALGORITHM_VORONOI_FIELDS::DISTRICT_STORAGE::FindDistrictID(const ID &DistrictID)
{
	for(DISTRICT_STORAGE::size_type i=0; i<this->size(); ++i)
	{
		if(this->at(i).GetID() == DistrictID)
			return &(this->at(i));
	}

	return nullptr;
}

ALGORITHM_VORONOI_FIELDS::DISTRICT_STORAGE::size_type ALGORITHM_VORONOI_FIELDS::DISTRICT_STORAGE::FindDistrictIDIterator(const ID &DistrictID)
{
	for(DISTRICT_STORAGE::size_type i=0; i < this->size(); ++i)
	{
		if(this->at(i).GetID() == DistrictID)
			return i;
	}

	return this->size();
}

ALGORITHM_VORONOI_FIELDS::SHORTEST_DIST_POS *ALGORITHM_VORONOI_FIELDS::SHORTEST_DIST_POS_VECTOR::FindID(const ID &ID1ToFind, const ID &ID2ToFind)
{
	for(auto &curSDP : this->Poses)
	{
		if((ID1ToFind == curSDP.DistrictID1 && ID2ToFind == curSDP.DistrictID2) ||
			(ID1ToFind == curSDP.DistrictID2 && ID2ToFind == curSDP.DistrictID1))
		{
			return &curSDP;
		}
	}

	return nullptr;
}

template class AlgorithmVoronoiFields<float>;
