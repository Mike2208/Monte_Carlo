#ifndef ALGORITHM_VORONOI_FIELDS_TEMPLATE_H
#define ALGORITHM_VORONOI_FIELDS_TEMPLATE_H

#include "algorithm_voronoi_fields.h"
#include "standard_definitions.h"
#include "robot_navigation.h"
#include "algorithm_d_star.h"

#include <queue>

template<class T>
void AlgorithmVoronoiFields<T>::CalculateVoronoiField(const Map2D<T> &OriginalMap, const ALGORITHM_VORONOI_FIELDS::DIST_MAP::CELL_TYPE &MinDistBetweenDistricts, const DistrictMap &OriginalDistrictData, ALGORITHM_VORONOI_FIELDS::DISTRICT_STORAGE &OccupiedDistricts, ALGORITHM_VORONOI_FIELDS::DISTRICT_STORAGE &FreeDistricts, ALGORITHM_VORONOI_FIELDS::ID &NextFreeDistrictID, ALGORITHM_VORONOI_FIELDS::ID_MAP *IDMap)
{
	const T cutOffValue = AlgorithmVoronoiFields<T>::CalculateMapAverage(OriginalMap, OriginalDistrictData);

	return AlgorithmVoronoiFields<T>::CalculateVoronoiField(OriginalMap, cutOffValue, MinDistBetweenDistricts, OriginalDistrictData, OccupiedDistricts, FreeDistricts, NextFreeDistrictID, IDMap);
}


template<class T>
void AlgorithmVoronoiFields<T>::CalculateVoronoiField(const Map2D<T> &OriginalMap, const T &CutOffValue, const ALGORITHM_VORONOI_FIELDS::DIST_MAP::CELL_TYPE &MinDistBetweenDistricts, const DistrictMap &OriginalDistrictData, ALGORITHM_VORONOI_FIELDS::DISTRICT_STORAGE &OccupiedDistricts, ALGORITHM_VORONOI_FIELDS::DISTRICT_STORAGE &FreeDistricts, ALGORITHM_VORONOI_FIELDS::ID &NextFreeDistrictID, ALGORITHM_VORONOI_FIELDS::ID_MAP *IDMap)
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
	AlgorithmVoronoiFields<T>::SeparateByShortestDistance(OriginalDistrictData, occupationMap, !ALGORITHM_VORONOI_FIELDS::CELL_OCCUPIED, MinDistBetweenDistricts, OccupiedDistricts, FreeDistricts, NextFreeDistrictID, idMap);

#ifdef DEBUG	// DEBUG
	idMap.PrintMap("/tmp/testID2.pgm", NextFreeDistrictID-1, 0);
	occupationMap.PrintMap("/tmp/testOcc2.pgb");
#endif			// ~DEBUG

	// Step 2.2: Find shortest distances between occupied districts
	AlgorithmVoronoiFields<T>::SeparateByShortestDistance(OriginalDistrictData, occupationMap, ALGORITHM_VORONOI_FIELDS::CELL_OCCUPIED, MinDistBetweenDistricts, FreeDistricts, OccupiedDistricts, NextFreeDistrictID, idMap);

#ifdef DEBUG	// DEBUG
	idMap.PrintMap("/tmp/testID3.pgm", NextFreeDistrictID-1, 0);
	occupationMap.PrintMap("/tmp/testOcc3.pgb");
