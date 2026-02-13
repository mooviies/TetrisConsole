#include "Facing.h"


Facing::Facing()
{
	_direction = NORTH;
}

Facing::Facing(const ROTATION direction, const vector<Vector2i> &minos, RotationPoint rotationPoints[5])
{
	_direction = direction;
	_minos = minos;

	for (int i = 0; i < 5; i++)
		_rotationPoints[i] = rotationPoints[i];
}

Facing::~Facing()
= default;

ROTATION Facing::getDirection() const
{
	return _direction;
}

int Facing::getMinoCount() const
{
	return static_cast<int>(_minos.size());
}

Vector2i Facing::getMino(const int mino) const
{
	return _minos[mino];
}

RotationPoint const & Facing::getRotationPoint(int point) const
{
	if (point < 0 || point > 4)
		point = 0;
	
	return _rotationPoints[point];
}