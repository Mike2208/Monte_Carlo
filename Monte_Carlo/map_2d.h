#ifndef MAP_2D_H
#define MAP_2D_H

#include "standard_definitions.h"
#include "pos_2d.h"
#include <vector>

namespace MAP_2D
{
	typedef float DIST_TYPE;
}

template<class T>
class Map2D
{
	public:
		typedef std::vector<T> CELL_STORAGE;
		typedef typename std::vector<T>::size_type CELL_STORAGE_INDEX;
		typedef T CELL_TYPE;

		Map2D(const POS_2D_TYPE &NewWidth, const POS_2D_TYPE &NewHeight, const T &DefaultCellValue) : _Height(NewHeight), _Width(NewWidth), _CellData(NewHeight*NewWidth) { this->ResetMap(NewWidth, NewHeight, DefaultCellValue); }

		Map2D() = default;
		Map2D(const Map2D &S) = default;
		Map2D(Map2D &&S) = default;
		Map2D &operator=(const Map2D &S) = default;
		Map2D &operator=(Map2D &&S) = default;

		void ResizeMap(const POS_2D_TYPE &NewWidth, const POS_2D_TYPE &NewHeight);
		void ResetMap(const POS_2D_TYPE &NewWidth, const POS_2D_TYPE &NewHeight, const T &DefaultCellValue);

		virtual void SetMapToValue(const T &Value);
		virtual void SetPixel(const POS_2D &Position, const T &Value) { this->_CellData.at(Position.X+this->_Width*Position.Y) = Value; }

		virtual T &GetPixelR(const POS_2D &Position) { return this->_CellData.at(Position.X+this->_Width*Position.Y); }
		virtual const T &GetPixel(const POS_2D &Position) const { return this->_CellData.at(Position.X+this->_Width*Position.Y); }
		virtual int GetPixel(const POS_2D &Position, T &Value) const;

		virtual void SetPathToValue(const POS_2D &StartPos, const POS_2D &EndPos, const T &Value);		// Set the path from StartPos to EndPod to the given value

		virtual POS_2D_TYPE GetHeight() const { return this->_Height; }
		virtual POS_2D_TYPE GetWidth() const { return this->_Width; }

		// Iterator access
		virtual const CELL_STORAGE_INDEX GetElementPosInStorage(const POS_2D &Position) const { return Position.X+this->_Width*Position.Y; }
		const T &GetPixelFromStorage(const CELL_STORAGE_INDEX &ElementPosition) const { return this->_CellData.at(ElementPosition); }

		// Gets entire cell storage ( usefull for parsing entire map )
		const CELL_STORAGE &GetCellStorage() const { return this->_CellData; }
		CELL_STORAGE &GetCellStorageR() { return this->_CellData; }

		// Prints map to file
		virtual void PrintMap(const char *FileName, const T &MaxVal, const T &MinVal) const;

		virtual bool IsInMap(const POS_2D &Position) const;

	protected:

		POS_2D_TYPE		_Height;		// Map height
		POS_2D_TYPE		_Width;			// Map width
		CELL_STORAGE	_CellData;		// Data in cells
};

#include "map_2d_bool.h"

#include "map_2d_templates.cpp"

typedef bool OGM_DISCRETE_TYPE;
const OGM_DISCRETE_TYPE OGM_DISCRETE_FULL = 1;
const OGM_DISCRETE_TYPE OGM_DISCRETE_EMPTY = 0;
typedef Map2D<bool> Map2D_Discrete;

#endif // MAP_2D_H
