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

	// Step 1: Separate map into districts
	AlgorithmVoronoiFields<T>::SeparateMapIntoUnconnectedDistricts(OriginalMap, OriginalDistrictData, CutOffValue, OccupiedDistricts, FreeDistricts, idMap, occupationMap, NextFreeDistrictID);

	// Step 2.1: Find shortest distances between free districts
	AlgorithmVoronoiFields<T>::SeparateByShortestDistance(OriginalDistrictData, occupationMap, !ALGORITHM_VORONOI_FIELDS::CELL_OCCUPIED, FreeDistricts, NextFreeDistrictID, idMap);

	// Step 2.2: Find shortest distances between occupied districts
	AlgorithmVoronoiFields<T>::SeparateByShortestDistance(OriginalDistrictData, occupationMap, ALGORITHM_VORONOI_FIELDS::CELL_OCCUPIED, OccupiedDistricts, NextFreeDistrictID, idMap);
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

	return static_cast<T>(tmpVal/numElements);
}

template<class T>
void AlgorithmVoronoiFields<T>::SeparateMapIntoUnconnectedDistricts(const Map2D<T> &OriginalMap, const DistrictMap &OriginalDistrictData, const T &CutOffValue, ALGORITHM_VORONOI_FIELDS::DISTRICT_STORAGE &OccupiedDistricts, ALGORITHM_VORONOI_FIELDS::DISTRICT_STORAGE &FreeDistricts, ALGORITHM_VORONOI_FIELDS::ID_MAP &IDMap, ALGORITHM_VORONOI_FIELDS::OCCUPATION_MAP &OccupationMap, ALGORITHM_VORONOI_FIELDS::ID &NextFreeID)
{
	IDMap.ResetMap(OriginalDistrictData.GetWidth(), OriginalDistrictData.GetHeight(), ALGORITHM_VORONOI_FIELDS::INVALID_ID);
	OccupationMap.ResetMap(OriginalDistrictData.GetWidth(), OriginalDistrictData.GetHeight(), ALGORITHM_VORONOI_FIELDS::CELL_OCCUPIED);

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

				// Check if current cell is supposed to be occupied or free
				ALGORITHM_VORONOI_FIELDS::OCCUPATION_MAP::CELL_TYPE occupationLevel;
				if(OriginalMap.GetPixel(globalPos) > CutOffValue)
					occupationLevel = ALGORITHM_VORONOI_FIELDS::CELL_OCCUPIED;
				else
					occupationLevel = !ALGORITHM_VORONOI_FIELDS::CELL_OCCUPIED;

				// Set Occupation Map
				OccupationMap.SetPixel(curLocalPos, occupationLevel);

				// Check if a surrounding cell that has same occupation has already been set to an ID
				for(const auto &curNavOption : NavigationOptions)
				{
					const POS_2D curAdjacentPos = curLocalPos+curNavOption;

					// Check that pos is in district and get adjacent ID
					//bool inDistrict;
					//if(OriginalDistrictData.GetPixel(curAdjacentPos, inDistrict) < 0 || inDistrict != DISTRICT_MAP::IN_DISTRICT)
					//	continue;		// not in district, ignore
					ALGORITHM_VORONOI_FIELDS::ID adjacentID;
					if(IDMap.GetPixel(curAdjacentPos, adjacentID) < 0 || adjacentID == ALGORITHM_VORONOI_FIELDS::INVALID_ID)
						continue;		// not yet checked or not in district, ignore

					// Compare occupation levels
					if(OccupationMap.GetPixel(curAdjacentPos) != occupationLevel)
						continue;		// skip if other occupation level

					// Save first ID that has same occupation level, and integrate all other adjacent IDs that are connected through curPos to this one
					if(r_cellID == ALGORITHM_VORONOI_FIELDS::INVALID_ID)
					{
						r_cellID = adjacentID;
						districtSizes.FindID(r_cellID)->ComparePos(curLocalPos);		// Check if size needs to be expanded
					}
					else if(r_cellID != adjacentID)		// Check if two IDs need to be merged
					{
						// Merge districts into one, then erase unnecessary districts
						if(occupationLevel == ALGORITHM_VORONOI_FIELDS::CELL_OCCUPIED)
							AlgorithmVoronoiFields<T>::CombineTwoIDs(r_cellID, adjacentID, curLocalPos, IDMap, districtSizes, OccupiedDistricts);
						else
							AlgorithmVoronoiFields<T>::CombineTwoIDs(r_cellID, adjacentID, curLocalPos, IDMap, districtSizes, FreeDistricts);
					}
				}

				// If no other adjacent districts where found, create a new one for this position
				if(r_cellID == ALGORITHM_VORONOI_FIELDS::INVALID_ID)
				{
					r_cellID = NextFreeID;

					// Create new size and create new district in correct vector (occupied or unoccupied)
					districtSizes.Sizes.push_back(ALGORITHM_VORONOI_FIELDS::DISTRICT_SIZE(r_cellID, curLocalPos));

					if(occupationLevel == ALGORITHM_VORONOI_FIELDS::CELL_OCCUPIED)
						OccupiedDistricts.push_back(DistrictMap(r_cellID, curLocalPos, 0, 0, DISTRICT_MAP::IN_DISTRICT));
					else
						FreeDistricts.push_back(DistrictMap(r_cellID, curLocalPos, 0, 0, DISTRICT_MAP::IN_DISTRICT));

					NextFreeID++;
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

template<class T>
void AlgorithmVoronoiFields<T>::SeparateByShortestDistance(const DistrictMap &OriginalDistrictMap, const ALGORITHM_VORONOI_FIELDS::OCCUPATION_MAP &OccupationMap, const ALGORITHM_VORONOI_FIELDS::OCCUPATION_MAP::CELL_TYPE &OccupatioLevel, ALGORITHM_VORONOI_FIELDS::DISTRICT_STORAGE &Districts, ALGORITHM_VORONOI_FIELDS::ID &NextFreeID, ALGORITHM_VORONOI_FIELDS::ID_MAP &IDMap)
{
	ALGORITHM_VORONOI_FIELDS::SHORTEST_DIST_POS_VECTOR shortestDistances;

	// Create map with all IDs and map with all distances
	ALGORITHM_VORONOI_FIELDS::ID_MAP districtIDMap;			// Map with IDs of closest district
	ALGORITHM_VORONOI_FIELDS::DIST_MAP districtDistMap;		// Map with distance to closest ID

	districtIDMap.ResetMap(OriginalDistrictMap.GetWidth(), OriginalDistrictMap.GetHeight(), ALGORITHM_VORONOI_FIELDS::INVALID_ID);
	districtDistMap.ResetMap(OriginalDistrictMap.GetWidth(), OriginalDistrictMap.GetHeight(), ALGORITHM_VORONOI_FIELDS::MAX_DIST);

	// Add all positions in district to queue
	std::queue<POS_2D> posToCheck;
	for(const auto &curDistrict : Districts)
	{
		const POS_2D globalPosInDistrict = curDistrict.GetGlobalPosInDistrict();

		// Set distance of this position to zero and idMap to correct ID
		districtDistMap.SetPixel(globalPosInDistrict, 0);
		districtIDMap.SetPixel(globalPosInDistrict, curDistrict.GetID());

		posToCheck.push(curDistrict.GetGlobalPosInDistrict());
	}

	do
	{
		const POS_2D &curPos = posToCheck.front();

		// Get distance and ID of this pos
		const ALGORITHM_VORONOI_FIELDS::DIST_MAP::CELL_TYPE &curDist = districtDistMap.GetPixel(curPos);
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
			ALGORITHM_VORONOI_FIELDS::DIST_MAP::CELL_TYPE &r_adjacentDist = districtDistMap.GetPixelR(adjacentPos);
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
			else if((r_adjacentID == curID) && (r_adjacentDist >= curDist-1))
			{
				// if this is an edge between two districts, add it to known edges
				ALGORITHM_VORONOI_FIELDS::SHORTEST_DIST_POS *shortestDist = shortestDistances.FindID(curID, r_adjacentID);
				if(shortestDist == nullptr)
				{
					// If this dist does not exist yet, add it
					shortestDistances.Poses.push_back(ALGORITHM_VORONOI_FIELDS::SHORTEST_DIST_POS(curID, r_adjacentID, curDist, curPos, adjacentPos));
				}
				else
				{
					// Compare distances and change if better dist is better
					if(curDist <  shortestDist->Distance)
					{
						shortestDist->DistrictID1 = curID;
						shortestDist->DistrictID2 = r_adjacentID;
						shortestDist->Distance = curDist;
						shortestDist->ShortestPosID1 = curPos;
						shortestDist->ShortestPosID2 = adjacentPos;
					}
				}
			}
		}

		posToCheck.pop();
	}
	while(posToCheck.size() > 0);

	// Separate along the given lines
	for(const auto &curShortestPos : shortestDistances.Poses)
	{
		const ALGORITHM_VORONOI_FIELDS::ID oldID = IDMap.GetPixel(curShortestPos.ShortestPosID1);

		// Divide district along path from shortest positions to district, and set path to an invalid id (this separates the old and new district)
		typename AlgorithmDStar<T>::PATH_VECTOR path1, path2;
		AlgorithmVoronoiFields<T>::SetPathToDistrict(districtDistMap, curShortestPos.ShortestPosID1, ALGORITHM_VORONOI_FIELDS::INVALID_ID, IDMap, path1);
		AlgorithmVoronoiFields<T>::SetPathToDistrict(districtDistMap, curShortestPos.ShortestPosID2, ALGORITHM_VORONOI_FIELDS::INVALID_ID, IDMap, path2);

		// Create new districts
		bool newDistrictCreated = false;
		for(const auto &navOption : NavigationOptions)
		{
			const POS_2D &adjacentPos = curShortestPos.ShortestPosID1+navOption;
			ALGORITHM_VORONOI_FIELDS::ID adjacentID;
			if(IDMap.GetPixel(adjacentPos, adjacentID) < 0 || adjacentID == ALGORITHM_VORONOI_FIELDS::INVALID_ID)
				continue;		// Don't use cell on dividing edge

			if(OccupationMap.GetPixel(adjacentPos) != OccupatioLevel)
				continue;		// Skip if not same occupation level

			DISTRICT_MAP::POS_IN_DISTRICT positionsInDistrict;
			DistrictMap newDistrict;
			newDistrict.SetDistrictFromIDMap(IDMap, adjacentPos, &positionsInDistrict);

			// Create new ID for this map
			newDistrict.SetID(NextFreeID);
			for(const auto &curPos : positionsInDistrict)		// Change IDs of global map
				IDMap.SetPixel(curPos, newDistrict.GetID());

			// Add new district to map
			Districts.push_back(std::move(newDistrict));

			NextFreeID++;
			newDistrictCreated = true;
			break;
		}

		// Set path back to original value
		for(const auto &curPathPos : path1)
			IDMap.SetPixel(curPathPos, oldID);
		for(const auto &curPathPos : path2)
			IDMap.SetPixel(curPathPos, oldID);

		// Resize original district if necessary
		if(newDistrictCreated)
		{
			DistrictMap *const oldDistrict = Districts.FindDistrictID(oldID);
			oldDistrict->SetDistrictFromIDMap(IDMap, curShortestPos.ShortestPosID1);
		}
	}
}

template<class T>
void AlgorithmVoronoiFields<T>::CombineTwoIDs(const ALGORITHM_VORONOI_FIELDS::ID &OriginalID, const ALGORITHM_VORONOI_FIELDS::ID &IDToCombine, const POS_2D &ConnectionPos, ALGORITHM_VORONOI_FIELDS::ID_MAP &IDMap, ALGORITHM_VORONOI_FIELDS::DISTRICT_SIZE_VECTOR &DistrictSizeStorage, ALGORITHM_VORONOI_FIELDS::DISTRICT_STORAGE &DistrictStorage)
{
	std::queue<POS_2D> posToCheck;

	const ALGORITHM_VORONOI_FIELDS::DISTRICT_SIZE_VECTOR::STORAGE_TYPE::size_type sizeIterator = DistrictSizeStorage.FindIDIterator(OriginalID);

	// Set first position to new value

	// Set To new ID
	IDMap.SetPixel(ConnectionPos, OriginalID);
	// Enlarge district map if necessary
	DistrictSizeStorage.Sizes.at(sizeIterator).ComparePos(ConnectionPos);

	posToCheck.push(ConnectionPos);
	do
	{
		const POS_2D &curPos = posToCheck.front();

		// Check adjacent position
		for(const auto &navOption : NavigationOptions)
		{
			const POS_2D curAdjacentPos = curPos + navOption;

			// Check if adjacent position is in map and has correct ID
			ALGORITHM_VORONOI_FIELDS::ID adjacentID;
			if(IDMap.GetPixel(curAdjacentPos, adjacentID) < 0 || adjacentID != IDToCombine)
				continue;		// Skip if not in map or not correct ID

			// Set To new ID
			IDMap.SetPixel(curAdjacentPos, OriginalID);

			// Enlarge district map if necessary
			DistrictSizeStorage.Sizes.at(sizeIterator).ComparePos(curAdjacentPos);

			posToCheck.push(curAdjacentPos);
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
	DistrictSizeStorage.Sizes.erase(DistrictSizeStorage.Sizes.begin() + static_cast<ALGORITHM_VORONOI_FIELDS::DISTRICT_SIZE_VECTOR::STORAGE_TYPE::difference_type>(DistrictSizeStorage.FindIDIterator(IDToCombine)));
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
int AlgorithmVoronoiFields<T>::SetPathToDistrict(const ALGORITHM_VORONOI_FIELDS::DIST_MAP &DistMap, const POS_2D &StartPos, const ALGORITHM_VORONOI_FIELDS::ID &IDToSetPathTo, ALGORITHM_VORONOI_FIELDS::ID_MAP &IDMap, typename AlgorithmDStar<T>::PATH_VECTOR &Path)
{
	// Clear path
	Path.clear();

	// Start at beginning
	POS_2D curPos = StartPos;
	ALGORITHM_VORONOI_FIELDS::DIST_MAP::CELL_TYPE curDist = DistMap.GetPixel(curPos);
	const ALGORITHM_VORONOI_FIELDS::ID &idOfDistrict = IDMap.GetPixel(curPos);

	IDMap.SetPixel(curPos, IDToSetPathTo);
	Path.push_back(curPos);

	// Continue along path until destination is reached
	while(curDist != 0)
	{
		ALGORITHM_VORONOI_FIELDS::DIST_MAP::CELL_TYPE bestDist = GetInfiniteVal<ALGORITHM_VORONOI_FIELDS::DIST_MAP::CELL_TYPE>();
		POS_2D bestPos = curPos;

		for(const auto &navOption : NavigationOptions)
		{
			const POS_2D adjacentPos = curPos+navOption;
			ALGORITHM_VORONOI_FIELDS::DIST_MAP::CELL_TYPE adjacentDist;

			if(DistMap.GetPixel(adjacentPos, adjacentDist) < 0)
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
		Path.push_back(curPos);
	}

	return 1;
}

#endif // ALGORITHM_VORONOI_FIELDS_TEMPLATE_H
