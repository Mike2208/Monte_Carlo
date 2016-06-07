#ifndef PNG_CONVERT_IMAGE_TO_OGM_H
#define PNG_CONVERT_IMAGE_TO_OGM_H

#include "png_file.h"
#include "occupancy_grid_map.h"

class PNGConvertImageToOGM
{
	public:
		static int ConvertPNGToOGM(const char *PNGFileName, OccupancyGridMap &NewMap);

		static OGM_CELL_TYPE CalculateOGMValue(const PNGFile::PIXEL_TYPE &Value);
};

#endif // PNG_CONVERT_IMAGE_TO_OGM_H
