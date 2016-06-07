#ifndef PNG_FILE_H
#define PNG_FILE_H

#include "standard_definitions.h"

#include <stdlib.h>	// FILE *
#include <zlib.h>

#define PNG_DEBUG 3
#include <png.h>	//

#include <vector>

class PNGFile
{
	public:
		typedef unsigned char PIXEL_TYPE;
		typedef unsigned int IMAGE_SIZE_TYPE;

		PNGFile() : _File(NULL) {}
		~PNGFile() { this->ClosePNGFile(); }

		int OpenPNGFile(const char *FileName, const char *);
		void ClosePNGFile();

		int ReadImageToArray(std::vector<PIXEL_TYPE> &ImageArray, IMAGE_SIZE_TYPE &ImageWidth, IMAGE_SIZE_TYPE &ImageHeight);

	private:
		FILE	*_File;		// PNG File
};

#endif // PNG_FILE_H
