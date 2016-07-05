#ifndef ALGORITHM_VORONOI_FIELDS_TEMPLATE_H
#define ALGORITHM_VORONOI_FIELDS_TEMPLATE_H

#include "algorithm_voronoi_fields.h"
#include "standard_definitions.h"
#include "robot_navigation.h"
#include "algorithm_d_star.h"

#include <queue>

template<class T>
void AlgorithmVoronoiFields<T>::CalculateVoronoiField(const Map2D<T> &OriginalMap, const DistrictMap &OriginalDistrictData, ALGORITHM_VORONOI_FIELDS::DISTRICT_STORAGE &OccupiedDistricts, ALGORITHM_VORONOI_FIELDS::DISTRICT_STORAGE &FreeDistricts, ALGORITHM_VORONOI_FIELDS::ID &NextFreeDistrictID)
{
	const T cutOffValue = AlgorithmVoronoiFields<T>::CalculateMapAverage(OriginalMap, OriginalDistrictData);

	return AlgorithmVoronoiFields<T>::CalculateVoronoiField(OriginalMap, cutOffValue, OriginalDistrictData, OccupiedDistricts, FreeDistricts, NextFreeDistrictID);
}


template<class T>
void AlgorithmVoronoiFields<T>::CalculateVoronoiField(const Map2D<T> &OriginalMap, const T &CutOffValue, const DistrictMap &OriginalDistrictData, ALGORITHM_VORONOI_FIELDS::DISTRICT_STORAGE &OccupiedDistricts, ALGORITHM_VORONOI_FIELDS::DISTRICT_STORAGE &FreeDistricts, ALGORITHM_VORONOI_FIELDS::ID &NextFreeDistrictID)
{
	FreeDistricts.clear();
	OccupiedDistricts.clear();

	ALGORITHM_VORONOI_FIELDS::ID_MAP idMap;
	ALGORITHM_VORONOI_FIELDS::OCCUPATION_MAP occupationMap;

#ifdef DEBUG	// DEBUG
	OriginalMap.PrintMap("/tmp/testOriginal.pgm", OGM_CELL_MAX, 0);
#endif			// ~DEBUG

	// Step 1: Separate map into districts
	AlgorithmVoronoiFields<T>::SeparateMapIntoUnconnectedDistricts(OriginalMap, OriginalDistrictData, CutOffValue, OccupiedDistricts, FreeDistricts, idMap, occupationMap, NextFreeDistrictID);

#ifdef DEBUG	// DEBUG
	idMap.PrintMap("/tmp/testID.pgm", NextFreeDistrictID-1, 0);
	occupationMap.PrintMap("/tmp/testOcc.pgb");
#endif			// ~DEBUG

	// Step 2.1: Find shortest distances between free districts
	AlgorithmVoronoiFields<T>::SeparateByShortestDistance(OriginalDistrictData, occupationMap, !ALGORITHM_VORONOI_FIELDS::CELL_OCCUPIED, FreeDistricts, OccupiedDistricts, NextFreeDistrictID, idMap);

	// Step 2.2: Find shortest distances between occupied districts
	AlgorithmVoronoiFields<T>::SeparateByShortestDistance(OriginalDistrictData, occupationMap, ALGORITHM_VORONOI_FIELDS::CELL_OCCUPIED, OccupiedDistricts, FreeDistricts, NextFreeDistrictID, idMap);
}

template<class T>
T AlgorithmVoronoiFields<T>::CalculateMapAverage(const Map2D<T> &OriginalMap, const DistrictMap &OriginalDistrictData)
{
	long double tmpVal = 0;
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

	if(std::numeric_limits<T>::is_integer)
		return static_cast<T>(round(tmpVal/numElements));		// round if integer
	else
		return static_cast<T>(tmpVal/numElements);
}

