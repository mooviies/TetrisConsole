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

	void printPreview(int line);
	void setPosition(Vector2i position);
	void move(Vector2i distance);
	void rotate(DIRECTION direction);
	bool isMino(int row, int column) const;
	virtual int getColor() const = 0;

protected:
	Tetrimino(int** matrix, string previewLine1, string previewLine2);

	void setFacing(ROTATION direction, Facing facing);

	int getMino(int row, int column) const;
	int getMino(const Vector2i& position) const;
	bool checkPositionValidity(Vector2i position, ROTATION rotation);

private:
	int** _matrix;

	Facing _facings[4];
	RotationPoint _rotationPoints[5];

	string _previewLine1;
	string _previewLine2;

	ROTATION _currentRotation;
	Vector2i _currentPosition;
};

