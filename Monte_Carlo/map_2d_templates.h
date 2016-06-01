#ifndef MAP_2D_TEMPLATES_H
#define MAP_2D_TEMPLATES_H

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
	const POS_2D_TYPE cellPosInVector = Position.X + this->_Width*Position.Y;
	if(cellPosInVector >= this->_CellData.size())
		return -1;

	Value = this->_CellData.at(cellPosInVector);
	return 1;
}

#endif // MAP_2D_TEMPLATES_H
