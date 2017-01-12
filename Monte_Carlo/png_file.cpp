#include "png_file.h"

const char HEADER_SIZE = 8;			// 8 is the maximum size that can be checked

int  PNGFile::OpenPNGFile(const char *FileName, const char *Modes)
{
	// Close previously openend file if necessary
	this->ClosePNGFile();

	// Open file
	this->_File = fopen(FileName, Modes);
	if(this->_File == NULL)
		return -1;

	// Check that this is PNG file
	unsigned char header[HEADER_SIZE];
	if(fread(header, 1, HEADER_SIZE, this->_File) != HEADER_SIZE)
	{
		fclose(this->_File);
		this->_File = NULL;

		return -2;
	}

	bool is_png = !png_sig_cmp(header, 0, HEADER_SIZE);		// Compare header
	if (!is_png)
	{
		fclose(this->_File);
		this->_File = NULL;		// not png, close file

		return -3;
	}

	fseek(this->_File, 0, SEEK_SET);		// Move back to start

	return 1;
}

void PNGFile::ClosePNGFile()
{
	if(this->_File != NULL)
	{
		fclose(this->_File);
		this->_File = NULL;
	}
}

int PNGFile::ReadImageToArray(std::vector<PIXEL_TYPE> &ImageArray, IMAGE_SIZE_TYPE &ImageWidth, IMAGE_SIZE_TYPE &ImageHeight)
{
	// Check that a file is open
	if(this->_File == NULL)
		return -1;

	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr)
		return -2;

	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		return -3;
	}

	png_infop end_info = png_create_info_struct(png_ptr);
	if (!end_info)
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		return -4;
	}

//	if (setjmp(png_jmpbuf(png_ptr)))
//	{
//		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
//		return -5;
//	}

	// Connect png_struct with file to read
	png_init_io(png_ptr, this->_File);

	fseek(this->_File, 0, SEEK_SET);
	png_set_sig_bytes(png_ptr, 0);

	// Read image info and get height and width
	png_read_info(png_ptr, info_ptr);
	ImageWidth = png_get_image_width(png_ptr, info_ptr);
	ImageHeight = png_get_image_height(png_ptr, info_ptr);

	//png_set_rgb_to_gray(png_ptr, 1, 0, 0);
	//png_set_filler(png_ptr, 0, PNG_FILLER_AFTER);
	//png_set_strip_16(png_ptr);

	auto rowbytes = png_get_rowbytes(png_ptr, info_ptr);
//	auto color_type = png_get_color_type(png_ptr, info_ptr);
//	auto bit_depth  = png_get_bit_depth(png_ptr, info_ptr);

	// Create row pointers to save where the image should be stored
	std::vector<png_byte> tmpImage;
	tmpImage.resize(rowbytes*ImageHeight);
	std::vector<void *> rowPointers;
	rowPointers.resize(ImageHeight);
	for(size_t i=0; i<rowPointers.size(); ++i)
	{
		rowPointers.at(i) = &(tmpImage.at(rowbytes*i));
	}

	// Read image to tmpImage
	png_read_image(png_ptr, reinterpret_cast<png_bytepp>(&(rowPointers.at(0))));

	// Clean up
	png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
	fseek(this->_File, 0, SEEK_SET);

	// Move tmpImage to array and remove alpha value
	// Allocate the required memory
	ImageArray.resize(ImageWidth*ImageHeight);
	const unsigned char bytesPerPixel = static_cast<unsigned char>(rowbytes/ImageWidth);
	for(unsigned int X=0; X<ImageWidth; ++X)
	{
		for(unsigned int Y=0; Y<ImageHeight; ++Y)
		{
			//ImageArray[X+Y*ImageWidth] = tmpImage[X*8+Y*rowbytes];
			ImageArray.at(X+Y*ImageWidth) = tmpImage.at(X*bytesPerPixel+(ImageHeight-1-Y)*rowbytes);		// Mirror image to correct orientation
		}
	}

	return 1;
}
