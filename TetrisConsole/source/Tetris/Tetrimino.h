#pragma once

#include <string>

#include "Facing.h"
#include "rlutil.h"

using namespace std;

class Tetrimino
{
public:
	virtual ~Tetrimino();

	void printPreview(int line, bool hold = false) const;
	bool setPosition(const Vector2i &position);
	bool move(const Vector2i &distance);
	bool simulateMove(const Vector2i &distance);
	bool rotate(DIRECTION direction);
	bool lock();
	void resetRotation();

	[[nodiscard]] bool isMino(int row, int column) const;
	[[nodiscard]] Vector2i const & getPosition() const { return _currentPosition; }

	[[nodiscard]] virtual int getColor() const = 0;
	[[nodiscard]] virtual Vector2i getStartingPosition() const { return {19, 4}; }
	[[nodiscard]] virtual bool canTSpin() const { return false; }
	virtual bool checkTSpin() { return false;  }
	virtual bool checkMiniTSpin() { return false; }

protected:
	Tetrimino(vector<vector<int>>& matrix, const string &previewLine1, const string &previewLine2);

	virtual void onLock() {}
	void setFacing(ROTATION direction, const Facing &facing);

	[[nodiscard]] int getMino(int row, int column) const;
	[[nodiscard]] int getMino(const Vector2i& position) const;
	[[nodiscard]] bool checkPositionValidity(const Vector2i &position, ROTATION rotation) const;

	[[nodiscard]] int getLastRotationPoint() const { return _lastRotationPoint; }
	[[nodiscard]] ROTATION getCurrentRotation() const { return _currentRotation; }

private:
	vector<vector<int>>& _matrix;

	Facing _facings[4];
	int _lastRotationPoint;

	string _previewLine1;
	string _previewLine2;

	ROTATION _currentRotation;
	Vector2i _currentPosition;
};

