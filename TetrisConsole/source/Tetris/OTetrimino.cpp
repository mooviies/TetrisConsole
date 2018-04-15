#include "OTetrimino.h"


OTetrimino::OTetrimino(vector<vector<int>>& matrix)
	: Tetrimino(matrix, "    лллл    ", "    лллл    ")
{
	vector<Vector2i> north = vector<Vector2i>();
	north.push_back(Vector2i());
	north.push_back(Vector2i(-1, 0));
	north.push_back(Vector2i(-1, 1));
	north.push_back(Vector2i(0, 1));
		
	RotationPoint rotationNorth[5] = {	RotationPoint(Vector2i(), Vector2i()) };

	setFacing(NORTH, Facing(NORTH, north, rotationNorth));
	setFacing(EAST, Facing(EAST, north, rotationNorth));
	setFacing(SOUTH, Facing(SOUTH, north, rotationNorth));
	setFacing(WEST, Facing(WEST, north, rotationNorth));
}

OTetrimino::~OTetrimino()
{
}