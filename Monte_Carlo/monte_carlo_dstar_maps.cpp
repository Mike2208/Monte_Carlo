#include "monte_carlo_dstar_maps.h"

#include <queue>

const OGM_LOG_TYPE MIN_OBSTACLE_VALUE = OGM_LOG_MAX;

struct RATIO_DATA
{
	OGM_LOG_TYPE Ratio;
	MOVE_DIST_TYPE Dist;
	OGM_LOG_TYPE Prob;

	RATIO_DATA(OGM_LOG_TYPE _Ratio, MOVE_DIST_TYPE _Dist, OGM_LOG_TYPE _Prob) : Ratio(_Ratio), Dist(_Dist), Prob(_Prob) {}
};

template<class T>
struct POS_DATA
{
	POS_2D	Position;
	T		Value;

	POS_DATA(const POS_2D &_Position, const T &_Value) : Position(_Position), Value(_Value) {}
};

MonteCarloDStarMaps::MonteCarloDStarMaps(const OGM_LOG_MAP_TYPE &OriginalMap, const POS_2D &Start, const POS_2D &Destination) : _StartPos(Start), _DestPos(Destination)
{
	this->ResetMaps(OriginalMap);
}

// Reset Maps to original data
void MonteCarloDStarMaps::ResetMapsWithNewStartPos(const OGM_LOG_MAP_TYPE &OriginalMap, const POS_2D &NewStartPos)
{
	this->_StartPos = NewStartPos;

	// Reset Maps
//	this->ResetDistMap(OriginalMap, this->_StartPos, MIN_OBSTACLE_VALUE, this->_DistToStart);
//	this->ResetProbMapFromTarget(OriginalMap, this->_StartPos, this->_DistToStart);
	this->ResetRatioMapFromTarget(OriginalMap, this->_StartPos, this->_RatioToStart, this->_RatioDistToStart, this->_RatioProbToStart);
}

void MonteCarloDStarMaps::ResetMapsWithNewDestPos(const OGM_LOG_MAP_TYPE &OriginalMap, const POS_2D &NewDestPos)
{
	this->_DestPos = NewDestPos;

	// Reset Maps
//	this->ResetDistMap(OriginalMap, this->_DestPos, MIN_OBSTACLE_VALUE, this->_DistToGoal);
//	this->ResetProbMapToTarget(OriginalMap, this->_DestPos, this->_DistToGoal);
	this->ResetRatioMapToTarget(OriginalMap, this->_DestPos, this->_RatioToGoal, this->_RatioDistToGoal, this->_RatioProbToGoal);
}

void MonteCarloDStarMaps::ResetMaps(const OGM_LOG_MAP_TYPE &OriginalMap)
{
	// Reset maps
	this->ResetMapsWithNewStartPos(OriginalMap, this->_StartPos);
	this->ResetMapsWithNewDestPos(OriginalMap, this->_DestPos);
}

// Update maps with new values
void MonteCarloDStarMaps::UpdateMaps(const OGM_LOG_MAP_TYPE &UpdatedOriginalMap, const std::vector<POS_2D> &UpdatedValues)
{
	this->UpdateStartMaps(UpdatedOriginalMap, UpdatedValues);
	this->UpdateDestMaps(UpdatedOriginalMap, UpdatedValues);
}

void MonteCarloDStarMaps::UpdateStartMaps(const OGM_LOG_MAP_TYPE &UpdatedOriginalMap, const std::vector<POS_2D> &UpdatedValues)
{
//	this->UpdateDistMap(UpdatedOriginalMap, UpdatedValues, this->_StartPos, MIN_OBSTACLE_VALUE, this->_DistToStart);
//	this->UpdateProbMapFromTarget(UpdatedOriginalMap, UpdatedValues, this->_StartPos, this->_ProbToStart);
	this->UpdateRatioMapFromTarget(UpdatedOriginalMap, UpdatedValues, this->_StartPos, this->_RatioToStart, this->_RatioDistToStart, this->_RatioProbToStart);
}

void MonteCarloDStarMaps::UpdateDestMaps(const OGM_LOG_MAP_TYPE &UpdatedOriginalMap, const std::vector<POS_2D> &UpdatedValues)
{
//	this->UpdateDistMap(UpdatedOriginalMap, UpdatedValues, this->_DestPos, MIN_OBSTACLE_VALUE, this->_DistToGoal);
//	this->UpdateProbMapToTarget(UpdatedOriginalMap, UpdatedValues, this->_DestPos, this->_ProbToGoal);
	this->UpdateRatioMapToTarget(UpdatedOriginalMap, UpdatedValues, this->_DestPos, this->_RatioToGoal, this->_RatioDistToGoal, this->_RatioProbToGoal);
}

// Calculate best path options
std::vector<POS_2D> MonteCarloDStarMaps::CalculateBestDistPathFromStart(const POS_2D &TargetPosition) const
{
	return std::vector<POS_2D>();
}

std::vector<POS_2D> MonteCarloDStarMaps::CalculateBestDistPathToGoal(const POS_2D &StartPosition) const
{
	return std::vector<POS_2D>();
}

std::vector<POS_2D> MonteCarloDStarMaps::CalculateBestDistPath(const POS_2D &IntermediatePosition) const
{
	return std::vector<POS_2D>();
}

