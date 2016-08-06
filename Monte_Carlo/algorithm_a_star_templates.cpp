#ifndef ALGORITHM_A_STAR_TEMPLATES_CPP
#define ALGORITHM_A_STAR_TEMPLATES_CPP

#include "algorithm_a_star.h"
#include <queue>

template<class T>
bool AlgorithmAStar::StaticCalculatePath(const Map2D<T> &Map, const T &CutOffValue, const POS_2D &StartPos, const POS_2D &Destination, const DistrictMap *const DistrictData, PATH_DATA *const Path, DISTANCE_TYPE *const Distance)
{
	AlgorithmAStar tmpStar;
	return tmpStar.CalculatePath<T>(Map, CutOffValue, StartPos, Destination, DistrictData, Path, Distance);
}

template<class T>
bool AlgorithmAStar::CalculatePath(const Map2D<T> &Map, const T &CutOffValue, const POS_2D &StartPos, const POS_2D &Destination, const DistrictMap * const DistrictData, PATH_DATA * const Path, DISTANCE_TYPE * const Distance)
{
	// Reset map
	this->DistMap.ResetMap(Map.GetWidth(), Map.GetHeight(), GetInfiniteVal<DISTANCE_TYPE>());
	this->DistMap.SetPixel(StartPos, 0);

	POSTOCHECK posToCheck;
	posToCheck.AddElement(POS_DIST(StartPos, AlgorithmAStar::ApproximateDistToGoal(StartPos, Destination)));

	DISTANCE_TYPE bestDist = GetInfiniteVal<DISTANCE_TYPE>();		// get best distance

	do
	{
		const POS_DIST &curPos = posToCheck.back();

		if(curPos.Distance > bestDist)
			break;		// Stop if no shorter distance was found

		const DISTANCE_TYPE &curDistance = this->DistMap.GetPixelR(curPos);

		for(const auto &navOption : NavigationOptions)
		{
			const POS_2D adjacentPos = curPos+navOption;

			T adjacentVal;
			if(Map.GetPixel(adjacentPos, adjacentVal) < 0)
				continue;			// Skip if not part of map

			if(adjacentVal >= CutOffValue)
				continue;			// Skip if value too large

			if(DistrictData != nullptr)
			{
				if(DistrictData->GetGlobalPixel(adjacentPos) != DISTRICT_MAP::IN_DISTRICT)
					continue;		// Skip if not in district
			}

			const auto movementCost = GetMovementCost(curPos,adjacentPos);
			DISTANCE_TYPE &r_adjacentDistance = this->DistMap.GetPixelR(adjacentPos);
			if(r_adjacentDistance > curDistance+movementCost)
			{
				r_adjacentDistance = curDistance+movementCost;

				if(adjacentPos == Destination)
					bestDist = r_adjacentDistance;			// Save new best distance
				else
					posToCheck.AddElement(POS_DIST(adjacentPos, r_adjacentDistance+AlgorithmAStar::ApproximateDistToGoal(adjacentPos, Destination)));
			}
		}

		posToCheck.pop_back();
	}
	while(!posToCheck.empty());

	if(bestDist < GetInfiniteVal<DISTANCE_TYPE>())
	{
		// If a path was found save it if requested
		if(Path != nullptr)
		{
			PATH_DATA reversePath;
			reversePath.push_back(Destination);

			auto curPos = Destination;
			do
			{
				DISTANCE_TYPE bestDist = GetInfiniteVal<DISTANCE_TYPE>();
				auto bestAdjacentPos = curPos;

				// Find best adjacent distance
				for(const auto &navOption : NavigationOptions)
				{
					const POS_2D adjacentPos = curPos+navOption;

					DISTANCE_TYPE adjacentDist;
					if(this->DistMap.GetPixel(adjacentPos, adjacentDist) < 0)
						continue;		// Skip if not part of map

					if(adjacentDist < bestDist)
					{
						bestDist = adjacentDist;
						bestAdjacentPos = adjacentPos;
					}
				}

				if(bestAdjacentPos == curPos)
					return false;		// ERROR

				curPos = bestAdjacentPos;

				// Save position
				reversePath.push_back(curPos);
			}
			while(curPos != StartPos);

			// Save Reversed path
			Path->clear();
			Path->reserve(reversePath.size());
			for(auto curPos = reversePath.rbegin(); curPos != reversePath.rend(); ++curPos)
				Path->push_back(*curPos);
		}

		if(Distance != nullptr)
			*Distance = this->DistMap.GetPixel(Destination);

		return true;
	}

	// No path found
	return false;
}

#endif
