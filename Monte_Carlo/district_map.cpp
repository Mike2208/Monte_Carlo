#include "district_map.h"

#include "robot_navigation.h"
#include <queue>

void DISTRICT_MAP::DISTRICT_SIZE::ComparePos(const POS_2D &Pos)
{
	if(Pos.X < this->MinPos.X)
		MinPos.X = Pos.X;
	else if(Pos.X > this->MaxPos.X)
		MaxPos.X = Pos.X;

	if(Pos.Y < this->MinPos.Y)
		MinPos.Y = Pos.Y;
	else if(Pos.Y > this->MaxPos.Y)
		MaxPos.Y = Pos.Y;
}

POS_2D_TYPE DISTRICT_MAP::DISTRICT_SIZE::GetWidth() const
{
	return this->MaxPos.X-this->MinPos.X+1;
}

POS_2D_TYPE DISTRICT_MAP::DISTRICT_SIZE::GetHeight() const
{
	return this->MaxPos.Y - this->MinPos.Y + 1;
}

DISTRICT_MAP::DISTRICT_SIZE *DISTRICT_MAP::DISTRICT_SIZE_VECTOR::FindID(const ID &IDToFind)
{
	for(auto &curDS : *this)
	{
		if(IDToFind == curDS.DistrictID)
		{
			return &curDS;
		}
	}

	return nullptr;
}

void DISTRICT_MAP::ADJACENT_DISTRICT::AddPosition(const POS_2D &OriginalDistrictPos, const POS_2D &AdjacentDistrictPos)
{
	// Check if position already exists
	for(const auto &curConnection : this->ConnectionPositions)
	{
		if(curConnection[0] == OriginalDistrictPos && curConnection[1] == AdjacentDistrictPos)
			return;		// stop if already saved
	}

	// Add to vector
	this->ConnectionPositions.push_back({{OriginalDistrictPos, AdjacentDistrictPos}});
}

bool DISTRICT_MAP::ADJACENT_DISTRICT::IsConnectedToAdjacentDistrict(const POS_2D &ConnectionPos) const
{
	for(const auto &curConnection : this->ConnectionPositions)
	{
		if(curConnection[0] == ConnectionPos)
			return true;
	}

	return false;
}

void DISTRICT_MAP::ADJACENT_DISTRICT_VECTOR::AddConnection(const ID &AdjacentID, const POS_2D &OriginalDistrictPos, const POS_2D &AdjacentDistrictPos)
{
	// Check if district is already stored
	for(auto &adjacentDistrict : this->AdjacentDistricts)
	{
		// Add to district
		if(adjacentDistrict.AdjacentDistrictID == AdjacentID)
		{
			adjacentDistrict.AddPosition(OriginalDistrictPos, AdjacentDistrictPos);
			return;
		}
	}

	// Create and add new connection
	DISTRICT_MAP::ADJACENT_DISTRICT newDistrictConnection(AdjacentID);
	newDistrictConnection.AddPosition(OriginalDistrictPos, AdjacentDistrictPos);
	this->AdjacentDistricts.push_back(std::move(newDistrictConnection));
}

void DISTRICT_MAP::ADJACENT_DISTRICT_VECTOR::GetConnectionData(const ID &AdjacentID, CONNECTED_POS_VECTOR &Connections) const
{
	// Check if district is already stored
	for(auto &adjacentDistrict : this->AdjacentDistricts)
	{
		// Copy district data
		if(adjacentDistrict.AdjacentDistrictID == AdjacentID)
		{
			Connections = adjacentDistrict.ConnectionPositions;
			return;
		}
	}

	// Clear connections if none are present
	Connections.clear();
}

std::vector<DISTRICT_MAP::DISTRICT_SIZE>::size_type DISTRICT_MAP::DISTRICT_SIZE_VECTOR::FindIDIterator(const ID &IDToFind)
{
	for(std::vector<DISTRICT_SIZE>::size_type i=0; i<this->size(); ++i)
	{
		if(IDToFind == this->at(i).DistrictID)
			return i;
	}

	return this->size();
}

bool DISTRICT_MAP::ID_CONNECTION_VECTOR::IsConnected(const ID DistrictID1, const ID DistrictID2) const
{
	// Check if the IDs are connected
	for(const auto &curConnection : *this)
	{
		if((curConnection.IDs[0] == DistrictID1 && curConnection.IDs[1] == DistrictID2) ||
				(curConnection.IDs[0] == DistrictID2 && curConnection.IDs[1] == DistrictID1))
			return true;
	}

	return false;
}

void DISTRICT_MAP::ID_CONNECTION_VECTOR::GetAllConnectedIDs(const ID DistrictID, std::vector<ID> &ConnectedIDs) const
{
	ConnectedIDs.clear();

	// Check which the IDs are connected
	for(const auto &curConnection : *this)
	{
		if(curConnection.IDs[0] == DistrictID)
			ConnectedIDs.push_back(curConnection.IDs[1]);
		else if(curConnection.IDs[1] == DistrictID)
			ConnectedIDs.push_back(curConnection.IDs[0]);
	}
}

void DISTRICT_MAP::ID_CONNECTION_VECTOR::AddConnection(const ID DistrictID1, const ID DistrictID2, const POS_2D &ConnectionPos1, const POS_2D &ConnectionPos2)
{
	if(!this->IsConnected(DistrictID1, DistrictID2))
		this->push_back(ID_CONNECTION(DistrictID1, DistrictID2, ConnectionPos1, ConnectionPos2));
}

