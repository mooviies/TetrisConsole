#ifdef TETRIS_DEBUG

#include "TestRunner.h"

#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <thread>

#include "Color.h"
#include "Platform.h"
#include "Timer.h"
#include "rlutil.h"

using namespace std;

// Timer IDs used by the game engine
static constexpr auto FALL       = "fall";
static constexpr auto LOCK_DOWN  = "lockdown";
static constexpr auto GENERATION = "generation";
static constexpr auto ANIMATE    = "animate";

TestRunner::TestRunner()
	: _controller(Timer::instance()) {}

// ---------------------------------------------------------------------------
// Matrix helper: parse a 10-char string into a matrix row
// 'X' = Color::GREY, '.' = empty
// ---------------------------------------------------------------------------
void TestRunner::parseRow(GameState& state, int row, const string& pattern) {
	for (int col = 0; col < TETRIS_WIDTH && col < static_cast<int>(pattern.size()); col++) {
		if (pattern[static_cast<size_t>(col)] == 'X')
			state.matrix[static_cast<size_t>(row)][static_cast<size_t>(col)] = Color::GREY;
		else
			state.matrix[static_cast<size_t>(row)][static_cast<size_t>(col)] = 0;
	}
}

// ---------------------------------------------------------------------------
// Ensure the piece at the current bag index is the desired type
// ---------------------------------------------------------------------------
void TestRunner::ensurePieceType(PieceType type) {
	const int targetColor = getPieceData(type).color;
	auto& bag = _state.pieces.bag;
	auto idx = _state.pieces.bagIndex;

	if (bag[idx]->getColor() == targetColor)
		return;

	for (size_t i = idx + 1; i < bag.size(); i++) {
		if (bag[i]->getColor() == targetColor) {
			bag[idx].swap(bag[i]);
			return;
		}
	}
}

// ---------------------------------------------------------------------------
// Fast-forward through the Generation phase to spawn the current piece
// ---------------------------------------------------------------------------
void TestRunner::spawnPiece() {
	Timer::instance().resetTimer(GENERATION, 999);
	_controller.step(_state, {});

	// Reset rotation to NORTH — piece objects persist in the bag and may
	// retain facing from a previous scenario.
	if (_state.pieces.current)
		_state.pieces.current->resetRotation();
}

// ---------------------------------------------------------------------------
// Apply N clockwise rotations at the spawn position (for pre-orienting)
// ---------------------------------------------------------------------------
void TestRunner::applyPreRotations(int count) {
	InputSnapshot rotateCW{};
	rotateCW.rotateCW = true;

	for (int i = 0; i < count; i++) {
		Timer::instance().resetTimer(FALL, 0);
		_controller.step(_state, rotateCW);
		_controller.step(_state, {}); // release to clear didRotate
	}
}

// ---------------------------------------------------------------------------
// Force a hard drop: repeatedly step with fall timer set high until the
// piece locks (phase leaves Falling)
// ---------------------------------------------------------------------------
void TestRunner::forceHardDrop() {
	InputSnapshot hardDrop{};
	hardDrop.hardDrop = true;

	Timer::instance().resetTimer(FALL, 999);
	_controller.step(_state, hardDrop);

	for (int i = 0; i < 50 && _state.phase == GamePhase::Falling; i++) {
		Timer::instance().resetTimer(FALL, 999);
		_controller.step(_state, {});
	}
}

// ---------------------------------------------------------------------------
// Step through Pattern/Iterate/Animate/Eliminate until Completion or Generation
// ---------------------------------------------------------------------------
void TestRunner::fastForwardToCompletion() {
	for (int i = 0; i < 30; i++) {
		if (_state.phase == GamePhase::Completion || _state.phase == GamePhase::Generation)
			break;
		if (_state.phase == GamePhase::Animate)
			Timer::instance().resetTimer(ANIMATE, 999);
		_controller.step(_state, {});
	}
	// Step through Completion to reach Generation
	if (_state.phase == GamePhase::Completion)
		_controller.step(_state, {});
}

// ---------------------------------------------------------------------------
void TestRunner::renderAndDelay(int ms) {
	_state.markDirty();
	_renderer.render(_state);
	this_thread::sleep_for(chrono::milliseconds(ms));
}

