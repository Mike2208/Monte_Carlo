#include "map_2d.h"
#include "occupancy_grid_map.h"

//template<>
void Map2D<bool>::ResizeMap(const POS_2D_TYPE &NewWidth, const POS_2D_TYPE &NewHeight)
{
	this->_Width = NewWidth;
	this->_Height = NewHeight;

	this->_CellData.resize(NewWidth*NewHeight);
}

//template<>
void Map2D<bool>::ResetMap(const POS_2D_TYPE &NewWidth, const POS_2D_TYPE &NewHeight, const bool &DefaultCellValue)
{
	this->ResizeMap(NewWidth, NewHeight);

	for(typename CELL_STORAGE::size_type i = 0; i<NewWidth*NewHeight; ++i)
		this->_CellData.at(i) = DefaultCellValue;
}

//template<>
void Map2D<bool>::SetMapToValue(const bool Value)
{
	for(typename CELL_STORAGE::size_type i = 0; i<this->_CellData.size(); ++i)
		this->_CellData.at(i) = Value;
}

//template<>
void Map2D<bool>::SetPixel(const POS_2D &Position, const bool &Value)
{
	this->_CellData.at(Position.X+this->_Width*Position.Y) = Value;
}

//template<>
//bool &Map2D<bool>::GetPixelR(const POS_2D &Position) = delete;

//template<>
bool Map2D<bool>::GetPixel(const POS_2D &Position) const
{
	return this->_CellData.at(Position.Y*this->_Width+Position.X);
}

//template<>
int Map2D<bool>::GetPixel(const POS_2D &Position, bool &Value) const
{
	if(Position.X >= this->_Width || Position.Y >= this->_Height
			|| Position.X < 0 || Position.Y < 0)
		return -1;

	Value = this->_CellData.at(Position.X + this->_Width*Position.Y);
	return 1;
}

//template<>
void Map2D<bool>::SetPathToValue(const POS_2D &StartPos, const POS_2D &EndPos, const bool &Value)
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

//template<>
POS_2D_TYPE Map2D<bool>::GetHeight() const
{
	return this->_Height;
}

//template<>
POS_2D_TYPE Map2D<bool>::GetWidth() const
{
	return this->_Width;
}

//template<>
const std::vector<bool> &Map2D<bool>::GetCellStorage() const
{
	return this->_CellData;
}

//template<>
std::vector<bool> &Map2D<bool>::GetCellStorageR()
{
	return this->_CellData;
}

//template<>
void Map2D<bool>::PrintMap(const char *FileName) const
{
	std::fstream file;
	file.open(FileName, std::ios_base::out);

	// Write header with height and width
	file << "P1" << std::endl;
	file << this->GetWidth() << " " << this->GetHeight() << std::endl;

	for(POS_2D_TYPE Y=this->GetHeight()-1; (std::is_signed<POS_2D_TYPE>() ? Y>=POS_2D_MIN : Y<this->GetHeight()); --Y)
	{
		for(POS_2D_TYPE X=POS_2D_MIN; X<this->GetWidth(); ++X)
		{
			file << std::to_string(static_cast<char>(!(this->GetPixel(POS_2D(X,Y))))) << " ";
		}

		file << std::endl;		// Begin new line after every finished image line
	}

	file.close();		// Close file after finish
}

//template<>
bool Map2D<bool>::IsInMap(const POS_2D &Position) const
{
	if(Position.X >= this->_Width || Position.Y >= this->_Height
			|| Position.X < 0 || Position.Y < 0)
		return false;

	return true;
}

template class Map2D<OGM_CELL_TYPE>;
template class Map2D<OGM_LOG_TYPE>;
