#ifndef OCCUPANCY_GRID_MAP_H
#define OCCUPANCY_GRID_MAP_H

#include "standard_definitions.h"
#include "map_2d.h"
#include <limits>

typedef char OGM_CELL_TYPE;
const OGM_CELL_TYPE	OGM_CELL_MAX = 100;
const OGM_CELL_TYPE	OGM_CELL_MIN = 0;

typedef float OGM_PROB_TYPE;
const OGM_PROB_TYPE	OGM_PROB_MAX = 1;
const OGM_PROB_TYPE OGM_PROB_MIN = 0;

typedef float OGM_LOG_TYPE;
const OGM_LOG_TYPE OGM_LOG_MAX = GetInfiniteVal<OGM_LOG_TYPE>();
const OGM_LOG_TYPE OGM_LOG_MIN = 0;

typedef float OGM_ENTROPY_TYPE;

typedef Map2D<OGM_CELL_TYPE> OGM_MAP_TYPE;
typedef Map2D<OGM_LOG_TYPE>	OGM_LOG_MAP_TYPE;

class OccupancyGridMap : public OGM_MAP_TYPE
{
	public:
		OccupancyGridMap();

		static OGM_LOG_TYPE CalculateLogValFromCell(const OGM_CELL_TYPE &Value);		// Cell -> Log
		static OGM_PROB_TYPE CalculateProbValFromCell(const OGM_CELL_TYPE &Value);		// Cell -> Prob

		static void CalculateLogMapFromCellMap(const OGM_MAP_TYPE &CellMap, OGM_LOG_MAP_TYPE &LogMap);		// Cell Map -> Log Map

		static OGM_LOG_TYPE CalculateLogValueFromProb(const OGM_PROB_TYPE &Value);		// Prob-> Log

		static OGM_PROB_TYPE CalculateProbValueFromLog(const OGM_LOG_TYPE &Value);		// Log -> Prob

		static OGM_ENTROPY_TYPE CalculateEntropyFromCell(const OGM_CELL_TYPE &Value);	// Cell -> Entropy

		static OGM_ENTROPY_TYPE CalculateEntropyFromMap(const OGM_MAP_TYPE &Map);		// Cellmap -> Entropy
};

#endif // OCCUPANCY_GRID_MAP_H