template<class T>
void AlgorithmVoronoiFields<T>::SeparateMapIntoUnconnectedDistricts(const Map2D<T> &OriginalMap, const DistrictMap &OriginalDistrictData, const T &CutOffValue, ALGORITHM_VORONOI_FIELDS::DISTRICT_STORAGE &OccupiedDistricts, ALGORITHM_VORONOI_FIELDS::DISTRICT_STORAGE &FreeDistricts, ALGORITHM_VORONOI_FIELDS::ID_MAP &IDMap, ALGORITHM_VORONOI_FIELDS::OCCUPATION_MAP &OccupationMap, ALGORITHM_VORONOI_FIELDS::ID &NextFreeID)
{
	IDMap.ResetMap(OriginalDistrictData.GetWidth(), OriginalDistrictData.GetHeight(), ALGORITHM_VORONOI_FIELDS::INVALID_ID);
	OccupationMap.ResetMap(OriginalDistrictData.GetWidth(), OriginalDistrictData.GetHeight(), !ALGORITHM_VORONOI_FIELDS::CELL_OCCUPIED);

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

				ALGORITHM_VORONOI_FIELDS::ID &r_cellID =  IDMap.GetPixelR(curLocalPos);
				if(r_cellID != ALGORITHM_VORONOI_FIELDS::INVALID_ID)
					continue;		// Skip if already in district


				// Check if current cell is supposed to be occupied or free
				ALGORITHM_VORONOI_FIELDS::OCCUPATION_MAP::CELL_TYPE occupationLevel = AlgorithmVoronoiFields<T>::IsOccupiedCell(OriginalMap.GetPixel(globalPos), CutOffValue);

				// Set Occupation Map
				OccupationMap.SetPixel(curLocalPos, occupationLevel);

				// Create new district
				r_cellID = NextFreeID;
				// Create new size and create new district in correct vector (occupied or unoccupied)
				districtSizes.push_back(ALGORITHM_VORONOI_FIELDS::DISTRICT_SIZE(r_cellID, curLocalPos));
				if(occupationLevel == ALGORITHM_VORONOI_FIELDS::CELL_OCCUPIED)
				{
					OccupiedDistricts.push_back(DistrictMap(r_cellID, curLocalPos, 0, 0, DISTRICT_MAP::IN_DISTRICT));
				}
				else
				{
					FreeDistricts.push_back(DistrictMap(r_cellID, curLocalPos, 0, 0, DISTRICT_MAP::IN_DISTRICT));
				}

				NextFreeID++;		// Move to next free ID

				// Check all surrounding positions and add them to this district if possible
				AlgorithmVoronoiFields<T>::CreateTotalDistrict(OriginalMap, OriginalDistrictData, globalPos, CutOffValue, occupationLevel, districtSizes.back(), IDMap, OccupationMap);
			}
		}
	}

	// Expand districts to correct size
	for(auto &curDistrict : OccupiedDistricts)
		AlgorithmVoronoiFields<T>::ExpandDistrict(IDMap, *districtSizes.FindID(curDistrict.GetID()), curDistrict);
	for(auto &curDistrict : FreeDistricts)
		AlgorithmVoronoiFields<T>::ExpandDistrict(IDMap, *districtSizes.FindID(curDistrict.GetID()), curDistrict);
}

