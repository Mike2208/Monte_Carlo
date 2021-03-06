#ifndef MAP_2D_BOOL_H
#define MAP_2D_BOOL_H

#include "map_2d.h"

template<>
class Map2D<bool>
{
	public:
		typedef std::vector<bool> CELL_STORAGE;
		typedef bool CELL_TYPE;

		Map2D(const POS_2D_TYPE &NewWidth, const POS_2D_TYPE &NewHeight, const bool &DefaultCellValue) : _Height(NewHeight), _Width(NewWidth), _CellData(NewHeight*NewWidth) { this->ResetMap(NewWidth, NewHeight, DefaultCellValue); }

		Map2D() = default;
		Map2D(const Map2D &S) = default;
		Map2D(Map2D &&S) = default;
		Map2D &operator=(const Map2D &S) = default;
		Map2D &operator=(Map2D &&S) = default;

		void ResizeMap(const POS_2D_TYPE &NewWidth, const POS_2D_TYPE &NewHeight);
		void ResetMap(const POS_2D_TYPE &NewWidth, const POS_2D_TYPE &NewHeight, const bool &DefaultCellValue);

		virtual void SetMapToValue(const bool Value);
		virtual void SetPixel(const POS_2D &Position, const bool &Value);

		//bool &GetPixelR(const POS_2D &Position);
		virtual bool GetPixel(const POS_2D &Position) const;
		virtual int GetPixel(const POS_2D &Position, bool &Value) const;

		virtual void SetPathToValue(const POS_2D &StartPos, const POS_2D &EndPos, const bool&Value);		// Set the path from StartPos to EndPod to the given value

		virtual POS_2D_TYPE GetHeight() const;
		virtual POS_2D_TYPE GetWidth() const;

		// Gets entire cell storage ( usefull for parsing entire map )
		const CELL_STORAGE &GetCellStorage() const;
		CELL_STORAGE &GetCellStorageR();

		virtual void PrintMap(const char *FileName) const;

		virtual bool IsInMap(const POS_2D &Position) const;

	protected:

		POS_2D_TYPE		_Height;		// Map height
		POS_2D_TYPE		_Width;			// Map width
		CELL_STORAGE	_CellData;		// Data in cells
};

#endif // MAP_2D_BOOL_H
