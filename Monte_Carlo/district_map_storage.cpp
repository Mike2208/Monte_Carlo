#include "district_map_storage.h"
#include "algorithm_voronoi_fields.h"

template<class T>
void DistrictMapStorage::ResetDistricts(const Map2D<T> &OriginalMap)
{
	// Reset all data
	this->_DistrictStorage.clear();
	this->_DistrictTree.GetRoot().Reset();

	DistrictMap tmpMap(0, POS_2D(0,0), OriginalMap.GetWidth(), OriginalMap.GetHeight(), DISTRICT_MAP::IN_DISTRICT);
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

	// Perform Division
	ALGORITHM_VORONOI_FIELDS::DISTRICT_STORAGE occupiedDistricts, freeDistricts;
	AlgorithmVoronoiFields<T>::CalculateVoronoiField(OriginalMap, oldDistrict, CutOffValue, occupiedDistricts, freeDistricts);

}
