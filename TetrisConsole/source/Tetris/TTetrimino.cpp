#include "TTetrimino.h"

using namespace std;


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

	std::array<RotationPoint, 5> rotationNorth = {{
		RotationPoint(Vector2i(),      Vector2i()),
		RotationPoint(Vector2i(0, 1),  Vector2i(0, -1)),
		RotationPoint(Vector2i(-1, 1), Vector2i(-1, -1)),
		RotationPoint(),
		RotationPoint(Vector2i(2, 1),  Vector2i(2, -1))
	}};

	std::array<RotationPoint, 5> rotationEast = {{
		RotationPoint(Vector2i(),      Vector2i()),
		RotationPoint(Vector2i(0, 1),  Vector2i(0, 1)),
		RotationPoint(Vector2i(1, 1),  Vector2i(1, 1)),
		RotationPoint(Vector2i(-2, 0), Vector2i(-2, 0)),
		RotationPoint(Vector2i(-2, 1), Vector2i(-2, 1))
	}};

	std::array<RotationPoint, 5> rotationSouth = {{
		RotationPoint(Vector2i(),       Vector2i()),
		RotationPoint(Vector2i(0, -1),  Vector2i(0, 1)),
		RotationPoint(),
		RotationPoint(Vector2i(2, 0),   Vector2i(2, 0)),
		RotationPoint(Vector2i(2, -1),  Vector2i(2, 1))
	}};

	std::array<RotationPoint, 5> rotationWest = {{
		RotationPoint(Vector2i(),       Vector2i()),
		RotationPoint(Vector2i(0, -1),  Vector2i(0, -1)),
		RotationPoint(Vector2i(1, -1),  Vector2i(1, -1)),
		RotationPoint(Vector2i(-2, 0),  Vector2i(-2, 0)),
		RotationPoint(Vector2i(-2, -1), Vector2i(-2, -1))
	}};

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

// T-spin detection uses the 3-corner rule:
// For a T-spin, at least 3 of the 4 diagonal cells adjacent to the T-piece center
// must be occupied. Positions A and B are the two corners "in front" of the T
// (relative to the current facing), and C and D are the two "behind" it.
//
// Full T-spin: A and B both occupied, plus at least one of C or D.
// The special case: if the 5th SRS kick was used, a mini T-spin is promoted to full.
//
// Mini T-spin: C and D both occupied, plus at least one of A or B.
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
