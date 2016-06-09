#ifndef MAP_2D_H
#define MAP_2D_H

#include "standard_definitions.h"
#include "pos_2d.h"
#include <vector>

template<class T>
class Map2D
{
		typedef std::vector<T> CELL_STORAGE;
	public:

		//Map2D() = default;

		void ResizeMap(const POS_2D_TYPE &NewWidth, const POS_2D_TYPE &NewHeight);
		void ResetMap(const POS_2D_TYPE &NewWidth, const POS_2D_TYPE &NewHeight, const T &DefaultCellValue);

		void SetPixel(const POS_2D &Position, const T &Value) { this->_CellData.at(Position.X+this->_Width*Position.Y) = Value; }

		T &GetPixelR(const POS_2D &Position) { return this->_CellData.at(Position.X+this->_Width*Position.Y); }
		const T &GetPixel(const POS_2D &Position) const { return this->_CellData.at(Position.X+this->_Width*Position.Y); }
		int GetPixel(const POS_2D &Position, T &Value) const;

		POS_2D_TYPE GetHeight() const { return this->_Height; }
		POS_2D_TYPE GetWidth() const { return this->_Width; }

		// Gets entire cell storage ( usefull for parsing entire map )
		const CELL_STORAGE &GetCellStorage() const { return this->_CellData; }

	private:

		POS_2D_TYPE		_Height;		// Map height
		POS_2D_TYPE		_Width;			// Map width
		CELL_STORAGE	_CellData;		// Data in cells
};

#include "map_2d_templates.h"

typedef bool OGM_DISCRETE_TYPE;
const OGM_DISCRETE_TYPE OGM_DISCRETE_FULL = 1;
const OGM_DISCRETE_TYPE OGM_DISCRETE_EMPTY = 0;
typedef Map2D<bool> Map2D_Discrete;

#endif // MAP_2D_H