template<class T>
int AlgorithmVoronoiFields<T>::SeparateByShortestDistance(const DistrictMap &OriginalDistrictMap, const ALGORITHM_VORONOI_FIELDS::OCCUPATION_MAP &OccupationMap, const ALGORITHM_VORONOI_FIELDS::OCCUPATION_MAP::CELL_TYPE &OccupationLevelOfDistrictsToDivide, const ALGORITHM_VORONOI_FIELDS::DISTRICT_STORAGE &OuterDistricts, ALGORITHM_VORONOI_FIELDS::DISTRICT_STORAGE &DistrictsToDivide, ALGORITHM_VORONOI_FIELDS::ID &NextFreeID, ALGORITHM_VORONOI_FIELDS::ID_MAP &IDMap)
{
	ALGORITHM_VORONOI_FIELDS::SHORTEST_DIST_POS_VECTOR shortestDistances;

	// Create map with all IDs and map with all distances
	ALGORITHM_VORONOI_FIELDS::ID_MAP districtIDMap;			// Map with IDs of closest district
	ALGORITHM_VORONOI_FIELDS::DIST_MAP skeletonDistMap;		// Map with distance to closest ID

	districtIDMap.ResetMap(OriginalDistrictMap.GetWidth(), OriginalDistrictMap.GetHeight(), ALGORITHM_VORONOI_FIELDS::INVALID_ID);
	skeletonDistMap.ResetMap(OriginalDistrictMap.GetWidth(), OriginalDistrictMap.GetHeight(), ALGORITHM_VORONOI_FIELDS::MAX_DIST);

	// Add all positions in district to queue
	std::queue<POS_2D> posToCheck;
	for(const auto &curDistrict : OuterDistricts)
	{
		const POS_2D globalPosInDistrict = curDistrict.GetGlobalPosInDistrict();

		// Set distance of this position to zero and idMap to correct ID
		skeletonDistMap.SetPixel(globalPosInDistrict, 0);
		districtIDMap.SetPixel(globalPosInDistrict, curDistrict.GetID());

		posToCheck.push(curDistrict.GetGlobalPosInDistrict());
	}

	do
	{
		const POS_2D &curPos = posToCheck.front();

		// Get distance and ID of this pos
		const ALGORITHM_VORONOI_FIELDS::DIST_MAP::CELL_TYPE &curDist = skeletonDistMap.GetPixel(curPos);
		const ALGORITHM_VORONOI_FIELDS::ID_MAP::CELL_TYPE &curID = districtIDMap.GetPixel(curPos);

		// Check all surrounding cells
		for(const auto &navOption : NavigationOptions)
		{
			// Get adjacent position
			const POS_2D adjacentPos = curPos+navOption;

			// Check if valid cell
			if(!OriginalDistrictMap.IsGlobalCellInDistrict(adjacentPos))
				continue;			// Skip if not in district

			// Get adjacent ID and district
			ALGORITHM_VORONOI_FIELDS::DIST_MAP::CELL_TYPE &r_adjacentDist = skeletonDistMap.GetPixelR(adjacentPos);
			ALGORITHM_VORONOI_FIELDS::ID_MAP::CELL_TYPE &r_adjacentID = districtIDMap.GetPixelR(adjacentPos);

			// Check if better dist is available
			if(r_adjacentDist > curDist+1)
			{
				// Check if still in same ID
				if(IDMap.GetPixel(adjacentPos) == curID)
				{
					// This position is still in district
					r_adjacentDist = 0;
					r_adjacentID = curID;
				}
				else
				{
					// At better dist, set distance and ID to better values
					r_adjacentDist = curDist+1;
					r_adjacentID = curID;
				}

				// Add adjacent position to ones that need to be checked
				posToCheck.push(adjacentPos);
			}
			else if((r_adjacentID != curID) && (r_adjacentDist >= curDist-1))
			{
				// if this is an edge between two districts, add it to known edges
				ALGORITHM_VORONOI_FIELDS::SHORTEST_DIST_POS *pShortestDist = shortestDistances.FindID(curID, r_adjacentID);
				if(pShortestDist == nullptr)
				{
					// If this dist does not exist yet, add it
					shortestDistances.Poses.push_back(ALGORITHM_VORONOI_FIELDS::SHORTEST_DIST_POS(curID, r_adjacentID, curDist, curPos, adjacentPos));
				}
				else
				{
					// Compare distances and change if better dist is better
					if(curDist <  pShortestDist->Distance)
					{
						pShortestDist->DistrictID1 = curID;
						pShortestDist->DistrictID2 = r_adjacentID;
						pShortestDist->Distance = curDist;
						pShortestDist->ShortestPosID1 = curPos;
						pShortestDist->ShortestPosID2 = adjacentPos;
					}
				}
			}
		}

		posToCheck.pop();
	}
	while(posToCheck.size() > 0);

	// Reset ID map for next step
	ALGORITHM_VORONOI_FIELDS::DIST_MAP districtDistMap;
	districtIDMap = IDMap;
	districtDistMap.ResetMap(IDMap.GetWidth(), IDMap.GetHeight(), ALGORITHM_VORONOI_FIELDS::MAX_DIST);

	// Set outer districts to unknown ID
	POS_2D curPos;
	for(curPos.X = 0; curPos.X < districtIDMap.GetWidth(); ++curPos.X)
	{
		for(curPos.Y = 0; curPos.Y < districtIDMap.GetHeight(); ++curPos.Y)
		{
			if(OccupationMap.GetPixel(curPos) != OccupationLevelOfDistrictsToDivide)
				districtIDMap.SetPixel(curPos, ALGORITHM_VORONOI_FIELDS::INVALID_ID);
		}
	}

	// Store district changes
	ALGORITHM_VORONOI_FIELDS::DISTRICT_CHANGE_VECTOR districtChanges;		// Stores changes that will later be applied

	// Separate along the given lines and store new separation positions
	for(const auto &curShortestPos : shortestDistances.Poses)
	{
		ALGORITHM_VORONOI_FIELDS::DISTRICT_CHANGE changeData;
		changeData.OldID = districtIDMap.GetPixel(curShortestPos.ShortestPosID1);

		// Create new ID
		changeData.ChangeNewIDPos = curShortestPos.ShortestPosID1;
		changeData.NewID = NextFreeID;

		// Divide district along path from shortest positions to district, and set path to new id (this separates the old and new district)
		AlgorithmVoronoiFields<T>::SetWholePathToDistrict(skeletonDistMap, curShortestPos, changeData.NewID, districtIDMap, districtDistMap);

		// Find an adjacent position to create a parallel path
		bool parallelPosFound = false;
		for(const auto &navOption : NavigationOptions)
		{
			const POS_2D adjacentPos = changeData.ChangeNewIDPos+navOption;
			ALGORITHM_VORONOI_FIELDS::ID adjacentID;
			if(districtIDMap.GetPixel(adjacentPos, adjacentID) < 0 || adjacentID != changeData.OldID)
				continue;		// Don't use this cell

			changeData.ChangeOldIDPos = adjacentPos;
			parallelPosFound = false;
			break;
		}

		if(!parallelPosFound)
			return -1;

		// Create the parallel path from the found position
		AlgorithmVoronoiFields<T>::CreateParellelPath(changeData.ChangeOldIDPos, changeData.OldID, changeData.OldID, IDMap, districtDistMap);

		// Save data for later, first separate all districts
		districtChanges.push_back(changeData);

		NextFreeID += 1;
	}

	// Add all change positions to queue
	for(const auto &curChange : districtChanges)
	{
		posToCheck.push(curChange.ChangeOldIDPos);
		posToCheck.push(curChange.ChangeNewIDPos);
	}

	// Enlarge districts
	while(posToCheck.size() > 0)
	{
		// Get Values of current position
		const POS_2D &curPos = posToCheck.front();
		const ALGORITHM_VORONOI_FIELDS::ID &curID = districtIDMap.GetPixel(curPos);
		const ALGORITHM_VORONOI_FIELDS::DIST_MAP::CELL_TYPE &curDist = districtDistMap.GetPixel(curPos);

		// Compare to adjacent positions
		for(const auto &navOption : NavigationOptions)
		{
			const POS_2D adjacentPos = curPos+navOption;

			DISTRICT_MAP::MAP_CELL_TYPE inDistrict;
			if(OriginalDistrictMap.GetPixel(adjacentPos, inDistrict) < 0 || inDistrict != DISTRICT_MAP::IN_DISTRICT)
				continue;		// Skip if outside of district

			ALGORITHM_VORONOI_FIELDS::ID &r_adjacentID = districtIDMap.GetPixelR(adjacentPos);

			if(r_adjacentID == ALGORITHM_VORONOI_FIELDS::INVALID_ID)
				continue;		// Skip if outer district

			ALGORITHM_VORONOI_FIELDS::DIST_MAP::CELL_TYPE &r_adjacentDist = districtDistMap.GetPixelR(adjacentPos);

			// Compare to curPos
			if(r_adjacentDist > curDist+1)
			{
				// If better dist is available, update
				r_adjacentDist = curDist+1;
				r_adjacentID = curID;
			}
			else if(r_adjacentDist >= curDist-1)
			{
				// If this is a border,
			}
		}
	}

	// Go through map and get district sizes
	ALGORITHM_VORONOI_FIELDS::DISTRICT_SIZE_VECTOR districtSizes;
	DistrictMap::GetAllDistrictSizesOfIDMap(districtIDMap, districtSizes);

	// Save all new districts
	DistrictsToDivide.clear();

	for(const auto &curSize : districtSizes)
	{
		DistrictsToDivide.push_back(DistrictMap(curSize));

		// Store all values in district
		DistrictMap &curDistrict = DistrictsToDivide.back();
		curDistrict.SetDistrictFromIDMap(districtIDMap, curSize.PosInDistrict, nullptr);
	}

	// Move new data to IDMap
	for(curPos.X = 0; curPos.X < IDMap.GetWidth(); ++curPos.X)
	{
		for(curPos.Y = 0; curPos.Y < IDMap.GetHeight(); ++curPos.Y)
		{
			const ALGORITHM_VORONOI_FIELDS::ID &curID = districtIDMap.GetPixel(curPos);

			// Only override data of Districts To Divide
			if(curID != ALGORITHM_VORONOI_FIELDS::INVALID_ID)
				IDMap.SetPixel(curPos, curID);
		}
	}

	return 1;
}

