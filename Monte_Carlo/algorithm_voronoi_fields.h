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
	struct DISTRICT_SIZE_VECTOR
	{
		typedef std::vector<DISTRICT_SIZE> STORAGE_TYPE;
		STORAGE_TYPE Sizes;

		DISTRICT_SIZE *FindID(const ID &IDToFind);
		STORAGE_TYPE::size_type FindIDIterator(const ID &IDToFind);
	};

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
}

template<class T>
class AlgorithmVoronoiFields
{
	public:

		static void CalculateVoronoiField(const Map2D<T> &OriginalMap, const DistrictMap &OriginalDistrictData, ALGORITHM_VORONOI_FIELDS::DISTRICT_STORAGE &OccupiedDistricts, ALGORITHM_VORONOI_FIELDS::DISTRICT_STORAGE &FreeDistricts, ALGORITHM_VORONOI_FIELDS::ID &NextFreeDistrictID);
		static void CalculateVoronoiField(const Map2D<T> &OriginalMap, const T &CutOffValue, const DistrictMap &OriginalDistrictData, ALGORITHM_VORONOI_FIELDS::DISTRICT_STORAGE &OccupiedDistricts, ALGORITHM_VORONOI_FIELDS::DISTRICT_STORAGE &FreeDistricts, ALGORITHM_VORONOI_FIELDS::ID &NextFreeDistrictID);

	private:

		static T CalculateMapAverage(const Map2D<T> &OriginalMap, const DistrictMap &OriginalDistrictData);

		// Methods for separating map
		static void SeparateMapIntoUnconnectedDistricts(const Map2D<T> &OriginalMap, const DistrictMap &OriginalDistrictData, const T &CutOffValue, ALGORITHM_VORONOI_FIELDS::DISTRICT_STORAGE &OccupiedDistricts, ALGORITHM_VORONOI_FIELDS::DISTRICT_STORAGE &FreeDistricts, ALGORITHM_VORONOI_FIELDS::ID_MAP &IDMap, ALGORITHM_VORONOI_FIELDS::OCCUPATION_MAP & OccupationMap, ALGORITHM_VORONOI_FIELDS::ID &NextFreeID);
		static void SeparateByShortestDistance(const DistrictMap &OriginalDistrictMap, const ALGORITHM_VORONOI_FIELDS::OCCUPATION_MAP &OccupationMap, const ALGORITHM_VORONOI_FIELDS::OCCUPATION_MAP::CELL_TYPE &OccupatioLevel, ALGORITHM_VORONOI_FIELDS::DISTRICT_STORAGE &Districts, ALGORITHM_VORONOI_FIELDS::ID &NextFreeID, ALGORITHM_VORONOI_FIELDS::ID_MAP &IDMap);

		// Extra methods used in SeparateMapIntoUnconnectedDistricts()
		static void CreateTotalDistrict(const Map2D<T> &OriginalMap, const DistrictMap &OriginalDistrictData, const POS_2D &GlobalStartPos, const T &CutOffValue, const ALGORITHM_VORONOI_FIELDS::OCCUPATION_MAP::CELL_TYPE &OccupationLevel, ALGORITHM_VORONOI_FIELDS::DISTRICT_SIZE &DistrictSize, ALGORITHM_VORONOI_FIELDS::ID_MAP &IDMap, ALGORITHM_VORONOI_FIELDS::OCCUPATION_MAP &OccupationMap);
		static void CombineTwoIDs(const ALGORITHM_VORONOI_FIELDS::ID &OrignalID, const ALGORITHM_VORONOI_FIELDS::ID &IDToCombine, const POS_2D &GlobalConnectionPos, const POS_2D &IDMapConnectionPos, ALGORITHM_VORONOI_FIELDS::ID_MAP &IDMap, ALGORITHM_VORONOI_FIELDS::DISTRICT_SIZE_VECTOR &DistrictSizeStorage, ALGORITHM_VORONOI_FIELDS::DISTRICT_STORAGE &DistrictStorage);
		static void ExpandDistrict(const ALGORITHM_VORONOI_FIELDS::ID_MAP &IDMap, const ALGORITHM_VORONOI_FIELDS::DISTRICT_SIZE &DistrictSize, DistrictMap &CurDistrict);

		// Extra methods used in SeparateByShortestDistance()
		static int SetPathToDistrict(const ALGORITHM_VORONOI_FIELDS::DIST_MAP &DistMap, const POS_2D &StartPos, const ALGORITHM_VORONOI_FIELDS::ID &IDToSetPathTo, ALGORITHM_VORONOI_FIELDS::ID_MAP &IDMap, typename AlgorithmDStar<T>::PATH_VECTOR &Path);

		static ALGORITHM_VORONOI_FIELDS::OCCUPATION_MAP::CELL_TYPE IsOccupiedCell(const T &CellValue, const T &CutOffValue)  { return (CellValue > CutOffValue ? ALGORITHM_VORONOI_FIELDS::CELL_OCCUPIED : !ALGORITHM_VORONOI_FIELDS::CELL_OCCUPIED); }
};

#include "algorithm_voronoi_fields_template.h"

#endif // ALGORITHM_VORONOI_FIELDS_H
