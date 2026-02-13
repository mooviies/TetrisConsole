#include "PieceData.h"
#include "rlutil.h"

#include <vector>

using namespace std;

static PieceData buildI()
{
	PieceData data;
	data.color = rlutil::LIGHTBLUE;
	data.previewLine1 = "  ▄▄▄▄▄▄▄▄  ";
	data.previewLine2 = "  ▀▀▀▀▀▀▀▀  ";
	data.startingPosition = {19, 4};

	auto north = vector<Vector2i>{{0,0}, {0,-1}, {0,1}, {0,2}};
	auto east  = vector<Vector2i>{{0,1}, {-1,1}, {1,1}, {2,1}};
	auto south = vector<Vector2i>{{1,0}, {1,-1}, {1,1}, {1,2}};
	auto west  = vector<Vector2i>{{-1,0}, {0,0}, {1,0}, {2,0}};

	array<RotationPoint, 5> rotN = {{ RotationPoint({0,0},{0,0}), RotationPoint({0,-1},{0,-2}), RotationPoint({0,2},{0,1}), RotationPoint({-2,-1},{1,-2}), RotationPoint({1,2},{-2,1}) }};
	array<RotationPoint, 5> rotE = {{ RotationPoint({0,0},{0,0}), RotationPoint({0,2},{0,-1}), RotationPoint({0,-1},{0,2}), RotationPoint({-1,2},{-2,-1}), RotationPoint({2,-1},{1,2}) }};
	array<RotationPoint, 5> rotS = {{ RotationPoint({0,0},{0,0}), RotationPoint({0,1},{0,2}), RotationPoint({0,-2},{0,-1}), RotationPoint({2,1},{-1,2}), RotationPoint({-1,-2},{2,-1}) }};
	array<RotationPoint, 5> rotW = {{ RotationPoint({0,0},{0,0}), RotationPoint({0,-2},{0,1}), RotationPoint({0,1},{0,-2}), RotationPoint({1,-2},{2,1}), RotationPoint({-2,1},{-1,-2}) }};

	data.facings[NORTH] = Facing(NORTH, north, rotN);
	data.facings[EAST]  = Facing(EAST, east, rotE);
	data.facings[SOUTH] = Facing(SOUTH, south, rotS);
	data.facings[WEST]  = Facing(WEST, west, rotW);

	return data;
}

static PieceData buildJ()
{
	PieceData data;
	data.color = rlutil::BLUE;
	data.previewLine1 = "   ██       ";
	data.previewLine2 = "   ██████   ";

	auto north = vector<Vector2i>{{0,0}, {0,-1}, {0,1}, {-1,-1}};
	auto east  = vector<Vector2i>{{0,0}, {-1,0}, {1,0}, {-1,1}};
	auto south = vector<Vector2i>{{0,0}, {0,-1}, {0,1}, {1,1}};
	auto west  = vector<Vector2i>{{0,0}, {1,0}, {-1,0}, {1,-1}};

	array<RotationPoint, 5> rotN = {{ RotationPoint({0,0},{0,0}), RotationPoint({0,1},{0,-1}), RotationPoint({-1,1},{-1,-1}), RotationPoint({2,0},{2,0}), RotationPoint({2,1},{2,-1}) }};
	array<RotationPoint, 5> rotE = {{ RotationPoint({0,0},{0,0}), RotationPoint({0,1},{0,1}), RotationPoint({1,1},{1,1}), RotationPoint({-2,0},{-2,0}), RotationPoint({-2,1},{-2,1}) }};
	array<RotationPoint, 5> rotS = {{ RotationPoint({0,0},{0,0}), RotationPoint({0,-1},{0,1}), RotationPoint({-1,-1},{-1,1}), RotationPoint({2,0},{2,0}), RotationPoint({2,-1},{2,1}) }};
	array<RotationPoint, 5> rotW = {{ RotationPoint({0,0},{0,0}), RotationPoint({0,-1},{0,-1}), RotationPoint({1,-1},{1,-1}), RotationPoint({-2,0},{-2,0}), RotationPoint({-2,-1},{-2,-1}) }};

	data.facings[NORTH] = Facing(NORTH, north, rotN);
	data.facings[EAST]  = Facing(EAST, east, rotE);
	data.facings[SOUTH] = Facing(SOUTH, south, rotS);
	data.facings[WEST]  = Facing(WEST, west, rotW);

	return data;
}

