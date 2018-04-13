#pragma once

#include <string>

#include "Tetrimino.h"

using namespace std;

struct TSpinPositions
{
	TSpinPositions()
	{

	}

	TSpinPositions(Vector2i a, Vector2i b, Vector2i c, Vector2i d)
		: A(a), B(b), C(c), D(d)
	{

	}

	Vector2i A;
	Vector2i B;
	Vector2i C;
	Vector2i D;
};

class TTetrimino : public Tetrimino
{
public:
	TTetrimino(vector<vector<int>>& matrix);
	virtual ~TTetrimino();
	virtual int getColor() const override  { return rlutil::MAGENTA; }
	virtual bool canTSpin() const override  { return true; }
	virtual bool checkTSpin() override;
	virtual bool checkMiniTSpin() override;
	virtual void onLock() override;

private:
	TSpinPositions _tSpinPositions[4];
	bool _didTSpinWith5;
};

