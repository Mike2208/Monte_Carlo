#include "test_map_2d.h"
#include "png_convert_image_to_ogm.h"

#include <stdlib.h>
#include <time.h>

int TestMap2D::CreateMapsFromProbabilityPNGFile(const char *FileName)
{
	// Get OGM map from file
	if(PNGConvertImageToOGM::ConvertPNGToOGM(FileName, this->_InitialMap) < 0)
		return -1;

	// Calculate "Real" map from OGM file
	this->EstimateRealMapFromOGM();

	return 1;
}

void TestMap2D::EstimateRealMapFromOGM()
{
	// Prepare real map
	this->_RealMap.ResizeMap(this->_InitialMap.GetWidth(), this->_InitialMap.GetHeight());

	// Seed randomizer
#ifdef DEBUG		// DEBUG
	std::cout << "Randomizer set to 0, will be repeatable" << std::endl;
	srand(0);	// Make it repeatable during debug
#else				// ~DEBUG
	srand(static_cast<unsigned int>(time(NULL)));
#endif

	// Estimate whether a position is occupied or not
	for(POS_2D_TYPE X=POS_2D_MIN; X<this->_InitialMap.GetWidth(); X++)
	{
		for(POS_2D_TYPE Y=POS_2D_MIN; Y<this->_InitialMap.GetHeight(); Y++)
		{
			// Get random value
			const OGM_CELL_TYPE randVal = rand() % (OGM_CELL_MAX);
			const POS_2D curPos(X,Y);

			// Check if this cell will be full/empty
			if(this->_InitialMap.GetPixel(curPos) >= randVal)
				this->_RealMap.SetPixel(curPos, OGM_DISCRETE_EMPTY);
			else
				this->_RealMap.SetPixel(curPos, OGM_DISCRETE_FULL);
		}
	}
}

void TestMap2D::RandomizeData(const RANDOMIZE_FACTOR_TYPE &RandomizeFactor, const OGM_CELL_TYPE MinValueToRandomize, const OGM_CELL_TYPE MaxValueToRandomize)
{
	// Seed randomizer
#ifdef DEBUG		// DEBUG
	std::cout << "Randomizer set to 0, will be repeatable" << std::endl;
	srand(0);	// Make it repeatable during debug
#else				// ~DEBUG
	srand(static_cast<unsigned int>(time(NULL)));
#endif

	// Change all cell values in valid range
	for(auto &curCell : this->_InitialMap.GetCellStorageR())
	{
		if(curCell >= MinValueToRandomize && curCell <= MaxValueToRandomize)
		{
			// Calculate random value
			OGM_CELL_TYPE randValue = rand()%RandomizeFactor;

			// Decide whether to add/subtract value and make sure values don't exceed minimum/maximum
			if(rand()%2)
			{
				if(curCell < randValue+OGM_CELL_MIN)
					curCell = OGM_CELL_MIN;
				else
					curCell -= randValue;
			}
			else
			{
				if(curCell > OGM_CELL_MAX-randValue)
					curCell = OGM_CELL_MAX;
				else
					curCell += randValue;
			}
		}
	}

	// Re-estimate real map
	this->EstimateRealMapFromOGM();
}

void TestMap2D::ScaleMapsDownByFactor(const SCALING_FACTOR_TYPE &DownScaleFactor)
{
	Map2D_Discrete newRealMap;
	OccupancyGridMap newInitialMap;

	// Get number of cells in new map
	const POS_2D_TYPE newWidth = static_cast<POS_2D_TYPE>(std::ceil(static_cast<double>(this->_RealMap.GetWidth())/DownScaleFactor));
	const POS_2D_TYPE newHeight = static_cast<POS_2D_TYPE>(std::ceil(static_cast<double>(this->_RealMap.GetHeight())/DownScaleFactor));

	newRealMap.ResizeMap(newWidth,newHeight);
	newInitialMap.ResizeMap(newWidth, newHeight);

	// Fill new cells
	POS_2D curPos;
	for(curPos.X = 0; curPos.X < newWidth; ++curPos.X)
	{
		for(curPos.Y = 0; curPos.Y < newHeight; ++curPos.Y)
		{
			const POS_2D oldCellPos(curPos.X*DownScaleFactor, curPos.Y*DownScaleFactor);

			newInitialMap.SetPixel(curPos, this->CalculateOGMScaledDownCell(oldCellPos, DownScaleFactor));
			newRealMap.SetPixel(curPos, this->CalculateRealMapScaledDownCell(oldCellPos, DownScaleFactor));
		}
	}

	this->_InitialMap = std::move(newInitialMap);
	this->_RealMap = std::move(newRealMap);
}

OGM_CELL_TYPE TestMap2D::CalculateOGMScaledDownCell(const POS_2D &OldCellPos, const SCALING_FACTOR_TYPE &Scale) const
{
	OGM_LOG_TYPE curLogProbability = 0;

	// Go through values that should be integrated into one
	for(SCALING_FACTOR_TYPE curValX = 0; curValX < Scale; ++curValX)
	{
		for(SCALING_FACTOR_TYPE curValY = 0; curValY < Scale; ++curValY)
		{
			const POS_2D curPos = OldCellPos + POS_2D(curValX, curValY);

			OGM_CELL_TYPE curCellVal;
			if(this->_InitialMap.GetPixel(curPos, curCellVal) < 0)
				continue;		// Skip if invalid pos (this happens at edges)

			curLogProbability += OccupancyGridMap::CalculateLogValFromCell(OGM_CELL_MAX - curCellVal);
		}
	}

	return OGM_CELL_MAX - OccupancyGridMap::CalculateCellValFromLog(curLogProbability);
}

OGM_DISCRETE_TYPE TestMap2D::CalculateRealMapScaledDownCell(const POS_2D &OldCellPos, const SCALING_FACTOR_TYPE &Scale) const
{
	OGM_DISCRETE_TYPE curVal;

	// Go through values that should be integrated into one
	for(SCALING_FACTOR_TYPE curValX = 0; curValX < Scale; ++curValX)
	{
		for(SCALING_FACTOR_TYPE curValY = 0; curValY < Scale; ++curValY)
		{
			const POS_2D curPos = OldCellPos + POS_2D(curValX, curValY);
			if(this->_RealMap.GetPixel(curPos, curVal) < 0)
				continue;		// Skip if invalid pos (this happens at edges)

			if(curVal == OGM_DISCRETE_FULL)
				return OGM_DISCRETE_FULL;		// If one cell is occupied, position can't be accessed
		}
	}

	return OGM_DISCRETE_EMPTY;
}
