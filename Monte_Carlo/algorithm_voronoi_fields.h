#ifndef ALGORITHM_VORONOI_FIELDS_H
#define ALGORITHM_VORONOI_FIELDS_H

/*	class AlgorithmVoronoiFields
 *		Calculates Voronoi Field of a given map
 */

#include "standard_definitions.h"
#include "map_2d.h"
#include "district_map.h"
#include "algorithm_d_star.h"

#include <vector>

namespace ALGORITHM_VORONOI_FIELDS
{
	typedef DISTRICT_MAP::ID			ID;
	typedef Map2D<ID>					ID_MAP;
	const ID INVALID_ID = DISTRICT_MAP::INVALID_DISTRICT_ID;
	//const ID SEPARATION_ID = INVALID_ID;

	typedef D_STAR_DIST_MAP				DIST_MAP;
	const DIST_MAP::CELL_TYPE			MAX_DIST = GetInfiniteVal<DIST_MAP::CELL_TYPE>();

	typedef Map2D_Discrete				OCCUPATION_MAP;
	const OCCUPATION_MAP::CELL_TYPE CELL_OCCUPIED = true;

	// Storage of districts
	struct DISTRICT_STORAGE : public std::vector<DistrictMap>
	{
		DistrictMap *FindDistrictID(const ID &DistrictID);
		DISTRICT_STORAGE::size_type FindDistrictIDIterator(const ID &DistrictID);
	};

	// Keep track of district sizes
	typedef DISTRICT_MAP::DISTRICT_SIZE DISTRICT_SIZE;
	typedef DISTRICT_MAP::DISTRICT_SIZE_VECTOR DISTRICT_SIZE_VECTOR;

	// Keep track of connected districts
	typedef DISTRICT_MAP::ID_CONNECTION ID_CONNECTION;
	typedef DISTRICT_MAP::ID_CONNECTION_VECTOR ID_CONNECTION_VECTOR;

	// Struct for storing shortest connection between districts
	struct SHORTEST_DIST_POS
	{
		ID DistrictID1;			// The two connected districtd
		ID DistrictID2;

		DIST_MAP::CELL_TYPE Distance;		// Distance of this point to districts
		POS_2D ShortestPosID1;				// Position that is halfway between both points in area of district ID1
		POS_2D ShortestPosID2;				// Position that is halfway between both points in area of district ID2

		SHORTEST_DIST_POS(const ID &_ID1, const ID &_ID2, const DIST_MAP::CELL_TYPE &_Distance, const POS_2D &_ShortestPosID1, const POS_2D &_ShortestPosID2) : DistrictID1(_ID1), DistrictID2(_ID2), Distance(_Distance), ShortestPosID1(_ShortestPosID1), ShortestPosID2(_ShortestPosID2) {}
	};

	struct SHORTEST_DIST_POS_VECTOR
	{
		std::vector<SHORTEST_DIST_POS> Poses;

		SHORTEST_DIST_POS *FindID(const ID &ID1ToFind, const ID &ID2ToFind);
	};

	// Keep track of district changes
	typedef std::vector<POS_2D> PATH_VECTOR;
	struct DISTRICT_CHANGE
	{
		ID OldID;			// ID of old district
		ID NewID;			// ID of new district on opposite side of old district
		POS_2D ChangeOldIDPos;
		POS_2D ChangeNewIDPos;
		PATH_VECTOR DivisionOldIDPath;
		PATH_VECTOR DivisionNewIDPath;
	};

	struct DISTRICT_CHANGE_VECTOR : public std::vector<DISTRICT_CHANGE>
	{
		DISTRICT_CHANGE_VECTOR() = default;

		bool AreChangedIDs(const ID &DistrictID1, const ID &DistrictID2) const;
	};

	typedef Map2D<ID> CHECKED_POSITIONS_MAP;

	// Contains both shortest dist position and also distance to next e
	struct SKEL_MAP_SHORTEST_DIST_POS;
	struct SKEL_MAP_SHORTEST_DIST_POS_VECTOR : public std::vector<SKEL_MAP_SHORTEST_DIST_POS>
	{
		void push_back(const value_type &element);
		void push_back(value_type &&element);

		SKEL_MAP_SHORTEST_DIST_POS *FindClosestDist(const SKEL_MAP_SHORTEST_DIST_POS &CurDist, ALGORITHM_VORONOI_FIELDS::DIST_MAP::CELL_TYPE &Distance) const;
		SKEL_MAP_SHORTEST_DIST_POS *FindClosestDistForward(const SKEL_MAP_SHORTEST_DIST_POS &CurDist, ALGORITHM_VORONOI_FIELDS::DIST_MAP::CELL_TYPE &Distance) const;
	};
	typedef SKEL_MAP_SHORTEST_DIST_POS_VECTOR::size_type SKEL_MAP_SHORTEST_DIST_POS_ID;
	const SKEL_MAP_SHORTEST_DIST_POS_ID SKEL_MAP_SHORTEST_DIST_POS_INVALID_ID = GetInfiniteVal<SKEL_MAP_SHORTEST_DIST_POS_ID>();

