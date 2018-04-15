#pragma once

#include <string>

#include "Facing.h"
#include "Constants.h"
#include "rlutil.h"

using namespace std;

class Tetrimino
{
public:
	virtual ~Tetrimino();

	void printPreview(int line, bool hold = false);
	bool setPosition(Vector2i position);
	bool move(Vector2i distance);
	bool simulateMove(Vector2i distance);
	bool rotate(DIRECTION direction);
	bool lock();
	void resetRotation();

	bool isMino(int row, int column) const;
	Vector2i const & getPosition() const { return _currentPosition; }

	virtual int getColor() const = 0;
	virtual Vector2i getStartingPosition() const { return Vector2i(19, 4); }
	virtual bool canTSpin() const { return false; }
	virtual bool checkTSpin() { return false;  }
	virtual bool checkMiniTSpin() { return false; }

protected:
	Tetrimino(vector<vector<int>>& matrix, string previewLine1, string previewLine2);

	virtual void onLock() {}
	void setFacing(ROTATION direction, Facing facing);

	int getMino(int row, int column) const;
	int getMino(const Vector2i& position) const;
	bool checkPositionValidity(Vector2i position, ROTATION rotation);

	int getLastRotationPoint() const { return _lastRotationPoint; }
	ROTATION getCurrentRotation() const { return _currentRotation; }

private:
	vector<vector<int>>& _matrix;

	Facing _facings[4];
	int _lastRotationPoint;

	string _previewLine1;
	string _previewLine2;

	ROTATION _currentRotation;
	Vector2i _currentPosition;
};

