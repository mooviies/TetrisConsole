#pragma once

#include <vector>
#include "Vector2i.h"

enum ROTATION {
	NORTH = 0,
	EAST,
	SOUTH,
	WEST
};

enum DIRECTION {
	LEFT = 0,
	RIGHT
};

struct RotationPoint
{
	RotationPoint()
	{
		_exist = false;
	}

	RotationPoint(const Vector2i& translationLeft, const Vector2i& translationRight)
	{
		_translations[LEFT] = translationLeft;
		_translations[RIGHT] = translationRight;
		_exist = true;
	}

	[[nodiscard]] bool exist() const
	{
		return _exist;
	}

	[[nodiscard]] Vector2i const & getTranslation(DIRECTION direction) const
	{
		return _translations[direction];
	}

private:
	Vector2i _translations[2];
	bool _exist;
};

using namespace std;

class Facing
{
public:
	Facing();
	Facing(ROTATION direction, const vector<Vector2i> &minos, RotationPoint rotationPoints[5]);
	~Facing();

	[[nodiscard]] ROTATION getDirection() const;
	[[nodiscard]] int getMinoCount() const;
	[[nodiscard]] Vector2i getMino(int mino) const;
	[[nodiscard]] RotationPoint const & getRotationPoint(int point) const;

private:
	vector<Vector2i> _minos;
	ROTATION _direction;
	RotationPoint _rotationPoints[5];
};

