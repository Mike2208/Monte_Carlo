#include "district_map_storage.h"

template<class T>
void DistrictMapStorage::ResetDistricts(const Map2D<T> &OriginalMap)
{
	// Reset all data
	this->_TotalGraph.Init();
	this->_TotalMap.ResetMap(OriginalMap.GetWidth(), OriginalMap.GetHeight(), 0);

	this->_DistrictStorage.clear();
	this->_DistrictTree.GetRoot().Reset();

	DistrictMap tmpMap;
	tmpMap.SetDistrictMap(this->_TotalMap, POS_2D(0,0));
	tmpMap.SetID(0);			// This is first map
	tmpMap.SetParentID(DISTRICT_MAP_DEFINITIONS::INVALID_DISTRICT_ID);		// No parent ID

	this->_DistrictStorage.push_back(std::move(tmpMap));			// Move tmpMap to storage
}

template<class T>
int DistrictMapStorage::DivideMap(const DISTRICT_ID &DistrictToDivide, const Map2D<T> &OriginalMap, const T &CutOffValue)
{
	// Check that map exists
	if(DistrictToDivide >= this->_DistrictStorage.size())
		return -1;

	// Make access easier
	const DistrictMap &oldDistrict = this->_DistrictStorage.at(DistrictToDivide);
}
