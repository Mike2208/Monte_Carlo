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
	tmpMap = this->_TotalMap;
}
