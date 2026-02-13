#pragma once

class GameState;

class GameRenderer
{
public:
	GameRenderer();
	~GameRenderer();

	static void display();
	void refresh(GameState& state);
	void printMatrix(const GameState& state, bool visible = true);

private:
	static void printLine(const GameState& state, int line, bool visible);
	static void printPreview(const GameState& state);
	static void printScore(const GameState& state);
};
