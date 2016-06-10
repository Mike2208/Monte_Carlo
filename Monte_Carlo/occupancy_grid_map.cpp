#include "occupancy_grid_map.h"
#include <cmath>
#include <fstream>

void OccupancyGridMap::PrintMap(const char *FileName)
{
	std::fstream file;
	file.open(FileName, std::ios_base::out);

	// Write header with height and width
	file << "P2" << std::endl;
	file << this->GetWidth() << " " << this->GetHeight() << std::endl;
	file << std::to_string(OGM_CELL_MAX) << std::endl;		// Max value

	for(POS_2D_TYPE Y=this->GetHeight()-1; (std::is_signed<POS_2D_TYPE>() ? Y>=POS_2D_MIN : Y<this->GetHeight()); --Y)
	{
		for(POS_2D_TYPE X=POS_2D_MIN; X<this->GetWidth(); ++X)
		{
			file << std::to_string(this->GetPixel(POS_2D(X,Y))) << " ";
		}

		file << std::endl;		// Begin new line after every finished image line
	}

	file.close();		// Close file after finish
}

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

	for(POS_2D_TYPE X=POS_2D_MIN; X<CellMap.GetWidth(); X++)
	{
		for(POS_2D_TYPE Y=POS_2D_MIN; Y<CellMap.GetHeight(); Y++)
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

OGM_PROB_TYPE OccupancyGridMap::CalculateCertaintyFromLog(const OGM_LOG_TYPE &Value)
{
	return static_cast<OGM_PROB_TYPE>(exp(-Value));
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
