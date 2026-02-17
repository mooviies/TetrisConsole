#include "PieceData.h"
#include "Color.h"

#include <vector>

using namespace std;

static PieceData buildI()
{
	PieceData data;
	data.color = Color::LIGHTBLUE;
	data.previewLine1 = "  ▄▄▄▄▄▄▄▄  ";
	data.previewLine2 = "  ▀▀▀▀▀▀▀▀  ";
	data.startingPosition = {19, 4};

	const auto north = vector<Vector2i>{{0,0}, {0,-1}, {0,1}, {0,2}};
	const auto east  = vector<Vector2i>{{0,1}, {-1,1}, {1,1}, {2,1}};
	const auto south = vector<Vector2i>{{1,0}, {1,-1}, {1,1}, {1,2}};
	const auto west  = vector<Vector2i>{{-1,0}, {0,0}, {1,0}, {2,0}};

	const array<RotationPoint, 5> rotN = {{ RotationPoint({0,0},{0,0}), RotationPoint({0,-1},{0,-2}), RotationPoint({0,2},{0,1}), RotationPoint({-2,-1},{1,-2}), RotationPoint({1,2},{-2,1}) }};
	const array<RotationPoint, 5> rotE = {{ RotationPoint({0,0},{0,0}), RotationPoint({0,2},{0,-1}), RotationPoint({0,-1},{0,2}), RotationPoint({-1,2},{-2,-1}), RotationPoint({2,-1},{1,2}) }};
	const array<RotationPoint, 5> rotS = {{ RotationPoint({0,0},{0,0}), RotationPoint({0,1},{0,2}), RotationPoint({0,-2},{0,-1}), RotationPoint({2,1},{-1,2}), RotationPoint({-1,-2},{2,-1}) }};
	const array<RotationPoint, 5> rotW = {{ RotationPoint({0,0},{0,0}), RotationPoint({0,-2},{0,1}), RotationPoint({0,1},{0,-2}), RotationPoint({1,-2},{2,1}), RotationPoint({-2,1},{-1,-2}) }};

	data.facings[static_cast<int>(Rotation::North)] = Facing(Rotation::North, north, rotN);
	data.facings[static_cast<int>(Rotation::East)]  = Facing(Rotation::East, east, rotE);
	data.facings[static_cast<int>(Rotation::South)] = Facing(Rotation::South, south, rotS);
	data.facings[static_cast<int>(Rotation::West)]  = Facing(Rotation::West, west, rotW);

	return data;
}

static PieceData buildJ()
{
	PieceData data;
	data.color = Color::BLUE;
	data.previewLine1 = "   ██       ";
	data.previewLine2 = "   ██████   ";

	const auto north = vector<Vector2i>{{0,0}, {0,-1}, {0,1}, {-1,-1}};
	const auto east  = vector<Vector2i>{{0,0}, {-1,0}, {1,0}, {-1,1}};
	const auto south = vector<Vector2i>{{0,0}, {0,-1}, {0,1}, {1,1}};
	const auto west  = vector<Vector2i>{{0,0}, {1,0}, {-1,0}, {1,-1}};

	const array<RotationPoint, 5> rotN = {{ RotationPoint({0,0},{0,0}), RotationPoint({0,1},{0,-1}), RotationPoint({-1,1},{-1,-1}), RotationPoint({2,0},{2,0}), RotationPoint({2,1},{2,-1}) }};
	const array<RotationPoint, 5> rotE = {{ RotationPoint({0,0},{0,0}), RotationPoint({0,1},{0,1}), RotationPoint({1,1},{1,1}), RotationPoint({-2,0},{-2,0}), RotationPoint({-2,1},{-2,1}) }};
	const array<RotationPoint, 5> rotS = {{ RotationPoint({0,0},{0,0}), RotationPoint({0,-1},{0,1}), RotationPoint({-1,-1},{-1,1}), RotationPoint({2,0},{2,0}), RotationPoint({2,-1},{2,1}) }};
	const array<RotationPoint, 5> rotW = {{ RotationPoint({0,0},{0,0}), RotationPoint({0,-1},{0,-1}), RotationPoint({1,-1},{1,-1}), RotationPoint({-2,0},{-2,0}), RotationPoint({-2,-1},{-2,-1}) }};

	data.facings[static_cast<int>(Rotation::North)] = Facing(Rotation::North, north, rotN);
	data.facings[static_cast<int>(Rotation::East)]  = Facing(Rotation::East, east, rotE);
	data.facings[static_cast<int>(Rotation::South)] = Facing(Rotation::South, south, rotS);
	data.facings[static_cast<int>(Rotation::West)]  = Facing(Rotation::West, west, rotW);

	return data;
}

