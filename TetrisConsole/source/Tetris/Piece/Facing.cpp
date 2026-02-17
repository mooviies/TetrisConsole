#include "Facing.h"

using namespace std;


Facing::Facing()
{
	_direction = Rotation::North;
}

Facing::Facing(const Rotation direction, const vector<Vector2i> &minos, const std::array<RotationPoint, 5>& rotationPoints)
	: _minos(minos), _direction(direction), _rotationPoints(rotationPoints)
{
}

Facing::~Facing()
= default;

Rotation Facing::getDirection() const
{
	return _direction;
}

int Facing::getMinoCount() const
{
	return static_cast<int>(_minos.size());
}

Vector2i Facing::getMino(const int mino) const
{
	return _minos[static_cast<size_t>(mino)];
}

RotationPoint const & Facing::getRotationPoint(int point) const
{
	if (point < 0 || point > 4)
		point = 0;

	return _rotationPoints[static_cast<size_t>(point)];
}