std::vector<POS_2D> MonteCarloDStarMaps::CalculateBestProbPathFromStart(const POS_2D &TargetPosition) const
{
	return std::vector<POS_2D>();
}

std::vector<POS_2D> MonteCarloDStarMaps::CalculateBestProbPathToGoal(const POS_2D &StartPosition) const
{
	return std::vector<POS_2D>();
}

std::vector<POS_2D> MonteCarloDStarMaps::CalculateBestProbPath(const POS_2D &IntermediatePosition) const
{
	return std::vector<POS_2D>();
}

std::vector<POS_2D> MonteCarloDStarMaps::CalculateBestRatioPathFromStart(const POS_2D &TargetPosition) const
{
	std::vector<POS_2D> reversePathVector;

	// Check if in map and that path is available
	if(!this->_RatioToStart.IsInMap(TargetPosition) || TargetPosition == this->_StartPos || this->_RatioToStart.GetPixel(TargetPosition) >= GetInfiniteVal<Map2D<OGM_LOG_TYPE>::CELL_TYPE>())
		return reversePathVector;		// Return empty vector if not in map or no path available

	// Determine how many elements are in pathVector
	//pathVector.reserve(this->_Ra);

	// Begin at TargetPosition, and follow gradient down to start
	POS_2D curPos = TargetPosition;
	while(curPos != this->_StartPos)
	{
		// Add position to vector
		reversePathVector.push_back(curPos);

		// Find neighbor that's closest to start
		Map2D<OGM_LOG_TYPE>::CELL_TYPE bestNeighborValue = GetInfiniteVal<Map2D<OGM_LOG_TYPE>::CELL_TYPE>();
		POS_2D bestNeighbor = curPos;
		for(const auto navOption : NavigationOptions)
		{
			const POS_2D adjacentPos = curPos+navOption;

			if(!this->_RatioToStart.IsInMap(adjacentPos))
				continue;		// Skip if not in map

			// Check if this is better neighbor
			const auto adjacentVal = this->_RatioToStart.GetPixel(adjacentPos);
			if(bestNeighborValue >= adjacentVal)
			{
				bestNeighborValue = adjacentVal;
				bestNeighbor = adjacentPos;
			}
		}

		curPos = bestNeighbor;
	}

	// Reverse path vector to begin at start
	std::vector<POS_2D> pathVector;
	pathVector.reserve(reversePathVector.size());

	for(auto reversePathIterator = reversePathVector.rbegin(); reversePathIterator != reversePathVector.rend(); ++reversePathIterator)
	{
		// Assign values from reverse path to correct one
		pathVector.push_back(*reversePathIterator);
	}

	return pathVector;
}

std::vector<POS_2D> MonteCarloDStarMaps::CalculateBestRatioPathToGoal(const POS_2D &StartPosition) const
{
	std::vector<POS_2D> reversePathVector;

	// Check if in map and that path is available
	if(!this->_RatioToGoal.IsInMap(StartPosition) || StartPosition == this->_DestPos || this->_RatioToGoal.GetPixel(StartPosition) >= GetInfiniteVal<Map2D<OGM_LOG_TYPE>::CELL_TYPE>())
		return reversePathVector;		// Return empty vector if not in map or no path available

	// Determine how many elements are in pathVector
	//pathVector.reserve(this->_Ra);

	// Begin at TargetPosition, and follow gradient down to goal
	POS_2D curPos = StartPosition;
	while(curPos != this->_DestPos)
	{
		// Find neighbor that's closest to goal
		Map2D<OGM_LOG_TYPE>::CELL_TYPE bestNeighborValue = GetInfiniteVal<Map2D<OGM_LOG_TYPE>::CELL_TYPE>();
		POS_2D bestNeighbor = curPos;
		for(const auto navOption : NavigationOptions)
		{
			const POS_2D adjacentPos = curPos+navOption;

			if(!this->_RatioToGoal.IsInMap(adjacentPos))
				continue;		// Skip if not in map

			// Check if this is better neighbor
			const auto adjacentVal = this->_RatioToGoal.GetPixel(adjacentPos);
			if(bestNeighborValue >= adjacentVal)
			{
				bestNeighborValue = adjacentVal;
				bestNeighbor = adjacentPos;
			}
		}

		curPos = bestNeighbor;

		// Add position to vector
		reversePathVector.push_back(curPos);
	}

	// Reverse path vector to begin at goal
	std::vector<POS_2D> pathVector;
	pathVector.reserve(reversePathVector.size());

	for(auto reversePathIterator = reversePathVector.rbegin(); reversePathIterator != reversePathVector.rend(); ++reversePathIterator)
	{
		// Assign values from reverse path to correct one
		pathVector.push_back(*reversePathIterator);
	}

	return pathVector;
}

