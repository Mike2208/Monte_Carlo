#include "quad_d_star_maps.h"
#include "map_2d_overlay.h"

QuadDStarMaps::QuadDStarMaps(const POS_2D_TYPE MapWidth, const POS_2D_TYPE MapHeight) : _QuadMap(MapWidth, MapHeight)
{
}

QuadDStarMaps::QuadDStarMaps(const OGM_LOG_MAP_TYPE &OriginalMap, const POS_2D &Start, const POS_2D &Destination) : _QuadMap(OriginalMap.GetWidth(), OriginalMap.GetHeight()), _DStarMaps(OriginalMap, Start, Destination)
{}

QuadDStarMaps::ID_RECTANGLE QuadDStarMaps::GetAdjacentRectangle(const ID CurQuadID, const ID AdjacentQuadID)
{
	ID_RECTANGLE connectRectangle;

	// Faster access to rectangles
	const auto &curQuadRectangle = this->_QuadMap.GetIDData(CurQuadID);
	const auto &adjacentQuadRectangle = this->_QuadMap.GetIDData(AdjacentQuadID);

	// Check which edge is connected
	if(curQuadRectangle.BottomLeftPos.X == adjacentQuadRectangle.BottomLeftPos.X + adjacentQuadRectangle.Width)
	{
		// Connected at left edge of curQuadRectangle.X
		connectRectangle.BottomLeftPos.X = curQuadRectangle.BottomLeftPos.X-1;
		connectRectangle.Width = 1;

		// Check bottom of connection
		if(curQuadRectangle.BottomLeftPos.Y >= adjacentQuadRectangle.BottomLeftPos.Y)
		{
			// Bottom at curQuadRectangle.BottomLeftPos.Y
			connectRectangle.BottomLeftPos.Y = curQuadRectangle.BottomLeftPos.Y;
		}
		else
		{
			// Bottom at adjacentQuadRectangle.BottomLeftPos.Y
			connectRectangle.BottomLeftPos.Y = adjacentQuadRectangle.BottomLeftPos.Y;
		}

		// Check top of connection
		if(curQuadRectangle.BottomLeftPos.Y + curQuadRectangle.Height < adjacentQuadRectangle.BottomLeftPos.Y + adjacentQuadRectangle.Height)
		{
			// Top at top of curQuad
			connectRectangle.Height = curQuadRectangle.BottomLeftPos.Y + curQuadRectangle.Height - connectRectangle.BottomLeftPos.Y;
		}
		else
		{
			// Top at top of adjacentQuad
			connectRectangle.Height = adjacentQuadRectangle.BottomLeftPos.Y + adjacentQuadRectangle.Height - connectRectangle.BottomLeftPos.Y;
		}
	}
	else if(adjacentQuadRectangle.BottomLeftPos.X == curQuadRectangle.BottomLeftPos.X + curQuadRectangle.Width)
	{
		// Connected at right edge of curQuadRectangle.X
		connectRectangle.BottomLeftPos.X = adjacentQuadRectangle.BottomLeftPos.X;
		connectRectangle.Width = 1;

		// Check bottom of connection
		if(curQuadRectangle.BottomLeftPos.Y >= adjacentQuadRectangle.BottomLeftPos.Y)
		{
			// Bottom at curQuadRectangle.BottomLeftPos.Y
			connectRectangle.BottomLeftPos.Y = curQuadRectangle.BottomLeftPos.Y;
		}
		else
		{
			// Bottom at adjacentQuadRectangle.BottomLeftPos.Y
			connectRectangle.BottomLeftPos.Y = adjacentQuadRectangle.BottomLeftPos.Y;
		}

		// Check top of connection
		if(curQuadRectangle.BottomLeftPos.Y + curQuadRectangle.Height < adjacentQuadRectangle.BottomLeftPos.Y + adjacentQuadRectangle.Height)
		{
			// Top at top of curQuad
			connectRectangle.Height = curQuadRectangle.BottomLeftPos.Y + curQuadRectangle.Height - connectRectangle.BottomLeftPos.Y;
		}
		else
		{
			// Top at top of adjacentQuad
			connectRectangle.Height = adjacentQuadRectangle.BottomLeftPos.Y + adjacentQuadRectangle.Height - connectRectangle.BottomLeftPos.Y;
		}
	}
	else if(curQuadRectangle.BottomLeftPos.Y == adjacentQuadRectangle.BottomLeftPos.Y + adjacentQuadRectangle.Height)
	{
		// Connected at bottom of curQuad
		connectRectangle.BottomLeftPos.Y = curQuadRectangle.BottomLeftPos.Y-1;
		connectRectangle.Height = 1;

		// Check left of connection
		if(curQuadRectangle.BottomLeftPos.X >= adjacentQuadRectangle.BottomLeftPos.X)
		{
			// Bottom at curQuadRectangle.BottomLeftPos
			connectRectangle.BottomLeftPos.X = curQuadRectangle.BottomLeftPos.X;
		}
		else
		{
			// Bottom at adjacentQuadRectangle.BottomLeftPos
			connectRectangle.BottomLeftPos.X = adjacentQuadRectangle.BottomLeftPos.X;
		}

		// Check left of connection
		if(curQuadRectangle.BottomLeftPos.X + curQuadRectangle.Width < adjacentQuadRectangle.BottomLeftPos.X + adjacentQuadRectangle.Width)
		{
			// Top at top of curQuad
			connectRectangle.Width = curQuadRectangle.BottomLeftPos.X + curQuadRectangle.Width - connectRectangle.BottomLeftPos.X;
		}
		else
		{
			// Top at top of adjacentQuad
			connectRectangle.Width = adjacentQuadRectangle.BottomLeftPos.X + adjacentQuadRectangle.Width - connectRectangle.BottomLeftPos.X;
		}
	}
	else
	{
		// Connected at top of curQuad
		connectRectangle.BottomLeftPos.Y = adjacentQuadRectangle.BottomLeftPos.Y;
		connectRectangle.Height = 1;

		// Check left of connection
		if(curQuadRectangle.BottomLeftPos.X >= adjacentQuadRectangle.BottomLeftPos.X)
		{
			// Bottom at curQuadRectangle.BottomLeftPos
			connectRectangle.BottomLeftPos.X = curQuadRectangle.BottomLeftPos.X;
		}
		else
		{
			// Bottom at adjacentQuadRectangle.BottomLeftPos
			connectRectangle.BottomLeftPos.X = adjacentQuadRectangle.BottomLeftPos.X;
		}

		// Check left of connection
		if(curQuadRectangle.BottomLeftPos.X + curQuadRectangle.Width < adjacentQuadRectangle.BottomLeftPos.X + adjacentQuadRectangle.Width)
		{
			// Top at top of curQuad
			connectRectangle.Width = curQuadRectangle.BottomLeftPos.X + curQuadRectangle.Width - connectRectangle.BottomLeftPos.X;
		}
		else
		{
			// Top at top of adjacentQuad
			connectRectangle.Width = adjacentQuadRectangle.BottomLeftPos.X + adjacentQuadRectangle.Width - connectRectangle.BottomLeftPos.X;
		}
	}

	return connectRectangle;
}

