#include "ITetrimino.h"


ITetrimino::ITetrimino(vector<vector<int>>& matrix)
	: Tetrimino(matrix, "  ��������  ", "  ��������  ")
{
	vector<Vector2i> north = vector<Vector2i>();
	north.push_back(Vector2i());
	north.push_back(Vector2i(0, -1));
	north.push_back(Vector2i(0, 1));
	north.push_back(Vector2i(0, 2));

	vector<Vector2i> east = vector<Vector2i>();
	east.push_back(Vector2i(0, 1));
	east.push_back(Vector2i(-1, 1));
	east.push_back(Vector2i(1, 1));
	east.push_back(Vector2i(2, 1));

	vector<Vector2i> south = vector<Vector2i>();
	south.push_back(Vector2i(1, 0));
	south.push_back(Vector2i(1, -1));
	south.push_back(Vector2i(1, 1));
	south.push_back(Vector2i(1, 2));

	vector<Vector2i> west = vector<Vector2i>();
	west.push_back(Vector2i(-1, 0));
	west.push_back(Vector2i(0, 0));
	west.push_back(Vector2i(1, 0));
	west.push_back(Vector2i(2, 0));
		
	RotationPoint rotationNorth[5] = {	RotationPoint(Vector2i(),	Vector2i()), 
										RotationPoint(Vector2i(0, -1),	Vector2i(0, -2)),
										RotationPoint(Vector2i(0, 2),	Vector2i(0, 1)), 
										RotationPoint(Vector2i(-2, -1),	Vector2i(1, -2)), 
										RotationPoint(Vector2i(1, 2),	Vector2i(-2, 1)) };

	RotationPoint rotationEast[5] = {	RotationPoint(Vector2i(),	Vector2i()),
										RotationPoint(Vector2i(0, 2),	Vector2i(0, -1)),
										RotationPoint(Vector2i(0, -1),	Vector2i(0, 2)),
										RotationPoint(Vector2i(-1, 2),	Vector2i(-2, -1)),
										RotationPoint(Vector2i(2, -1),	Vector2i(1, 2)) };

	RotationPoint rotationSouth[5] = {	RotationPoint(Vector2i(),	Vector2i()),
										RotationPoint(Vector2i(0, 1),	Vector2i(0, 2)),
										RotationPoint(Vector2i(0, -2),	Vector2i(0, -1)),
										RotationPoint(Vector2i(2, 1),	Vector2i(-1, 2)),
										RotationPoint(Vector2i(-1, -2),	Vector2i(2, -1)) };

	RotationPoint rotationWest[5] = {	RotationPoint(Vector2i(),	Vector2i()),
										RotationPoint(Vector2i(0, -2),	Vector2i(0, 1)),
										RotationPoint(Vector2i(0, 1),	Vector2i(0, -2)),
										RotationPoint(Vector2i(1, -2),	Vector2i(2, 1)),
										RotationPoint(Vector2i(-2, 1),	Vector2i(-1, -2)) };

	setFacing(NORTH, Facing(NORTH, north, rotationNorth));
	setFacing(EAST, Facing(EAST, east, rotationEast));
	setFacing(SOUTH, Facing(SOUTH, south, rotationSouth));
	setFacing(WEST, Facing(WEST, west, rotationWest));
}


ITetrimino::~ITetrimino()
{
}