#endif			// ~DEBUG

	// Save idMap if requested
	if(IDMap != nullptr)
		*IDMap = std::move(idMap);
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
int AlgorithmVoronoiFields<T>::SeparateByShortestDistance(const DistrictMap &OriginalDistrictMap, const ALGORITHM_VORONOI_FIELDS::OCCUPATION_MAP &OccupationMap, const ALGORITHM_VORONOI_FIELDS::OCCUPATION_MAP::CELL_TYPE &OccupationLevelOfDistrictsToDivide, const ALGORITHM_VORONOI_FIELDS::DIST_MAP::CELL_TYPE &MinDistBetweenDistricts, const ALGORITHM_VORONOI_FIELDS::DISTRICT_STORAGE &OuterDistricts, ALGORITHM_VORONOI_FIELDS::DISTRICT_STORAGE &DistrictsToDivide, ALGORITHM_VORONOI_FIELDS::ID &NextFreeID, ALGORITHM_VORONOI_FIELDS::ID_MAP &IDMap)
{
	ALGORITHM_VORONOI_FIELDS::SHORTEST_DIST_POS_VECTOR shortestDistances;

	// Create map with all IDs and map with all distances
	ALGORITHM_VORONOI_FIELDS::ID_MAP districtIDMap;			// Map with IDs of closest district
	ALGORITHM_VORONOI_FIELDS::DIST_MAP skeletonDistMap;		// Map with distance to closest ID
	ALGORITHM_VORONOI_FIELDS::DIST_MAP completeSkelMap;		// Finished skeleton map

	districtIDMap.ResetMap(OriginalDistrictMap.GetWidth(), OriginalDistrictMap.GetHeight(), ALGORITHM_VORONOI_FIELDS::INVALID_ID);
	skeletonDistMap.ResetMap(OriginalDistrictMap.GetWidth(), OriginalDistrictMap.GetHeight(), ALGORITHM_VORONOI_FIELDS::MAX_DIST);
	completeSkelMap.ResetMap(OriginalDistrictMap.GetWidth(), OriginalDistrictMap.GetHeight(), ALGORITHM_VORONOI_FIELDS::MAX_DIST);

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
				// if this is an edge between two districts, add it to complete skeleton map
				completeSkelMap.SetPixel(curPos, curDist);
				completeSkelMap.SetPixel(adjacentPos, r_adjacentDist);
			}
		}

		posToCheck.pop();
	}
	while(posToCheck.size() > 0);

	// Find all shortest distances from skeleton map
	ALGORITHM_VORONOI_FIELDS::CHECKED_POSITIONS_MAP posChecked(completeSkelMap.GetWidth(), completeSkelMap.GetHeight(), ALGORITHM_VORONOI_FIELDS::INVALID_ID);			// Map to log which positions have already been checked
	ALGORITHM_VORONOI_FIELDS::CHECKED_POSITIONS_MAP::CELL_TYPE nextSkeletonTypeID = 0;
	POS_2D tmpCurPos;
	for(tmpCurPos.X = 0; tmpCurPos.X < districtIDMap.GetWidth(); ++tmpCurPos.X)
	{
		for(tmpCurPos.Y = 0; tmpCurPos.Y < districtIDMap.GetHeight(); ++tmpCurPos.Y)
		{
			if(posChecked.GetPixel(tmpCurPos) != ALGORITHM_VORONOI_FIELDS::INVALID_ID)
				continue;			// Skip if position has already been checked

			// Check if this position is on the skeleton map, then get all shortest distances on this part of the skeleton
			GetShortestDistPoses(completeSkelMap, tmpCurPos, MinDistBetweenDistricts, districtIDMap, skeletonDistMap, posChecked, nextSkeletonTypeID, shortestDistances);

			nextSkeletonTypeID++;
		}
	}

	// Reset ID map for next step
	ALGORITHM_VORONOI_FIELDS::DIST_MAP districtDistMap;
	districtIDMap = IDMap;
	districtDistMap.ResetMap(IDMap.GetWidth(), IDMap.GetHeight(), ALGORITHM_VORONOI_FIELDS::MAX_DIST);

	// Set outer districts to unknown ID
	for(tmpCurPos.X = 0; tmpCurPos.X < districtIDMap.GetWidth(); ++tmpCurPos.X)
	{
		for(tmpCurPos.Y = 0; tmpCurPos.Y < districtIDMap.GetHeight(); ++tmpCurPos.Y)
		{
			if(OccupationMap.GetPixel(tmpCurPos) != OccupationLevelOfDistrictsToDivide)
				districtIDMap.SetPixel(tmpCurPos, ALGORITHM_VORONOI_FIELDS::INVALID_ID);
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
		AlgorithmVoronoiFields<T>::SetWholePathToDistrict(skeletonDistMap, curShortestPos, changeData.NewID, 0, districtIDMap, districtDistMap);

		// Find an adjacent position to create a parallel path
		bool parallelPosFound = false;
		for(const auto &navOption : NavigationOptions)
		{
			const POS_2D adjacentPos = changeData.ChangeNewIDPos+navOption;
			ALGORITHM_VORONOI_FIELDS::ID adjacentID;
			if(districtIDMap.GetPixel(adjacentPos, adjacentID) < 0 || adjacentID != changeData.OldID)
				continue;		// Don't use this cell

			changeData.ChangeOldIDPos = adjacentPos;
			parallelPosFound = true;
			break;
		}

		if(!parallelPosFound)
		{
			// Reverse path setting
			AlgorithmVoronoiFields<T>::SetWholePathToDistrict(skeletonDistMap, curShortestPos, changeData.OldID, ALGORITHM_VORONOI_FIELDS::MAX_DIST, districtIDMap, districtDistMap);

			break;
		}

		// Create the parallel path from the found position
		AlgorithmVoronoiFields<T>::CreateParellelPath(changeData.ChangeOldIDPos, changeData.OldID, changeData.NewID, districtIDMap, districtDistMap);

		// Save data for later, first separate all districts
		districtChanges.push_back(changeData);

		NextFreeID += 1;
	}

#ifdef DEBUG	// DEBUG
	districtDistMap.PrintMap("/tmp/districtDist.pbm", ALGORITHM_VORONOI_FIELDS::MAX_DIST, 0);
	districtIDMap.PrintMap("/tmp/districtID.pbm", NextFreeID, 0);
#endif			// ~DEBUG

	// Add all change positions to queue
	for(const auto &curChange : districtChanges)
	{
		posToCheck.push(curChange.ChangeOldIDPos);
		posToCheck.push(curChange.ChangeNewIDPos);
	}

	ALGORITHM_VORONOI_FIELDS::ID_CONNECTION_VECTOR connectedIDs;

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

				posToCheck.push(adjacentPos);
			}
			else if(r_adjacentDist >= curDist-1)
			{
				// If this is a border, add connection
				if(r_adjacentID != curID && curDist > 0 && r_adjacentDist > 0)
					connectedIDs.AddConnection(curID, r_adjacentID, curPos, adjacentPos);
			}
		}

		posToCheck.pop();
	}

