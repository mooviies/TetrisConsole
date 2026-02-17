#pragma once

#ifdef TETRIS_DEBUG

#include <optional>
#include <string>
#include <tuple>
#include <vector>

#include "Constants.h"
#include "GameController.h"
#include "GameRenderer.h"
#include "GameState.h"
#include "InputSnapshot.h"
#include "PieceData.h"
#include "Vector2i.h"

struct TestAction {
    InputSnapshot input;
};

// Additional piece drop within a single scenario (for B2B and Combo tests)
struct DropSpec {
    PieceType pieceType = PieceType::I;
    int preRotations = 0;
    Vector2i prePosition{20, 4};
    // Matrix cells to add before this drop
    std::vector<std::tuple<int, int, int>> matrixCells;
};

struct TestExpectation {
    std::optional<int64_t> scoreChange;
    std::optional<int> linesCleared;
    std::optional<bool> tSpin;
    std::optional<bool> miniTSpin;
    std::optional<Vector2i> piecePosition;
    std::optional<bool> backToBackActive;
    std::optional<int> maxCombo;
};

struct TestScenario {
    std::string name;
    std::string description;
    PieceType pieceType = PieceType::T;
    int startingLevel = 1;
    // Matrix setup: {row, col, color} triples
    std::vector<std::tuple<int, int, int>> matrixCells;
    // Number of CW rotations before teleporting (0=NORTH, 1=EAST, 2=SOUTH, 3=WEST)
    int preRotations = 0;
    // Where to teleport piece before the action sequence
    Vector2i prePosition{20, 4};
    // Sequence of inputs
    std::vector<TestAction> actions;
    // Whether to hard drop after the action sequence (locks piece)
    bool hardDropAfterActions = true;
    TestExpectation expected;
    // Additional drops after the main piece (for multi-piece scenarios)
    std::vector<DropSpec> extraDrops;
};

struct TestResult {
    std::string name;
    bool passed{};
    std::string detail;
    std::string expected;
    int64_t scoreBefore{};
    int64_t scoreAfter{};
    int linesBefore{};
    int linesAfter{};
    bool tSpinDetected{};
    bool miniTSpinDetected{};
    Vector2i piecePositionAfter{};
};

class TestRunner {
public:
    TestRunner();

    void run();

private:
    void runScenario(const TestScenario &scenario);
    void ensurePieceType(PieceType type);
    void spawnPiece();
    void applyPreRotations(int count);
    void forceHardDrop();
    void fastForwardToCompletion();
    void renderAndDelay(int ms = 200);
    void writeReport() const;
    void printProgress() const;

    static std::vector<TestScenario> buildScenarios();
    static void parseRow(GameState &state, int row, const std::string &pattern);

    GameState _state;
    GameController _controller;
    GameRenderer _renderer;
    std::vector<TestResult> _results;
};

#endif