	struct SKEL_MAP_DATA
	{
		DIST_MAP::CELL_TYPE DistToPrevElement;
		SKEL_MAP_SHORTEST_DIST_POS_ID PrevElementID;

		SKEL_MAP_DATA() = default;
		SKEL_MAP_DATA(const DIST_MAP::CELL_TYPE &_DistToPrevElement, const SKEL_MAP_SHORTEST_DIST_POS_ID &_PrevElementID);
	};

	struct SKEL_MAP_POS_DATA : public POS_2D, public SKEL_MAP_DATA
	{
		SKEL_MAP_POS_DATA(const POS_2D &_Pos, const SKEL_MAP_DATA &_SkelMapData);
		SKEL_MAP_POS_DATA(const POS_2D &_Pos, const DIST_MAP::CELL_TYPE &_DistToPrevElement, const SKEL_MAP_SHORTEST_DIST_POS_ID &_PrevElementID);
	};

	struct SKEL_MAP_SHORTEST_DIST_POS : public SKEL_MAP_DATA
	{
		POS_2D Position;
		DIST_MAP::CELL_TYPE Distance;
		std::vector<SKEL_MAP_SHORTEST_DIST_POS_ID> NextElementIDs;
		bool ValidElement = true;

		SHORTEST_DIST_POS ConvertToShortestDistPos(const ID_MAP &IDMap, const DIST_MAP &DistMap) const;
	};
}

template<class T>
class AlgorithmVoronoiFields
{
		typedef ALGORITHM_VORONOI_FIELDS::SKEL_MAP_DATA					SKEL_MAP_DATA;
		typedef ALGORITHM_VORONOI_FIELDS::SKEL_MAP_POS_DATA				SKEL_MAP_POS_DATA;
		typedef ALGORITHM_VORONOI_FIELDS::SKEL_MAP_SHORTEST_DIST_POS	SKEL_MAP_SHORTEST_DIST_POS;
	public:

		static void CalculateVoronoiField(const Map2D<T> &OriginalMap, const ALGORITHM_VORONOI_FIELDS::DIST_MAP::CELL_TYPE &MinDistBetweenDistricts, const DistrictMap &OriginalDistrictData, ALGORITHM_VORONOI_FIELDS::DISTRICT_STORAGE &OccupiedDistricts, ALGORITHM_VORONOI_FIELDS::DISTRICT_STORAGE &FreeDistricts, ALGORITHM_VORONOI_FIELDS::ID &NextFreeDistrictID, ALGORITHM_VORONOI_FIELDS::ID_MAP *IDMap = nullptr);
		static void CalculateVoronoiField(const Map2D<T> &OriginalMap, const T &CutOffValue, const ALGORITHM_VORONOI_FIELDS::DIST_MAP::CELL_TYPE &MinDistBetweenDistricts, const DistrictMap &OriginalDistrictData, ALGORITHM_VORONOI_FIELDS::DISTRICT_STORAGE &OccupiedDistricts, ALGORITHM_VORONOI_FIELDS::DISTRICT_STORAGE &FreeDistricts, ALGORITHM_VORONOI_FIELDS::ID &NextFreeDistrictID, ALGORITHM_VORONOI_FIELDS::ID_MAP *IDMap = nullptr);

	private:

		static T CalculateMapAverage(const Map2D<T> &OriginalMap, const DistrictMap &OriginalDistrictData);

		// Methods for separating map
		static void SeparateMapIntoUnconnectedDistricts(const Map2D<T> &OriginalMap, const DistrictMap &OriginalDistrictData, const T &CutOffValue, ALGORITHM_VORONOI_FIELDS::DISTRICT_STORAGE &OccupiedDistricts, ALGORITHM_VORONOI_FIELDS::DISTRICT_STORAGE &FreeDistricts, ALGORITHM_VORONOI_FIELDS::ID_MAP &IDMap, ALGORITHM_VORONOI_FIELDS::OCCUPATION_MAP & OccupationMap, ALGORITHM_VORONOI_FIELDS::ID &NextFreeID);
		static int SeparateByShortestDistance(const DistrictMap &OriginalDistrictMap, const ALGORITHM_VORONOI_FIELDS::OCCUPATION_MAP &OccupationMap, const ALGORITHM_VORONOI_FIELDS::OCCUPATION_MAP::CELL_TYPE &OccupationLevelOfDistrictsToDivide, const ALGORITHM_VORONOI_FIELDS::DIST_MAP::CELL_TYPE &MinDistBetweenDistricts, const ALGORITHM_VORONOI_FIELDS::DISTRICT_STORAGE &OuterDistricts, ALGORITHM_VORONOI_FIELDS::DISTRICT_STORAGE &DistrictsToDivide, ALGORITHM_VORONOI_FIELDS::ID &NextFreeID, ALGORITHM_VORONOI_FIELDS::ID_MAP &IDMap);