std::vector<POS_2D> MonteCarloDStarMaps::CalculateBestRatioPath(const POS_2D &IntermediatePosition) const
{
	std::vector<POS_2D> tmpStartVector;

	// If intermediate position is not start pos, calculate path
	if(IntermediatePosition != this->_StartPos)
	{
		tmpStartVector = std::move(this->CalculateBestRatioPathFromStart(IntermediatePosition));

		// Check if path was found
		if(tmpStartVector.empty())
			return tmpStartVector;		// Return empty path if none found
	}

	std::vector<POS_2D> tmpDestVector;
	if(IntermediatePosition != this->_DestPos)
	{
		tmpDestVector = std::move(this->CalculateBestRatioPathToGoal(IntermediatePosition));

		// Check if path was found
		if(tmpDestVector.empty())
			return tmpDestVector;		// Return empty path if none found
	}

	// Reserve enough for dest data
	tmpStartVector.reserve(tmpStartVector.size()+tmpDestVector.size());

	// Insert dest data to path
	for(auto curDestPathPos = tmpDestVector.begin(); curDestPathPos != tmpDestVector.end(); ++curDestPathPos)
	{
		tmpStartVector.push_back(*curDestPathPos);
	}

	return tmpStartVector;
}

void MonteCarloDStarMaps::ResetDistMap(const OGM_LOG_MAP_TYPE &OriginalMap, const POS_2D &TargetPos, const OGM_LOG_TYPE ObstacleValue, Map2D<MOVE_DIST_TYPE> &DistMap)
{
	// Reset Map
	DistMap.ResetMap(OriginalMap.GetWidth(), OriginalMap.GetHeight(), GetInfiniteVal<MOVE_DIST_TYPE>());
	DistMap.SetPixel(TargetPos, 0);

	// Go through map and calculate shortest distances
	std::queue<POS_2D> posToCheck;
	posToCheck.push(TargetPos);
	do
	{
		// Current position values
		const POS_2D &curPos = posToCheck.front();
		auto curDist = DistMap.GetPixel(curPos);

		// Compare to adjacent positions
		for(const auto navOption : NavigationOptions)
		{
			const POS_2D adjacentPos = curPos + navOption;
			const auto movementCost = GetMovementCost(curPos, adjacentPos);

			if(!DistMap.IsInMap(adjacentPos))
				continue;			// Skip if not in map

			if(OriginalMap.GetPixel(adjacentPos) >= ObstacleValue)
				continue;			// Skip if adjacent pos is obstacle

			// Get adjacent dist values
			auto &r_adjacentDist = DistMap.GetPixelR(adjacentPos);

			// Compare values
			if(curDist + movementCost < r_adjacentDist)
			{
				// If smaller dist available, update adjacent pos and check its adjacent positions later
				r_adjacentDist = curDist + movementCost;
				posToCheck.push(adjacentPos);
			}
		}

		// Remove checked element
		posToCheck.pop();
	}
	while(!posToCheck.empty());
}

void MonteCarloDStarMaps::ResetProbMapFromTarget(const OGM_LOG_MAP_TYPE &OriginalMap, const POS_2D &TargetPos, Map2D<OGM_LOG_TYPE> &ProbMap)
{
	// Reset Map
	ProbMap.ResetMap(OriginalMap.GetWidth(), OriginalMap.GetHeight(), GetInfiniteVal<OGM_LOG_TYPE>());
	ProbMap.SetPixel(TargetPos, OGM_LOG_MIN);

	// Go through map and calculate lowest probability to target pos
	std::queue<POS_2D> posToCheck;
	posToCheck.push(TargetPos);
	do
	{
		// Current position values
		const POS_2D &curPos = posToCheck.front();
		auto curProb = ProbMap.GetPixel(curPos);
		//auto curOriginalProb = OriginalMap.GetPixel(curPos);

		// Compare to adjacent positions
		for(const auto navOption : NavigationOptions)
		{
			const POS_2D adjacentPos = curPos + navOption;

			if(!ProbMap.IsInMap(adjacentPos))
				continue;			// Skip if not in map

			// Get adjacent prob values
			auto &r_adjacentProb = ProbMap.GetPixelR(adjacentPos);

			auto adjacentOriginalProb = OriginalMap.GetPixel(adjacentPos);

			// Compare values
			if(curProb + adjacentOriginalProb < r_adjacentProb)
			{
				// If smaller prob available, update adjacent pos and check it later
				r_adjacentProb = curProb + adjacentOriginalProb;
				posToCheck.push(adjacentPos);
			}
		}

		// Remove checked element
		posToCheck.pop();
	}
	while(!posToCheck.empty());
}

void MonteCarloDStarMaps::ResetProbMapToTarget(const OGM_LOG_MAP_TYPE &OriginalMap, const POS_2D &TargetPos, Map2D<OGM_LOG_TYPE> &ProbMap)
{
	// Reset Map
	ProbMap.ResetMap(OriginalMap.GetWidth(), OriginalMap.GetHeight(), GetInfiniteVal<OGM_LOG_TYPE>());
	ProbMap.SetPixel(TargetPos, OGM_LOG_MIN);

	// Go through map and calculate lowest probability to target pos
	std::queue<POS_2D> posToCheck;
	posToCheck.push(TargetPos);
	do
	{
		// Current position values
		const POS_2D &curPos = posToCheck.front();
		auto curProb = ProbMap.GetPixel(curPos);
		auto curOriginalProb = OriginalMap.GetPixel(curPos);

		// Compare to adjacent positions
		for(const auto navOption : NavigationOptions)
		{
			const POS_2D adjacentPos = curPos + navOption;

			if(!ProbMap.IsInMap(adjacentPos))
				continue;			// Skip if not in map

			// Get adjacent prob values
			auto &r_adjacentProb = ProbMap.GetPixelR(adjacentPos);

			//auto adjacentOriginalProb = OriginalMap.GetPixel(adjacentPos);

			// Compare values
			if(curProb + curOriginalProb < r_adjacentProb)
			{
				// If smaller prob available, update adjacent pos and check it later
				r_adjacentProb = curProb + curOriginalProb;
				posToCheck.push(adjacentPos);
			}
		}

		// Remove checked element
		posToCheck.pop();
	}
	while(!posToCheck.empty());
}

