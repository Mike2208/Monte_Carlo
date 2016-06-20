#include "district_map.h"

void DistrictMap::SetDistrictMap(const DISTRICT_MAP_TYPE &NewMapData, const POS_2D &NewMapPos)
{
	*(static_cast<DISTRICT_MAP_TYPE *>(this)) = NewMapData;
	this->_GlobalMapPosition = NewMapPos;
}

void DistrictMap::SetID(const DISTRICT_ID &NewID)
{
	this->_ID = NewID;
}

void DistrictMap::SetParentID(const DISTRICT_ID &ParentID)
{
	this->_ParentID = ParentID;
}
