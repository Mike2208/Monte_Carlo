#include "quad_map.h"

#include <array>

using namespace QUAD_MAP;

bool QUAD_MAP::ID_RECTANGLE::IsPosInRectangle(const POS_2D &Pos) const
{
	if(Pos.X < this->BottomLeftPos.X || Pos.X >= this->BottomLeftPos.X+this->Width)
		return false;

	if(Pos.Y < this->BottomLeftPos.Y || Pos.Y >= this->BottomLeftPos.Y+this->Height)
		return false;

	return true;
}

bool QUAD_MAP::ID_DATA::EraseID(const ID IDToErase)
{
	for(auto &r_curID : this->AdjacentIDs)
	{
		if(r_curID == IDToErase)
		{
			// Copy last value to this position, then decrement vector size by one
			r_curID = this->AdjacentIDs.back();
			this->AdjacentIDs.pop_back();		// Erase end
			return true;
		}
	}

	return false;
}

struct ID_DATA_EXTRA
{
	ID QuadID = 0;
	ID_DATA *QuadData = nullptr;

	ID_DATA_EXTRA() = default;
	ID_DATA_EXTRA(const ID _QuadID, ID_DATA &_QuadData) : QuadID(_QuadID), QuadData(&_QuadData)
	{}
};

QUAD_MAP::ID_DATA CreateZeroID(const POS_2D_TYPE MapWidth, const POS_2D_TYPE MapHeight)
{
	QUAD_MAP::ID_DATA zeroID;
	zeroID.BottomLeftPos = POS_2D(0,0);
	zeroID.Height = MapHeight;
	zeroID.Width = MapWidth;

	return zeroID;
}

QuadMap::QuadMap(const POS_2D_TYPE MapWidth, const POS_2D_TYPE MapHeight) : _Map(MapWidth, MapHeight, 0), _IDData({CreateZeroID(MapWidth, MapHeight)})
{}

void QuadMap::ResetToOneQuad()
{
	// Reset stored IDData
	this->_IDData = {CreateZeroID(this->_Map.GetWidth(),this->_Map.GetHeight())};

	// Reset map
	this->_Map.ResetMap(this->_Map.GetWidth(),this->_Map.GetHeight(), 0);
}


