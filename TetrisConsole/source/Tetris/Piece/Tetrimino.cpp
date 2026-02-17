#include "Tetrimino.h"

#include "Constants.h"
#include "Platform.h"

using namespace std;


Tetrimino::Tetrimino(const PieceType type, GameMatrix& matrix)
	: _matrix(matrix)
{
	const PieceData& data = getPieceData(type);
	_color = data.color;
	_startingPosition = data.startingPosition;
	_previewLine1 = data.previewLine1;
	_previewLine2 = data.previewLine2;
	_facings = data.facings;
	_hasTSpin = data.hasTSpin;
	_tSpinPositions = data.tSpinPositions;

	_currentRotation = Rotation::North;
	_lastRotationPoint = -1;
}

Tetrimino::~Tetrimino()
= default;

bool Tetrimino::setPosition(const Vector2i& position)
{
	if (checkPositionValidity(position, _currentRotation))
	{
		_currentPosition = position;
		return true;
	}

	return false;
}

bool Tetrimino::move(const Vector2i& distance)
{
	if (const Vector2i newPosition = _currentPosition + distance; checkPositionValidity(newPosition, _currentRotation))
	{
		_currentPosition = newPosition;
		_lastRotationPoint = -1;
		return true;
	}

	return false;
}

bool Tetrimino::simulateMove(const Vector2i& distance) const {
	const Vector2i newPosition = _currentPosition + distance;
	return checkPositionValidity(newPosition, _currentRotation);
}

bool Tetrimino::rotate(const Direction direction)
{
	Rotation newRotation = _currentRotation;
	if (direction == Direction::Left)
	{
		if (newRotation == Rotation::North)
			newRotation = Rotation::West;
		else
			newRotation = static_cast<Rotation>(static_cast<int>(newRotation) - 1);
	}
	else if (direction == Direction::Right)
	{
		if (newRotation == Rotation::West)
			newRotation = Rotation::North;
		else
			newRotation = static_cast<Rotation>(static_cast<int>(newRotation) + 1);
	}

	Facing const & currentFacing = _facings[static_cast<int>(_currentRotation)];
	for (int i = 0; i < 5; i++)
	{
		RotationPoint const & rotationPoint = currentFacing.getRotationPoint(i);
		if (!rotationPoint.exist())
			continue;
		if (const Vector2i newPosition = _currentPosition + rotationPoint.getTranslation(direction); checkPositionValidity(newPosition, newRotation))
		{
			_currentPosition = newPosition;
			_currentRotation = newRotation;
			_lastRotationPoint = i + 1;
			return true;
		}
	}

	return false;
}

bool Tetrimino::lock()
{
	const Facing& facing = _facings[static_cast<int>(_currentRotation)];
	const int minoCount = facing.getMinoCount();

	bool gameOver = true;
	for (int i = 0; i < minoCount; i++)
	{
		const Vector2i minoPos = _currentPosition + facing.getMino(i);
		_matrix[minoPos.row][minoPos.column] = _color;
		if (minoPos.row >= MATRIX_START)
			gameOver = false;
	}

	_currentRotation = Rotation::North;
	_lastRotationPoint = -1;

	return !gameOver;
}

void Tetrimino::resetRotation()
{
	_currentRotation = Rotation::North;
	_lastRotationPoint = -1;
}

bool Tetrimino::isMino(const int row, const int column) const
{
	const auto position = Vector2i(row, column);
	Facing const & currentFacing = _facings[static_cast<int>(_currentRotation)];
	const int minoCount = currentFacing.getMinoCount();
	const Vector2i relativePos = position - _currentPosition;

	bool result = false;
	for (int i = 0; i < minoCount; i++)
	{
		if (currentFacing.getMino(i) == relativePos)
		{
			result = true;
			break;
		}
	}

	return result;
}

int Tetrimino::getMino(const int row, const int column) const
{
	if (row >= 0 && row < TETRIS_HEIGHT && column >= 0 && column < TETRIS_WIDTH)
		return _matrix[row][column];

	return 1;
}

int Tetrimino::getMino(const Vector2i & position) const
{
	return getMino(position.row, position.column);
}

bool Tetrimino::checkPositionValidity(const Vector2i& position, const Rotation rotation) const {
	const Facing& facing = _facings[static_cast<int>(rotation)];
	const int minoCount = facing.getMinoCount();
	bool valid = true;
	for (int i = 0; i < minoCount; i++)
	{
		if (getMino(position + facing.getMino(i)) > 0)
			valid = false;
	}
	return valid;
}

// T-spin detection uses the 3-corner rule:
// For a T-spin, at least 3 of the 4 diagonal cells adjacent to the T-piece center
// must be occupied. Positions A and B are the two corners "in front" of the T
// (relative to the current facing), and C and D are the two "behind" it.
//
// Full T-spin: A and B both occupied, plus at least one of C or D.
// Mini T-spin: C and D both occupied, plus at least one of A or B,
//              AND a wall kick was used (rotation point >= 2).
// Special case: the 5th SRS kick (TST kick) always promotes a mini to full.
bool Tetrimino::checkTSpin() const {
	if (!_hasTSpin)
		return false;

	if (getLastRotationPoint() < 0)
		return false;

	TSpinPositions const & tspinPos = _tSpinPositions[static_cast<int>(getCurrentRotation())];

	if (const Vector2i position = getPosition(); getMino(position + tspinPos.A) && getMino(position + tspinPos.B) && (getMino(position + tspinPos.C) || getMino(position + tspinPos.D)))
		return true;

	// TST kick (point 5) promotes mini to full
	if (checkMiniTSpin())
		return getLastRotationPoint() == 5;

	return false;
}

bool Tetrimino::checkMiniTSpin() const {
	if (!_hasTSpin)
		return false;

	// Must be a wall kick (point >= 2); basic rotation (point 1) doesn't qualify
	if (getLastRotationPoint() <= 1)
		return false;

	TSpinPositions const & tspinPos = _tSpinPositions[static_cast<int>(getCurrentRotation())];

	if (const Vector2i position = getPosition(); getMino(position + tspinPos.C) && getMino(position + tspinPos.D) && (getMino(position + tspinPos.A) || getMino(position + tspinPos.B)))
		return true;

	return false;
}
