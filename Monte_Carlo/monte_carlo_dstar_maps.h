#ifndef MONTE_CARLO_DSTAR_MAPS_H
#define MONTE_CARLO_DSTAR_MAPS_H

#include "standard_definitions.h"
#include "occupancy_grid_map.h"

namespace MONTE_CARLO_DSTAR_MAPS
{
	struct MAP_UPDATES
	{
		POS_2D			Position;
		OGM_LOG_TYPE	Value;

		MAP_UPDATES() = default;
		MAP_UPDATES(const POS_2D &_Position, const OGM_LOG_TYPE _Value) : Position(_Position), Value(_Value)
		{}
	};
}

class MonteCarloDStarMaps
{
	public:
		typedef MONTE_CARLO_DSTAR_MAPS::MAP_UPDATES MAP_UPDATES;

		MonteCarloDStarMaps() = default;
		MonteCarloDStarMaps(const OGM_LOG_MAP_TYPE &OriginalMap, const POS_2D &Start, const POS_2D &Destination);

		// Reset Maps to original data
		void ResetMapsWithNewStartPos(const OGM_LOG_MAP_TYPE &OriginalMap, const POS_2D &NewStartPos);
		void ResetMapsWithNewDestPos(const OGM_LOG_MAP_TYPE &OriginalMap, const POS_2D &NewDestPos);
		void ResetMaps(const OGM_LOG_MAP_TYPE &OriginalMap);

		// Update maps with new positions
		void UpdateMaps(const OGM_LOG_MAP_TYPE &UpdatedOriginalMap, const std::vector<POS_2D> &UpdatedValues);
		void UpdateStartMaps(const OGM_LOG_MAP_TYPE &UpdatedOriginalMap, const std::vector<POS_2D> &UpdatedValues);
		void UpdateDestMaps(const OGM_LOG_MAP_TYPE &UpdatedOriginalMap, const std::vector<POS_2D> &UpdatedValues);

		// Calculate best path options
		std::vector<POS_2D> CalculateBestDistPathFromStart(const POS_2D &TargetPosition) const;
		std::vector<POS_2D> CalculateBestDistPathToGoal(const POS_2D &StartPosition) const;
		std::vector<POS_2D> CalculateBestDistPath(const POS_2D &IntermediatePosition) const;

		std::vector<POS_2D> CalculateBestProbPathFromStart(const POS_2D &TargetPosition) const;
		std::vector<POS_2D> CalculateBestProbPathToGoal(const POS_2D &StartPosition) const;
		std::vector<POS_2D> CalculateBestProbPath(const POS_2D &IntermediatePosition) const;

		std::vector<POS_2D> CalculateBestRatioPathFromStart(const POS_2D &TargetPosition) const;
		std::vector<POS_2D> CalculateBestRatioPathToGoal(const POS_2D &StartPosition) const;
		std::vector<POS_2D> CalculateBestRatioPath(const POS_2D &IntermediatePosition) const;

		// Access maps
		const Map2D<MOVE_DIST_TYPE> &DistToGoalMap() const { return this->_DistToGoal; }
		const Map2D<MOVE_DIST_TYPE> &DistToStartMap() const { return this->_DistToStart; }
		const Map2D<OGM_LOG_TYPE> &ProbToGoalMap() const { return this->_ProbToGoal; }
		const Map2D<OGM_LOG_TYPE> &ProbToStartMap() const { return this->_ProbToStart; }
		const Map2D<OGM_LOG_TYPE> &RatioToGoalMap() const { return this->_RatioToGoal; }
		const Map2D<OGM_LOG_TYPE> &RatioToStartMap() const { return this->_RatioToStart; }

		const Map2D<OGM_LOG_TYPE> &RatioCertaintyToGoalMap() const { return this->_RatioProbToGoal; }
		const Map2D<OGM_LOG_TYPE> &RatioCertaintyToStartMap() const { return this->_RatioProbToStart; }

		const Map2D<OGM_LOG_TYPE> &RatioDistToGoalMap() const { return this->_RatioDistToGoal; }
		const Map2D<OGM_LOG_TYPE> &RatioDistToStartMap() const { return this->_RatioDistToStart; }

		// Access maps unsafe
		Map2D<MOVE_DIST_TYPE> &DistToGoalMapR() { return this->_DistToGoal; }
		Map2D<MOVE_DIST_TYPE> &DistToStartMapR() { return this->_DistToStart; }
		Map2D<OGM_LOG_TYPE> &ProbToGoalMapR() { return this->_ProbToGoal; }
		Map2D<OGM_LOG_TYPE> &ProbToStartMapR() { return this->_ProbToStart; }
		Map2D<OGM_LOG_TYPE> &RatioToGoalMapR() { return this->_RatioToGoal; }
		Map2D<OGM_LOG_TYPE> &RatioToStartMapR() { return this->_RatioToStart; }


