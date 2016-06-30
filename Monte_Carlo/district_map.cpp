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

void DistrictMap::SetDistrictMap(const DISTRICT_MAP_TYPE &NewMapData, const POS_2D &NewMapPos)
{
	*(static_cast<DISTRICT_MAP_TYPE *>(this)) = NewMapData;
	this->_GlobalMapPosition = NewMapPos;
}

void DistrictMap::SetID(const DISTRICT_ID &NewID)
{
	this->_ID = NewID;
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
			if(IDMap.GetPixel(adjacentPos, adjacentID) < 0 || adjacentID != idToUse)
				continue;		// Skip if not correct ID

			// Add to vector and correct size
			positionsInDistrict.push_back(adjacentPos);
			size.ComparePos(adjacentPos);
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
