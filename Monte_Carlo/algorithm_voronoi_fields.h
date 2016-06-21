#ifndef ALGORITHM_VORONOI_FIELDS_H
#define ALGORITHM_VORONOI_FIELDS_H

/*	class AlgorithmVoronoiFields
 *		Calculates Voronoi Field of a given map
 */

#include "standard_definitions.h"
#include "map_2d.h"
#include "district_map.h"

#include <vector>

namespace ALGORITHM_VORONOI_FIELDS
{
	typedef std::vector<DistrictMap>	DISTRICT_STORAGE;

	typedef DISTRICT_MAP::ID			ID;
	typedef Map2D<ID>					ID_MAP;
	const ID INVALID_ID = DISTRICT_MAP::INVALID_DISTRICT_ID;

	typedef Map2D_Discrete				OCCUPATION_MAP;
	const bool CELL_OCCUPIED = true;

	// Struct to temporarily store size of district (Min- and MaxPos form rectangle with all cells in them)
	struct DISTRICT_SIZE
	{
		ID DistrictID;		// ID of district
		POS_2D MinPos;		// Minimum position of cells
		POS_2D MaxPos;		// Maximum position of cells

		DISTRICT_SIZE(const ID &NewID, const POS_2D &NewPos) : DistrictID(NewID), MinPos(NewPos), MaxPos(NewPos) {}
		DISTRICT_SIZE() = delete;

		void ComparePos(const POS_2D &Pos);
	};

	struct DISTRICT_SIZE_VECTOR
	{
		std::vector<DISTRICT_SIZE> Sizes;

		DISTRICT_SIZE *FindID(const ID &IDToFind);
	};
}

template<class T>
class AlgorithmVoronoiFields
{
	public:

		static void CalculateVoronoiField(const Map2D<T> &OriginalMap, const DistrictMap &OriginalDistrictData, ALGORITHM_VORONOI_FIELDS::DISTRICT_STORAGE &OccupiedDistricts, ALGORITHM_VORONOI_FIELDS::DISTRICT_STORAGE &FreeDistricts);
		static void CalculateVoronoiField(const Map2D<T> &OriginalMap, const T &CutOffValue, const DistrictMap &OriginalDistrictData, ALGORITHM_VORONOI_FIELDS::DISTRICT_STORAGE &OccupiedDistricts, ALGORITHM_VORONOI_FIELDS::DISTRICT_STORAGE &FreeDistricts);

	private:

		static T CalculateMapAverage(const Map2D<T> &OriginalMap, const DistrictMap &OriginalDistrictData);
		static void SeparateMapIntoUnconnectedDistricts(const Map2D<T> &OriginalMap, const DistrictMap &OriginalDistrictData, const T &CutOffValue, ALGORITHM_VORONOI_FIELDS::DISTRICT_STORAGE &OccupiedDistricts, ALGORITHM_VORONOI_FIELDS::DISTRICT_STORAGE &FreeDistricts, ALGORITHM_VORONOI_FIELDS::ID_MAP &IDMap, ALGORITHM_VORONOI_FIELDS::OCCUPATION_MAP & OccupationMap);

		static void CombineTwoIDs(const ALGORITHM_VORONOI_FIELDS::ID &OrignalID, const ALGORITHM_VORONOI_FIELDS::ID &IDToCombine, const POS_2D &ConnectionPos, ALGORITHM_VORONOI_FIELDS::ID_MAP &IDMap, ALGORITHM_VORONOI_FIELDS::DISTRICT_SIZE &DistrictSize);
		static void ExpandDistrict(const ALGORITHM_VORONOI_FIELDS::ID_MAP &IDMap, const ALGORITHM_VORONOI_FIELDS::DISTRICT_SIZE &DistrictSize, DistrictMap &CurDistrict);
};

#endif // ALGORITHM_VORONOI_FIELDS_H
