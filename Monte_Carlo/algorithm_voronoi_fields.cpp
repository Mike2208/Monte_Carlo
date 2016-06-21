#include "algorithm_voronoi_fields.h"

void ALGORITHM_VORONOI_FIELDS::DISTRICT_SIZE::ComparePos(const POS_2D &Pos)
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

ALGORITHM_VORONOI_FIELDS::DISTRICT_SIZE *ALGORITHM_VORONOI_FIELDS::DISTRICT_SIZE_VECTOR::FindID(const ID &IDToFind)
{
	for(auto &curDS : this->Sizes)
	{
		if(IDToFind == curDS.DistrictID)
		{
			return &curDS;
		}
	}

	return nullptr;
}

typedef float T;

#include <queue>
#include "robot_navigation.h"
template<>
void AlgorithmVoronoiFields<T>::CalculateVoronoiField(const Map2D<T> &OriginalMap, const DistrictMap &OriginalDistrictData, ALGORITHM_VORONOI_FIELDS::DISTRICT_STORAGE &OccupiedDistricts, ALGORITHM_VORONOI_FIELDS::DISTRICT_STORAGE &FreeDistricts)
{
	const T cutOffValue = AlgorithmVoronoiFields<T>::CalculateMapAverage(OriginalMap, OriginalDistrictData);

	return AlgorithmVoronoiFields<T>::CalculateVoronoiField(OriginalMap, cutOffValue, OriginalDistrictData, OccupiedDistricts, FreeDistricts);
}


template<>
void AlgorithmVoronoiFields<T>::CalculateVoronoiField(const Map2D<T> &OriginalMap, const T &CutOffValue, const DistrictMap &OriginalDistrictData, ALGORITHM_VORONOI_FIELDS::DISTRICT_STORAGE &OccupiedDistricts, ALGORITHM_VORONOI_FIELDS::DISTRICT_STORAGE &FreeDistricts)
{
	FreeDistricts.clear();
	OccupiedDistricts.clear();

	// Step 1: Separate map into districts


	// Step 2.1: Find shortest distances between free districts

	// Step 2.2: Find shortest distances between occupied districts
}

template<>
T AlgorithmVoronoiFields<T>::CalculateMapAverage(const Map2D<T> &OriginalMap, const DistrictMap &OriginalDistrictData)
{
	T tmpVal = 0;
	unsigned int numElements = 0;

	// Go through district map
	POS_2D curPos;
	for(curPos.Y = 0; curPos.Y < OriginalDistrictData.GetHeight(); ++curPos.Y)
	{
		for(curPos.X = 0; curPos.X < OriginalDistrictData.GetWidth(); ++curPos.X)
		{
			// Only use elements in map that are in district
			if(OriginalDistrictData.GetPixel(curPos) == DISTRICT_MAP::IN_DISTRICT)
			{
				tmpVal += OriginalMap.GetPixel(OriginalDistrictData.ConvertToGlobalPosition(curPos));
				numElements++;
			}
		}
	}

	return tmpVal/numElements;
}