QuadDStarMaps::ID QuadDStarMaps::GetQuadIDAtPos(const POS_2D &Position)
{
	return this->_QuadMap.GetIDAtPos(Position);
}

const std::vector<QuadDStarMaps::ID> &QuadDStarMaps::GetAdjacentQuadIDs(const ID QuadID)
{
	return this->_QuadMap.GetAdjacentIDs(QuadID);
}

void QuadDStarMaps::ResetDestPos(const OGM_LOG_MAP_TYPE &OriginalMap, const POS_2D &Destination)
{
	this->_DStarMaps.ResetMapsWithNewDestPos(OriginalMap, Destination);

	this->_DestMapsSynced = true;
}

void QuadDStarMaps::ResetToOneQuad(const OGM_LOG_MAP_TYPE &OriginalMap, bool UpdateDStarDestMaps, bool UpdateDStarStartMaps)
{
	this->_QuadMap.ResetToOneQuad();

	// Check if dest maps should be synced
	if(UpdateDStarDestMaps)
	{
		this->_DStarMaps.ResetMapsWithNewStartPos(OriginalMap, this->_BotPosition);

		this->_DestMapsSynced = true;
	}
	else
		this->_DestMapsSynced = false;

	// Check if start maps should be synced
	if(UpdateDStarStartMaps)
	{
		this->_DStarMaps.ResetMapsWithNewDestPos(OriginalMap, this->_DStarMaps.GetDestPos());

		//this->UpdateMoveMaps();

		this->UpdateBotQuadPos();

		this->_StartMapsSynced = true;
	}
	else
		this->_StartMapsSynced = false;
}