template<class T>
void AlgorithmVoronoiFields<T>::CreateTotalDistrict(const Map2D<T> &OriginalMap, const DistrictMap &OriginalDistrictData, const POS_2D &GlobalStartPos, const T &CutOffValue, const ALGORITHM_VORONOI_FIELDS::OCCUPATION_MAP::CELL_TYPE &OccupationLevel, ALGORITHM_VORONOI_FIELDS::DISTRICT_SIZE &DistrictSize, ALGORITHM_VORONOI_FIELDS::ID_MAP &IDMap, ALGORITHM_VORONOI_FIELDS::OCCUPATION_MAP &OccupationMap)
{
	std::queue<POS_2D> posToCheck;

	posToCheck.push(GlobalStartPos);
	do
	{
		for(const auto &navOption : NavigationOptions)
		{
			const POS_2D adjacentGlobalPos = posToCheck.front() + navOption;
			const POS_2D adjacentLocalPos = OriginalDistrictData.ConvertToLocalPosition(adjacentGlobalPos);

			DistrictMap::CELL_TYPE inDistrict;
			if(OriginalDistrictData.GetPixel(adjacentLocalPos, inDistrict) < 0 || inDistrict != DISTRICT_MAP::IN_DISTRICT)
				continue;		// Skip if not in district

			if(IDMap.GetPixel(adjacentLocalPos) != ALGORITHM_VORONOI_FIELDS::INVALID_ID)
				continue;		// Skip if already checked

			// Check if adjacent cell has same occuptation level
			const ALGORITHM_VORONOI_FIELDS::OCCUPATION_MAP::CELL_TYPE adjacentOccupationLevel = IsOccupiedCell(OriginalMap.GetPixel(adjacentGlobalPos), CutOffValue);
			if(OccupationLevel == adjacentOccupationLevel)
			{
				// Add this position to district
				IDMap.SetPixel(adjacentLocalPos, DistrictSize.DistrictID);
				OccupationMap.SetPixel(adjacentLocalPos, OccupationLevel);
				DistrictSize.ComparePos(adjacentGlobalPos);

				posToCheck.push(adjacentGlobalPos);
			}
		}

		posToCheck.pop();
	}
	while(posToCheck.size() > 0);
}

