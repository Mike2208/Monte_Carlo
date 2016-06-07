#include "standard_definitions.h"
#include "monte_carlo_option1.h"
#include "png_convert_image_to_ogm.h"

#include <fstream>
#include <iostream>

using namespace std;

int main()
{
	OccupancyGridMap testMap;
	PNGConvertImageToOGM::ConvertPNGToOGM("test.png", testMap);

	testMap.PrintMap("/tmp/tmp.pgm");

	return 1;
}

int TestPNGFileReader()
{
	PNGFile testFile;
	PNGFile::IMAGE_SIZE_TYPE height,width;
	std::vector<PNGFile::PIXEL_TYPE> image;
	testFile.OpenPNGFile("test.png", "rb");

	testFile.ReadImageToArray(image, width, height);

	testFile.ClosePNGFile();

	std::vector<PNGFile::PIXEL_TYPE> diffPixels;
	bool alreadyRec;
	for(const auto &curPix : image)
	{
		alreadyRec = false;
		for(const auto &prevVal : diffPixels)
		{
			if(curPix == prevVal)
			{
				alreadyRec = true;
				break;
			}
		}
		if(!alreadyRec)
			diffPixels.push_back(curPix);
	}

	std::sort(diffPixels.begin(), diffPixels.end());

	return 1;
}

int TestMonteCarlo1()
{
	std::ofstream out("/tmp/tmp.txt");
	std::streambuf *coutbuf = std::cout.rdbuf(); //save old buf
	std::cout.rdbuf(out.rdbuf()); //redirect std::cout to out.txt!

	Map2D<OGM_CELL_TYPE> tMap;
	tMap.ResetMap(3,3, OGM_CELL_MIN);
	tMap.SetPixel(POS_2D(1,0), OGM_CELL_MAX/4);
	tMap.SetPixel(POS_2D(1,1), OGM_CELL_MAX/3);
	tMap.SetPixel(POS_2D(1,2), OGM_CELL_MAX/2);

	OccupancyGridMap testMap;
	testMap.SetMap(tMap);

	MonteCarloOption1 test;
	test.PerformMonteCarlo(testMap, POS_2D(0,0), POS_2D(2,2));

	std::cout.rdbuf(coutbuf); //reset to standard output again

	return 1;
}