static PieceData buildL()
{
	PieceData data;
	data.color = Color::BROWN;
	data.previewLine1 = "       ██   ";
	data.previewLine2 = "   ██████   ";

	const auto north = vector<Vector2i>{{0,0}, {0,-1}, {0,1}, {-1,1}};
	const auto east  = vector<Vector2i>{{0,0}, {-1,0}, {1,0}, {1,1}};
	const auto south = vector<Vector2i>{{0,0}, {0,-1}, {0,1}, {1,-1}};
	const auto west  = vector<Vector2i>{{0,0}, {1,0}, {-1,0}, {-1,-1}};

	const array<RotationPoint, 5> rotN = {{ RotationPoint({0,0},{0,0}), RotationPoint({0,1},{0,-1}), RotationPoint({-1,1},{-1,-1}), RotationPoint({2,0},{2,0}), RotationPoint({2,1},{5,-1}) }};
	const array<RotationPoint, 5> rotE = {{ RotationPoint({0,0},{0,0}), RotationPoint({0,1},{0,1}), RotationPoint({1,1},{1,1}), RotationPoint({-2,0},{-2,0}), RotationPoint({-2,1},{-2,1}) }};
	const array<RotationPoint, 5> rotS = {{ RotationPoint({0,0},{0,0}), RotationPoint({0,-1},{0,1}), RotationPoint({-1,-1},{-1,1}), RotationPoint({2,0},{2,0}), RotationPoint({2,-1},{2,1}) }};
	const array<RotationPoint, 5> rotW = {{ RotationPoint({0,0},{0,0}), RotationPoint({0,-1},{0,-1}), RotationPoint({1,-1},{1,-1}), RotationPoint({-2,0},{-2,0}), RotationPoint({-2,-1},{-2,-1}) }};

	data.facings[static_cast<int>(Rotation::North)] = Facing(Rotation::North, north, rotN);
	data.facings[static_cast<int>(Rotation::East)]  = Facing(Rotation::East, east, rotE);
	data.facings[static_cast<int>(Rotation::South)] = Facing(Rotation::South, south, rotS);
	data.facings[static_cast<int>(Rotation::West)]  = Facing(Rotation::West, west, rotW);

	return data;
}

static PieceData buildO()
{
	PieceData data;
	data.color = Color::YELLOW;
	data.previewLine1 = "    ████    ";
	data.previewLine2 = "    ████    ";

	const auto north = vector<Vector2i>{{0,0}, {-1,0}, {-1,1}, {0,1}};
	const array<RotationPoint, 5> rotN = {{ RotationPoint({0,0},{0,0}) }};

	data.facings[static_cast<int>(Rotation::North)] = Facing(Rotation::North, north, rotN);
	data.facings[static_cast<int>(Rotation::East)]  = Facing(Rotation::East, north, rotN);
	data.facings[static_cast<int>(Rotation::South)] = Facing(Rotation::South, north, rotN);
	data.facings[static_cast<int>(Rotation::West)]  = Facing(Rotation::West, north, rotN);

	return data;
}

static PieceData buildS()
{
	PieceData data;
	data.color = Color::GREEN;
	data.previewLine1 = "     ████   ";
	data.previewLine2 = "   ████     ";

	const auto north = vector<Vector2i>{{0,0}, {0,-1}, {-1,0}, {-1,1}};
	const auto east  = vector<Vector2i>{{0,0}, {-1,0}, {0,1}, {1,1}};
	const auto south = vector<Vector2i>{{0,0}, {0,1}, {1,0}, {1,-1}};
	const auto west  = vector<Vector2i>{{0,0}, {1,0}, {0,-1}, {-1,-1}};

	const array<RotationPoint, 5> rotN = {{ RotationPoint({0,0},{0,0}), RotationPoint({0,1},{0,-1}), RotationPoint({-1,1},{-1,-1}), RotationPoint({2,0},{2,0}), RotationPoint({2,1},{2,-1}) }};
	const array<RotationPoint, 5> rotE = {{ RotationPoint({0,0},{0,0}), RotationPoint({0,1},{0,1}), RotationPoint({1,1},{1,1}), RotationPoint({-2,0},{-2,0}), RotationPoint({-2,1},{-2,1}) }};
	const array<RotationPoint, 5> rotS = {{ RotationPoint({0,0},{0,0}), RotationPoint({0,-1},{0,1}), RotationPoint({-1,-1},{-1,1}), RotationPoint({2,0},{2,0}), RotationPoint({2,-1},{2,1}) }};
	const array<RotationPoint, 5> rotW = {{ RotationPoint({0,0},{0,0}), RotationPoint({0,-1},{0,-1}), RotationPoint({1,-1},{1,-1}), RotationPoint({-2,0},{-2,0}), RotationPoint({-2,-1},{-2,-1}) }};

	data.facings[static_cast<int>(Rotation::North)] = Facing(Rotation::North, north, rotN);
	data.facings[static_cast<int>(Rotation::East)]  = Facing(Rotation::East, east, rotE);
	data.facings[static_cast<int>(Rotation::South)] = Facing(Rotation::South, south, rotS);
	data.facings[static_cast<int>(Rotation::West)]  = Facing(Rotation::West, west, rotW);

	return data;
}