template<class T>
void AlgorithmVoronoiFields<T>::CombineTwoIDs(const ALGORITHM_VORONOI_FIELDS::ID &OriginalID, const ALGORITHM_VORONOI_FIELDS::ID &IDToCombine, const POS_2D &GlobalConnectionPos, const POS_2D &IDMapConnectionPos, ALGORITHM_VORONOI_FIELDS::ID_MAP &IDMap, ALGORITHM_VORONOI_FIELDS::DISTRICT_SIZE_VECTOR &DistrictSizeStorage, ALGORITHM_VORONOI_FIELDS::DISTRICT_STORAGE &DistrictStorage)
{
	std::queue<POS_2D> posToCheck;

	const POS_2D idMapOffset = IDMapConnectionPos - GlobalConnectionPos;
	const ALGORITHM_VORONOI_FIELDS::DISTRICT_SIZE_VECTOR::STORAGE_TYPE::size_type sizeIterator = DistrictSizeStorage.FindIDIterator(OriginalID);

	// Set first position to new value

	// Set To new ID
	IDMap.SetPixel(IDMapConnectionPos, OriginalID);
	// Enlarge district map if necessary
	DistrictSizeStorage.at(sizeIterator).ComparePos(GlobalConnectionPos);

	posToCheck.push(GlobalConnectionPos);
	do
	{
		const POS_2D &curGlobalPos = posToCheck.front();

		// Check adjacent position
		for(const auto &navOption : NavigationOptions)
		{
			const POS_2D curGlobalAdjacentPos = curGlobalPos + navOption;

			// Check if adjacent position is in map and has correct ID
			ALGORITHM_VORONOI_FIELDS::ID adjacentID;
			if(IDMap.GetPixel(curGlobalAdjacentPos+idMapOffset, adjacentID) < 0 || adjacentID != IDToCombine)
				continue;		// Skip if not in map or not correct ID

			// Set To new ID
			IDMap.SetPixel(curGlobalAdjacentPos+idMapOffset, OriginalID);

			// Enlarge district map if necessary
			DistrictSizeStorage.at(sizeIterator).ComparePos(curGlobalAdjacentPos);

			posToCheck.push(curGlobalAdjacentPos);
		}

		// Move to next position
		posToCheck.pop();
	}
	while(posToCheck.size() > 0);

#ifdef DEBUG
	for(const auto &i : IDMap.GetCellStorage())
		if(i == IDToCombine)
			std::cerr << "ERROR: AlgorithmVoronoiFields<T>::CombineTwoIDs didn't erase all old IDs!";
#endif // ~DEBUG

	// Remove unnecessary ID from storage
	DistrictStorage.erase(DistrictStorage.begin()+static_cast<ALGORITHM_VORONOI_FIELDS::DISTRICT_STORAGE::difference_type>(DistrictStorage.FindDistrictIDIterator(IDToCombine)));
	DistrictSizeStorage.erase(DistrictSizeStorage.begin() + static_cast<ALGORITHM_VORONOI_FIELDS::DISTRICT_SIZE_VECTOR::STORAGE_TYPE::difference_type>(DistrictSizeStorage.FindIDIterator(IDToCombine)));
}