// ---------------------------------------------------------------------------
void TestRunner::printProgress() const {
	int passed = 0;
	for (const auto& r : _results)
		if (r.passed) passed++;

	rlutil::locate(1, 4);
	cout << "  Progress: " << _results.size() << " tests run, "
	     << passed << " passed, "
	     << (_results.size() - static_cast<size_t>(passed)) << " failed" << flush;
}

// ---------------------------------------------------------------------------
// Run a single test scenario
// ---------------------------------------------------------------------------
void TestRunner::runScenario(const TestScenario& scenario) {
	// Reset state
	_controller.configurePolicies(LockDownMode::Extended);
	_controller.configureVariant(GameVariant::Marathon, _state);
	_renderer.configure(0, false, false);
	_controller.start(_state);

	// Set level and prevent level-ups during tests
	_state.stats.level = scenario.startingLevel;
	_state.stats.goal = 999;

	// Fill matrix
	for (const auto& [row, col, color] : scenario.matrixCells)
		_state.matrix[static_cast<size_t>(row)][static_cast<size_t>(col)] = color;

	// Ensure correct piece type in bag
	ensurePieceType(scenario.pieceType);

	// Spawn piece (fast-forward Generation)
	spawnPiece();

	// Apply pre-rotations at the spawn position
	if (scenario.preRotations > 0)
		applyPreRotations(scenario.preRotations);

	// Teleport to prePosition
	if (!_state.pieces.current->setPosition(scenario.prePosition)) {
		TestResult result;
		result.name = scenario.name;
		result.passed = false;
		result.detail = "Failed to teleport piece to prePosition";
		result.expected = "Valid position";
		_results.push_back(result);
		return;
	}

	// Render initial state
	renderAndDelay(400);

	// Record state before actions
	int64_t scoreBefore = _state.stats.score;
	int linesBefore = _state.stats.lines;

	// Execute action sequence
	bool tSpinDetected = false;
	bool miniTSpinDetected = false;

	for (const auto& action : scenario.actions) {
		// Set fall timer: high for soft/hard drops (so they trigger), 0 otherwise
		if (action.input.softDrop || action.input.hardDrop)
			Timer::instance().resetTimer(FALL, 999);
		else
			Timer::instance().resetTimer(FALL, 0); // suppress gravity

		_controller.step(_state, action.input);

		// Capture T-Spin flags right after rotation (before they're consumed)
		if (action.input.rotateCW || action.input.rotateCCW) {
			if (_state.flags.lastMoveIsTSpin) tSpinDetected = true;
			if (_state.flags.lastMoveIsMiniTSpin) miniTSpinDetected = true;
		}

		// Release keys
		Timer::instance().resetTimer(FALL, 0);
		_controller.step(_state, {});

		if (_state.phase == GamePhase::Falling)
			renderAndDelay(200);
	}

	// Record piece position (for movement/rotation tests)
	Vector2i piecePositionAfter{};
	if (_state.pieces.current)
		piecePositionAfter = _state.pieces.current->getPosition();

	// Hard drop to lock if requested
	if (scenario.hardDropAfterActions && _state.phase == GamePhase::Falling)
		forceHardDrop();

	// Fast-forward through line clear phases
	if (_state.phase != GamePhase::Falling)
		fastForwardToCompletion();

	// Render final state
	renderAndDelay(400);

	// Extra drops (for multi-piece scenarios like B2B and Combo)
	for (const auto& drop : scenario.extraDrops) {
		for (const auto& [row, col, color] : drop.matrixCells)
			_state.matrix[static_cast<size_t>(row)][static_cast<size_t>(col)] = color;

		ensurePieceType(drop.pieceType);
		spawnPiece();

		if (drop.preRotations > 0)
			applyPreRotations(drop.preRotations);

		if (!_state.pieces.current->setPosition(drop.prePosition)) {
			TestResult result;
			result.name = scenario.name;
			result.passed = false;
			result.detail = "Failed to teleport extra drop piece";
			result.expected = "Valid position";
			_results.push_back(result);
			return;
		}

		renderAndDelay(200);

		if (_state.phase == GamePhase::Falling)
			forceHardDrop();

		if (_state.phase != GamePhase::Falling)
			fastForwardToCompletion();

		renderAndDelay(200);
	}

	// Record results
	int64_t scoreAfter = _state.stats.score;
	int linesAfter = _state.stats.lines;

	// Build TestResult
	TestResult result;
	result.name = scenario.name;
	result.scoreBefore = scoreBefore;
	result.scoreAfter = scoreAfter;
	result.linesBefore = linesBefore;
	result.linesAfter = linesAfter;
	result.tSpinDetected = tSpinDetected;
	result.miniTSpinDetected = miniTSpinDetected;
	result.piecePositionAfter = piecePositionAfter;
	result.passed = true;

	// Validate expectations
	ostringstream detail, expect;

	if (scenario.expected.scoreChange) {
		int64_t actualChange = scoreAfter - scoreBefore;
		int64_t expectedChange = *scenario.expected.scoreChange;
		detail << "Score: " << scoreBefore << " -> " << scoreAfter
		       << " (+" << actualChange << ")";
		expect << "Score change: +" << expectedChange;
		if (actualChange != expectedChange) result.passed = false;
	}

	if (scenario.expected.linesCleared) {
		int actualLines = linesAfter - linesBefore;
		int expectedLines = *scenario.expected.linesCleared;
		if (!detail.str().empty()) { detail << ", "; expect << ", "; }
		detail << "Lines: " << actualLines;
		expect << "Lines: " << expectedLines;
		if (actualLines != expectedLines) result.passed = false;
	}

	if (scenario.expected.tSpin) {
		if (!detail.str().empty()) { detail << ", "; expect << ", "; }
		detail << "T-Spin: " << (tSpinDetected ? "Yes" : "No");
		expect << "T-Spin: " << (*scenario.expected.tSpin ? "Yes" : "No");
		if (tSpinDetected != *scenario.expected.tSpin) result.passed = false;
	}

	if (scenario.expected.miniTSpin) {
		if (!detail.str().empty()) { detail << ", "; expect << ", "; }
		detail << "Mini T-Spin: " << (miniTSpinDetected ? "Yes" : "No");
		expect << "Mini T-Spin: " << (*scenario.expected.miniTSpin ? "Yes" : "No");
		if (miniTSpinDetected != *scenario.expected.miniTSpin) result.passed = false;
	}

	if (scenario.expected.piecePosition) {
		const auto& expectedPos = *scenario.expected.piecePosition;
		if (!detail.str().empty()) { detail << ", "; expect << ", "; }
		detail << "Pos: (" << piecePositionAfter.row << "," << piecePositionAfter.column << ")";
		expect << "Pos: (" << expectedPos.row << "," << expectedPos.column << ")";
		if (!(piecePositionAfter == expectedPos)) result.passed = false;
	}

	if (scenario.expected.backToBackActive) {
		if (!detail.str().empty()) { detail << ", "; expect << ", "; }
		detail << "B2B: " << (_state.stats.backToBackBonus ? "Yes" : "No");
		expect << "B2B: " << (*scenario.expected.backToBackActive ? "Yes" : "No");
		if (_state.stats.backToBackBonus != *scenario.expected.backToBackActive) result.passed = false;
	}

	if (scenario.expected.maxCombo) {
		if (!detail.str().empty()) { detail << ", "; expect << ", "; }
		detail << "Combo: " << _state.stats.combos;
		expect << "Combo: " << *scenario.expected.maxCombo;
		if (_state.stats.combos != *scenario.expected.maxCombo) result.passed = false;
	}

	result.detail = detail.str();
	result.expected = expect.str();
	_results.push_back(result);

	printProgress();
}