void MonteCarloDStarMaps::ResetRatioMapFromTarget(const OGM_LOG_MAP_TYPE &OriginalMap, const POS_2D &TargetPos, Map2D<OGM_LOG_TYPE> &RatioMap, Map2D<MOVE_DIST_TYPE> &RatioDistMap, Map2D<OGM_LOG_TYPE> &RatioProbMap)
{

	// Reset Maps
	RatioMap.ResetMap(OriginalMap.GetWidth(), OriginalMap.GetHeight(), GetInfiniteVal<OGM_LOG_TYPE>());
	RatioDistMap.ResetMap(OriginalMap.GetWidth(), OriginalMap.GetHeight(), GetInfiniteVal<MOVE_DIST_TYPE>());
	RatioProbMap.ResetMap(OriginalMap.GetWidth(), OriginalMap.GetHeight(), GetInfiniteVal<OGM_LOG_TYPE>());

	RatioDistMap.SetPixel(TargetPos, 0);
	RatioProbMap.SetPixel(TargetPos, OGM_LOG_MIN);
	RatioMap.SetPixel(TargetPos, this->CalculateRatio(0, OGM_LOG_MIN));

	// Go through map and calculate shortest distances
	std::queue<POS_2D> posToCheck;
	posToCheck.push(TargetPos);
	do
	{
		// Current position values
		const POS_2D &curPos = posToCheck.front();
		auto curDist = RatioDistMap.GetPixel(curPos);
		auto curProb = RatioProbMap.GetPixel(curPos);

		// Get original prob value
		//auto curOriginalProb = OriginalMap.GetPixel(curPos);

		// Compare to adjacent positions
		for(const auto navOption : NavigationOptions)
		{
			const POS_2D adjacentPos = curPos + navOption;
			const auto movementCost = GetMovementCost(curPos, adjacentPos);

			if(!RatioMap.IsInMap(adjacentPos))
				continue;			// Skip if not in map

			// Get adjacent ratio value
			auto &r_adjacentRatio = RatioMap.GetPixelR(adjacentPos);

			// Get original probability value
			auto adjacentOriginalProb = OriginalMap.GetPixel(adjacentPos);

			// Compare values
			const auto newAdjacentRatio = this->CalculateRatio(curDist+movementCost, curProb+adjacentOriginalProb);
			if(newAdjacentRatio < r_adjacentRatio)
			{
				// If smaller ratio available, update adjacent ratio and check its neighbors later
				RatioDistMap.SetPixel(adjacentPos, curDist+movementCost);
				RatioProbMap.SetPixel(adjacentPos, curProb+adjacentOriginalProb);
				r_adjacentRatio = newAdjacentRatio;

				posToCheck.push(adjacentPos);
			}
		}

		// Remove checked element
		posToCheck.pop();
	}
	while(!posToCheck.empty());
}

void MonteCarloDStarMaps::ResetRatioMapToTarget(const OGM_LOG_MAP_TYPE &OriginalMap, const POS_2D &TargetPos, Map2D<OGM_LOG_TYPE> &RatioMap, Map2D<MOVE_DIST_TYPE> &RatioDistMap, Map2D<OGM_LOG_TYPE> &RatioProbMap)
{

	// Reset Maps
	RatioMap.ResetMap(OriginalMap.GetWidth(), OriginalMap.GetHeight(), GetInfiniteVal<OGM_LOG_TYPE>());
	RatioDistMap.ResetMap(OriginalMap.GetWidth(), OriginalMap.GetHeight(), GetInfiniteVal<MOVE_DIST_TYPE>());
	RatioProbMap.ResetMap(OriginalMap.GetWidth(), OriginalMap.GetHeight(), GetInfiniteVal<OGM_LOG_TYPE>());

	RatioDistMap.SetPixel(TargetPos, 0);
	RatioProbMap.SetPixel(TargetPos, OGM_LOG_MIN);
	RatioMap.SetPixel(TargetPos, this->CalculateRatio(0, OGM_LOG_MIN));

	// Go through map and calculate shortest distances
	std::queue<POS_2D> posToCheck;
	posToCheck.push(TargetPos);
	do
	{
		// Current position values
		const POS_2D &curPos = posToCheck.front();
		auto curDist = RatioDistMap.GetPixel(curPos);
		auto curProb = RatioProbMap.GetPixel(curPos);

		// Get original prob value
		auto curOriginalProb = OriginalMap.GetPixel(curPos);

		// Compare to adjacent positions
		for(const auto navOption : NavigationOptions)
		{
			const POS_2D adjacentPos = curPos + navOption;
			const auto movementCost = GetMovementCost(curPos, adjacentPos);

			if(!RatioMap.IsInMap(adjacentPos))
				continue;			// Skip if not in map

			// Get adjacent ratio value
			auto &r_adjacentRatio = RatioMap.GetPixelR(adjacentPos);

			// Get original probability value
			//auto adjacentOriginalProb = OriginalMap.GetPixel(adjacentPos);

			// Compare values
			const auto newAdjacentRatio = this->CalculateRatio(curDist+movementCost, curProb+curOriginalProb);
			if(newAdjacentRatio < r_adjacentRatio)
			{
				// If smaller ratio available, update adjacent ratio and check its neighbors later
				RatioDistMap.SetPixel(adjacentPos, curDist+movementCost);
				RatioProbMap.SetPixel(adjacentPos, curProb+curOriginalProb);
				r_adjacentRatio = newAdjacentRatio;

				posToCheck.push(adjacentPos);
			}
		}

		// Remove checked element
		posToCheck.pop();
	}
	while(!posToCheck.empty());
}

