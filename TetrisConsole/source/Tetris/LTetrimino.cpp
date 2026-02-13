#include "LTetrimino.h"

using namespace std;


LTetrimino::LTetrimino(vector<vector<int>>& matrix)
	: Tetrimino(matrix, "       ██   ", "   ██████   ")
{
	auto north = vector<Vector2i>();
	north.emplace_back();
	north.emplace_back(0, -1);
	north.emplace_back(0, 1);
	north.emplace_back(-1, 1);

	auto east = vector<Vector2i>();
	east.emplace_back();
	east.emplace_back(-1, 0);
	east.emplace_back(1, 0);
	east.emplace_back(1, 1);

	auto south = vector<Vector2i>();
	south.emplace_back();
	south.emplace_back(0, -1);
	south.emplace_back(0, 1);
	south.emplace_back(1, -1);

	auto west = vector<Vector2i>();
	west.emplace_back();
	west.emplace_back(1, 0);
	west.emplace_back(-1, 0);
	west.emplace_back(-1, -1);
		
	std::array<RotationPoint, 5> rotationNorth = {{	RotationPoint(Vector2i(),	Vector2i()), 
										RotationPoint(Vector2i(0, 1),	Vector2i(0, -1)),
										RotationPoint(Vector2i(-1, 1),	Vector2i(-1, -1)), 
										RotationPoint(Vector2i(2, 0),	Vector2i(2, 0)), 
										RotationPoint(Vector2i(2, 1),	Vector2i(5, -1)) }};

	std::array<RotationPoint, 5> rotationEast = {{	RotationPoint(Vector2i(),	Vector2i()),
										RotationPoint(Vector2i(0, 1),	Vector2i(0, 1)),
										RotationPoint(Vector2i(1, 1),	Vector2i(1, 1)),
										RotationPoint(Vector2i(-2, 0),	Vector2i(-2, 0)),
										RotationPoint(Vector2i(-2, 1),	Vector2i(-2, 1)) }};

	std::array<RotationPoint, 5> rotationSouth = {{	RotationPoint(Vector2i(),	Vector2i()),
										RotationPoint(Vector2i(0, -1),	Vector2i(0, 1)),
										RotationPoint(Vector2i(-1, -1),	Vector2i(-1, 1)),
										RotationPoint(Vector2i(2, 0),	Vector2i(2, 0)),
										RotationPoint(Vector2i(2, -1),	Vector2i(2, 1)) }};

	std::array<RotationPoint, 5> rotationWest = {{	RotationPoint(Vector2i(),	Vector2i()),
										RotationPoint(Vector2i(0, -1),	Vector2i(0, -1)),
										RotationPoint(Vector2i(1, -1),	Vector2i(1, -1)),
										RotationPoint(Vector2i(-2, 0),	Vector2i(-2, 0)),
										RotationPoint(Vector2i(-2, -1),	Vector2i(-2, -1)) }};

	setFacing(NORTH, Facing(NORTH, north, rotationNorth));
	setFacing(EAST, Facing(EAST, east, rotationEast));
	setFacing(SOUTH, Facing(SOUTH, south, rotationSouth));
	setFacing(WEST, Facing(WEST, west, rotationWest));
}


LTetrimino::~LTetrimino()
= default;