static PieceData buildL()
{
	PieceData data;
	data.color = rlutil::BROWN;
	data.previewLine1 = "       ██   ";
	data.previewLine2 = "   ██████   ";

	auto north = vector<Vector2i>{{0,0}, {0,-1}, {0,1}, {-1,1}};
	auto east  = vector<Vector2i>{{0,0}, {-1,0}, {1,0}, {1,1}};
	auto south = vector<Vector2i>{{0,0}, {0,-1}, {0,1}, {1,-1}};
	auto west  = vector<Vector2i>{{0,0}, {1,0}, {-1,0}, {-1,-1}};

	array<RotationPoint, 5> rotN = {{ RotationPoint({0,0},{0,0}), RotationPoint({0,1},{0,-1}), RotationPoint({-1,1},{-1,-1}), RotationPoint({2,0},{2,0}), RotationPoint({2,1},{5,-1}) }};
	array<RotationPoint, 5> rotE = {{ RotationPoint({0,0},{0,0}), RotationPoint({0,1},{0,1}), RotationPoint({1,1},{1,1}), RotationPoint({-2,0},{-2,0}), RotationPoint({-2,1},{-2,1}) }};
	array<RotationPoint, 5> rotS = {{ RotationPoint({0,0},{0,0}), RotationPoint({0,-1},{0,1}), RotationPoint({-1,-1},{-1,1}), RotationPoint({2,0},{2,0}), RotationPoint({2,-1},{2,1}) }};
	array<RotationPoint, 5> rotW = {{ RotationPoint({0,0},{0,0}), RotationPoint({0,-1},{0,-1}), RotationPoint({1,-1},{1,-1}), RotationPoint({-2,0},{-2,0}), RotationPoint({-2,-1},{-2,-1}) }};

	data.facings[NORTH] = Facing(NORTH, north, rotN);
	data.facings[EAST]  = Facing(EAST, east, rotE);
	data.facings[SOUTH] = Facing(SOUTH, south, rotS);
	data.facings[WEST]  = Facing(WEST, west, rotW);

	return data;
}

static PieceData buildO()
{
	PieceData data;
	data.color = rlutil::YELLOW;
	data.previewLine1 = "    ████    ";
	data.previewLine2 = "    ████    ";

	auto north = vector<Vector2i>{{0,0}, {-1,0}, {-1,1}, {0,1}};
	array<RotationPoint, 5> rotN = {{ RotationPoint({0,0},{0,0}) }};

	data.facings[NORTH] = Facing(NORTH, north, rotN);
	data.facings[EAST]  = Facing(EAST, north, rotN);
	data.facings[SOUTH] = Facing(SOUTH, north, rotN);
	data.facings[WEST]  = Facing(WEST, north, rotN);

	return data;
}

static PieceData buildS()
{
	PieceData data;
	data.color = rlutil::GREEN;
	data.previewLine1 = "     ████   ";
	data.previewLine2 = "   ████     ";

	auto north = vector<Vector2i>{{0,0}, {0,-1}, {-1,0}, {-1,1}};
	auto east  = vector<Vector2i>{{0,0}, {-1,0}, {0,1}, {1,1}};
	auto south = vector<Vector2i>{{0,0}, {0,1}, {1,0}, {1,-1}};
	auto west  = vector<Vector2i>{{0,0}, {1,0}, {0,-1}, {-1,-1}};

	array<RotationPoint, 5> rotN = {{ RotationPoint({0,0},{0,0}), RotationPoint({0,1},{0,-1}), RotationPoint({-1,1},{-1,-1}), RotationPoint({2,0},{2,0}), RotationPoint({2,1},{2,-1}) }};
	array<RotationPoint, 5> rotE = {{ RotationPoint({0,0},{0,0}), RotationPoint({0,1},{0,1}), RotationPoint({1,1},{1,1}), RotationPoint({-2,0},{-2,0}), RotationPoint({-2,1},{-2,1}) }};
	array<RotationPoint, 5> rotS = {{ RotationPoint({0,0},{0,0}), RotationPoint({0,-1},{0,1}), RotationPoint({-1,-1},{-1,1}), RotationPoint({2,0},{2,0}), RotationPoint({2,-1},{2,1}) }};
	array<RotationPoint, 5> rotW = {{ RotationPoint({0,0},{0,0}), RotationPoint({0,-1},{0,-1}), RotationPoint({1,-1},{1,-1}), RotationPoint({-2,0},{-2,0}), RotationPoint({-2,-1},{-2,-1}) }};

	data.facings[NORTH] = Facing(NORTH, north, rotN);
	data.facings[EAST]  = Facing(EAST, east, rotE);
	data.facings[SOUTH] = Facing(SOUTH, south, rotS);
	data.facings[WEST]  = Facing(WEST, west, rotW);

	return data;
}