// ---------------------------------------------------------------------------
// Run all scenarios
// ---------------------------------------------------------------------------
void TestRunner::run() {
	rlutil::cls();
	GameRenderer::renderTitle("Test Runner");

	auto scenarios = buildScenarios();
	for (const auto& scenario : scenarios)
		runScenario(scenario);

	writeReport();

	// Show summary and wait for keypress
	rlutil::locate(1, 6);
	int passed = 0;
	for (const auto& r : _results)
		if (r.passed) passed++;

	cout << "  Results: " << passed << "/" << _results.size() << " passed" << endl;

	int line = 8;
	for (const auto& r : _results) {
		rlutil::locate(1, line++);
		if (r.passed)
			cout << "  [PASS] " << r.name;
		else
			cout << "  [FAIL] " << r.name << " — " << r.detail;
		if (line > 28) break;
	}

	rlutil::locate(1, line + 1);
	cout << "  Report written. Press any key to return..." << flush;
	Platform::getKey();
}

// ---------------------------------------------------------------------------
// Write detailed report to file
// ---------------------------------------------------------------------------
void TestRunner::writeReport() const {
	const string path = Platform::getDataDir() + "/test_report.txt";
	ofstream out(path);
	if (!out.is_open()) return;

	auto now = chrono::system_clock::now();
	auto time = chrono::system_clock::to_time_t(now);
	out << "TetrisConsole Test Report — " << put_time(localtime(&time), "%Y-%m-%d %H:%M:%S") << "\n";
	out << "================================================\n\n";

	int passed = 0;
	for (const auto& r : _results) {
		if (r.passed) {
			passed++;
			out << "[PASS] " << r.name << "\n";
		} else {
			out << "[FAIL] " << r.name << "\n";
		}
		out << "  Actual:   " << r.detail << "\n";
		out << "  Expected: " << r.expected << "\n\n";
	}

	out << "================================================\n";
	out << "Results: " << passed << "/" << _results.size() << " passed, "
	    << (_results.size() - static_cast<size_t>(passed)) << " failed\n";
	out.close();
}