static PieceData buildT()
{
	PieceData data;
	data.color = Color::MAGENTA;
	data.previewLine1 = "     ██     ";
	data.previewLine2 = "   ██████   ";

	const auto north = vector<Vector2i>{{0,0}, {0,-1}, {0,1}, {-1,0}};
	const auto east  = vector<Vector2i>{{0,0}, {0,1}, {-1,0}, {1,0}};
	const auto south = vector<Vector2i>{{0,0}, {0,-1}, {0,1}, {1,0}};
	const auto west  = vector<Vector2i>{{0,0}, {0,-1}, {-1,0}, {1,0}};

	const array<RotationPoint, 5> rotN = {{
		RotationPoint({0,0},{0,0}),
		RotationPoint({0,1},{0,-1}),
		RotationPoint({-1,1},{-1,-1}),
		RotationPoint(),
		RotationPoint({2,1},{2,-1})
	}};
	const array<RotationPoint, 5> rotE = {{
		RotationPoint({0,0},{0,0}),
		RotationPoint({0,1},{0,1}),
		RotationPoint({1,1},{1,1}),
		RotationPoint({-2,0},{-2,0}),
		RotationPoint({-2,1},{-2,1})
	}};
	const array<RotationPoint, 5> rotS = {{
		RotationPoint({0,0},{0,0}),
		RotationPoint({0,-1},{0,1}),
		RotationPoint(),
		RotationPoint({2,0},{2,0}),
		RotationPoint({2,-1},{2,1})
	}};
	const array<RotationPoint, 5> rotW = {{
		RotationPoint({0,0},{0,0}),
		RotationPoint({0,-1},{0,-1}),
		RotationPoint({1,-1},{1,-1}),
		RotationPoint({-2,0},{-2,0}),
		RotationPoint({-2,-1},{-2,-1})
	}};

	data.facings[static_cast<int>(Rotation::North)] = Facing(Rotation::North, north, rotN);
	data.facings[static_cast<int>(Rotation::East)]  = Facing(Rotation::East, east, rotE);
	data.facings[static_cast<int>(Rotation::South)] = Facing(Rotation::South, south, rotS);
	data.facings[static_cast<int>(Rotation::West)]  = Facing(Rotation::West, west, rotW);

	data.hasTSpin = true;
	data.tSpinPositions[static_cast<int>(Rotation::North)] = TSpinPositions({-1,-1}, {-1,1}, {1,-1}, {1,1});
	data.tSpinPositions[static_cast<int>(Rotation::East)]  = TSpinPositions({-1,1}, {1,1}, {-1,-1}, {1,-1});
	data.tSpinPositions[static_cast<int>(Rotation::South)] = TSpinPositions({1,1}, {1,-1}, {-1,-1}, {-1,1});
	data.tSpinPositions[static_cast<int>(Rotation::West)]  = TSpinPositions({1,-1}, {-1,-1}, {1,1}, {-1,1});

	return data;
}

static PieceData buildZ()
{
	PieceData data;
	data.color = Color::RED;
	data.previewLine1 = "   ████     ";
	data.previewLine2 = "     ████   ";

	const auto north = vector<Vector2i>{{0,0}, {0,1}, {-1,0}, {-1,-1}};
	const auto east  = vector<Vector2i>{{0,0}, {1,0}, {0,1}, {-1,1}};
	const auto south = vector<Vector2i>{{0,0}, {0,-1}, {1,0}, {1,1}};
	const auto west  = vector<Vector2i>{{0,0}, {-1,0}, {0,-1}, {1,-1}};

	const array<RotationPoint, 5> rotN = {{ RotationPoint({0,0},{0,0}), RotationPoint({0,1},{0,-1}), RotationPoint({-1,1},{-1,-1}), RotationPoint({2,0},{2,0}), RotationPoint({2,1},{2,-1}) }};
	const array<RotationPoint, 5> rotE = {{ RotationPoint({0,0},{0,0}), RotationPoint({0,1},{0,1}), RotationPoint({1,1},{1,1}), RotationPoint({-2,0},{-2,0}), RotationPoint({-2,1},{-2,1}) }};
	const array<RotationPoint, 5> rotS = {{ RotationPoint({0,0},{0,0}), RotationPoint({0,-1},{0,1}), RotationPoint({-1,-1},{-1,1}), RotationPoint({2,0},{2,0}), RotationPoint({2,-1},{2,1}) }};
	const array<RotationPoint, 5> rotW = {{ RotationPoint({0,0},{0,0}), RotationPoint({0,-1},{0,-1}), RotationPoint({1,-1},{1,-1}), RotationPoint({-2,0},{-2,0}), RotationPoint({-2,-1},{-2,-1}) }};

	data.facings[static_cast<int>(Rotation::North)] = Facing(Rotation::North, north, rotN);
	data.facings[static_cast<int>(Rotation::East)]  = Facing(Rotation::East, east, rotE);
	data.facings[static_cast<int>(Rotation::South)] = Facing(Rotation::South, south, rotS);
	data.facings[static_cast<int>(Rotation::West)]  = Facing(Rotation::West, west, rotW);

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
