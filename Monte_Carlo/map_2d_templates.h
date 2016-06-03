#ifndef MAP_2D_TEMPLATES_H
#define MAP_2D_TEMPLATES_H

#include "standard_definitions.h"
#include "map_2d.h"

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
int Map2D<T>::GetPixel(const POS_2D &Position, T &Value) const
{
	if(Position.X >= this->_Width || Position.Y >= this->_Height
			|| Position.X < 0 || Position.Y < 0)
		return -1;

	Value = this->_CellData.at(Position.X + this->_Width*Position.Y);
	return 1;
}

#endif // MAP_2D_TEMPLATES_H