static PieceData buildT()
{
	PieceData data;
	data.color = rlutil::MAGENTA;
	data.previewLine1 = "     ██     ";
	data.previewLine2 = "   ██████   ";

	auto north = vector<Vector2i>{{0,0}, {0,-1}, {0,1}, {-1,0}};
	auto east  = vector<Vector2i>{{0,0}, {0,1}, {-1,0}, {1,0}};
	auto south = vector<Vector2i>{{0,0}, {0,-1}, {0,1}, {1,0}};
	auto west  = vector<Vector2i>{{0,0}, {0,-1}, {-1,0}, {1,0}};

	array<RotationPoint, 5> rotN = {{
		RotationPoint({0,0},{0,0}),
		RotationPoint({0,1},{0,-1}),
		RotationPoint({-1,1},{-1,-1}),
		RotationPoint(),
		RotationPoint({2,1},{2,-1})
	}};
	array<RotationPoint, 5> rotE = {{
		RotationPoint({0,0},{0,0}),
		RotationPoint({0,1},{0,1}),
		RotationPoint({1,1},{1,1}),
		RotationPoint({-2,0},{-2,0}),
		RotationPoint({-2,1},{-2,1})
	}};
	array<RotationPoint, 5> rotS = {{
		RotationPoint({0,0},{0,0}),
		RotationPoint({0,-1},{0,1}),
		RotationPoint(),
		RotationPoint({2,0},{2,0}),
		RotationPoint({2,-1},{2,1})
	}};
	array<RotationPoint, 5> rotW = {{
		RotationPoint({0,0},{0,0}),
		RotationPoint({0,-1},{0,-1}),
		RotationPoint({1,-1},{1,-1}),
		RotationPoint({-2,0},{-2,0}),
		RotationPoint({-2,-1},{-2,-1})
	}};

	data.facings[NORTH] = Facing(NORTH, north, rotN);
	data.facings[EAST]  = Facing(EAST, east, rotE);
	data.facings[SOUTH] = Facing(SOUTH, south, rotS);
	data.facings[WEST]  = Facing(WEST, west, rotW);

	data.hasTSpin = true;
	data.tSpinPositions[NORTH] = TSpinPositions({-1,-1}, {-1,1}, {1,-1}, {1,1});
	data.tSpinPositions[EAST]  = TSpinPositions({-1,1}, {1,1}, {-1,-1}, {1,-1});
	data.tSpinPositions[SOUTH] = TSpinPositions({1,1}, {1,-1}, {-1,-1}, {-1,1});
	data.tSpinPositions[WEST]  = TSpinPositions({1,-1}, {-1,-1}, {1,1}, {-1,1});

	return data;
}

static PieceData buildZ()
{
	PieceData data;
	data.color = rlutil::RED;
	data.previewLine1 = "   ████     ";
	data.previewLine2 = "     ████   ";

	auto north = vector<Vector2i>{{0,0}, {0,1}, {-1,0}, {-1,-1}};
	auto east  = vector<Vector2i>{{0,0}, {1,0}, {0,1}, {-1,1}};
	auto south = vector<Vector2i>{{0,0}, {0,-1}, {1,0}, {1,1}};
	auto west  = vector<Vector2i>{{0,0}, {-1,0}, {0,-1}, {1,-1}};

	array<RotationPoint, 5> rotN = {{ RotationPoint({0,0},{0,0}), RotationPoint({0,1},{0,-1}), RotationPoint({-1,1},{-1,-1}), RotationPoint({2,0},{2,0}), RotationPoint({2,1},{2,-1}) }};
	array<RotationPoint, 5> rotE = {{ RotationPoint({0,0},{0,0}), RotationPoint({0,1},{0,1}), RotationPoint({1,1},{1,1}), RotationPoint({-2,0},{-2,0}), RotationPoint({-2,1},{-2,1}) }};
	array<RotationPoint, 5> rotS = {{ RotationPoint({0,0},{0,0}), RotationPoint({0,-1},{0,1}), RotationPoint({-1,-1},{-1,1}), RotationPoint({2,0},{2,0}), RotationPoint({2,-1},{2,1}) }};
	array<RotationPoint, 5> rotW = {{ RotationPoint({0,0},{0,0}), RotationPoint({0,-1},{0,-1}), RotationPoint({1,-1},{1,-1}), RotationPoint({-2,0},{-2,0}), RotationPoint({-2,-1},{-2,-1}) }};

	data.facings[NORTH] = Facing(NORTH, north, rotN);
	data.facings[EAST]  = Facing(EAST, east, rotE);
	data.facings[SOUTH] = Facing(SOUTH, south, rotS);
	data.facings[WEST]  = Facing(WEST, west, rotW);

	return data;
}

static std::array<PieceData, 7> buildPieceTable()
{
	std::array<PieceData, 7> table;
	table[static_cast<int>(PieceType::I)] = buildI();
	table[static_cast<int>(PieceType::J)] = buildJ();
	table[static_cast<int>(PieceType::L)] = buildL();
	table[static_cast<int>(PieceType::O)] = buildO();
	table[static_cast<int>(PieceType::S)] = buildS();
	table[static_cast<int>(PieceType::T)] = buildT();
	table[static_cast<int>(PieceType::Z)] = buildZ();
	return table;
}

const PieceData& getPieceData(PieceType type)
{
	static const auto table = buildPieceTable();
	return table[static_cast<size_t>(type)];
}
