#include "TTetrimino.h"


TTetrimino::TTetrimino(vector<vector<int>>& matrix)
	: Tetrimino(matrix, "     ██     ", "   ██████   ")
{
	auto north = vector<Vector2i>();
	north.emplace_back();
	north.emplace_back(0, -1);
	north.emplace_back(0, 1);
	north.emplace_back(-1, 0);

	auto east = vector<Vector2i>();
	east.emplace_back();
	east.emplace_back(0, 1);
	east.emplace_back(-1, 0);
	east.emplace_back(1, 0);

	auto south = vector<Vector2i>();
	south.emplace_back();
	south.emplace_back(0, -1);
	south.emplace_back(0, 1);
	south.emplace_back(1, 0);

	auto west = vector<Vector2i>();
	west.emplace_back();
	west.emplace_back(0, -1);
	west.emplace_back(-1, 0);
	west.emplace_back(1, 0);
		
	RotationPoint rotationNorth[5] = {	RotationPoint(Vector2i(),		Vector2i()), 
										RotationPoint(Vector2i(0, 1),	Vector2i(0, -1)),
										RotationPoint(Vector2i(-1, 1),	Vector2i(-1, -1)), 
										RotationPoint(), 
										RotationPoint(Vector2i(2, 1),		Vector2i(2, -1)) };

	RotationPoint rotationEast[5] = {	RotationPoint(Vector2i(),		Vector2i()),
										RotationPoint(Vector2i(0, 1),	Vector2i(0, 1)),
										RotationPoint(Vector2i(1, 1),	Vector2i(1, 1)),
										RotationPoint(Vector2i(-2, 0),	Vector2i(-2, 0)),
										RotationPoint(Vector2i(-2, 1),		Vector2i(-2, 1)) };

	RotationPoint rotationSouth[5] = {	RotationPoint(Vector2i(),		Vector2i()),
										RotationPoint(Vector2i(0, -1),	Vector2i(0, 1)),
										RotationPoint(),
										RotationPoint(Vector2i(2, 0),	Vector2i(2, 0)),
										RotationPoint(Vector2i(2, -1),		Vector2i(2, 1)) };

	RotationPoint rotationWest[5] = {	RotationPoint(Vector2i(),		Vector2i()),
										RotationPoint(Vector2i(0, -1),	Vector2i(0, -1)),
										RotationPoint(Vector2i(1, -1),	Vector2i(1, -1)),
										RotationPoint(Vector2i(-2, 0),	Vector2i(-2, 0)),
										RotationPoint(Vector2i(-2, -1),		Vector2i(-2, -1)) };


	_tSpinPositions[NORTH] = TSpinPositions(Vector2i(-1, -1), Vector2i(-1, 1), Vector2i(1, -1), Vector2i(1, 1));
	_tSpinPositions[EAST] = TSpinPositions(Vector2i(-1, 1), Vector2i(1, 1), Vector2i(-1, -1), Vector2i(1, -1));
	_tSpinPositions[SOUTH] = TSpinPositions(Vector2i(1, 1), Vector2i(1, -1), Vector2i(-1, -1), Vector2i(-1, 1));
	_tSpinPositions[WEST] = TSpinPositions(Vector2i(1, -1), Vector2i(-1, -1), Vector2i(1, 1), Vector2i(-1, 1));

	setFacing(NORTH, Facing(NORTH, north, rotationNorth));
	setFacing(EAST, Facing(EAST, east, rotationEast));
	setFacing(SOUTH, Facing(SOUTH, south, rotationSouth));
	setFacing(WEST, Facing(WEST, west, rotationWest));

	_didTSpinWith5 = false;
}


TTetrimino::~TTetrimino()
= default;

bool TTetrimino::checkTSpin()
{
	if (getLastRotationPoint() < 0)
		return false;

	TSpinPositions const & tspinPos = _tSpinPositions[getCurrentRotation()];

	if (const Vector2i position = getPosition(); getMino(position + tspinPos.A) && getMino(position + tspinPos.B) && (getMino(position + tspinPos.C) || getMino(position + tspinPos.D)))
	{
		if (getLastRotationPoint() == 5)
			_didTSpinWith5 = true;

		return true;
	}

	if (checkMiniTSpin())
	{
		if (_didTSpinWith5)
			return true;

		return getLastRotationPoint() == 5;
	}

	return false;
}

bool TTetrimino::checkMiniTSpin()
{
	if (getLastRotationPoint() < 0)
		return false;

	TSpinPositions const & tspinPos = _tSpinPositions[getCurrentRotation()];

	if (const Vector2i position = getPosition(); getMino(position + tspinPos.C) && getMino(position + tspinPos.D) && (getMino(position + tspinPos.A) || getMino(position + tspinPos.B)))
	{
		return true;
	}

	return false;
}

void TTetrimino::onLock()
{
	_didTSpinWith5 = false;
}