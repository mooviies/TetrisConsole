#pragma once

#include "Tetrimino.h"

using namespace std;

struct TSpinPositions
{
	TSpinPositions()
	= default;

	TSpinPositions(const Vector2i& a, const Vector2i& b, const Vector2i& c, const Vector2i& d)
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
	~TTetrimino() override;
	[[nodiscard]] int getColor() const override  { return rlutil::MAGENTA; }
	[[nodiscard]] bool canTSpin() const override  { return true; }
	bool checkTSpin() override;
	bool checkMiniTSpin() override;
	void onLock() override;

private:
	TSpinPositions _tSpinPositions[4];
	bool _didTSpinWith5;
};