void MonteCarloDStarMaps::UpdateDistMap(const OGM_LOG_MAP_TYPE &UpdatedOriginalMap, const std::vector<POS_2D> &UpdatedValues, const POS_2D &TargetPos, const OGM_LOG_TYPE ObstacleValue, Map2D<MOVE_DIST_TYPE> &DistMap)
{
	// Set all updated values
	std::queue<POS_2D> updatedPosToCheck;

	for(const auto &curUpdate : UpdatedValues)
	{
		// Add position to queue
		updatedPosToCheck.push(curUpdate);
	}

	// Vector to store positions that should be checked next
	std::vector<POS_DATA<MOVE_DIST_TYPE>> storedPositions;

	std::queue<POS_2D> nextPositionsToCheck;

	// Go through all updated positions
	while(!updatedPosToCheck.empty())
	{
		const auto &curPos = updatedPosToCheck.front();
		auto &r_curDist = DistMap.GetPixelR(curPos);

		const auto updatedCurValue = UpdatedOriginalMap.GetPixel(curPos);

		// Check that updated actually changes values in a significant manner
		const auto oldCurDist = r_curDist;
		if(updatedCurValue >= ObstacleValue)
		{
			if(r_curDist == GetInfiniteVal<MOVE_DIST_TYPE>())
			{
				// No significant change, skip
				updatedPosToCheck.pop();
				continue;
			}

			r_curDist = GetInfiniteVal<MOVE_DIST_TYPE>();

			for(const auto navOption : NavigationOptions)
			{
				// Get adjacent positions
				const auto adjacentPos = curPos + navOption;

				// Check that pos is in map
				if(!DistMap.IsInMap(adjacentPos))
					continue;

				const auto adjacentDist = DistMap.GetPixel(adjacentPos);

				// Check whether adjacent position depends on this one
				if(adjacentDist < GetInfiniteVal<MOVE_DIST_TYPE>() && adjacentDist > oldCurDist)
					nextPositionsToCheck.push(adjacentPos);
			}
		}
		else
		{
			if(r_curDist != GetInfiniteVal<MOVE_DIST_TYPE>())
			{
				// No significant change, skip
				updatedPosToCheck.pop();
				continue;
			}

			// Find new Distance
			auto bestDist = GetInfiniteVal<MOVE_DIST_TYPE>();
			for(const auto navOption : NavigationOptions)
			{
				// Get adjacent positions
				const auto adjacentPos = curPos + navOption;
				const auto movementCost = GetMovementCost(adjacentPos, curPos);

				// Check that pos is in map
				if(!DistMap.IsInMap(adjacentPos))
					continue;

				const auto adjacentDist = DistMap.GetPixel(adjacentPos);

				// Find best adjacent distance
				if(adjacentDist + movementCost < bestDist)
				{
					bestDist = adjacentDist + movementCost;
				}

				// Store position for later
				storedPositions.push_back(POS_DATA<MOVE_DIST_TYPE>(adjacentPos, adjacentDist));
			}

			// Update curDist
			r_curDist = bestDist;

			// Find all positions that can be updated
			for(const auto &curStoredPosition : storedPositions)
			{
				// Check if this stored position can be improved with new value
				if(r_curDist + GetMovementCost(curPos, curStoredPosition.Position) < curStoredPosition.Value)
				{
					nextPositionsToCheck.push(curStoredPosition.Position);
				}
			}

			// Clear stored data
			storedPositions.clear();
		}
	}

	while(!nextPositionsToCheck.empty())
	{
		const auto &curPos = nextPositionsToCheck.front();
		auto &r_curDist = DistMap.GetPixelR(curPos);

		auto bestDist = GetInfiniteVal<MOVE_DIST_TYPE>();
		for(const auto navOption : NavigationOptions)
		{
			// Get adjacent positions
			const auto adjacentPos = curPos + navOption;
			const auto movementCost = GetMovementCost(adjacentPos, curPos);

			// Check that pos is in map
			if(!DistMap.IsInMap(adjacentPos))
				continue;

			auto adjacentDist = DistMap.GetPixel(adjacentPos);

			// Check if this position is dependent on updated value
			if(adjacentDist < GetInfiniteVal<MOVE_DIST_TYPE>())
			{
				//if(adjacentDist >= r_curDist + movementCost)
				if(adjacentDist > r_curDist)
				{
					// Add adjacent distances that are dependent on this one to positions to check
					nextPositionsToCheck.push(adjacentPos);
				}
				else
				{
					if(adjacentDist + movementCost < bestDist)
					{
						bestDist = adjacentDist + movementCost;
					}

					// Store value to check whether adjacent dist can be improved with new curDist later
					storedPositions.push_back(POS_DATA<MOVE_DIST_TYPE>(adjacentPos, adjacentDist));
				}
			}
		}

		// Check all stored positions to see which can be improved with new best value
		if(bestDist < r_curDist)
		{
			// Check if this stored position can be improved with new value
			for(const auto &curStoredPos : storedPositions)
			{
				if(bestDist + GetMovementCost(curPos, curStoredPos.Position) < curStoredPos.Value)
					nextPositionsToCheck.push(curStoredPos.Position);
			}
		}

		storedPositions.clear();

		// Update curDist
		r_curDist = bestDist;

		nextPositionsToCheck.pop();
	}
}

