#include "stdafx.h"
#include "Facing.h"


Facing::Facing()
{
	_direction = NORTH;
}

Facing::Facing(ROTATION direction, vector<Vector2i> minos, RotationPoint rotationPoints[5])
{
	_direction = direction;
	_minos = minos;

	for (int i = 0; i < 5; i++)
		_rotationPoints[i] = rotationPoints[i];
}

Facing::~Facing()
{
}

ROTATION Facing::getDirection() const
{
	return _direction;
}

int Facing::getMinoCount() const
{
	return _minos.size();
}

Vector2i Facing::getMino(int mino) const
{
	return _minos[mino];
}

RotationPoint const & Facing::getRotationPoint(int point) const
{
	if (point < 0 || point > 4)
		point = 0;
	
	return _rotationPoints[point];
}