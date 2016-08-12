#include "district_map_storage.h"
#include "algorithm_voronoi_fields.h"

const DistrictMap &DistrictMapStorage::GetDistrict(const DISTRICT_ID &DistrictID) const
{
	return this->_DistrictStorage.at(DistrictID);
}

void DistrictMapStorage::GetDistrictAtGlobalPos(const POS_2D &Position, const DistrictMap **District) const
{
	const DISTRICT_ID &IDAtPos = this->GetDistrictIDAtGlobalPos(Position);
	if(IDAtPos < this->_DistrictStorage.size())
		*District = &(this->_DistrictStorage.at(IDAtPos));
}

DistrictMapStorage::DISTRICT_ID DistrictMapStorage::GetDistrictIDAtGlobalPos(const POS_2D &Position) const
{
	return this->_IDMap.GetPixel(Position);
}