template<class T>
void AlgorithmVoronoiFields<T>::ExpandDistrict(const ALGORITHM_VORONOI_FIELDS::ID_MAP &IDMap, const ALGORITHM_VORONOI_FIELDS::DISTRICT_SIZE &DistrictSize, DistrictMap &CurDistrict)
{
	// Set map to correct size
	CurDistrict.ResizeMap(DistrictSize.MaxPos.X-DistrictSize.MinPos.X+1, DistrictSize.MaxPos.Y-DistrictSize.MinPos.Y+1);

	// Set global position of one cell in district
	CurDistrict.SetLocalPosInDistrict(CurDistrict.GetGlobalMapPosition());

	// Set global position
	CurDistrict.SetGlobalMapPosition(DistrictSize.MinPos);

	// Convert global pos to local pos
	CurDistrict.SetLocalPosInDistrict(CurDistrict.ConvertToLocalPosition(CurDistrict.GetLocalPosInDistrict()));

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

template<class T>
int AlgorithmVoronoiFields<T>::SetPathToDistrict(const ALGORITHM_VORONOI_FIELDS::DIST_MAP &SkelDistMap, const POS_2D &StartPos, const ALGORITHM_VORONOI_FIELDS::ID &IDToSetPathTo, ALGORITHM_VORONOI_FIELDS::ID_MAP &IDMap, ALGORITHM_VORONOI_FIELDS::DIST_MAP &DistMap, ALGORITHM_VORONOI_FIELDS::DISTRICT_SIZE *DistrictSize, typename AlgorithmDStar<T>::PATH_VECTOR *Path)
{
	typename AlgorithmDStar<T>::PATH_VECTOR tmpPath;
	ALGORITHM_VORONOI_FIELDS::DISTRICT_SIZE tmpDistrictSize(IDToSetPathTo, StartPos);

	// Start at beginning
	POS_2D curPos = StartPos;
	ALGORITHM_VORONOI_FIELDS::DIST_MAP::CELL_TYPE curDist = SkelDistMap.GetPixel(curPos);
	const ALGORITHM_VORONOI_FIELDS::ID &idOfDistrict = IDMap.GetPixel(curPos);

	IDMap.SetPixel(curPos, IDToSetPathTo);
	tmpPath.push_back(curPos);

	// Continue along path until destination is reached
	while(curDist != 0)
	{
		ALGORITHM_VORONOI_FIELDS::DIST_MAP::CELL_TYPE bestDist = GetInfiniteVal<ALGORITHM_VORONOI_FIELDS::DIST_MAP::CELL_TYPE>();
		POS_2D bestPos = curPos;

		for(const auto &navOption : NavigationOptions)
		{
			const POS_2D adjacentPos = curPos+navOption;
			ALGORITHM_VORONOI_FIELDS::DIST_MAP::CELL_TYPE adjacentDist;

			if(SkelDistMap.GetPixel(adjacentPos, adjacentDist) < 0)
				continue;		// Skip if not on map

			if(adjacentDist <= bestDist)
			{
				if(IDMap.GetPixel(adjacentPos) != idOfDistrict)
					continue;	// Skip if not in same district

				bestDist = adjacentDist;
				bestPos = adjacentPos;
			}
		}

		// Check if better position was found
		if(bestPos == curPos)
			return -1;

		// Move to next position and add it to path
		curPos = bestPos;

		IDMap.SetPixel(curPos, IDToSetPathTo);
		DistMap.SetPixel(curPos, 0);		// Set distance to zero
		tmpPath.push_back(curPos);
		tmpDistrictSize.ComparePos(curPos);		// Adjust size
	}

	// Move data if requested
	if(Path != nullptr)
		*Path = std::move(tmpPath);

	if(DistrictSize != nullptr)
		*DistrictSize = std::move(tmpDistrictSize);

	return 1;
}

template<class T>
int AlgorithmVoronoiFields<T>::SetWholePathToDistrict(const ALGORITHM_VORONOI_FIELDS::DIST_MAP &SkelDistMap, const ALGORITHM_VORONOI_FIELDS::SHORTEST_DIST_POS &ShortestDistPos, const ALGORITHM_VORONOI_FIELDS::ID &IDToSetPathTo, ALGORITHM_VORONOI_FIELDS::ID_MAP &IDMap, ALGORITHM_VORONOI_FIELDS::DIST_MAP &DistMap, ALGORITHM_VORONOI_FIELDS::DISTRICT_SIZE *DistrictSize, typename AlgorithmDStar<T>::PATH_VECTOR *Path)
{
	int tmp1, tmp2;
	ALGORITHM_VORONOI_FIELDS::DISTRICT_SIZE tmpDistrict1(IDToSetPathTo, ShortestDistPos.ShortestPosID1);
	ALGORITHM_VORONOI_FIELDS::DISTRICT_SIZE tmpDistrict2(tmpDistrict1);
	typename AlgorithmDStar<T>::PATH_VECTOR tmpPath1, tmpPath2;

	// Divide district along path from shortest positions to district, and set path to an invalid id (this separates the old and new district)
	tmp1 = AlgorithmVoronoiFields<T>::SetPathToDistrict(SkelDistMap, ShortestDistPos.ShortestPosID1, IDToSetPathTo, IDMap, DistMap, &tmpDistrict1, &tmpPath1);
	tmp2 = AlgorithmVoronoiFields<T>::SetPathToDistrict(SkelDistMap, ShortestDistPos.ShortestPosID2, IDToSetPathTo, IDMap, DistMap, &tmpDistrict2, &tmpPath2);

	// Combine both districts
	tmpDistrict1.ComparePos(tmpDistrict2.MinPos);
	tmpDistrict1.ComparePos(tmpDistrict2.MaxPos);

	// Combine both paths
	for(const auto &curPathPos : tmpPath2)
		tmpPath1.push_back(curPathPos);

	// Propagate data upwards
	if(DistrictSize != nullptr)
		*DistrictSize = std::move(tmpDistrict1);

	if(Path != nullptr)
		*Path = std::move(tmpPath1);

	if(tmp1 < 0 || tmp2 < 0)
		return -1;

	return 1;
}

template<class T>
int AlgorithmVoronoiFields<T>::CreateParellelPath(const POS_2D &StartPos, const ALGORITHM_VORONOI_FIELDS::ID &IDToSetPathTo, const ALGORITHM_VORONOI_FIELDS::ID &ParallelID, ALGORITHM_VORONOI_FIELDS::ID_MAP &IDMap, ALGORITHM_VORONOI_FIELDS::DIST_MAP &DistMap, ALGORITHM_VORONOI_FIELDS::DISTRICT_SIZE *DistrictSize, typename AlgorithmDStar<T>::PATH_VECTOR *Path)
{
	std::queue<POS_2D> posToCheck;
	const ALGORITHM_VORONOI_FIELDS::ID oldID = IDMap.GetPixel(StartPos);
	std::vector<POS_2D> adjacentPoses;
	adjacentPoses.reserve(NumNavigationOptions);

	typename AlgorithmDStar<T>::PATH_VECTOR tmpPath;
	ALGORITHM_VORONOI_FIELDS::DISTRICT_SIZE tmpDistrictSize(IDToSetPathTo, StartPos);

	// Start at beginning
	posToCheck.push(StartPos);
	do
	{
		const POS_2D &curPos = posToCheck.front();
		ALGORITHM_VORONOI_FIELDS::ID &r_curID = IDMap.GetPixelR(curPos);

		// Get all connecting positions
		adjacentPoses.resize(0);
		bool posIsParallel = false;
		for(const auto &navOption : NavigationOptions)
		{
			const POS_2D adjacentPos = curPos+navOption;
			ALGORITHM_VORONOI_FIELDS::ID adjacentID;

			if(IDMap.GetPixel(adjacentPos, adjacentID) < 0)
				continue;		// Skip if outside of map

			if(adjacentID == ParallelID)
			{
				posIsParallel = true;
			}
			else if(adjacentID == oldID)
			{
				adjacentPoses.push_back(adjacentPos);		// Position needs to be checked later
			}
		}

		// If this position is parallel, set it to correct value and add to path
		if(posIsParallel)
		{
			r_curID = IDToSetPathTo;
			DistMap.SetPixel(curPos, 0);		// Set start distance to zero
			tmpPath.push_back(curPos);
			tmpDistrictSize.ComparePos(curPos);

			// Add all adjacent poses that haven't been checked yet to check queue
			for(const auto &curAdjacentPos : adjacentPoses)
			{
				if(DistMap.GetPixel(curAdjacentPos) != 0)
					posToCheck.push(curAdjacentPos);
			}
		}

		posToCheck.pop();
	}
	while(posToCheck.size() > 0);

	// Save path if requested
	if(Path != nullptr)
		*Path = std::move(tmpPath);

	// Save size if requested
	if(DistrictSize != nullptr)
		*DistrictSize = std::move(tmpDistrictSize);

	if(Path->size() == 0)
		return -1;		// return error if no pixels where set

	return 1;
}


#endif // ALGORITHM_VORONOI_FIELDS_TEMPLATE_H
