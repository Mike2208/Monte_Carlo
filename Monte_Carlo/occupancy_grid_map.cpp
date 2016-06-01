#include "occupancy_grid_map.h"
#include <cmath>

OGM_LOG_TYPE OccupancyGridMap::CalculateLogValFromCell(const OGM_CELL_TYPE &Value)
{
	return static_cast<OGM_LOG_TYPE>(-log(OGM_CELL_MAX-Value)+log(OGM_CELL_MAX));
}

OGM_PROB_TYPE OccupancyGridMap::CalculateProbValFromCell(const OGM_CELL_TYPE &Value)
{
	// basically converts from char to float
	return (static_cast<OGM_PROB_TYPE>(Value)/static_cast<OGM_PROB_TYPE>(OGM_CELL_MAX));
}

void OccupancyGridMap::CalculateLogMapFromCellMap(const OGM_MAP_TYPE &CellMap, OGM_LOG_MAP_TYPE &LogMap)
{
	LogMap.ResizeMap(CellMap.GetWidth(), CellMap.GetHeight());

	for(POS_2D_TYPE X=0; X<CellMap.GetWidth(); X++)
	{
		for(POS_2D_TYPE Y=0; Y<CellMap.GetHeight(); Y++)
		{
			POS_2D tmpPos(X,Y);
			LogMap.GetPixelR(tmpPos) = OccupancyGridMap::CalculateLogValFromCell(CellMap.GetPixel(tmpPos));
		}
	}
}

OGM_LOG_TYPE OccupancyGridMap::CalculateLogValueFromProb(const OGM_PROB_TYPE &Value)
{
	return static_cast<OGM_LOG_TYPE>(-log(OGM_PROB_MAX-Value));
}

OGM_PROB_TYPE OccupancyGridMap::CalculateProbValueFromLog(const OGM_LOG_TYPE &Value)
{
	return OGM_PROB_MAX-static_cast<OGM_PROB_TYPE>(exp(-Value));
}

OGM_ENTROPY_TYPE OccupancyGridMap::CalculateEntropyFromCell(const OGM_CELL_TYPE &Value)
{
	// prevent 0*inf
	if(Value == OGM_CELL_MAX || Value == OGM_CELL_MIN)
		return 0;

	OGM_CELL_TYPE invertedVal = OGM_CELL_MAX-Value;
	return static_cast<OGM_ENTROPY_TYPE>(-(log2(Value)*Value+log2(invertedVal)*invertedVal));
}

OGM_ENTROPY_TYPE OccupancyGridMap::CalculateEntropyFromMap(const OGM_MAP_TYPE &Map)
{
	// Add together all cell entropy values
	OGM_ENTROPY_TYPE retEntropy = 0;
	for(POS_2D_TYPE X=0; X<Map.GetWidth(); ++X)
	{
		for(POS_2D_TYPE Y=0; Y<Map.GetHeight(); ++Y)
		{
			retEntropy += OccupancyGridMap::CalculateEntropyFromCell(Map.GetPixel(POS_2D(X,Y)));
		}
	}

	return retEntropy;
}