//	for(unsigned int i=0; i<connectedIDs.size(); ++i)
//	{
//		// Erase invalid connections
//		if(districtChanges.AreChangedIDs(connectedIDs.at(i).IDs[0], connectedIDs.at(i).IDs[1]))
//		{
//			connectedIDs.erase(connectedIDs.begin()+i);
//			i--;
//		}
//	}

	// Combine districts in such a way that only one district remains in an area
	for(const auto &curConnection : connectedIDs)
	{
		if(districtIDMap.GetPixel(curConnection.ConnectionPos[1]) != curConnection.IDs[1])
			AlgorithmVoronoiFields<T>::CombineTwoIDs(curConnection.IDs[0], districtIDMap.GetPixel(curConnection.ConnectionPos[1]), curConnection.ConnectionPos[0], districtIDMap);
		else
			AlgorithmVoronoiFields<T>::CombineTwoIDs(curConnection.IDs[1], districtIDMap.GetPixel(curConnection.ConnectionPos[0]), curConnection.ConnectionPos[1], districtIDMap);
	}

#ifdef DEBUG	// DEBUG
	districtDistMap.PrintMap("/tmp/districtDist.pbm", ALGORITHM_VORONOI_FIELDS::MAX_DIST, 0);
	districtIDMap.PrintMap("/tmp/districtID.pbm", NextFreeID, 0);
#endif			// ~DEBUG

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
	for(tmpCurPos.X = 0; tmpCurPos.X < IDMap.GetWidth(); ++tmpCurPos.X)
	{
		for(tmpCurPos.Y = 0; tmpCurPos.Y < IDMap.GetHeight(); ++tmpCurPos.Y)
		{
			const ALGORITHM_VORONOI_FIELDS::ID &curID = districtIDMap.GetPixel(tmpCurPos);

			// Only override data of Districts To Divide
			if(curID != ALGORITHM_VORONOI_FIELDS::INVALID_ID)
				IDMap.SetPixel(tmpCurPos, curID);
		}
	}

#ifdef DEBUG	// DEBUG
	districtDistMap.PrintMap("/tmp/districtDist.pbm", ALGORITHM_VORONOI_FIELDS::MAX_DIST, 0);
	districtIDMap.PrintMap("/tmp/finalID.pbm", NextFreeID, 0);
#endif			// ~DEBUG


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
	// Call expansion function
	CurDistrict.SetDistrictFromIDMap(IDMap, CurDistrict.GetLocalPosInDistrict()+DistrictSize.MinPos);

//	// Set map to correct size
//	CurDistrict.ResizeMap(DistrictSize.GetWidth(), DistrictSize.GetHeight());

//	// Set global position of one cell in district
//	CurDistrict.SetLocalPosInDistrict(CurDistrict.GetGlobalMapPosition());

//	// Set global position
//	CurDistrict.SetGlobalMapPosition(DistrictSize.MinPos);

//	// Convert global pos to local pos
//	CurDistrict.SetLocalPosInDistrict(CurDistrict.ConvertToLocalPosition(CurDistrict.GetLocalPosInDistrict()));