void MonteCarloDStarMaps::UpdateProbMapFromTarget(const OGM_LOG_MAP_TYPE &UpdatedOriginalMap, const std::vector<POS_2D> &UpdatedValues, const POS_2D &TargetPos, Map2D<OGM_LOG_TYPE> &ProbMap)
{
	// Set all updated values
	std::queue<POS_2D> nextPositionsToCheck;

	for(const auto &curUpdate : UpdatedValues)
	{
		// Add position to queue
		nextPositionsToCheck.push(curUpdate);
	}

	// Vector to store positions that should be checked next
	std::vector<POS_DATA<OGM_LOG_TYPE>> storedPositions;

	while(!nextPositionsToCheck.empty())
	{
		const auto &curPos = nextPositionsToCheck.front();
		auto &r_curProb = ProbMap.GetPixelR(curPos);

		auto bestProb = GetInfiniteVal<OGM_LOG_TYPE>();
		for(const auto navOption : NavigationOptions)
		{
			// Get adjacent positions
			const auto adjacentPos = curPos + navOption;

			// Check that pos is in map
			if(!ProbMap.IsInMap(adjacentPos))
				continue;

			auto adjacentProb = ProbMap.GetPixel(adjacentPos);
			const auto adjacentUpdatedProb = UpdatedOriginalMap.GetPixel(adjacentPos);

			// Check if this position is dependent on updated value
			if(adjacentProb > r_curProb)
			{
				// Add adjacent distance to positions to check
				nextPositionsToCheck.push(adjacentPos);
			}
			else
			{
				// Store best probability and current position + value
				//const auto curUpdatedProb = UpdatedOriginalMap.GetPixel(curPos);
				if(adjacentProb + adjacentUpdatedProb < bestProb)
				{
					bestProb = adjacentProb + adjacentUpdatedProb;
				}

				// Store value to check whether adjacent dist can be improved with new curDist later
				storedPositions.push_back(POS_DATA<OGM_LOG_TYPE>(adjacentPos, adjacentProb));
			}
		}

		// Check all stored positions to see which can be improved with new best value
		if(bestProb < r_curProb)
		{
			for(const auto &curStoredPos : storedPositions)
			{
				// TODO: Maybe save UpdatedOriginalMap.GetPixel(curStoredPos.Position) earlier for faster access
				if(bestProb + UpdatedOriginalMap.GetPixel(curStoredPos.Position) < curStoredPos.Value)
					nextPositionsToCheck.push(curStoredPos.Position);
			}
		}

		// Clear saved data
		storedPositions.clear();

		// Update curProb with best new possible value
		r_curProb = bestProb;

		nextPositionsToCheck.pop();
	}
}

void MonteCarloDStarMaps::UpdateProbMapToTarget(const OGM_LOG_MAP_TYPE &UpdatedOriginalMap, const std::vector<POS_2D> &UpdatedValues, const POS_2D &TargetPos, Map2D<OGM_LOG_TYPE> &ProbMap)
{
	// Set all updated values
	std::queue<POS_2D> nextPositionsToCheck;

	for(const auto &curUpdate : UpdatedValues)
	{
		// Add position to queue
		nextPositionsToCheck.push(curUpdate);
	}

	// Vector to store positions that should be checked next
	std::vector<POS_DATA<OGM_LOG_TYPE>> storedPositions;

	while(!nextPositionsToCheck.empty())
	{
		const auto &curPos = nextPositionsToCheck.front();
		auto &r_curProb = ProbMap.GetPixelR(curPos);

		auto bestProb = GetInfiniteVal<OGM_LOG_TYPE>();
		for(const auto navOption : NavigationOptions)
		{
			// Get adjacent positions
			const auto adjacentPos = curPos + navOption;

			// Check that pos is in map
			if(!ProbMap.IsInMap(adjacentPos))
				continue;

			auto adjacentProb = ProbMap.GetPixel(adjacentPos);
			//const auto adjacentUpdatedProb = UpdatedOriginalMap.GetPixel(adjacentPos);

			// Check if this position is dependent on updated value
			if(adjacentProb > r_curProb)
			{
				// Add adjacent distance to positions to check
				nextPositionsToCheck.push(adjacentPos);
			}
			else
			{
				// Store best probability and current position + value
				const auto curUpdatedProb = UpdatedOriginalMap.GetPixel(curPos);
				if(adjacentProb + curUpdatedProb < bestProb)
				{
					bestProb = adjacentProb + curUpdatedProb;
				}

				// Store value to check whether adjacent dist can be improved with new curDist later
				storedPositions.push_back(POS_DATA<OGM_LOG_TYPE>(adjacentPos, adjacentProb));
			}
		}

		// Check all stored positions to see which can be improved with new best value
		if(bestProb < r_curProb)
		{
			for(const auto &curStoredPos : storedPositions)
			{
				// TODO: Maybe save UpdatedOriginalMap.GetPixel(curStoredPos.Position) earlier for faster access
				if(bestProb + UpdatedOriginalMap.GetPixel(curStoredPos.Position) < curStoredPos.Value)
					nextPositionsToCheck.push(curStoredPos.Position);
			}
		}

		// Clear saved data
		storedPositions.clear();

		// Update curProb with best new possible value
		r_curProb = bestProb;

		nextPositionsToCheck.pop();
	}
}

