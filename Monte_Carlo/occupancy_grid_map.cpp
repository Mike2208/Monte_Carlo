#include "occupancy_grid_map.h"
#include <cmath>
#include <fstream>

OGM_CELL_TYPE OccupancyGridMap::CalculateCellValFromLog(const OGM_LOG_TYPE &Value)
{
	return OGM_CELL_MAX-static_cast<OGM_CELL_TYPE>(exp(log(OGM_CELL_MAX)-static_cast<double>(Value)));
}

OGM_LOG_TYPE OccupancyGridMap::CalculateCertaintyLogFromCell(const OGM_CELL_TYPE &Value)
{
	return static_cast<OGM_LOG_TYPE>(-log(OGM_CELL_MAX-Value)+log(OGM_CELL_MAX));
}

OGM_LOG_TYPE OccupancyGridMap::CalculateLogValFromCell(const OGM_CELL_TYPE &Value)
{
	return static_cast<OGM_LOG_TYPE>(-log(Value)+log(OGM_CELL_MAX));
}

OGM_PROB_TYPE OccupancyGridMap::CalculateProbValFromCell(const OGM_CELL_TYPE &Value)
{
	// basically converts from char to float
	return (static_cast<OGM_PROB_TYPE>(Value)/static_cast<OGM_PROB_TYPE>(OGM_CELL_MAX));
}

void OccupancyGridMap::CalculateLogMapFromCellMap(const OGM_MAP_TYPE &CellMap, OGM_LOG_MAP_TYPE &LogMap)
{
	LogMap.ResizeMap(CellMap.GetWidth(), CellMap.GetHeight());

	for(POS_2D_TYPE X=POS_2D_MIN; X<CellMap.GetWidth(); X++)
	{
		for(POS_2D_TYPE Y=POS_2D_MIN; Y<CellMap.GetHeight(); Y++)
		{
			POS_2D tmpPos(X,Y);
			LogMap.GetPixelR(tmpPos) = OccupancyGridMap::CalculateLogValFromCell(CellMap.GetPixel(tmpPos));
		}
	}
}

void OccupancyGridMap::CalculateCertaintyLogMapFromCellMap(const OGM_MAP_TYPE &CellMap, OGM_LOG_MAP_TYPE &CertaintyMap)
{
	CertaintyMap.ResizeMap(CellMap.GetWidth(), CellMap.GetHeight());

	for(POS_2D_TYPE X=POS_2D_MIN; X<CellMap.GetWidth(); X++)
	{
		for(POS_2D_TYPE Y=POS_2D_MIN; Y<CellMap.GetHeight(); Y++)
		{
			POS_2D tmpPos(X,Y);
			CertaintyMap.GetPixelR(tmpPos) = OccupancyGridMap::CalculateCertaintyLogFromCell(CellMap.GetPixel(tmpPos));
		}
	}
}

void OccupancyGridMap::CalculateProbMapFromCellMap(const OGM_MAP_TYPE &CellMap, OGM_PROB_MAP_TYPE &ProbMap)
{
	ProbMap.ResizeMap(CellMap.GetWidth(), CellMap.GetHeight());

	for(POS_2D_TYPE X=POS_2D_MIN; X<CellMap.GetWidth(); X++)
	{
		for(POS_2D_TYPE Y=POS_2D_MIN; Y<CellMap.GetHeight(); Y++)
		{
			POS_2D tmpPos(X,Y);
			ProbMap.GetPixelR(tmpPos) = OccupancyGridMap::CalculateProbValFromCell(CellMap.GetPixel(tmpPos));
		}
	}
}