//	// Go through all positions of map and set them
//	POS_2D localPos;
//	for(localPos.X = 0; localPos.X < CurDistrict.GetWidth(); ++localPos.X)
//	{
//		for(localPos.Y = 0; localPos.Y < CurDistrict.GetHeight(); ++localPos.Y)
//		{
//			const POS_2D globalPos = CurDistrict.ConvertToGlobalPosition(localPos);
//			if(IDMap.GetPixel(globalPos) == CurDistrict.GetID())
//				CurDistrict.SetPixel(localPos, DISTRICT_MAP::IN_DISTRICT);
//			else
//				CurDistrict.SetPixel(localPos, !DISTRICT_MAP::IN_DISTRICT);
//		}
//	}
}

template<class T>
int AlgorithmVoronoiFields<T>::GetShortestDistPoses(const ALGORITHM_VORONOI_FIELDS::DIST_MAP &CompleteSkelMap, const POS_2D &StartPos, const ALGORITHM_VORONOI_FIELDS::DIST_MAP::CELL_TYPE &MinDist, const ALGORITHM_VORONOI_FIELDS::ID_MAP &IDMap, const ALGORITHM_VORONOI_FIELDS::DIST_MAP &DistMap, ALGORITHM_VORONOI_FIELDS::CHECKED_POSITIONS_MAP &PositionsChecked, ALGORITHM_VORONOI_FIELDS::CHECKED_POSITIONS_MAP::CELL_TYPE &NextFreeID, ALGORITHM_VORONOI_FIELDS::SHORTEST_DIST_POS_VECTOR &ShortestDistPoses)
{
	ALGORITHM_VORONOI_FIELDS::DIST_MAP::CELL_TYPE curDist = CompleteSkelMap.GetPixel(StartPos);
	if(curDist == ALGORITHM_VORONOI_FIELDS::MAX_DIST)
		return 0;		// Start Position is not on Skeleton Map, nothing to do

	// Add StartPos to shortest dit array
	SKEL_MAP_SHORTEST_DIST_POS tmpShortestDist;
	tmpShortestDist.Position = StartPos;
	tmpShortestDist.Distance = curDist;
	tmpShortestDist.DistToPrevElement = ALGORITHM_VORONOI_FIELDS::MAX_DIST;
	tmpShortestDist.PrevElementID = ALGORITHM_VORONOI_FIELDS::SKEL_MAP_SHORTEST_DIST_POS_INVALID_ID;
	ALGORITHM_VORONOI_FIELDS::SKEL_MAP_SHORTEST_DIST_POS_VECTOR newShortestDistances;
	newShortestDistances.push_back(tmpShortestDist);

	// Go through skeleton and find all minimum positions
	std::vector<POS_2D> nextAdjacentPositions;
	std::queue<SKEL_MAP_POS_DATA> posToCheck;
	posToCheck.push(SKEL_MAP_POS_DATA(StartPos, 0, 0));
	do
	{
		nextAdjacentPositions.clear();
		const SKEL_MAP_POS_DATA &curPosData = posToCheck.front();;
		const POS_2D &curPos = curPosData;

		// Get current distance
		curDist = CompleteSkelMap.GetPixel(curPos);

		// Set position as checked
		PositionsChecked.SetPixel(curPos, NextFreeID);

		// Find first adjacent positions
		bool isSmallestDist = false;		// Check if a smaller dist exists
		unsigned char numNewSameDists = 0;		// number of same distances adjacent to current pos
		unsigned char numOldSameDists = 0;		// number of same distances adjacent to current pos
		for(const auto &navOption : NavigationOptions)
		{
			const POS_2D adjacentPos = curPos+navOption;

			ALGORITHM_VORONOI_FIELDS::DIST_MAP::CELL_TYPE adjacentDist;
			if(CompleteSkelMap.GetPixel(adjacentPos, adjacentDist) < 0)
				continue; // Not on map, skip

			if(adjacentDist == ALGORITHM_VORONOI_FIELDS::MAX_DIST)
				continue;	// Not on skeleton map, skip

			const ALGORITHM_VORONOI_FIELDS::ID adjacentPosChecked = PositionsChecked.GetPixel(adjacentPos);

			if(adjacentDist == curDist)
			{
				if(adjacentPosChecked == NextFreeID)
					numOldSameDists++;
				else if(adjacentPosChecked == ALGORITHM_VORONOI_FIELDS::INVALID_ID)
					numNewSameDists++;
			}
			else if(adjacentDist < curDist)
			{
				isSmallestDist = false;
			}

			if(adjacentPosChecked != NextFreeID)
				nextAdjacentPositions.push_back(adjacentPos);		// Check adjacent position next
		}

		// Check if this position warrants a new shortest dist entry
		if(isSmallestDist || (numOldSameDists == 0 && numNewSameDists != 0) || (numOldSameDists != 0 && numNewSameDists == 0))
		{
			// Add this position to array
			tmpShortestDist.Position = curPos;
			tmpShortestDist.Distance = curDist;
			tmpShortestDist.DistToPrevElement = curPosData.DistToPrevElement;
			tmpShortestDist.PrevElementID = curPosData.PrevElementID;
			newShortestDistances.push_back(tmpShortestDist);

			// Add all adjacent positions
			for(const auto &curAdjacentPosition : nextAdjacentPositions)
				posToCheck.push(SKEL_MAP_POS_DATA(curAdjacentPosition, static_cast<ALGORITHM_VORONOI_FIELDS::DIST_MAP::CELL_TYPE>(GetMovementCost(curPos, curAdjacentPosition)), newShortestDistances.size()-1));
		}
		else
		{
			// Add all adjacent positions
			for(const auto &curAdjacentPosition : nextAdjacentPositions)
				posToCheck.push(SKEL_MAP_POS_DATA(curAdjacentPosition, curPosData.DistToPrevElement+static_cast<ALGORITHM_VORONOI_FIELDS::DIST_MAP::CELL_TYPE>(GetMovementCost(curPos, curAdjacentPosition)), curPosData.PrevElementID));
		}

		// Element checked, remove it
		posToCheck.pop();
	}
	while(posToCheck.size() > 0);

	// After all elements have been found, remove those that are too close to another one
	std::queue<SKEL_MAP_SHORTEST_DIST_POS*> distsToCheck;
	distsToCheck.push(&newShortestDistances.at(0));
	do
	{
		SKEL_MAP_SHORTEST_DIST_POS &curDist = *(distsToCheck.front());

		// Get closest distances and remove those that are too close to each other
		if(curDist.ValidElement == true)
		{
			while(1)
			{
				ALGORITHM_VORONOI_FIELDS::DIST_MAP::CELL_TYPE distance;
				SKEL_MAP_SHORTEST_DIST_POS *pClosestDist = newShortestDistances.FindClosestDist(curDist, distance);

				if(distance < MinDist)
				{
					// If distance is too small, remove the one with the larger inter-obstacle distance
					if(pClosestDist->Distance < curDist.Distance)
					{
						curDist.ValidElement = false;
						break;		// STOP, if this element is no longer valid
					}
					else
						pClosestDist->ValidElement = false;
				}
				else
					break;		// STOP, if the closest element is far enough away
			}
		}

		// Now add all following elements to array
		for(auto &nextElement : curDist.NextElementIDs)
			distsToCheck.push(&newShortestDistances.at(nextElement));

		// Remove current element from buffer
		distsToCheck.pop();
	}
	while(distsToCheck.size() > 0);

	// Now add all remaining distances to buffer
	for(const auto &curDist : newShortestDistances)
	{
		if(curDist.ValidElement)
		{
			ShortestDistPoses.Poses.push_back(curDist.ConvertToShortestDistPos(IDMap, DistMap));
		}
	}

	return 1;
}

