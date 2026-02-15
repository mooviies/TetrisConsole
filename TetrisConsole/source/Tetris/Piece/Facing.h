#pragma once

#include <vector>
#include <array>
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

// SRS wall kick data: each RotationPoint holds two translations (one per direction).
// Point 0 is basic rotation, points 1-4 are SRS wall kick offsets tested in order.
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
	std::array<Vector2i, 2> _translations;
	bool _exist;
};

class Facing
{
public:
	Facing();
	Facing(ROTATION direction, const std::vector<Vector2i> &minos, const std::array<RotationPoint, 5>& rotationPoints);
	~Facing();

	[[nodiscard]] ROTATION getDirection() const;
	[[nodiscard]] int getMinoCount() const;
	[[nodiscard]] Vector2i getMino(int mino) const;
	[[nodiscard]] RotationPoint const & getRotationPoint(int point) const;

private:
	std::vector<Vector2i> _minos;
	ROTATION _direction;
	std::array<RotationPoint, 5> _rotationPoints;
};