bool QuadMap::DivideQuad(const ID QuadID)
{
	if(QuadID >= this->_IDData.size())
		return false;

	// Make sure that height and width of old quad is large enough to create new top and right quads
	bool newTopLeftQuad  = true;
	bool newBottomRightQuad = true;
	bool newTopRightQuad = false;

	// Check that quad is large enough to be divided
	if(this->_IDData[QuadID].Height <= 1)
		newTopLeftQuad = false;

	if(this->_IDData[QuadID].Width <= 1)
		newBottomRightQuad = false;

	// If both a top and right quad must be created, a top right quad need to be created as well
	if(newTopLeftQuad & newBottomRightQuad)
		newTopRightQuad = true;
	else if(!(newTopLeftQuad | newBottomRightQuad))
	{
		// If no new quads can be created, abort
		return false;
	}

	// Save old data
	ID_DATA oldData;
	oldData.AdjacentIDs = std::move(this->_IDData[QuadID].AdjacentIDs);
	oldData.BottomLeftPos = this->_IDData[QuadID].BottomLeftPos;
	oldData.Height = this->_IDData[QuadID].Height;
	oldData.Width = this->_IDData[QuadID].Width;

	// Create space for new quads (if they don't exist, they will be deleted later on)
	this->_IDData.resize(this->_IDData.size()+3);

	// Use an array to make access easier and set quad IDs of new ones
	std::array<ID_DATA_EXTRA,4> newIDData;

	// Determine which quads are necessary, and move unnecessary ones to back
	if(newTopRightQuad || newTopLeftQuad)
		newIDData = { { ID_DATA_EXTRA(QuadID, this->_IDData[QuadID]), ID_DATA_EXTRA(this->_IDData.size()-3, *(this->_IDData.rbegin()+2)), ID_DATA_EXTRA(this->_IDData.size()-2, *(this->_IDData.rbegin()+1)), ID_DATA_EXTRA(this->_IDData.size()-1, *(this->_IDData.rbegin())) } };
	else
	{
		// Switch topleft and bottomright quad, as topleft and topright quads will be erased later on
		//	To prevent wrong id assignment, make sure that the correct IDs exist
		newIDData = { { ID_DATA_EXTRA(QuadID, this->_IDData[QuadID]), ID_DATA_EXTRA(this->_IDData.size()-2, *(this->_IDData.rbegin()+1)), ID_DATA_EXTRA(this->_IDData.size()-3, *(this->_IDData.rbegin()+2)), ID_DATA_EXTRA(this->_IDData.size()-1, *(this->_IDData.rbegin())) } };
	}
	// Bottom left quad:
	newIDData[BOTTOM_LEFT_QUAD].QuadData->BottomLeftPos = oldData.BottomLeftPos;
	newIDData[BOTTOM_LEFT_QUAD].QuadData->Width = oldData.Width/2;
	newIDData[BOTTOM_LEFT_QUAD].QuadData->Height = oldData.Height/2;

	// Top left quad:
	newIDData[TOP_LEFT_QUAD].QuadData->BottomLeftPos = POS_2D(oldData.BottomLeftPos.X, oldData.BottomLeftPos.Y + oldData.Height/2);
	newIDData[TOP_LEFT_QUAD].QuadData->Width = oldData.Width/2;
	newIDData[TOP_LEFT_QUAD].QuadData->Height = oldData.Height - oldData.Height/2;

	// Bottom right quad:
	newIDData[BOTTOM_RIGHT_QUAD].QuadData->BottomLeftPos = POS_2D(oldData.BottomLeftPos.X + oldData.Width/2, oldData.BottomLeftPos.Y);
	newIDData[BOTTOM_RIGHT_QUAD].QuadData->Width = oldData.Width - oldData.Width/2;
	newIDData[BOTTOM_RIGHT_QUAD].QuadData->Height = oldData.Height/2;

	// Top right quad:
	newIDData[TOP_RIGHT_QUAD].QuadData->BottomLeftPos = POS_2D(oldData.BottomLeftPos.X + oldData.Width/2, oldData.BottomLeftPos.Y + oldData.Height/2);
	newIDData[TOP_RIGHT_QUAD].QuadData->Width = oldData.Width - oldData.Width/2;
	newIDData[TOP_RIGHT_QUAD].QuadData->Height = oldData.Height - oldData.Height/2;

	// Save the top right of the quad
	//const POS_2D topRightPos = POS_2D(oldData.BottomLeftPos.X+oldData.Width, oldData.BottomLeftPos.Y+oldData.Height);

	// Adjust up neighboring quads
	for(const auto curAdjacentID : oldData.AdjacentIDs)
	{
		auto &curAdjacentQuad = this->_IDData[curAdjacentID];

		// Check which quads are now bordering this one
		if(curAdjacentQuad.BottomLeftPos.X < newIDData[TOP_RIGHT_QUAD].QuadData->BottomLeftPos.X)
		{
			if(curAdjacentQuad.BottomLeftPos.Y < newIDData[TOP_RIGHT_QUAD].QuadData->BottomLeftPos.Y)
			{
				// This quad is connected to BOTTOM_LEFT_QUAD
				// BOTTOM_LEFT_QUAD has old ID, that's still in adjacent Quads list
				newIDData[BOTTOM_LEFT_QUAD].QuadData->AdjacentIDs.push_back(curAdjacentID);

				if(curAdjacentQuad.BottomLeftPos.X + curAdjacentQuad.Width > newIDData[TOP_RIGHT_QUAD].QuadData->BottomLeftPos.X)
				{
					// This quad is connected to BOTTOM_RIGHT_QUAD
					if(newBottomRightQuad)
					{
						curAdjacentQuad.AdjacentIDs.push_back(newIDData[BOTTOM_RIGHT_QUAD].QuadID);
						newIDData[BOTTOM_RIGHT_QUAD].QuadData->AdjacentIDs.push_back(curAdjacentID);
					}
				}
				else if(curAdjacentQuad.BottomLeftPos.Y + curAdjacentQuad.Height > newIDData[TOP_RIGHT_QUAD].QuadData->BottomLeftPos.Y)
				{
					// This quad is connected to TOP_LEFT_QUAD
					if(newTopLeftQuad)
					{
						curAdjacentQuad.AdjacentIDs.push_back(newIDData[TOP_LEFT_QUAD].QuadID);
						newIDData[TOP_LEFT_QUAD].QuadData->AdjacentIDs.push_back(curAdjacentID);
					}
				}
			}
			else
			{
				// Delete BOTTOM_LEFT_QUAD from adjacent quads list
				curAdjacentQuad.EraseID(newIDData[BOTTOM_LEFT_QUAD].QuadID);

				// This quad is connected to TOP_LEFT_QUAD
				if(newTopRightQuad)
				{
					curAdjacentQuad.AdjacentIDs.push_back(newIDData[TOP_LEFT_QUAD].QuadID);
					newIDData[TOP_LEFT_QUAD].QuadData->AdjacentIDs.push_back(curAdjacentID);
				}

				if(curAdjacentQuad.BottomLeftPos.X + curAdjacentQuad.Width > newIDData[TOP_RIGHT_QUAD].QuadData->BottomLeftPos.X)
				{
					// This quad is connected to TOP_RIGHT_QUAD
					if(newTopRightQuad)
					{
						curAdjacentQuad.AdjacentIDs.push_back(newIDData[TOP_RIGHT_QUAD].QuadID);
						newIDData[TOP_RIGHT_QUAD].QuadData->AdjacentIDs.push_back(curAdjacentID);
					}
				}
			}
		}
		else
		{
			// Delete BOTTOM_LEFT_QUAD from adjacent quads list
			curAdjacentQuad.EraseID(newIDData[BOTTOM_LEFT_QUAD].QuadID);

			if(curAdjacentQuad.BottomLeftPos.Y < newIDData[TOP_RIGHT_QUAD].QuadData->BottomLeftPos.Y)
			{
				// This quad is connected to BOTTOM_RIGHT_QUAD
				if(newBottomRightQuad)
				{
					curAdjacentQuad.AdjacentIDs.push_back(newIDData[BOTTOM_RIGHT_QUAD].QuadID);
					newIDData[BOTTOM_RIGHT_QUAD].QuadData->AdjacentIDs.push_back(curAdjacentID);
				}
			}

			if(curAdjacentQuad.BottomLeftPos.Y + curAdjacentQuad.Height > newIDData[TOP_RIGHT_QUAD].QuadData->BottomLeftPos.Y)
			{
				// This quad is connected to TOP_RIGHT_QUAD
				if(newTopLeftQuad)
				{
					curAdjacentQuad.AdjacentIDs.push_back(newIDData[TOP_RIGHT_QUAD].QuadID);
					newIDData[TOP_RIGHT_QUAD].QuadData->AdjacentIDs.push_back(curAdjacentID);
				}
			}

		}
	}

	// Set map areas
	for(const auto &curQuad : newIDData)
	{
		POS_2D curPos;
		for(curPos.X = curQuad.QuadData->BottomLeftPos.X; curPos.X < curQuad.QuadData->BottomLeftPos.X + curQuad.QuadData->Width; ++curPos.X)
		{
			for(curPos.Y = curQuad.QuadData->BottomLeftPos.Y; curPos.Y < curQuad.QuadData->BottomLeftPos.Y + curQuad.QuadData->Height; ++curPos.Y)
			{
				this->_Map.SetPixel(curPos, curQuad.QuadID);
			}
		}
	}

	// Erase those vector elements that aren't needed anymore
	if(!newTopRightQuad)
		this->_IDData.erase(this->_IDData.begin()+newIDData[TOP_RIGHT_QUAD].QuadID);

	if(!newBottomRightQuad)
		this->_IDData.erase(this->_IDData.begin()+newIDData[BOTTOM_RIGHT_QUAD].QuadID);

	if(!newTopLeftQuad)
		this->_IDData.erase(this->_IDData.begin()+newIDData[TOP_LEFT_QUAD].QuadID);

	// Add new adjacent quads
	if(newTopRightQuad)
	{
		// If all quads created
		newIDData[BOTTOM_LEFT_QUAD].QuadData->AdjacentIDs.push_back(newIDData[TOP_LEFT_QUAD].QuadID);
		newIDData[BOTTOM_LEFT_QUAD].QuadData->AdjacentIDs.push_back(newIDData[BOTTOM_RIGHT_QUAD].QuadID);

		newIDData[TOP_LEFT_QUAD].QuadData->AdjacentIDs.push_back(newIDData[BOTTOM_LEFT_QUAD].QuadID);
		newIDData[TOP_LEFT_QUAD].QuadData->AdjacentIDs.push_back(newIDData[TOP_RIGHT_QUAD].QuadID);

		newIDData[BOTTOM_RIGHT_QUAD].QuadData->AdjacentIDs.push_back(newIDData[BOTTOM_LEFT_QUAD].QuadID);
		newIDData[BOTTOM_RIGHT_QUAD].QuadData->AdjacentIDs.push_back(newIDData[TOP_RIGHT_QUAD].QuadID);

		newIDData[TOP_RIGHT_QUAD].QuadData->AdjacentIDs.push_back(newIDData[TOP_LEFT_QUAD].QuadID);
		newIDData[TOP_RIGHT_QUAD].QuadData->AdjacentIDs.push_back(newIDData[BOTTOM_RIGHT_QUAD].QuadID);
	}
	else if(newTopLeftQuad)
	{
		// If only top quad created
		newIDData[BOTTOM_LEFT_QUAD].QuadData->AdjacentIDs.push_back(newIDData[TOP_LEFT_QUAD].QuadID);

		newIDData[TOP_LEFT_QUAD].QuadData->AdjacentIDs.push_back(newIDData[BOTTOM_LEFT_QUAD].QuadID);
	}
	else
	{
		// If only right quad
		newIDData[BOTTOM_LEFT_QUAD].QuadData->AdjacentIDs.push_back(newIDData[BOTTOM_RIGHT_QUAD].QuadID);

		newIDData[BOTTOM_RIGHT_QUAD].QuadData->AdjacentIDs.push_back(newIDData[BOTTOM_LEFT_QUAD].QuadID);
	}

	return true;
}

QuadMap::ID QuadMap::GetIDAtPos(const POS_2D &Position) const
{
	return this->_Map.GetPixel(Position);
}

const Map2D<ID> &QuadMap::GetIDMap() const
{
	return this->_Map;
}

const ID_DATA &QuadMap::GetIDData(const ID QuadID) const
{
	return this->_IDData.at(QuadID);
}

const std::vector<ID> &QuadMap::GetAdjacentIDs(const ID QuadID) const
{
	return this->GetIDData(QuadID).AdjacentIDs;
}
