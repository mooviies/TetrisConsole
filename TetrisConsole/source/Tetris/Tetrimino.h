#pragma once

#include <string>
#include <array>

#include "Facing.h"
#include "rlutil.h"

class Tetrimino
{
public:
	virtual ~Tetrimino();

	void printPreview(int line, bool hold = false) const;
	[[nodiscard]] bool setPosition(const Vector2i &position);
	[[nodiscard]] bool move(const Vector2i &distance);
	[[nodiscard]] bool simulateMove(const Vector2i &distance);
	[[nodiscard]] bool rotate(DIRECTION direction);
	[[nodiscard]] bool lock();
	void resetRotation();

	[[nodiscard]] bool isMino(int row, int column) const;
	[[nodiscard]] Vector2i const & getPosition() const { return _currentPosition; }

	[[nodiscard]] virtual int getColor() const = 0;
	[[nodiscard]] virtual Vector2i getStartingPosition() const { return {19, 4}; }
	[[nodiscard]] virtual bool canTSpin() const { return false; }
	virtual bool checkTSpin() { return false;  }
	virtual bool checkMiniTSpin() { return false; }

protected:
	Tetrimino(std::vector<std::vector<int>>& matrix, const std::string &previewLine1, const std::string &previewLine2);

	virtual void onLock() {}
	void setFacing(ROTATION direction, const Facing &facing);

	[[nodiscard]] int getMino(int row, int column) const;
	[[nodiscard]] int getMino(const Vector2i& position) const;
	[[nodiscard]] bool checkPositionValidity(const Vector2i &position, ROTATION rotation) const;

	[[nodiscard]] int getLastRotationPoint() const { return _lastRotationPoint; }
	[[nodiscard]] ROTATION getCurrentRotation() const { return _currentRotation; }

private:
	std::vector<std::vector<int>>& _matrix;

	std::array<Facing, 4> _facings;
	int _lastRotationPoint;

	std::string _previewLine1;
	std::string _previewLine2;

	ROTATION _currentRotation;
	Vector2i _currentPosition;
};