template<>
void AlgorithmVoronoiFields<T>::SeparateMapIntoUnconnectedDistricts(const Map2D<T> &OriginalMap, const DistrictMap &OriginalDistrictData, const T &CutOffValue, ALGORITHM_VORONOI_FIELDS::DISTRICT_STORAGE &OccupiedDistricts, ALGORITHM_VORONOI_FIELDS::DISTRICT_STORAGE &FreeDistricts, ALGORITHM_VORONOI_FIELDS::ID_MAP &IDMap, ALGORITHM_VORONOI_FIELDS::OCCUPATION_MAP & OccupationMap)
{
	IDMap.ResetMap(OriginalDistrictData.GetWidth(), OriginalDistrictData.GetHeight(), ALGORITHM_VORONOI_FIELDS::INVALID_ID);
	OccupationMap.ResetMap(OriginalDistrictData.GetWidth(), OriginalDistrictData.GetHeight(), ALGORITHM_VORONOI_FIELDS::CELL_OCCUPIED);

	ALGORITHM_VORONOI_FIELDS::ID nextFreeID = 0;

	// Vector to store district sizes
	ALGORITHM_VORONOI_FIELDS::DISTRICT_SIZE_VECTOR districtSizes;

	POS_2D curLocalPos;
	for(curLocalPos.Y = 0; curLocalPos.Y < OriginalDistrictData.GetHeight(); ++curLocalPos.Y)
	{
		for(curLocalPos.X = 0; curLocalPos.X < OriginalDistrictData.GetWidth(); ++curLocalPos.X)
		{
			// Only look at positions in district
			if(OriginalDistrictData.GetPixel(curLocalPos) == DISTRICT_MAP::IN_DISTRICT)
			{
				// Convert local to global map pos
				const POS_2D globalPos = OriginalDistrictData.ConvertToGlobalPosition(curLocalPos);

				ALGORITHM_VORONOI_FIELDS::ID cellID =  IDMap.GetPixelR(curLocalPos);

				// Check if current cell is occupied or free
				bool &occupationCell = OccupationMap.GetPixelR(curLocalPos);
				if(OriginalMap.GetPixel(globalPos) > CutOffValue)
				{
					occupationCell = ALGORITHM_VORONOI_FIELDS::CELL_OCCUPIED;

					if(cellID == ALGORITHM_VORONOI_FIELDS::INVALID_ID)
					{
						OccupiedDistricts.push_back(DistrictMap(nextFreeID, curLocalPos, 0, 0, !DISTRICT_MAP::IN_DISTRICT));

						// Create new district size
						districtSizes.Sizes.push_back(ALGORITHM_VORONOI_FIELDS::DISTRICT_SIZE(nextFreeID, curLocalPos));

						nextFreeID++;
					}
				}
				else
				{
					occupationCell = !ALGORITHM_VORONOI_FIELDS::CELL_OCCUPIED;

					if(cellID == ALGORITHM_VORONOI_FIELDS::INVALID_ID)
					{
						FreeDistricts.push_back(DistrictMap(nextFreeID, curLocalPos, 0, 0, !DISTRICT_MAP::IN_DISTRICT));

						// Create new district size
						districtSizes.Sizes.push_back(ALGORITHM_VORONOI_FIELDS::DISTRICT_SIZE(nextFreeID, curLocalPos));

						nextFreeID++;
					}
				}

				ALGORITHM_VORONOI_FIELDS::DISTRICT_SIZE *pCurDistrict = districtSizes.FindID(cellID);
				pCurDistrict->ComparePos(curLocalPos);

				// Check if a surrounding cell that has same occupation has already been set to an ID
				for(const auto &curNavOption : NavigationOptions)
				{
					const POS_2D curAdjacentPos = curLocalPos+curNavOption;

					// Check that pos is in district and get adjacent ID
					//bool inDistrict;
					//if(OriginalDistrictData.GetPixel(curAdjacentPos, inDistrict) < 0 || inDistrict != DISTRICT_MAP::IN_DISTRICT)
					//	continue;		// not in district, ignore
					ALGORITHM_VORONOI_FIELDS::ID adjacentID;
					if(IDMap.GetPixel(curAdjacentPos, adjacentID) || adjacentID != cellID)
						continue;		// not yet checked or not in district, ignore

					// Combine both districts
					AlgorithmVoronoiFields<T>::CombineTwoIDs(cellID, adjacentID, curLocalPos, IDMap, *pCurDistrict);
				}
			}
		}
	}

	// Expand districts to correct size
	for(auto &curDistrict : OccupiedDistricts)
		AlgorithmVoronoiFields<T>::ExpandDistrict(IDMap, *districtSizes.FindID(curDistrict.GetID()), curDistrict);
	for(auto &curDistrict : FreeDistricts)
		AlgorithmVoronoiFields<T>::ExpandDistrict(IDMap, *districtSizes.FindID(curDistrict.GetID()), curDistrict);
}

template<>
void AlgorithmVoronoiFields<T>::CombineTwoIDs(const ALGORITHM_VORONOI_FIELDS::ID &OriginalID, const ALGORITHM_VORONOI_FIELDS::ID &IDToCombine, const POS_2D &ConnectionPos, ALGORITHM_VORONOI_FIELDS::ID_MAP &IDMap, ALGORITHM_VORONOI_FIELDS::DISTRICT_SIZE &DistrictSize)
{
	std::queue<POS_2D> posToCheck;

	posToCheck.push(ConnectionPos);
	do
	{
		const POS_2D &curPos = posToCheck.front();

		// Set To new ID
		IDMap.SetPixel(curPos, OriginalID);

		// Enlarge district map if necessary
		DistrictSize.ComparePos(curPos);

		// Check adjacent position
		for(const auto &navOption : NavigationOptions)
		{
			const POS_2D curAdjacentPos = curPos + navOption;

			// Check if adjacent position is in map and has correct ID
			ALGORITHM_VORONOI_FIELDS::ID adjacentID;
			if(IDMap.GetPixel(curAdjacentPos, adjacentID) < 0 || adjacentID != IDToCombine)
				continue;		// Skip if not in map or not correct ID

			posToCheck.push(curAdjacentPos);
		}

		// Move to next position
		posToCheck.pop();
	}
	while(posToCheck.size() > 0);
}

template<>
void AlgorithmVoronoiFields<T>::ExpandDistrict(const ALGORITHM_VORONOI_FIELDS::ID_MAP &IDMap, const ALGORITHM_VORONOI_FIELDS::DISTRICT_SIZE &DistrictSize, DistrictMap &CurDistrict)
{
	// Set map to correct size
	CurDistrict.ResizeMap(DistrictSize.MaxPos.X-DistrictSize.MinPos.X+1, DistrictSize.MaxPos.Y-DistrictSize.MinPos.Y+1);

	// Set global position
	CurDistrict.SetGlobalMapPosition(DistrictSize.MinPos);

	// Go through all positions of map and set them
	POS_2D localPos;
	for(localPos.X = 0; localPos.X < CurDistrict.GetWidth(); ++localPos.X)
	{
		for(localPos.Y = 0; localPos.Y < CurDistrict.GetHeight(); ++localPos.Y)
		{
			const POS_2D globalPos = CurDistrict.ConvertToGlobalPosition(localPos);
			if(IDMap.GetPixel(globalPos) == CurDistrict.GetID())
				CurDistrict.SetPixel(localPos, DISTRICT_MAP::IN_DISTRICT);
			else
				CurDistrict.SetPixel(localPos, !DISTRICT_MAP::IN_DISTRICT);
		}
	}
}