// ===========================================================================
// SCENARIO DEFINITIONS
// ===========================================================================

// Helper to build matrix cells from a row pattern string
static vector<tuple<int, int, int>> rowCells(int row, const string& pattern) {
	vector<tuple<int, int, int>> cells;
	for (int col = 0; col < TETRIS_WIDTH && col < static_cast<int>(pattern.size()); col++) {
		char ch = pattern[static_cast<size_t>(col)];
		if (ch == 'X')
			cells.emplace_back(row, col, Color::GREY);
	}
	return cells;
}

static void addRow(vector<tuple<int, int, int>>& cells, int row, const string& pattern) {
	auto rc = rowCells(row, pattern);
	cells.insert(cells.end(), rc.begin(), rc.end());
}

static InputSnapshot makeRotateCW() {
	InputSnapshot s{};
	s.rotateCW = true;
	return s;
}

static InputSnapshot makeRotateCCW() {
	InputSnapshot s{};
	s.rotateCCW = true;
	return s;
}

static InputSnapshot makeLeft() {
	InputSnapshot s{};
	s.left = true;
	return s;
}

static InputSnapshot makeRight() {
	InputSnapshot s{};
	s.right = true;
	return s;
}

static InputSnapshot makeSoftDrop() {
	InputSnapshot s{};
	s.softDrop = true;
	return s;
}