bool QuadDStarMaps::DivideQuad(const OGM_LOG_MAP_TYPE &OriginalMap, const std::vector<POS_2D> &UpdatedPositions, const ID QuadID, bool UpdateDStarDestMaps, bool UpdateDStarStartMaps)
{
	// Check if bot ID is changed by this division
	bool botQuadChanged = true;
	if(UpdateDStarStartMaps)
		botQuadChanged = (this->GetBotQuadID() == QuadID);

	// Divide quad
	const bool retVal = this->_QuadMap.DivideQuad(QuadID);

	// Update dest maps if requested
	if(UpdateDStarDestMaps)
	{
		// Update dest map
		if(this->_DestMapsSynced)
			this->_DStarMaps.UpdateDestMaps(OriginalMap, UpdatedPositions);		// If previously synced, just update positions
		else
			this->_DStarMaps.ResetMapsWithNewDestPos(OriginalMap, this->_DStarMaps.GetDestPos());		// If previously not synced, recalculate map

		this->_DestMapsSynced = true;
	}
	else
		this->_DestMapsSynced = false;	// Maps are no longer synced

	// Update start maps if requested
	if(UpdateDStarStartMaps)
	{
		// Get Rectangle and edge of current quad
		ID_RECTANGLE curIDRectangle = this->GetQuadRectangleWithEdges();

		// Update relevant start quad map
		if(!this->_StartMapsSynced || botQuadChanged)
		{
			// If map wasn't synced beforehand or quad was changed, recalculate completely

			// Recalculate DStarStartMaps completely (Overlay is used to only take area in quad into account)
			this->_DStarMaps.ResetMapsWithNewStartPos(Map2DOverlay<OGM_LOG_MAP_TYPE::CELL_TYPE>( OriginalMap, curIDRectangle.BottomLeftPos, curIDRectangle.Width, curIDRectangle.Height), this->_BotPosition);

			// Update move maps
			//this->UpdateMoveMaps();
		}
		else
		{
			// Determine which positions are in the quad
			std::vector<POS_2D> validPos;
			for(const auto &curPos : UpdatedPositions)
			{
				// Calculate relative coordinates for positions in quad
				const POS_2D tmpPos = curPos - curIDRectangle.BottomLeftPos;
				if(tmpPos.X < curIDRectangle.Width && tmpPos.Y < curIDRectangle.Height)		// Add to vector if in quad
					validPos.push_back(tmpPos);
			}

			// Update quad with new positions (Overlay is used to only take area in quad into account)
			this->_DStarMaps.UpdateStartMaps(Map2DOverlay<OGM_LOG_MAP_TYPE::CELL_TYPE>( OriginalMap, curIDRectangle.BottomLeftPos, curIDRectangle.Width, curIDRectangle.Height), validPos);
		}

		if(botQuadChanged || !this->_StartMapsSynced)
			this->UpdateBotQuadPos();

		// Set start maps to synced
		this->_StartMapsSynced = true;
	}
	else
		this->_StartMapsSynced = false;		// Maps are no longer synced
}

const MonteCarloDStarMaps &QuadDStarMaps::UpdateDStarMaps(const OGM_LOG_MAP_TYPE &OriginalMap, const std::vector<POS_2D> &UpdatedPositions, const POS_2D &BotPosition, bool UpdateDStarDestMaps, bool UpdateDStarStartMaps)
{
	// Update dest map if already synced
	if(UpdateDStarDestMaps)
	{
		if(this->_DestMapsSynced)
			this->_DStarMaps.UpdateDestMaps(OriginalMap, UpdatedPositions);
		else
			this->_DStarMaps.ResetMapsWithNewDestPos(OriginalMap, this->_DStarMaps.GetDestPos());

		this->_DestMapsSynced = true;
	}
	else
		this->_DestMapsSynced = false;

	// Update relevant start quad map if already synced and bot position doesn't change
	if(UpdateDStarStartMaps)
	{
		if(this->_StartMapsSynced && this->_BotPosition == BotPosition)
		{
			// Changing bot position must be done here to allow check above
			this->_BotPosition = BotPosition;
			ID_RECTANGLE curIDRectangle = this->GetQuadRectangleWithEdges();

			this->_DStarMaps.UpdateStartMaps(Map2DOverlay<OGM_LOG_MAP_TYPE::CELL_TYPE>( OriginalMap, curIDRectangle.BottomLeftPos, curIDRectangle.Width, curIDRectangle.Height), UpdatedPositions);
		}
		else
		{
			// Changing bot position must be done here to allow check above
			this->_BotPosition = BotPosition;
			ID_RECTANGLE curIDRectangle = this->GetQuadRectangleWithEdges();

			// Calculate DStarMap in this rectangle
			this->_DStarMaps.ResetMapsWithNewStartPos(Map2DOverlay<OGM_LOG_MAP_TYPE::CELL_TYPE>( OriginalMap, curIDRectangle.BottomLeftPos, curIDRectangle.Width, curIDRectangle.Height), this->_BotPosition);

			this->UpdateBotQuadPos();

			// Update move maps
			//this->UpdateMoveMaps();
		}

		// Set maps to synced
		this->_StartMapsSynced = true;
	}
	else
		this->_StartMapsSynced = false;

	return this->_DStarMaps;
}