void MonteCarloDStarMaps::UpdateRatioMapFromTarget(const OGM_LOG_MAP_TYPE &UpdatedOriginalMap, const std::vector<POS_2D> &UpdatedValues, const POS_2D &TargetPos, Map2D<OGM_LOG_TYPE> &RatioMap, Map2D<MOVE_DIST_TYPE> &RatioDistMap, Map2D<OGM_LOG_TYPE> &RatioProbMap)
{
	// Set all updated values
	std::queue<POS_2D> nextPositionsToCheck;

	for(const auto &curUpdate : UpdatedValues)
	{
		// Check if map origin was updated
		if(curUpdate == TargetPos)
		{
			// In this case, simply reset map as everything will change
			this->ResetRatioMapFromTarget(UpdatedOriginalMap, TargetPos, RatioMap, RatioDistMap, RatioProbMap);
			return;
		}

		// Add position to queue
		nextPositionsToCheck.push(curUpdate);
	}

	// Vector to store positions that should be checked next
	std::vector<POS_DATA<RATIO_DATA>> storedPositions;

	while(!nextPositionsToCheck.empty())
	{
		const auto &curPos = nextPositionsToCheck.front();
		auto &r_curRatio = RatioMap.GetPixelR(curPos);
		auto &r_curRatioDist = RatioDistMap.GetPixelR(curPos);
		auto &r_curRatioProb = RatioProbMap.GetPixelR(curPos);

		//const auto curUpdatedProb = UpdatedOriginalMap.GetPixel(curPos);

		auto bestRatio = GetInfiniteVal<OGM_LOG_TYPE>();
		auto bestRatioDist = GetInfiniteVal<MOVE_DIST_TYPE>();
		auto bestRatioProb = GetInfiniteVal<OGM_LOG_TYPE>();
		for(const auto navOption : NavigationOptions)
		{
			// Get adjacent positions
			const auto adjacentPos = curPos + navOption;
			const auto movementCost = GetMovementCost(curPos, adjacentPos);

			// Check that pos is in map
			if(!RatioMap.IsInMap(adjacentPos))
				continue;

			auto adjacentRatio = RatioMap.GetPixel(adjacentPos);
			auto adjacentRatioDist = RatioDistMap.GetPixel(adjacentPos);
			auto adjacentRatioProb = RatioProbMap.GetPixel(adjacentPos);

			const auto adjacentUpdatedProb = UpdatedOriginalMap.GetPixel(adjacentPos);

			// Check if this position is dependent on old value
			//if(adjacentRatio >= this->CalculateRatio(r_curRatioDist+movementCost, r_curRatioProb+adjacentUpdatedProb))
			if(adjacentRatio > r_curRatio)
			{
				// Add adjacent distance to positions to check
				nextPositionsToCheck.push(adjacentPos);
			}
			else
			{
				// Store best probability and current position + value
				const auto tmpRatioDist = adjacentRatioDist+movementCost;
				const auto tmpRatioProb = adjacentRatioProb + adjacentUpdatedProb;
				const auto tmpRatio = this->CalculateRatio(tmpRatioDist, tmpRatioProb);
				if(tmpRatio < bestRatio)
				{
					bestRatio = tmpRatio;
					bestRatioDist = tmpRatioDist;
					bestRatioProb = tmpRatioProb;
				}

				// Store value to check whether adjacent dist can be improved with new curDist later
				storedPositions.push_back(POS_DATA<RATIO_DATA>(adjacentPos, RATIO_DATA(adjacentRatio, adjacentRatioDist, adjacentRatioProb)));
			}
		}

		// Check all stored positions to see which can be improved with new best value
		if(bestRatio < r_curRatio)
		{
			for(const auto &curStoredPos : storedPositions)
			{
				// TODO: Maybe save UpdatedOriginalMap.GetPixel(curStoredPos.Position) earlier for faster access
				if(this->CalculateRatio(bestRatioDist+GetMovementCost(curPos, curStoredPos.Position), bestRatioProb+UpdatedOriginalMap.GetPixel(curStoredPos.Position)) < curStoredPos.Value.Ratio)
					nextPositionsToCheck.push(curStoredPos.Position);
			}
		}

		// Clear saved data
		storedPositions.clear();

		// Update curProb with best new possible value
		r_curRatio = bestRatio;
		r_curRatioDist = bestRatioDist;
		r_curRatioProb = bestRatioProb;

		nextPositionsToCheck.pop();
	}
}

