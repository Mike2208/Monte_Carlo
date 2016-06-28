#include "png_convert_image_to_ogm.h"

int PNGConvertImageToOGM::ConvertPNGToOGM(const char *PNGFileName, OccupancyGridMap &NewMap)
{
	// Open file
	PNGFile curFile;
	if(curFile.OpenPNGFile(PNGFileName, "rb") < 0)
		return -1;

	// Read data
	PNGFile::IMAGE_SIZE_TYPE width, height;
	std::vector<PNGFile::PIXEL_TYPE> imageArray;
	if(curFile.ReadImageToArray(imageArray, width, height) < 0)
		return -2;

	// Close File
	curFile.ClosePNGFile();

	// Calculate all OGM values from PNG
	NewMap.ResizeMap(width, height);
	for(unsigned int X=POS_2D_MIN; X<width; ++X)
	{
		for(unsigned int Y=POS_2D_MIN; Y<height; ++Y)
		{
			NewMap.SetPixel(POS_2D(X,Y), PNGConvertImageToOGM::CalculateOGMValue(imageArray.at(X+width*Y)));
		}
	}

	return 1;
}

OGM_CELL_TYPE PNGConvertImageToOGM::CalculateOGMValue(const PNGFile::PIXEL_TYPE &Value)
{
	return static_cast<OGM_CELL_TYPE>(static_cast<double>(Value)/static_cast<double>(std::numeric_limits<PNGFile::PIXEL_TYPE>::max())*static_cast<double>(OGM_CELL_MAX));
}