template<class T>
int AlgorithmVoronoiFields<T>::SetPathToDistrict(const ALGORITHM_VORONOI_FIELDS::DIST_MAP &SkelDistMap, const POS_2D &StartPos, const ALGORITHM_VORONOI_FIELDS::ID &IDToSetPathTo, const ALGORITHM_VORONOI_FIELDS::DIST_MAP::CELL_TYPE &DistToSetTo, ALGORITHM_VORONOI_FIELDS::ID_MAP &IDMap, ALGORITHM_VORONOI_FIELDS::DIST_MAP &DistMap, ALGORITHM_VORONOI_FIELDS::DISTRICT_SIZE *DistrictSize, typename AlgorithmDStar<T>::PATH_VECTOR *Path)
{
	typename AlgorithmDStar<T>::PATH_VECTOR tmpPath;
	ALGORITHM_VORONOI_FIELDS::DISTRICT_SIZE tmpDistrictSize(IDToSetPathTo, StartPos);

	// Start at beginning
	POS_2D curPos = StartPos;
	ALGORITHM_VORONOI_FIELDS::DIST_MAP::CELL_TYPE curDist = SkelDistMap.GetPixel(curPos);
	const ALGORITHM_VORONOI_FIELDS::ID idOfDistrict = IDMap.GetPixel(curPos);

	IDMap.SetPixel(curPos, IDToSetPathTo);
	DistMap.SetPixel(curPos, DistToSetTo);
	tmpPath.push_back(curPos);

	// Continue along path until outer district is reached (stop at distance 1 so as not to add outer district cell)
	while(curDist > 1)
	{
		ALGORITHM_VORONOI_FIELDS::DIST_MAP::CELL_TYPE bestDist = curDist;
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
		curDist = bestDist;

		IDMap.SetPixel(curPos, IDToSetPathTo);
		DistMap.SetPixel(curPos, DistToSetTo);		// Set distance to zero
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
int AlgorithmVoronoiFields<T>::SetWholePathToDistrict(const ALGORITHM_VORONOI_FIELDS::DIST_MAP &SkelDistMap, const ALGORITHM_VORONOI_FIELDS::SHORTEST_DIST_POS &ShortestDistPos, const ALGORITHM_VORONOI_FIELDS::ID &IDToSetPathTo, const ALGORITHM_VORONOI_FIELDS::DIST_MAP::CELL_TYPE &DistToSetTo, ALGORITHM_VORONOI_FIELDS::ID_MAP &IDMap, ALGORITHM_VORONOI_FIELDS::DIST_MAP &DistMap, ALGORITHM_VORONOI_FIELDS::DISTRICT_SIZE *DistrictSize, typename AlgorithmDStar<T>::PATH_VECTOR *Path)
{
	int tmp1, tmp2;
	ALGORITHM_VORONOI_FIELDS::DISTRICT_SIZE tmpDistrict1(IDToSetPathTo, ShortestDistPos.ShortestPosID1);
	ALGORITHM_VORONOI_FIELDS::DISTRICT_SIZE tmpDistrict2(tmpDistrict1);
	typename AlgorithmDStar<T>::PATH_VECTOR tmpPath1, tmpPath2;

	// Divide district along path from shortest positions to district, and set path to an invalid id (this separates the old and new district)
	tmp1 = AlgorithmVoronoiFields<T>::SetPathToDistrict(SkelDistMap, ShortestDistPos.ShortestPosID1, IDToSetPathTo, DistToSetTo, IDMap, DistMap, &tmpDistrict1, &tmpPath1);
	tmp2 = AlgorithmVoronoiFields<T>::SetPathToDistrict(SkelDistMap, ShortestDistPos.ShortestPosID2, IDToSetPathTo, DistToSetTo, IDMap, DistMap, &tmpDistrict2, &tmpPath2);

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

	// Save size if requested
	if(DistrictSize != nullptr)
		*DistrictSize = std::move(tmpDistrictSize);

	// Check if pixels where found
	if(tmpPath.size() == 0)
	{
		if(Path != nullptr)
			Path->clear();

		return -1;		// return error if no pixels where set
	}

	// Save path if requested
	if(Path != nullptr)
		*Path = std::move(tmpPath);

	return 1;
}

template<class T>
void AlgorithmVoronoiFields<T>::CombineTwoIDs(const ALGORITHM_VORONOI_FIELDS::ID &OriginalID, const ALGORITHM_VORONOI_FIELDS::ID &ReplacementID, const POS_2D &ConnectionPos, ALGORITHM_VORONOI_FIELDS::ID_MAP &IDMap)
{
	// Skip if both IDs are the same
	if(OriginalID == ReplacementID)
		return;

	if(IDMap.GetPixel(ConnectionPos) != OriginalID)
		return;

	IDMap.SetPixel(ConnectionPos, ReplacementID);

	std::queue<POS_2D> posToCheck;

	posToCheck.push(ConnectionPos);
	do
	{
		const POS_2D &curPos = posToCheck.front();

		for(const auto &navOption : NavigationOptions)
		{
			const POS_2D adjacentPos = curPos + navOption;

			// Check if pos exists
			ALGORITHM_VORONOI_FIELDS::ID adjacentID;
			if(IDMap.GetPixel(adjacentPos, adjacentID) < 0)
				continue;

			if(adjacentID == OriginalID)
			{
				IDMap.SetPixel(adjacentPos, ReplacementID);
				posToCheck.push(adjacentPos);
			}
		}

		posToCheck.pop();
	}
	while(posToCheck.size() > 0);
}

#endif // ALGORITHM_VORONOI_FIELDS_TEMPLATE_H
