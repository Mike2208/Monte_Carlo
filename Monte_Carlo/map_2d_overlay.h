#ifndef MAP_2D_OVERLAY_H
#define MAP_2D_OVERLAY_H

#include "standard_definitions.h"
#include "map_2d.h"

template<class T>
class Map2DOverlay : public virtual Map2D<T>
{
	public:
		typedef std::vector<T> CELL_STORAGE;
		typedef typename std::vector<T>::size_type CELL_STORAGE_INDEX;
		typedef T CELL_TYPE;

		Map2DOverlay(Map2D<T> OriginalMap, const POS_2D &BottomLeftPos, const POS_2D_TYPE Width, const POS_2D_TYPE Height)  : _OriginalMap(OriginalMap), _BottomLeftPos(BottomLeftPos), _OverlayWidth(Width), _OverlayHeight(Height)
		{}

		void SetMapToValue(const T &Value) override;
		void SetPixel(const POS_2D &Position, const T &Value) override;

		T &GetPixelR(const POS_2D &Position) override;
		const T &GetPixel(const POS_2D &Position) const override;
		int GetPixel(const POS_2D &Position, T &Value) const override;

		void SetPathToValue(const POS_2D &StartPos, const POS_2D &EndPos, const T &Value) override;		// Set the path from StartPos to EndPod to the given value

		POS_2D_TYPE GetHeight() const override { return this->_OverlayHeight; }
		POS_2D_TYPE GetWidth() const override { return this->_OverlayWidth; }

		// Iterator access
		const CELL_STORAGE_INDEX GetElementPosInStorage(const POS_2D &Position) const  override;

		// Prints map to file
		void PrintMap(const char *FileName, const T &MaxVal, const T &MinVal) const override;

		bool IsInMap(const POS_2D &Position) const override;

	private:

		POS_2D			_BottomLeftPos;			// Bottom left position of overlaid map
		POS_2D_TYPE		_OverlayWidth;			// Width of overlay
		POS_2D_TYPE		_OverlayHeight;			// Height of overlay
		Map2D<T>	&_OriginalMap;
};

template<class T>
void Map2DOverlay<T>::SetMapToValue(const T &Value)
{
	POS_2D curPos;
	for(curPos.X = this->_BottomLeftPos.X; curPos.X <= this->_Width; ++curPos.X)
	{
		for(curPos.Y = this->_BottomLeftPos.Y; curPos.Y <= this->_Height; ++curPos.Y)
		{
			this->_OriginalMap.SetPixel(curPos, Value);
		}
	}
}

template<class T>
void Map2DOverlay<T>::SetPixel(const POS_2D &Position, const T &Value)
{
	// If position is too big, let map2D throw error
	if(Position.X >= this->_OverlayWidth || Position.Y >= this->_OverlayHeight)
	{
		this->_OriginalMap.SetPixel(POS_2D(this->_Width, this->_Height), Value);
		return;
	}

	this->_OriginalMap.SetPixel(this->_BottomLeftPos+Position, Value);
}

template<class T>
T &Map2DOverlay<T>::GetPixelR(const POS_2D &Position)
{
	// If position is too big, let map2D throw error
	if(Position.X >= this->_OverlayWidth || Position.Y >= this->_OverlayHeight)
	{
		return this->_OriginalMap.GetPixelR(POS_2D(this->_Width, this->_Height));
	}

	return this->_OriginalMap.GetPixelR(this->_BottomLeftPos+Position);
}

template<class T>
const T &Map2DOverlay<T>::GetPixel(const POS_2D &Position) const
{
	// If position is too big, let map2D throw error
	if(Position.X >= this->_OverlayWidth || Position.Y >= this->_OverlayHeight)
	{
		return this->_OriginalMap.GetPixelR(POS_2D(this->_Width, this->_Height));
	}

	return this->_OriginalMap.GetPixelR(this->_BottomLeftPos+Position);
}

template<class T>
int Map2DOverlay<T>::GetPixel(const POS_2D &Position, T &Value) const
{
	// If position is too big, let map2D throw error
	if(Position.X >= this->_OverlayWidth || Position.Y >= this->_OverlayHeight)
	{
		return this->_OriginalMap.GetPixel(POS_2D(this->_Width, this->_Height), Value);
	}

	return this->_OriginalMap.GetPixel(this->_BottomLeftPos+Position, Value);
}

template<class T>
void Map2DOverlay<T>::SetPathToValue(const POS_2D &StartPos, const POS_2D &EndPos, const T &Value)
{
	// If position is too big, let map2D throw error
	if(StartPos.X >= this->_OverlayWidth || StartPos.Y >= this->_OverlayHeight ||
		EndPos.X >= this->_OverlayWidth || EndPos.Y >= this->_OverlayHeight)
	{
		this->_OriginalMap.SetPathToValue(POS_2D(this->_Width, this->_Height), POS_2D(this->_Width, this->_Height), Value);
		return;
	}

	this->_OriginalMap.SetPathToValue(this->_BottomLeftPos+StartPos, this->_BottomLeftPos+EndPos, Value);
	return;
}

template<class T>
const typename Map2DOverlay<T>::CELL_STORAGE_INDEX Map2DOverlay<T>::GetElementPosInStorage(const POS_2D &Position) const
{
	// If position is too big, let map2D throw error
	if(Position.X >= this->_OverlayWidth || Position.Y >= this->_OverlayHeight)
	{
		return this->_OriginalMap.GetElementPosInStorage(POS_2D(this->_Width, this->_Height));
	}

	return this->_OriginalMap.GetElementPosInStorage(Position);
}

template<class T>
void Map2DOverlay<T>::PrintMap(const char *FileName, const T &MaxVal, const T &MinVal) const
{
	std::fstream file;
	file.open(FileName, std::ios_base::out);

	// Write header with height and width
	file << "P2" << std::endl;
	file << this->GetWidth() << " " << this->GetHeight() << std::endl;
	file << std::to_string(255) << std::endl;		// Max value

	POS_2D curPos;
	for(curPos.Y=this->GetHeight()-1; (std::is_signed<POS_2D_TYPE>() ? curPos.Y>=POS_2D_MIN : curPos.Y<this->GetHeight()); --curPos.Y)
	{
		for(curPos.X=POS_2D_MIN; curPos.X<this->GetWidth(); ++curPos.X)
		{
			file << std::to_string(static_cast<unsigned char>(255*static_cast<float>(this->GetPixel(curPos)-MinVal)/static_cast<float>(MaxVal))) << " ";
		}

		file << std::endl;		// Begin new line after every finished image line
	}

	file.close();		// Close file after finish
}

template<class T>
bool Map2DOverlay<T>::IsInMap(const POS_2D &Position) const
{
	if(Position.X >= this->_OverlayWidth || Position.Y >= this->_OverlayHeight)
		return false;

	return true;
}

#endif // MAP_2D_OVERLAY_H