		// Extra methods used in SeparateMapIntoUnconnectedDistricts()
		static void CreateTotalDistrict(const Map2D<T> &OriginalMap, const DistrictMap &OriginalDistrictData, const POS_2D &GlobalStartPos, const T &CutOffValue, const ALGORITHM_VORONOI_FIELDS::OCCUPATION_MAP::CELL_TYPE &OccupationLevel, ALGORITHM_VORONOI_FIELDS::DISTRICT_SIZE &DistrictSize, ALGORITHM_VORONOI_FIELDS::ID_MAP &IDMap, ALGORITHM_VORONOI_FIELDS::OCCUPATION_MAP &OccupationMap);
		static void CombineTwoIDs(const ALGORITHM_VORONOI_FIELDS::ID &OrignalID, const ALGORITHM_VORONOI_FIELDS::ID &IDToCombine, const POS_2D &GlobalConnectionPos, const POS_2D &IDMapConnectionPos, ALGORITHM_VORONOI_FIELDS::ID_MAP &IDMap, ALGORITHM_VORONOI_FIELDS::DISTRICT_SIZE_VECTOR &DistrictSizeStorage, ALGORITHM_VORONOI_FIELDS::DISTRICT_STORAGE &DistrictStorage);
		static void ExpandDistrict(const ALGORITHM_VORONOI_FIELDS::ID_MAP &IDMap, const ALGORITHM_VORONOI_FIELDS::DISTRICT_SIZE &DistrictSize, DistrictMap &CurDistrict);

		// Extra methods used in SeparateByShortestDistance()
		static int GetShortestDistPoses(const ALGORITHM_VORONOI_FIELDS::DIST_MAP &CompleteSkelMap, const POS_2D &StartPos, const ALGORITHM_VORONOI_FIELDS::DIST_MAP::CELL_TYPE &MinDist, const ALGORITHM_VORONOI_FIELDS::ID_MAP &IDMap, const ALGORITHM_VORONOI_FIELDS::DIST_MAP &DistMap, ALGORITHM_VORONOI_FIELDS::CHECKED_POSITIONS_MAP &PositionsChecked, ALGORITHM_VORONOI_FIELDS::CHECKED_POSITIONS_MAP::CELL_TYPE &NextFreeID, ALGORITHM_VORONOI_FIELDS::SHORTEST_DIST_POS_VECTOR &ShortestDistPoses);
		static int SetPathToDistrict(const ALGORITHM_VORONOI_FIELDS::DIST_MAP &SkelDistMap, const POS_2D &StartPos, const ALGORITHM_VORONOI_FIELDS::ID &IDToSetPathTo, const ALGORITHM_VORONOI_FIELDS::DIST_MAP::CELL_TYPE &DistToSetTo, ALGORITHM_VORONOI_FIELDS::ID_MAP &IDMap, ALGORITHM_VORONOI_FIELDS::DIST_MAP &DistMap, ALGORITHM_VORONOI_FIELDS::DISTRICT_SIZE *DistrictSize = nullptr, typename AlgorithmDStar<T>::PATH_VECTOR *Path = nullptr);
		static int SetWholePathToDistrict(const ALGORITHM_VORONOI_FIELDS::DIST_MAP &SkelDistMap, const ALGORITHM_VORONOI_FIELDS::SHORTEST_DIST_POS &ShortestDistPos, const ALGORITHM_VORONOI_FIELDS::ID &IDToSetPathTo, const ALGORITHM_VORONOI_FIELDS::DIST_MAP::CELL_TYPE &DistToSetTo, ALGORITHM_VORONOI_FIELDS::ID_MAP &IDMap, ALGORITHM_VORONOI_FIELDS::DIST_MAP &DistMap, ALGORITHM_VORONOI_FIELDS::DISTRICT_SIZE *DistrictSize = nullptr, typename AlgorithmDStar<T>::PATH_VECTOR *Path = nullptr);
		static int CreateParellelPath(const POS_2D &StartPos, const ALGORITHM_VORONOI_FIELDS::ID &IDToSetPathTo, const ALGORITHM_VORONOI_FIELDS::ID &ParallelID, ALGORITHM_VORONOI_FIELDS::ID_MAP &IDMap, ALGORITHM_VORONOI_FIELDS::DIST_MAP &DistMap, ALGORITHM_VORONOI_FIELDS::DISTRICT_SIZE *DistrictSize = nullptr, typename AlgorithmDStar<T>::PATH_VECTOR *Path = nullptr);
		static void CombineTwoIDs(const ALGORITHM_VORONOI_FIELDS::ID &OrignalID, const ALGORITHM_VORONOI_FIELDS::ID &ReplacementID, const POS_2D &ConnectionPos, ALGORITHM_VORONOI_FIELDS::ID_MAP &IDMap);

		static ALGORITHM_VORONOI_FIELDS::OCCUPATION_MAP::CELL_TYPE IsOccupiedCell(const T &CellValue, const T &CutOffValue)  { return (CellValue > CutOffValue ? ALGORITHM_VORONOI_FIELDS::CELL_OCCUPIED : !ALGORITHM_VORONOI_FIELDS::CELL_OCCUPIED); }	
};

#include "algorithm_voronoi_fields_template.cpp"

#endif // ALGORITHM_VORONOI_FIELDS_H