void OccupancyGridMap::CalculateProbMapFromCertaintyLogMap(const OGM_LOG_MAP_TYPE &CertaintyLogMap, OGM_PROB_MAP_TYPE &ProbMap)
{
	ProbMap.ResizeMap(CertaintyLogMap.GetWidth(), CertaintyLogMap.GetHeight());

	for(POS_2D_TYPE X=POS_2D_MIN; X<CertaintyLogMap.GetWidth(); X++)
	{
		for(POS_2D_TYPE Y=POS_2D_MIN; Y<CertaintyLogMap.GetHeight(); Y++)
		{
			POS_2D tmpPos(X,Y);
			ProbMap.GetPixelR(tmpPos) = OccupancyGridMap::CalculateProbValueFromCertaintyLog(CertaintyLogMap.GetPixel(tmpPos));
		}
	}
}

OGM_LOG_TYPE OccupancyGridMap::CalculateLogValueFromProb(const OGM_PROB_TYPE &Value)
{
	return static_cast<OGM_LOG_TYPE>(-log(OGM_PROB_MAX-Value));
}

OGM_LOG_TYPE OccupancyGridMap::CalculateCertaintyLogValueFromCertaintyProb(const OGM_PROB_TYPE &Value)
{
	return static_cast<OGM_LOG_TYPE>(-log(Value));
}

OGM_PROB_TYPE OccupancyGridMap::CalculateCertaintyProbValueFromCertaintyLog(const OGM_LOG_TYPE &Value)
{
	return static_cast<OGM_PROB_TYPE>(exp(-Value));
}

OGM_PROB_TYPE CalculateProbValueFromCertaintyLog(const OGM_LOG_TYPE &Value)
{
	return OGM_PROB_MAX-static_cast<OGM_PROB_TYPE>(exp(-Value));
}

OGM_PROB_TYPE OccupancyGridMap::CalculateCertaintyProbFromLog(const OGM_LOG_TYPE &Value)
{
	return OGM_PROB_MAX-static_cast<OGM_PROB_TYPE>(exp(-Value));
}

OGM_PROB_TYPE OccupancyGridMap::CalculateProbValueFromCertaintyLog(const OGM_LOG_TYPE &Value)
{
	return OGM_PROB_MAX - static_cast<OGM_PROB_TYPE>(exp(-Value));
}

OGM_LOG_TYPE OccupancyGridMap::CalculateCertaintyLogFromProb(const OGM_PROB_TYPE &Value)
{
	return static_cast<OGM_LOG_TYPE>(-log(OGM_PROB_MAX-Value));
}

OGM_ENTROPY_TYPE OccupancyGridMap::CalculateEntropyFromCell(const OGM_CELL_TYPE &Value)
{
	// prevent 0*inf
	if(Value == OGM_CELL_MAX || Value == OGM_CELL_MIN)
		return 0;

	// Convert to probability
	const OGM_PROB_TYPE prob = OccupancyGridMap::CalculateProbValFromCell(Value);
	const OGM_PROB_TYPE invertedProb = OGM_PROB_MAX-prob;
	return static_cast<OGM_ENTROPY_TYPE>(-(log2(prob)*prob+log2(invertedProb)*invertedProb));
}

OGM_ENTROPY_TYPE OccupancyGridMap::CalculateEntropyFromProb(const OGM_PROB_TYPE &Value)
{
	// prevent 0*inf
	if(Value == OGM_PROB_MAX || Value == OGM_PROB_MIN)
		return 0;

	const OGM_PROB_TYPE invertedVal = OGM_PROB_MAX - Value;
	return static_cast<OGM_ENTROPY_TYPE>(-(log2(Value)*Value+log2(invertedVal)*invertedVal));
}

OGM_ENTROPY_TYPE OccupancyGridMap::CalculateEntropyFromMap(const OGM_MAP_TYPE &Map)
{
	// Add together all cell entropy values
	OGM_ENTROPY_TYPE retEntropy = 0;
	for(POS_2D_TYPE X=POS_2D_MIN; X<Map.GetWidth(); ++X)
	{
		for(POS_2D_TYPE Y=POS_2D_MIN; Y<Map.GetHeight(); ++Y)
		{
			retEntropy += OccupancyGridMap::CalculateEntropyFromCell(Map.GetPixel(POS_2D(X,Y)));
		}
	}

	return retEntropy;
}