vector<TestScenario> TestRunner::buildScenarios() {
	vector<TestScenario> scenarios;

	// ===================================================================
	// T-SPIN SCENARIOS
	// ===================================================================

	// T-Spin Zero: T NORTH at (38,8), rotate CW → EAST. 3 corners, no line clears.
	{
		TestScenario s;
		s.name = "T-Spin Zero";
		s.description = "T rotates CW into a slot with 3 corners, no line clears";
		s.pieceType = PieceType::T;
		addRow(s.matrixCells, 37, ".......X.X");
		addRow(s.matrixCells, 38, "XXXXXXX...");
		addRow(s.matrixCells, 39, "XXXXXXX..X");
		s.prePosition = {38, 8};
		s.actions = {{makeRotateCW()}};
		s.expected.scoreChange = 400;
		s.expected.linesCleared = 0;
		s.expected.tSpin = true;
		scenarios.push_back(s);
	}

	// T-Spin Single: T NORTH at (38,8), rotate CW → EAST. Row 39 completes.
	{
		TestScenario s;
		s.name = "T-Spin Single";
		s.description = "T rotates CW into slot, clears 1 line";
		s.pieceType = PieceType::T;
		addRow(s.matrixCells, 37, ".........X");
		addRow(s.matrixCells, 38, "XXXXXXX...");
		addRow(s.matrixCells, 39, "XXXXXXXX.X");
		s.prePosition = {38, 8};
		s.actions = {{makeRotateCW()}};
		s.expected.scoreChange = 800;
		s.expected.linesCleared = 1;
		s.expected.tSpin = true;
		scenarios.push_back(s);
	}

	// T-Spin Double: T NORTH at (38,8), rotate CW → EAST. Rows 37,39 complete.
	{
		TestScenario s;
		s.name = "T-Spin Double";
		s.description = "T rotates CW into slot, clears 2 lines";
		s.pieceType = PieceType::T;
		addRow(s.matrixCells, 37, "XXXXXXXX.X");
		addRow(s.matrixCells, 38, "XXXXXXX...");
		addRow(s.matrixCells, 39, "XXXXXXXX.X");
		s.prePosition = {38, 8};
		s.actions = {{makeRotateCW()}};
		s.expected.scoreChange = 1200;
		s.expected.linesCleared = 2;
		s.expected.tSpin = true;
		scenarios.push_back(s);
	}

	// T-Spin Triple: T SOUTH at (35,1), rotate CW → WEST at (37,1) via SRS kick 4.
	// Block at (34,1) forces kick past basic rotation. All 4 corners occupied.
	{
		TestScenario s;
		s.name = "T-Spin Triple";
		s.description = "T rotates CW from SOUTH using SRS kick, clears 3 lines";
		s.pieceType = PieceType::T;
		addRow(s.matrixCells, 34, ".X........");
		addRow(s.matrixCells, 36, "X.XXXXXXXX");
		addRow(s.matrixCells, 37, "..XXXXXXXX");
		addRow(s.matrixCells, 38, "X.XXXXXXXX");
		addRow(s.matrixCells, 39, "X.XXXXXXXX");
		s.preRotations = 2; // NORTH → EAST → SOUTH
		s.prePosition = {35, 1};
		s.actions = {{makeRotateCW()}};
		s.expected.scoreChange = 1600;
		s.expected.linesCleared = 3;
		s.expected.tSpin = true;
		scenarios.push_back(s);
	}

	// Mini T-Spin Zero: T NORTH at (38,1), CW → EAST at (38,0) via kick 1.
	// Back corners (OOB) + 1 front corner occupied. No completable rows.
	{
		TestScenario s;
		s.name = "Mini T-Spin Zero";
		s.description = "T rotates CW using wall kick, back corners occupied, no lines";
		s.pieceType = PieceType::T;
		addRow(s.matrixCells, 38, "...XXXXXXX");
		addRow(s.matrixCells, 39, ".X.XXXXXXX");
		s.prePosition = {38, 1};
		s.actions = {{makeRotateCW()}};
		s.expected.scoreChange = 100;
		s.expected.linesCleared = 0;
		s.expected.tSpin = false;
		s.expected.miniTSpin = true;
		scenarios.push_back(s);
	}

	// Mini T-Spin Single: T NORTH at (38,1), CW → EAST at (38,0) via kick 1.
	// Row 39 completes.
	{
		TestScenario s;
		s.name = "Mini T-Spin Single";
		s.description = "T rotates CW using wall kick, clears 1 line (mini)";
		s.pieceType = PieceType::T;
		addRow(s.matrixCells, 38, "...XXXXXXX");
		addRow(s.matrixCells, 39, ".XXXXXXXXX");
		s.prePosition = {38, 1};
		s.actions = {{makeRotateCW()}};
		s.expected.scoreChange = 200;
		s.expected.linesCleared = 1;
		s.expected.tSpin = false;
		s.expected.miniTSpin = true;
		scenarios.push_back(s);
	}

	// ===================================================================
	// MOVEMENT SCENARIOS
	// ===================================================================

	// Move Left
	{
		TestScenario s;
		s.name = "Move Left";
		s.description = "Piece moves left by 1 column";
		s.pieceType = PieceType::T;
		s.prePosition = {30, 5};
		s.actions = {{makeLeft()}};
		s.hardDropAfterActions = false;
		s.expected.piecePosition = Vector2i{30, 4};
		scenarios.push_back(s);
	}

	// Move Right
	{
		TestScenario s;
		s.name = "Move Right";
		s.description = "Piece moves right by 1 column";
		s.pieceType = PieceType::T;
		s.prePosition = {30, 5};
		s.actions = {{makeRight()}};
		s.hardDropAfterActions = false;
		s.expected.piecePosition = Vector2i{30, 6};
		scenarios.push_back(s);
	}

	// Soft Drop Score: 1 point per row dropped
	{
		TestScenario s;
		s.name = "Soft Drop Score";
		s.description = "Soft drop awards 1 point per row";
		s.pieceType = PieceType::T;
		s.prePosition = {30, 5};
		s.actions = {{makeSoftDrop()}};
		s.hardDropAfterActions = false;
		s.expected.scoreChange = 1;
		s.expected.piecePosition = Vector2i{31, 5};
		scenarios.push_back(s);
	}

	// Hard Drop + Lock: I at (30,4) NORTH, drops 9 rows → score = 2 * 9 = 18
	{
		TestScenario s;
		s.name = "Hard Drop + Lock";
		s.description = "Hard drop scores 2 points per row dropped";
		s.pieceType = PieceType::I;
		s.prePosition = {30, 4};
		s.hardDropAfterActions = true;
		s.expected.scoreChange = 18;
		s.expected.linesCleared = 0;
		scenarios.push_back(s);
	}

	// ===================================================================
	// ROTATION SCENARIOS
	// ===================================================================

	// CW Rotation (NORTH → EAST)
	{
		TestScenario s;
		s.name = "CW Rotation";
		s.description = "Piece rotates clockwise (NORTH -> EAST)";
		s.pieceType = PieceType::T;
		s.prePosition = {30, 5};
		s.actions = {{makeRotateCW()}};
		s.hardDropAfterActions = false;
		s.expected.piecePosition = Vector2i{30, 5};
		scenarios.push_back(s);
	}

	// CCW Rotation (NORTH → WEST)
	{
		TestScenario s;
		s.name = "CCW Rotation";
		s.description = "Piece rotates counter-clockwise (NORTH -> WEST)";
		s.pieceType = PieceType::T;
		s.prePosition = {30, 5};
		s.actions = {{makeRotateCCW()}};
		s.hardDropAfterActions = false;
		s.expected.piecePosition = Vector2i{30, 5};
		scenarios.push_back(s);
	}

	// Wall Kick Right: I EAST at (30,8), CW → SOUTH. OOB forces kick to (30,7).
	{
		TestScenario s;
		s.name = "Wall Kick Right";
		s.description = "I-piece against right wall kicks left on CW rotation";
		s.pieceType = PieceType::I;
		s.preRotations = 1; // EAST
		s.prePosition = {30, 8};
		s.actions = {{makeRotateCW()}};
		s.hardDropAfterActions = false;
		s.expected.piecePosition = Vector2i{30, 7};
		scenarios.push_back(s);
	}

	// Wall Kick Left: I WEST at (30,1), CCW → SOUTH. Block at (31,0) forces kick to (30,2).
	{
		TestScenario s;
		s.name = "Wall Kick Left";
		s.description = "I-piece near left wall kicks right on CCW rotation";
		s.pieceType = PieceType::I;
		s.preRotations = 3; // WEST
		s.prePosition = {30, 1};
		s.matrixCells.emplace_back(31, 0, Color::GREY);
		s.actions = {{makeRotateCCW()}};
		s.hardDropAfterActions = false;
		s.expected.piecePosition = Vector2i{30, 2};
		scenarios.push_back(s);
	}

	// ===================================================================
	// LINE CLEAR SCENARIOS
	// ===================================================================

	// Single: I EAST at (37,7) fills col 8. Only row 39 completes.
	{
		TestScenario s;
		s.name = "Single";
		s.description = "Clear 1 line";
		s.pieceType = PieceType::I;
		s.preRotations = 1; // EAST (vertical)
		addRow(s.matrixCells, 36, "XXXXXXX..X");
		addRow(s.matrixCells, 37, "XXXXXXX..X");
		addRow(s.matrixCells, 38, "XXXXXXX..X");
		addRow(s.matrixCells, 39, "XXXXXXXX.X");
		s.prePosition = {37, 7};
		s.expected.scoreChange = 100;
		s.expected.linesCleared = 1;
		scenarios.push_back(s);
	}

	// Double: I EAST at (37,7) fills col 8. Rows 38,39 complete.
	{
		TestScenario s;
		s.name = "Double";
		s.description = "Clear 2 lines";
		s.pieceType = PieceType::I;
		s.preRotations = 1; // EAST (vertical)
		addRow(s.matrixCells, 36, "XXXXXXX..X");
		addRow(s.matrixCells, 37, "XXXXXXX..X");
		addRow(s.matrixCells, 38, "XXXXXXXX.X");
		addRow(s.matrixCells, 39, "XXXXXXXX.X");
		s.prePosition = {37, 7};
		s.expected.scoreChange = 300;
		s.expected.linesCleared = 2;
		scenarios.push_back(s);
	}

	// Triple: I EAST at (37,7) fills col 8. Rows 37,38,39 complete.
	{
		TestScenario s;
		s.name = "Triple";
		s.description = "Clear 3 lines";
		s.pieceType = PieceType::I;
		s.preRotations = 1; // EAST (vertical)
		addRow(s.matrixCells, 36, "XXXXXXX..X");
		addRow(s.matrixCells, 37, "XXXXXXXX.X");
		addRow(s.matrixCells, 38, "XXXXXXXX.X");
		addRow(s.matrixCells, 39, "XXXXXXXX.X");
		s.prePosition = {37, 7};
		s.expected.scoreChange = 500;
		s.expected.linesCleared = 3;
		scenarios.push_back(s);
	}

	// Tetris: I EAST at (37,7) fills col 8. All 4 rows complete.
	{
		TestScenario s;
		s.name = "Tetris";
		s.description = "Clear 4 lines with I-piece";
		s.pieceType = PieceType::I;
		s.preRotations = 1; // EAST (vertical)
		addRow(s.matrixCells, 36, "XXXXXXXX.X");
		addRow(s.matrixCells, 37, "XXXXXXXX.X");
		addRow(s.matrixCells, 38, "XXXXXXXX.X");
		addRow(s.matrixCells, 39, "XXXXXXXX.X");
		s.prePosition = {37, 7};
		s.expected.scoreChange = 800;
		s.expected.linesCleared = 4;
		scenarios.push_back(s);
	}

	// ===================================================================
	// BACK-TO-BACK & COMBO SCENARIOS
	// ===================================================================

	// Back-to-Back Tetris: Two consecutive Tetrises. Second gets 50% bonus.
	// 8 rows with gap at col 9. First I EAST fills rows 36-39 → 800.
	// Rows 32-35 shift down. Second I EAST fills again → 1200 (B2B).
	{
		TestScenario s;
		s.name = "Back-to-Back Tetris";
		s.description = "Two Tetrises in a row, second gets 50% B2B bonus";
		s.pieceType = PieceType::I;
		s.preRotations = 1; // EAST (vertical)
		for (int row = 32; row <= 39; row++)
			addRow(s.matrixCells, row, "XXXXXXXXX.");
		s.prePosition = {37, 8};

		DropSpec drop2;
		drop2.pieceType = PieceType::I;
		drop2.preRotations = 1;
		drop2.prePosition = {37, 8};
		s.extraDrops.push_back(drop2);

		s.expected.scoreChange = 2000;
		s.expected.linesCleared = 8;
		s.expected.backToBackActive = true;
		scenarios.push_back(s);
	}

	// Combo: 3 consecutive clears. Combo counter reaches 2.
	// 4 rows with 4-col gap (cols 6-9). Drops 1,2: I NORTH fills cols 6-9.
	// Drop 3: Add cols 6,7 to row 39 → 2-col gap. J WEST fills cols 8,9.
	{
		TestScenario s;
		s.name = "Combo";
		s.description = "3 consecutive line clears, combo counter reaches 2";
		s.pieceType = PieceType::I;
		for (int row = 36; row <= 39; row++)
			addRow(s.matrixCells, row, "XXXXXX....");
		s.prePosition = {39, 7};

		DropSpec drop2;
		drop2.pieceType = PieceType::I;
		drop2.prePosition = {39, 7};
		s.extraDrops.push_back(drop2);

		DropSpec drop3;
		drop3.pieceType = PieceType::J;
		drop3.preRotations = 3; // WEST
		drop3.prePosition = {38, 9};
		addRow(drop3.matrixCells, 39, "......XX..");
		s.extraDrops.push_back(drop3);

		s.expected.scoreChange = 300;
		s.expected.linesCleared = 3;
		s.expected.maxCombo = 2;
		scenarios.push_back(s);
	}

	return scenarios;
}

#endif
