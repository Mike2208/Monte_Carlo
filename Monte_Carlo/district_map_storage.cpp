#include "district_map_storage.h"
#include "algorithm_voronoi_fields.h"

template<class T>
void DistrictMapStorage::ResetDistricts(const Map2D<T> &OriginalMap)
{
	// Reset all data
	this->_DistrictStorage.clear();
	this->_DistrictTree.GetRoot().Reset();
	this->_DistrictGraph.Init();
	this->_IDMap.ResizeMap(0,0);

	DistrictMap tmpMap(0, POS_2D(0,0), OriginalMap.GetWidth(), OriginalMap.GetHeight(), DISTRICT_MAP::IN_DISTRICT);
	this->_DistrictStorage.push_back(std::move(tmpMap));			// Move tmpMap to storage
}

template<class T>
int DistrictMapStorage::DivideMap(const DISTRICT_ID &DistrictToDivide, const Map2D<T> &OriginalMap, const T &CutOffValue)
{
	// Check that map exists
	if(DistrictToDivide >= this->_DistrictStorage.size())
		return -1;

	DISTRICT_ID nextFreeID = this->_DistrictStorage.size();

	// Make access easier
	const DistrictMap &oldDistrict = this->_DistrictStorage.at(DistrictToDivide);

	// Perform Division
	ALGORITHM_VORONOI_FIELDS::DISTRICT_STORAGE occupiedDistricts, freeDistricts;
	ALGORITHM_VORONOI_FIELDS::DISTRICT_STORAGE::size_type curOccupiedDistrictsPos, curFreeDistrictsPos;
	AlgorithmVoronoiFields<T>::CalculateVoronoiField(OriginalMap, oldDistrict, CutOffValue, occupiedDistricts, freeDistricts, nextFreeID, &this->_IDMap);

	// Add new districts to storage and try to change the district IDs as little as possible
	const DISTRICT_ID startOfNewDistrictID = this->_DistrictStorage.size();
	DISTRICT_ID curDistrictID = this->_DistrictStorage.size();
	curOccupiedDistrictsPos = 0;
	curFreeDistrictsPos = 0;

	// FOR NOW, JUST ADD THEM AS SIMPLE AS POSSIBLE
	while(curOccupiedDistrictsPos < occupiedDistricts.size() && curFreeDistrictsPos < freeDistricts.size())
	{
		// Check IDs and add the one with the correct ID if possible
		bool useFreeDistrictPos;
		if(occupiedDistricts.at(curOccupiedDistrictsPos).GetID() == curDistrictID)
			useFreeDistrictPos = false;
		else if(freeDistricts.at(curFreeDistrictsPos).GetID() == curDistrictID)
			useFreeDistrictPos = true;
		else
		{
			// If both don't have the correct ID, just add one of them and change it's ID (preferably the larger one)
			if(curFreeDistrictsPos < freeDistricts.size() && curOccupiedDistrictsPos < occupiedDistricts.size())
			{
				if(freeDistricts.at(curFreeDistrictsPos).GetID() < occupiedDistricts.at(curOccupiedDistrictsPos).GetID())
					useFreeDistrictPos = false;
				else
					useFreeDistrictPos = true;
			}
			else if(curFreeDistrictsPos < freeDistricts.size())
				useFreeDistrictPos = true;
			else
				useFreeDistrictPos = false;
		}

		// Add next district
		DistrictMap *addDistrict;
		if(useFreeDistrictPos)
		{
			addDistrict = &freeDistricts.at(curFreeDistrictsPos);
			curFreeDistrictsPos++;
		}
		else
		{
			addDistrict = &occupiedDistricts.at(curOccupiedDistrictsPos);
			curOccupiedDistrictsPos++;
		}

		// Change ID (function checks if it's necessary)
		addDistrict->ChangeID(curDistrictID, this->_IDMap);

		// Add district
		this->_DistrictStorage.push_back(std::move(*addDistrict));

		// Move to next district
		curDistrictID++;
	}

	// Recalculate all old districts next to divided district to include new adjacency data
	for(DISTRICT_ID curID = 0; curID < startOfNewDistrictID; ++curID)
	{
		DistrictMap curDistrict = this->_DistrictStorage.at(curID);
		if(curDistrict.IsConnectedToDistrict(DistrictToDivide))
			curDistrict.SetDistrictFromIDMap(this->_IDMap, curDistrict.ConvertToGlobalPosition(curDistrict.GetLocalPosInDistrict()));
	}

	// Now add new districts to tree
	DISTRICT_TREE::TREE_NODE &parentNode = this->_DistrictTree.GetStorage().at(this->_DistrictStorage.GetTreeID(DistrictToDivide));
	for(DISTRICT_ID curID = startOfNewDistrictID; curID < this->_DistrictStorage.size(); ++curID)
	{
		this->_DistrictStorage.SetTreeID(curID, parentNode.AddChild(DISTRICT_TREE::NODE_DATA(curID)));
	}

	return 1;
}
