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
	for(POS_2D_TYPE X=0; X<this->_InitialMap.GetWidth(); X++)
	{
		for(POS_2D_TYPE Y=0; Y<this->_InitialMap.GetHeight(); Y++)
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
