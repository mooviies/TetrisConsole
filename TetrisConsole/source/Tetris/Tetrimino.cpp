#include "Tetrimino.h"

#include "Constants.h"
#include "rlutil.h"


Tetrimino::Tetrimino(vector<vector<int>>& matrix, const string &previewLine1, const string &previewLine2)
	: _matrix(matrix)
{
	_currentRotation = NORTH;
	_previewLine1 = previewLine1;
	_previewLine2 = previewLine2;
	_lastRotationPoint = -1;
}

Tetrimino::~Tetrimino()
= default;

void Tetrimino::setFacing(const ROTATION direction, const Facing& facing)
{
	_facings[direction] = facing;
}

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

bool Tetrimino::simulateMove(const Vector2i& distance)
{
	const Vector2i newPosition = _currentPosition + distance;
	return checkPositionValidity(newPosition, _currentRotation);
}

bool Tetrimino::rotate(const DIRECTION direction)
{
	ROTATION newRotation = _currentRotation;
	if (direction == LEFT)
	{
		if (newRotation == NORTH)
			newRotation = WEST;
		else
			newRotation = static_cast<ROTATION>(newRotation - 1);
	}
	else if (direction == RIGHT)
	{
		if (newRotation == WEST)
			newRotation = NORTH;
		else
			newRotation = static_cast<ROTATION>(newRotation + 1);
	}

	Facing const & currentFacing = _facings[_currentRotation];
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
	const Facing& facing = _facings[_currentRotation];
	const int minoCount = facing.getMinoCount();

	bool gameOver = true;
	for (int i = 0; i < minoCount; i++)
	{
		const Vector2i minoPos = _currentPosition + facing.getMino(i);
		_matrix[minoPos.row][minoPos.column] = getColor();
		if (minoPos.row >= MATRIX_START)
			gameOver = false;
	}

	_currentRotation = NORTH;
	_lastRotationPoint = -1;

	onLock();
	return !gameOver;
}

void Tetrimino::resetRotation()
{
	_currentRotation = NORTH;
	_lastRotationPoint = -1;
}

bool Tetrimino::isMino(const int row, const int column) const
{
	const auto position = Vector2i(row, column);
	Facing const & currentFacing = _facings[_currentRotation];
	const int nbMino = currentFacing.getMinoCount();
	const Vector2i relativePos = position - _currentPosition;

	bool result = false;
	for (int i = 0; i < nbMino; i++)
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

bool Tetrimino::checkPositionValidity(const Vector2i& position, const ROTATION rotation) const {
	const Facing& facing = _facings[rotation];
	const int minoCount = facing.getMinoCount();
	bool valid = true;
	for (int i = 0; i < minoCount; i++)
	{
		if (getMino(position + facing.getMino(i)) > 0)
			valid = false;
	}
	return valid;
}

void Tetrimino::printPreview(const int line, const bool hold) const {
	int y = line;
	if (hold)
		y += 24;
	else
		y += 10;

	rlutil::locate(60, y);
	rlutil::setColor(getColor());

	if (line == 0)
		cout << _previewLine1;
	else if (line == 1)
		cout << _previewLine2;
	else
		cout << "            ";
	
	rlutil::setColor(rlutil::WHITE);
}
