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