		const POS_2D &GetStartPos() const { return this->_StartPos; }
		const POS_2D &GetDestPos() const { return this->_DestPos; }

	private:

		Map2D<MOVE_DIST_TYPE>	_DistToStart;			// Distance to start from any position
		Map2D<MOVE_DIST_TYPE>	_DistToGoal;			// Distance to goal from any position

		Map2D<OGM_LOG_TYPE>		_ProbToGoal;			// Probability to goal from any position
		Map2D<OGM_LOG_TYPE>		_ProbToStart;			// Probability to start from any position

		Map2D<OGM_LOG_TYPE>		_RatioToGoal;			// Ratio to goal from any position
		Map2D<OGM_LOG_TYPE>		_RatioToStart;			// Ratio to start from any position

		Map2D<MOVE_DIST_TYPE>	_RatioDistToStart;			// Distance to start from any position
		Map2D<MOVE_DIST_TYPE>	_RatioDistToGoal;			// Distance to goal from any position

		Map2D<OGM_LOG_TYPE>		_RatioProbToGoal;			// Probability to goal from any position
		Map2D<OGM_LOG_TYPE>		_RatioProbToStart;			// Probability to start from any position

		POS_2D _StartPos;				// Starting position
		POS_2D _DestPos;				// Destination

		// Resetting maps
		void ResetDistMap(const OGM_LOG_MAP_TYPE &OriginalMap, const POS_2D &TargetPos, const OGM_LOG_TYPE ObstacleValue, Map2D<MOVE_DIST_TYPE> &DistMap);
		void ResetProbMapToTarget(const OGM_LOG_MAP_TYPE &OriginalMap, const POS_2D &TargetPos, Map2D<OGM_LOG_TYPE> &ProbMap);
		void ResetProbMapFromTarget(const OGM_LOG_MAP_TYPE &OriginalMap, const POS_2D &TargetPos, Map2D<OGM_LOG_TYPE> &ProbMap);
		void ResetRatioMapToTarget(const OGM_LOG_MAP_TYPE &OriginalMap, const POS_2D &TargetPos, Map2D<OGM_LOG_TYPE> &RatioMap, Map2D<MOVE_DIST_TYPE> &RatioDistMap, Map2D<OGM_LOG_TYPE> &RatioProbMap);
		void ResetRatioMapFromTarget(const OGM_LOG_MAP_TYPE &OriginalMap, const POS_2D &TargetPos, Map2D<OGM_LOG_TYPE> &RatioMap, Map2D<MOVE_DIST_TYPE> &RatioDistMap, Map2D<OGM_LOG_TYPE> &RatioProbMap);

		// Updating maps
		void UpdateDistMap(const OGM_LOG_MAP_TYPE &UpdatedOriginalMap, const std::vector<POS_2D> &UpdatedValues, const POS_2D &TargetPos, const OGM_LOG_TYPE ObstacleValue, Map2D<MOVE_DIST_TYPE> &DistMap);
		void UpdateProbMapToTarget(const OGM_LOG_MAP_TYPE &UpdatedOriginalMap, const std::vector<POS_2D> &UpdatedValues, const POS_2D &TargetPos, Map2D<OGM_LOG_TYPE> &ProbMap);
		void UpdateProbMapFromTarget(const OGM_LOG_MAP_TYPE &UpdatedOriginalMap, const std::vector<POS_2D> &UpdatedValues, const POS_2D &TargetPos, Map2D<OGM_LOG_TYPE> &ProbMap);
		void UpdateRatioMapToTarget(const OGM_LOG_MAP_TYPE &UpdatedOriginalMap, const std::vector<POS_2D> &UpdatedValues, const POS_2D &TargetPos, Map2D<OGM_LOG_TYPE> &RatioMap, Map2D<MOVE_DIST_TYPE> &RatioDistMap, Map2D<OGM_LOG_TYPE> &RatioProbMap);
		void UpdateRatioMapFromTarget(const OGM_LOG_MAP_TYPE &UpdatedOriginalMap, const std::vector<POS_2D> &UpdatedValues, const POS_2D &TargetPos, Map2D<OGM_LOG_TYPE> &RatioMap, Map2D<MOVE_DIST_TYPE> &RatioDistMap, Map2D<OGM_LOG_TYPE> &RatioProbMap);

		inline OGM_LOG_TYPE CalculateRatio(const MOVE_DIST_TYPE Dist, const OGM_LOG_TYPE LogValueFromMaps) const;

		friend class QuadDStarMaps;
};

#endif // MONTE_CARLO_DSTAR_MAPS_H
