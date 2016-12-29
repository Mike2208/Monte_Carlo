#ifndef QUAD_D_STAR_MAPS_H
#define QUAD_D_STAR_MAPS_H

#include "standard_definitions.h"
#include "monte_carlo_dstar_maps.h"
#include "quad_map.h"
#include "map_2d_move.h"

class QuadDStarMaps
{ 
	public:
		typedef QuadMap::ID ID;
		typedef QuadMap::ID_RECTANGLE ID_RECTANGLE;
		typedef QuadMap::ID_DATA ID_DATA;

		QuadDStarMaps(const POS_2D_TYPE MapWidth, const POS_2D_TYPE MapHeight);
		QuadDStarMaps(const OGM_LOG_MAP_TYPE &OriginalMap, const POS_2D &Start, const POS_2D &Destination);

		ID_RECTANGLE GetAdjacentRectangle(const ID CurQuadID, const ID AdjacentQuadID);
		ID GetQuadIDAtPos(const POS_2D &Position);
		const std::vector<ID> &GetAdjacentQuadIDs(const ID QuadID);

		void ResetDestPos(const OGM_LOG_MAP_TYPE &OriginalMap, const POS_2D &Destination);

		void ResetToOneQuad(const OGM_LOG_MAP_TYPE &OriginalMap, bool UpdateDStarDestMaps = true, bool UpdateDStarStartMaps = true);						// Reset map to one quad
		bool DivideQuad(const OGM_LOG_MAP_TYPE &OriginalMap, const std::vector<POS_2D> &UpdatedPositions, const ID QuadID, bool UpdateDStarDestMaps = true, bool UpdateDStarStartMaps = true);			// Divide a quad into four smaller ones if size is big enough

		const MonteCarloDStarMaps &UpdateDStarMaps(const OGM_LOG_MAP_TYPE &OriginalMap, const std::vector<POS_2D> &UpdatedPositions, const POS_2D &BotPosition, bool UpdateDStarDestMaps = true, bool UpdateDStarStartMaps = true);			// Recalculates D* maps
		void UpdateBotPosition(const OGM_LOG_MAP_TYPE &OriginalMap, const POS_2D &BotPosition);

		const POS_2D &GetDStarBotPos() const { return this->_BotPosition; }			// Robot position in calculated map
		bool IsSynced() const { return (this->_DestMapsSynced & this->_StartMapsSynced); }							// Returns whether data is synced

		void SyncMaps(const OGM_LOG_MAP_TYPE &OriginalMap);		// Syncs maps if they aren't already

		std::vector<POS_2D> CalculateRatioPathToGoal(const POS_2D &StartPos) { return this->_DStarMaps.CalculateBestRatioPathToGoal(StartPos); }
		std::vector<POS_2D> CalculateRatioPathFromStart(const POS_2D &TargetPos) { return this->_DStarMaps.CalculateBestRatioPathFromStart(TargetPos); }
		std::vector<POS_2D> CalculateRatioPathOverPos(const POS_2D &IntermediatePos) { return this->_DStarMaps.CalculateBestRatioPath(IntermediatePos); }

		operator const MonteCarloDStarMaps&() const { return this->_DStarMaps; }
		operator const QuadMap&() const { return this->_QuadMap; }

		const Map2DMove<MOVE_DIST_TYPE>		GetDStarDistMapToGoal() const { return Map2DMove<MOVE_DIST_TYPE>(const_cast<Map2D<MOVE_DIST_TYPE>&>(this->_DStarMaps.DistToGoalMap()), POS_2D(0,0)); }
		const Map2DMove<OGM_LOG_TYPE>		GetDStarProbMapToGoal() const { return Map2DMove<OGM_LOG_TYPE>(const_cast<Map2D<OGM_LOG_TYPE>&>(this->_DStarMaps.ProbToGoalMap()), POS_2D(0,0)); }
		const Map2DMove<OGM_LOG_TYPE>		GetDStarRatioMapToGoal() const { return Map2DMove<OGM_LOG_TYPE>(const_cast<Map2D<OGM_LOG_TYPE>&>(this->_DStarMaps.RatioToGoalMap()), POS_2D(0,0)); }
		const Map2DMove<MOVE_DIST_TYPE>		GetDStarDistMapToStart() const { return Map2DMove<MOVE_DIST_TYPE>(const_cast<Map2D<MOVE_DIST_TYPE>&>(this->_DStarMaps.DistToStartMap()), this->_BotQuadPos); }
		const Map2DMove<OGM_LOG_TYPE>		GetDStarProbMapToStart() const { return Map2DMove<OGM_LOG_TYPE>(const_cast<Map2D<OGM_LOG_TYPE>&>(this->_DStarMaps.ProbToStartMap()), this->_BotQuadPos); }
		const Map2DMove<OGM_LOG_TYPE>		GetDStarRatioMapToStart() const { return Map2DMove<OGM_LOG_TYPE>(const_cast<Map2D<OGM_LOG_TYPE>&>(this->_DStarMaps.RatioToStartMap()), this->_BotQuadPos); }
		const Map2DMove<OGM_LOG_TYPE>		GetDStarRatioCertaintyMapToStart() const { return Map2DMove<OGM_LOG_TYPE>(const_cast<Map2D<OGM_LOG_TYPE>&>(this->_DStarMaps.RatioCertaintyToStartMap()), this->_BotQuadPos); }
		const Map2DMove<OGM_LOG_TYPE>		GetDStarRatioCertaintyMapToGoal() const { return Map2DMove<OGM_LOG_TYPE>(const_cast<Map2D<OGM_LOG_TYPE>&>(this->_DStarMaps.RatioCertaintyToGoalMap()), POS_2D(0,0)); }
		const Map2DMove<OGM_LOG_TYPE>		GetDStarRatioDistMapToStart() const { return Map2DMove<OGM_LOG_TYPE>(const_cast<Map2D<OGM_LOG_TYPE>&>(this->_DStarMaps.RatioDistToStartMap()), this->_BotQuadPos); }
		const Map2DMove<OGM_LOG_TYPE>		GetDStarRatioDistMapToGoal() const { return Map2DMove<OGM_LOG_TYPE>(const_cast<Map2D<OGM_LOG_TYPE>&>(this->_DStarMaps.RatioDistToGoalMap()), POS_2D(0,0)); }

	private:

		QuadMap				_QuadMap;		// Quad Map data
		MonteCarloDStarMaps _DStarMaps;		// D* Maps
		POS_2D				_BotPosition;	// position of bot for which DStar maps are calculated
		POS_2D				_BotQuadPos;	// Position of quad bot is in
		bool				_DestMapsSynced;	// Are the D* maps to the destination synced with the current bot position and quad data
		bool				_StartMapsSynced;	// Are the D* maps from the start synced with the current bot position and quad data

//		Map2DMove<MOVE_DIST_TYPE>			_DStarDistMap;
//		Map2DMove<OGM_LOG_TYPE>				_DStarProbMap;
//		Map2DMove<OGM_LOG_TYPE>				_DStarRatioMap;
//		Map2DMove<OGM_LOG_TYPE>				_DStarRatioProbMap;
//		Map2DMove<MOVE_DIST_TYPE>			_DStarRatioDistMap;

		//inline void UpdateMoveMaps();
		inline void UpdateBotQuadPos();
		inline ID_RECTANGLE GetQuadRectangleWithEdges();
		inline ID GetBotQuadID() const { return this->_QuadMap.GetIDAtPos(this->_BotPosition); }
};

#endif // QUAD_D_STAR_MAPS_H
