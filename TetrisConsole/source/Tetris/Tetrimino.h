#pragma once

#include <string>
#include <array>

#include "Constants.h"
#include "PieceData.h"
#include "rlutil.h"

class Tetrimino
{
public:
	Tetrimino(PieceType type, GameMatrix& matrix);
	~Tetrimino();

	[[nodiscard]] bool setPosition(const Vector2i &position);
	[[nodiscard]] bool move(const Vector2i &distance);
	[[nodiscard]] bool simulateMove(const Vector2i &distance);
	[[nodiscard]] bool rotate(DIRECTION direction);
	[[nodiscard]] bool lock();
	void resetRotation();

	[[nodiscard]] bool isMino(int row, int column) const;
	[[nodiscard]] Vector2i const & getPosition() const { return _currentPosition; }

	[[nodiscard]] int getColor() const { return _color; }
	[[nodiscard]] const std::string& getPreviewLine1() const { return _previewLine1; }
	[[nodiscard]] const std::string& getPreviewLine2() const { return _previewLine2; }
	[[nodiscard]] Vector2i getStartingPosition() const { return _startingPosition; }
	[[nodiscard]] bool canTSpin() const { return _hasTSpin; }
	bool checkTSpin();
	bool checkMiniTSpin();

private:
	void onLock();

	[[nodiscard]] int getMino(int row, int column) const;
	[[nodiscard]] int getMino(const Vector2i& position) const;
	[[nodiscard]] bool checkPositionValidity(const Vector2i &position, ROTATION rotation) const;

	[[nodiscard]] int getLastRotationPoint() const { return _lastRotationPoint; }
	[[nodiscard]] ROTATION getCurrentRotation() const { return _currentRotation; }

	GameMatrix& _matrix;

	int _color;
	Vector2i _startingPosition;
	std::array<Facing, 4> _facings;
	int _lastRotationPoint;

	std::string _previewLine1;
	std::string _previewLine2;

	ROTATION _currentRotation;
	Vector2i _currentPosition;

	// T-spin data (only meaningful for T piece)
	bool _hasTSpin;
	std::array<TSpinPositions, 4> _tSpinPositions;
	bool _didTSpinWith5 = false;
};