void DistrictMap::SetDistrictMap(const DISTRICT_MAP_TYPE &NewMapData, const POS_2D &NewMapPos)
{
	*(static_cast<DISTRICT_MAP_TYPE *>(this)) = NewMapData;
	this->_GlobalMapPosition = NewMapPos;
}

void DistrictMap::SetID(const DISTRICT_ID &NewID)
{
	this->_ID = NewID;
}

void DistrictMap::ChangeID(const DISTRICT_ID &NewID, DISTRICT_MAP::ID_MAP &GlobalIDMap)
{
	// Nothing to do if IDs are the same
	if(NewID == this->_ID)
		return;

	// Change all IDs in IDMap, starting at LocalPositionInDistrict
	POS_2D localPos;
	for(localPos.X = 0; localPos.X < this->GetWidth(); ++localPos.X)
	{
		for(localPos.Y = 0; localPos.Y < this->GetHeight(); ++localPos.Y)
		{
			// Convert all cells to new ID
			if(this->GetPixel(localPos) == DISTRICT_MAP::IN_DISTRICT)
				GlobalIDMap.SetPixel(this->ConvertToGlobalPosition(localPos), NewID);
		}
	}

	this->_ID = NewID;
}

void DistrictMap::GetAllDistrictSizesOfIDMap(const DISTRICT_MAP::ID_MAP &IDMap, DISTRICT_MAP::DISTRICT_SIZE_VECTOR &DistrictSizes)
{
	DistrictSizes.clear();
	DISTRICT_MAP::DISTRICT_SIZE *pCurSize;

	POS_2D curPos;
	for(curPos.X = 0; curPos.X < IDMap.GetWidth(); ++curPos.X)
	{
		for(curPos.Y = 0; curPos.Y < IDMap.GetHeight(); ++curPos.Y)
		{
			const DISTRICT_ID curID = IDMap.GetPixel(curPos);
			if(curID == DISTRICT_MAP::INVALID_DISTRICT_ID)
				continue;		// Skip if invalid ID

			// Check if current ID is already recorded
			pCurSize = DistrictSizes.FindID(curID);
			if(pCurSize == nullptr)
				DistrictSizes.push_back(DISTRICT_MAP::DISTRICT_SIZE(curID, curPos));		// Create size if it doesn't exist
			else
				pCurSize->ComparePos(curPos);			// Check if curPos is already in size
		}
	}
}

bool DistrictMap::IsGlobalCellInDistrict(const POS_2D &GlobalPosition) const
{
	// Check cell
	DistrictMap::CELL_TYPE tmpVal;
	if(this->GetGlobalPixel(GlobalPosition, tmpVal) < 0 || tmpVal != DISTRICT_MAP::IN_DISTRICT)
		return false;

	return true;
}

void DistrictMap::SetDistrictFromIDMap(const DISTRICT_MAP::ID_MAP &IDMap, const POS_2D &PosOfOneID, DISTRICT_MAP::POS_IN_DISTRICT *GlobalPosInDistrict)
{
	// ID to use
	const DISTRICT_ID idToUse = IDMap.GetPixel(PosOfOneID);

	DISTRICT_MAP::DISTRICT_SIZE size(idToUse, PosOfOneID);

	// Add all district positions to vector
	DISTRICT_MAP::POS_IN_DISTRICT positionsInDistrict;
	positionsInDistrict.push_back(PosOfOneID);

	std::queue<POS_2D> posToCheck;
	posToCheck.push(PosOfOneID);
	do
	{
		const POS_2D &curPos = posToCheck.front();

		for(const auto &navOption : NavigationOptions)
		{
			const POS_2D adjacentPos = curPos+navOption;

			DISTRICT_ID adjacentID;
			if(IDMap.GetPixel(adjacentPos, adjacentID) < 0)
				continue;		// Skip if outside of map

			if(adjacentID != idToUse)
			{
				// Add to connections
				this->_AdjacentDistricts.AddConnection(adjacentID, curPos, adjacentPos);
			}
			else
			{
				// Add to vector and correct size
				positionsInDistrict.push_back(adjacentPos);
				size.ComparePos(adjacentPos);
			}
		}

		posToCheck.pop();
	}
	while(posToCheck.size() > 0);

	// Resize map and set correct global pos
	this->ResetMap(size.MaxPos.X-size.MinPos.X+1, size.MaxPos.Y-size.MinPos.Y+1, !DISTRICT_MAP::IN_DISTRICT);
	this->SetGlobalMapPosition(size.MinPos);

	// Add cells to district
	for(const auto &curPos : positionsInDistrict)
		this->SetGlobalPixel(curPos, DISTRICT_MAP::IN_DISTRICT);

	this->SetLocalPosInDistrict(this->ConvertToLocalPosition(PosOfOneID));

	// Set ID
	this->SetID(idToUse);

	// Save positions in district if requested
	if(GlobalPosInDistrict != nullptr)
		*GlobalPosInDistrict = std::move(positionsInDistrict);
}

void DistrictMap::SetFreeDistrict(const POS_2D_TYPE &Width, const POS_2D_TYPE &Height, const DISTRICT_ID &ID)
{
	this->ResetMap(Width, Height, DISTRICT_MAP::IN_DISTRICT);

	this->SetGlobalMapPosition(POS_2D(0,0));
	this->SetLocalPosInDistrict(POS_2D(0,0));

	this->SetID(ID);
}
