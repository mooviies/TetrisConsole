#include "OTetrimino.h"

using namespace std;


OTetrimino::OTetrimino(vector<vector<int>>& matrix)
	: Tetrimino(matrix, "    ████    ", "    ████    ")
{
	auto north = vector<Vector2i>();
	north.emplace_back();
	north.emplace_back(-1, 0);
	north.emplace_back(-1, 1);
	north.emplace_back(0, 1);
		
	std::array<RotationPoint, 5> rotationNorth = {{	RotationPoint(Vector2i(), Vector2i()) }};

	setFacing(NORTH, Facing(NORTH, north, rotationNorth));
	setFacing(EAST, Facing(EAST, north, rotationNorth));
	setFacing(SOUTH, Facing(SOUTH, north, rotationNorth));
	setFacing(WEST, Facing(WEST, north, rotationNorth));
}

OTetrimino::~OTetrimino()
= default;