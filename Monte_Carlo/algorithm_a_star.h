#ifndef ALGORITHM_A_STAR_H
#define ALGORITHM_A_STAR_H

#include "standard_definitions.h"
#include "map_2d.h"
#include "district_map.h"
#include "robot_navigation.h"

#include <list>

namespace ALGORITHM_A_STAR
{
	typedef std::vector<POS_2D> PATH_DATA;

	typedef MOVE_DIST_TYPE DISTANCE_TYPE;			// Approximate Distance to goal

	// Structure to save position and distances during A* search
	struct POS_DIST : public POS_2D
	{
		DISTANCE_TYPE Distance;

		POS_DIST() = default;
		POS_DIST(const POS_2D &_Position, const DISTANCE_TYPE &_Distance);
	};

	struct POSTOCHECK : std::list<POS_DIST>
	{
		void AddElement(const POS_DIST &Element);
	};
}

class AlgorithmAStar
{
	public:
		typedef ALGORITHM_A_STAR::PATH_DATA PATH_DATA;
		typedef ALGORITHM_A_STAR::POS_DIST POS_DIST;
		typedef ALGORITHM_A_STAR::DISTANCE_TYPE DISTANCE_TYPE;
		typedef ALGORITHM_A_STAR::POSTOCHECK POSTOCHECK;

		AlgorithmAStar() = default;

		template<class T>
		static bool StaticCalculatePath(const Map2D<T> &Map, const T &CutOffValue, const POS_2D &StartPos, const POS_2D &Destination, const DistrictMap *const DistrictData, PATH_DATA *const Path, DISTANCE_TYPE *const Distance);		//	Calculates the path from start to dest (if DistrictData is given, the bot will remain in the district)

		template<class T>
		bool CalculatePath(const Map2D<T> &Map, const T &CutOffValue, const POS_2D &StartPos, const POS_2D &Destination, const DistrictMap *const DistrictData, PATH_DATA *const Path, DISTANCE_TYPE *const Distance);		//	Calculates the path from start to dest (if DistrictData is given, the bot will remain in the district)

	private:

		static DISTANCE_TYPE ApproximateDistToGoal(const POS_2D &CurPos, const POS_2D &Destination);

		Map2D<DISTANCE_TYPE> DistMap;

};

#endif // ALGORITHM_A_STAR_H