void QuadDStarMaps::SyncMaps(const OGM_LOG_MAP_TYPE &OriginalMap)
{
	if(!this->_DestMapsSynced)
	{
		this->_DStarMaps.ResetMapsWithNewDestPos(OriginalMap, this->_DStarMaps.GetDestPos());

		this->_DestMapsSynced = true;
	}

	if(!this->_StartMapsSynced)
	{
		// Update quad map data
		ID_RECTANGLE curIDRectangle = this->GetQuadRectangleWithEdges();
		this->_DStarMaps.ResetMapsWithNewStartPos(Map2DOverlay<OGM_LOG_MAP_TYPE::CELL_TYPE>(OriginalMap, curIDRectangle.BottomLeftPos, curIDRectangle.Width, curIDRectangle.Height), this->_BotPosition);

		this->_StartMapsSynced = true;
	}
}

void QuadDStarMaps::UpdateBotQuadPos()
{
	this->_BotQuadPos = this->_QuadMap.GetIDData(this->_QuadMap.GetIDAtPos(this->_BotPosition)).BottomLeftPos;

	// Adjust to add borders
	if(this->_BotQuadPos.X > 0)
		this->_BotQuadPos.X--;

	if(this->_BotQuadPos.Y > 0)
		this->_BotPosition.Y--;
}

QuadDStarMaps::ID_RECTANGLE QuadDStarMaps::GetQuadRectangleWithEdges()
{
	ID_RECTANGLE curIDRectangle = this->_QuadMap.GetIDData(this->_QuadMap.GetIDAtPos(this->_BotPosition));

	// Grow rectangle by one if possible
	if(curIDRectangle.BottomLeftPos.X > 0)
		curIDRectangle.BottomLeftPos.X -= 1;
	if(curIDRectangle.BottomLeftPos.Y > 0)
		curIDRectangle.BottomLeftPos.Y -= 1;

	if(curIDRectangle.BottomLeftPos.X + curIDRectangle.Width < this->_QuadMap.GetIDMap().GetWidth() )
			curIDRectangle.Width += 1;
	if(curIDRectangle.BottomLeftPos.Y + curIDRectangle.Height < this->_QuadMap.GetIDMap().GetHeight() )
			curIDRectangle.Height += 1;

	return curIDRectangle;
}

//void QuadDStarMaps::UpdateMoveMaps()
//{
//	// Get move position
//	const POS_2D &movePos = this->_QuadMap.GetIDData(this->GetBotQuadID()).BottomLeftPos;

//	// Update all D* movements
//	this->_DStarDistMap = Map2DMove<MOVE_DIST_TYPE>(this->_DStarMaps._DistToStart, movePos);
//	this->_DStarProbMap = Map2DMove<OGM_LOG_TYPE>(this->_DStarMaps._ProbToStart, movePos);
//	this->_DStarRatioMap = Map2DMove<OGM_LOG_TYPE>(this->_DStarMaps._RatioToStart, movePos);
//	this->_DStarRatioDistMap = Map2DMove<MOVE_DIST_TYPE>(this->_DStarMaps._RatioDistToStart, movePos);
//	this->_DStarRatioProbMap = Map2DMove<OGM_LOG_TYPE>(this->_DStarMaps._RatioProbToStart, movePos);
//}