void MonteCarloDStarMaps::UpdateRatioMapToTarget(const OGM_LOG_MAP_TYPE &UpdatedOriginalMap, const std::vector<POS_2D> &UpdatedValues, const POS_2D &TargetPos, Map2D<OGM_LOG_TYPE> &RatioMap, Map2D<MOVE_DIST_TYPE> &RatioDistMap, Map2D<OGM_LOG_TYPE> &RatioProbMap)
{
	// Set all updated values
	std::queue<POS_2D> nextPositionsToCheck;

	for(const auto &curUpdate : UpdatedValues)
	{
		// Check if map origin was updated
		if(curUpdate == TargetPos)
		{
			// In this case, simply reset map as everything will change
			this->ResetRatioMapToTarget(UpdatedOriginalMap, TargetPos, RatioMap, RatioDistMap, RatioProbMap);

			return;
		}

		// Add position to queue
		nextPositionsToCheck.push(curUpdate);
	}

	// Vector to store positions that should be checked next
	std::vector<POS_DATA<RATIO_DATA>> storedPositions;

	while(!nextPositionsToCheck.empty())
	{
		const auto &curPos = nextPositionsToCheck.front();
		auto &r_curRatio = RatioMap.GetPixelR(curPos);
		auto &r_curRatioDist = RatioDistMap.GetPixelR(curPos);
		auto &r_curRatioProb = RatioProbMap.GetPixelR(curPos);

		const auto curUpdatedProb = UpdatedOriginalMap.GetPixel(curPos);

		auto bestRatio = GetInfiniteVal<OGM_LOG_TYPE>();
		auto bestRatioDist = GetInfiniteVal<MOVE_DIST_TYPE>();
		auto bestRatioProb = GetInfiniteVal<OGM_LOG_TYPE>();
		for(const auto navOption : NavigationOptions)
		{
			// Get adjacent positions
			const auto adjacentPos = curPos + navOption;
			const auto movementCost = GetMovementCost(curPos, adjacentPos);

			// Check that pos is in map
			if(!RatioMap.IsInMap(adjacentPos))
				continue;

			auto adjacentRatio = RatioMap.GetPixel(adjacentPos);
			auto adjacentRatioDist = RatioDistMap.GetPixel(adjacentPos);
			auto adjacentRatioProb = RatioProbMap.GetPixel(adjacentPos);

			//const auto adjacentUpdatedProb = UpdatedOriginalMap.GetPixel(adjacentPos);

			// Check if this position is dependent on old value
			//if(adjacentRatio >= this->CalculateRatio(r_curRatioDist+movementCost, r_curRatioProb+adjacentUpdatedProb))
			if(adjacentRatio > r_curRatio)
			{
				// Add adjacent distance to positions to check
				nextPositionsToCheck.push(adjacentPos);
			}
			else
			{
				// Store best probability and current position + value
				const auto tmpRatioDist = adjacentRatioDist+movementCost;
				const auto tmpRatioProb = adjacentRatioProb + curUpdatedProb;
				const auto tmpRatio = this->CalculateRatio(tmpRatioDist, tmpRatioProb);
				if(tmpRatio < bestRatio)
				{
					bestRatio = tmpRatio;
					bestRatioDist = tmpRatioDist;
					bestRatioProb = tmpRatioProb;
				}

				// Store value to check whether adjacent dist can be improved with new curDist later
				storedPositions.push_back(POS_DATA<RATIO_DATA>(adjacentPos, RATIO_DATA(adjacentRatio, adjacentRatioDist, adjacentRatioProb)));
			}
		}

		// Check all stored positions to see which can be improved with new best value
		if(bestRatio < r_curRatio)
		{
			for(const auto &curStoredPos : storedPositions)
			{
				// TODO: Maybe save UpdatedOriginalMap.GetPixel(curStoredPos.Position) earlier for faster access
				if(this->CalculateRatio(bestRatioDist+GetMovementCost(curPos, curStoredPos.Position), bestRatioProb+UpdatedOriginalMap.GetPixel(curStoredPos.Position)) < curStoredPos.Value.Ratio)
					nextPositionsToCheck.push(curStoredPos.Position);
			}
		}

		// Clear saved data
		storedPositions.clear();

		// Update curProb with best new possible value
		r_curRatio = bestRatio;
		r_curRatioDist = bestRatioDist;
		r_curRatioProb = bestRatioProb;

		nextPositionsToCheck.pop();
	}
}

OGM_LOG_TYPE MonteCarloDStarMaps::CalculateRatio(const MOVE_DIST_TYPE Dist, const OGM_LOG_TYPE LogValueFromMaps) const
{
	return static_cast<OGM_LOG_TYPE>(Dist)/OccupancyGridMap::CalculateCertaintyProbValueFromCertaintyLog(LogValueFromMaps);
	//return static_cast<OGM_LOG_TYPE>(log(Dist+1))+(LogValueFromMaps+1);
}
