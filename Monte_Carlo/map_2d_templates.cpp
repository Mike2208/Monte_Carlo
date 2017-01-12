#ifndef MAP_2D_TEMPLATES_H
#define MAP_2D_TEMPLATES_H

#include "standard_definitions.h"
#include "map_2d.h"
#include "robot_navigation.h"

#include <cmath>
#include <type_traits>
#include <fstream>

template<class T>
void Map2D<T>::ResizeMap(const POS_2D_TYPE &NewWidth, const POS_2D_TYPE &NewHeight)
{
	this->_Width = NewWidth;
	this->_Height = NewHeight;

	this->_CellData.resize(NewWidth*NewHeight);
}

template<class T>
void Map2D<T>::ResetMap(const POS_2D_TYPE &NewWidth, const POS_2D_TYPE &NewHeight, const T &DefaultCellValue)
{
	this->ResizeMap(NewWidth, NewHeight);

	for(auto &curCell : this->_CellData)
		curCell = DefaultCellValue;
}

template<class T>
void Map2D<T>::SetMapToValue(const T &Value)
{
	for(auto &cellValue : this->_CellData)
		cellValue = Value;
}

template<class T>
int Map2D<T>::GetPixel(const POS_2D &Position, T &Value) const
{
	if(Position.X >= this->_Width || Position.Y >= this->_Height
			|| Position.X < 0 || Position.Y < 0)
		return -1;

	Value = this->_CellData.at(Position.X + this->_Width*Position.Y);
	return 1;
}

template<class T>
void Map2D<T>::SetPathToValue(const POS_2D &StartPos, const POS_2D &EndPos, const T &Value)
{
	// Set start position to correct value
	this->SetPixel(StartPos, Value);

	// Go through path
	POS_2D curPos = StartPos;
	while(curPos != EndPos)
	{
		MAP_2D::DIST_TYPE bestDist = GetInfiniteVal<MAP_2D::DIST_TYPE>();
		POS_2D bestPos;

		// Go through all adjacent positions and find the one closest to destination
		for(const auto &navOption : NavigationOptions)
		{
			// Calculate distance
			const POS_2D adjacentPos = curPos+navOption;
			MAP_2D::DIST_TYPE curDist;

			if(EndPos.X > adjacentPos.X)
				curDist = (EndPos.X-adjacentPos.X)*(EndPos.X-adjacentPos.X);
			else
				curDist = (adjacentPos.X-EndPos.X)*(adjacentPos.X-EndPos.X);

			if(EndPos.Y > adjacentPos.Y)
				curDist += (EndPos.Y-adjacentPos.Y)*(EndPos.Y-adjacentPos.Y);
			else
				curDist += (adjacentPos.Y-EndPos.Y)*(adjacentPos.Y-EndPos.Y);

			curDist = sqrtf(curDist);

			// Compare distances
			if(curDist <= bestDist)
			{
				bestDist = curDist;
				bestPos = curPos;
			}
		}

		// Move to next position
		curPos = bestPos;

		// Set pixel
		this->SetPixel(curPos, Value);
	}
}

template<class T>
void Map2D<T>::PrintMap(const char *FileName, const T &MaxVal, const T &MinVal) const
{
	std::fstream file;
	file.open(FileName, std::ios_base::out);

	// Write header with height and width
	file << "P2" << std::endl;
	file << this->GetWidth() << " " << this->GetHeight() << std::endl;
	file << std::to_string(255) << std::endl;		// Max value

	for(POS_2D_TYPE Y=this->GetHeight()-1; (std::is_signed<POS_2D_TYPE>() ? Y>=POS_2D_MIN : Y<this->GetHeight()); --Y)
	{
		for(POS_2D_TYPE X=POS_2D_MIN; X<this->GetWidth(); ++X)
		{
			const auto &curPixelVal = this->GetPixel(POS_2D(X,Y));
			if(curPixelVal > MaxVal)
				file << std::to_string(static_cast<unsigned char>(255)) << " ";
			else
				file << std::to_string(static_cast<unsigned char>(255.0f*static_cast<float>(curPixelVal - MinVal)/static_cast<float>(MaxVal))) << " ";
		}

		file << std::endl;		// Begin new line after every finished image line
	}

	file.close();		// Close file after finish
}

template<class T>
bool Map2D<T>::IsInMap(const POS_2D &Position) const
{
	if(Position.X >= this->_Width || Position.Y >= this->_Height
			|| Position.X < 0 || Position.Y < 0)
		return false;

	return true;
}

#endif // MAP_2D_TEMPLATES_H
