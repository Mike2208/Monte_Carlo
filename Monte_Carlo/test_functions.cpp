#include "test_functions.h"

template<class T>
void PrintPath(const AlgorithmAStar::PATH_DATA &Path, const T &ValueToSet, Map2D<T> &Map)
{
	for(const auto &curPos : Path)
	{
		Map.SetPixel(curPos, ValueToSet);
	}
}

namespace TEST_FUNCTIONS
{
	struct TWO_VALS
	{
		PNGFile::PIXEL_TYPE Value;
		unsigned int Occurences = 0;

		TWO_VALS(const PNGFile::PIXEL_TYPE &_Value) : Value(_Value), Occurences(0) {}
	};

	struct TWO_VALS_COMPARE
	{
		bool operator()(const TWO_VALS &i, const TWO_VALS &j) const { return (i.Value < j.Value ? 1:0); }
	};

	int TestPNGFileReader()
	{
		PNGFile testFile;
		PNGFile::IMAGE_SIZE_TYPE height,width;
		std::vector<PNGFile::PIXEL_TYPE> image;
		testFile.OpenPNGFile("test.png", "rb");

		testFile.ReadImageToArray(image, width, height);

		testFile.ClosePNGFile();

		std::vector<TWO_VALS> diffPixels;
		bool alreadyRec;
		for(const auto &curPix : image)
		{
			alreadyRec = false;
			for(auto &prevVal : diffPixels)
			{
				if(curPix == prevVal.Value)
				{
					alreadyRec = true;
					prevVal.Occurences++;
				}
			}
			if(!alreadyRec)
				diffPixels.push_back(TWO_VALS(curPix));
		}

		std::sort(diffPixels.begin(), diffPixels.end(), TWO_VALS_COMPARE());

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

		//MonteCarloOption2 test(std::vector<OccupancyGridMap>{testMap});
		//test.PerformMonteCarlo(POS_2D(0,0), POS_2D(2,2), 1);

		std::cout.rdbuf(coutbuf); //reset to standard output again

		return 1;
	}

	int TestPolicy()
	{
		TestMap2D::SCALING_FACTOR_TYPE scale = 2;

		POS_2D startPos(100,50);
		POS_2D destPos(101,50);

		TestMap2D testMaps;
		testMaps.CreateMapsFromProbabilityPNGFile("test.png");

		testMaps.GetOGMap().PrintMap("/tmp/tmp.pgm");

		testMaps.ScaleMapsDownByFactor(scale);
		startPos = POS_2D(startPos.X/scale, startPos.Y/scale);
		destPos = POS_2D(destPos.X/scale, destPos.Y/scale);

		testMaps.GetOGMap().PrintMap("/tmp/tmp.pgm");

		//MonteCarloOption2 testMonteCarlo(std::vector<OccupancyGridMap>{testMaps.GetOGMapR()});
		//testMonteCarlo.PerformMonteCarlo(startPos, destPos, 1);

		//PolicyTree testPolicy;
		//PolicyMonteCarloConverter::ConvertMonteCarloToPolicyTree(testMonteCarlo.GetTree(), testPolicy);

		//class TestPolicy policyTester;
		//policyTester.PerformTest(testPolicy, startPos, destPos, testMaps.GetRealMap());

		return 1;
	}

	int TestVoronoiField()
	{
		OccupancyGridMap testMap;
		testMap.ResetMap(10,10, 0);
		testMap.SetPixel(POS_2D(2,2), OGM_CELL_MAX);
		testMap.SetPixel(POS_2D(7,7), OGM_CELL_MAX);
		testMap.SetPixel(POS_2D(2,7), OGM_CELL_MAX);
		testMap.SetPixel(POS_2D(7,2), OGM_CELL_MAX);
		//PNGConvertImageToOGM::ConvertPNGToOGM("test.png", testMap);

		testMap.PrintMap("/tmp/testOGM.pgm");

		DistrictMap testDistrict;
		testDistrict.SetFreeDistrict(testMap.GetWidth(), testMap.GetHeight(), 0);
		//testDistrict.SetGlobalMapPosition(POS_2D(0,0));
		//testDistrict.SetID(0);

		ALGORITHM_VORONOI_FIELDS::ID nextFreeID = 1;
		ALGORITHM_VORONOI_FIELDS::DISTRICT_STORAGE freeDistricts, occupiedDistricts;
		AlgorithmVoronoiFields<OccupancyGridMap::CELL_TYPE>::CalculateVoronoiField(testMap, 90, 0, testDistrict, occupiedDistricts, freeDistricts, nextFreeID);

		return 1;
	}

	int TestPath()
	{
		TestMap2D testMap(0, 100, 10);
		testMap.CreateMapsFromProbabilityPNGFile("test.png");

		unsigned int numCells = 0;
		for(const auto curData : testMap.GetOGMap().GetCellStorage())
		{
			if(curData != OGM_CELL_MIN && curData != OGM_CELL_MAX && curData != 10)
			{
				std::cout << std::to_string(curData) << std::endl;
				numCells++;
			}
		}

		//testMap.GetOGMap().PrintMap("/tmp/testOGM.pgm");
		testMap.GetRealMap().PrintMap("/tmp/testREAL.pbm");

		AlgorithmAStar tmpAStar;
		AlgorithmAStar::PATH_DATA tmpPath;
		AlgorithmAStar::DISTANCE_TYPE tmpDist;
		tmpAStar.CalculatePath(testMap.GetRealMap(), true, POS_2D(836,testMap.GetOGMap().GetHeight()-1-491), POS_2D(950,testMap.GetOGMap().GetHeight()-1-490), nullptr, &tmpPath, &tmpDist);

		PrintPath(tmpPath, true, testMap.GetRealMapR());

		testMap.GetRealMap().PrintMap("/tmp/testREAL_Path.pbm");

		return 1;
	}
}
