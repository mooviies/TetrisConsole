#include "Table.h"

#include "SymbolsLibrary.h"

using namespace std;
using namespace konsole;

Table::Table(uint rows, uint columns, Border border, Margin margin)
{
	if (rows == 0)
		rows = 1;

	if (columns == 0)
		columns = 1;

	_rows = rows;
	_columns = columns;
	_border = border;
	_margin = margin;

	for (int r = 0; r < _rows; r++)
	{
		_elements.push_back(vector<TableElement>());
		for (int c = 0; c < _columns; c++)
		{
			_elements[r].push_back(TableElement(nullptr, margin));
		}
	}
	generate();
}

Table::~Table()
{
}

void Table::setMargin(Margin margin)
{
	_margin = margin;
	for (int r = 0; r < _rows; r++)
	{
		for (int c = 0; c < _columns; c++)
		{
			_elements[r][c].margin = margin;
		}
	}
	generate();
}

void Table::setMargin(uint row, uint column, Margin margin)
{
	_elements[row][column].margin = margin;
	generate();
}

void Table::setMarginRow(uint row, Margin margin)
{
	for (int c = 0; c < _columns; c++)
	{
		_elements[row][c].margin = margin;
	}
	generate();
}

void Table::setMarginColumn(uint column, Margin margin)
{
	for (int r = 0; r < _rows; r++)
	{
		_elements[r][column].margin = margin;
	}
	generate();
}

void Table::addRow()
{
	_rows++;
	_elements.push_back(vector<TableElement>());
	for (int c = 0; c < _columns; c++)
	{
		_elements[_rows - 1].push_back(TableElement(nullptr, _margin));
	}
	generate();
}

void Table::addColumn()
{
	_columns++;
	for (int r = 0; r < _rows; r++)
	{
		_elements[r].push_back(TableElement(nullptr, _margin));
	}
	generate();
}

void Table::insertRowBefore(uint row)
{
	if (row >= _rows)
	{
		addRow();
		return;
	}

	_rows++;
	_elements.insert(_elements.begin() + row, vector<TableElement>());
	for (int c = 0; c < _columns; c++)
	{
		_elements[row].push_back(TableElement(nullptr, _margin));
	}
	generate();
}

void Table::insertColumnBefore(uint column)
{
	if (column >= _columns)
	{
		addColumn();
		return;
	}

	_columns++;
	for (int r = 0; r < _rows; r++)
	{
		_elements[r].insert(_elements[r].begin() + column, TableElement(nullptr, _margin));
	}
	generate();
}

void Table::removeRow(uint row)
{
	if (_rows <= 1)
		return;

	_rows--;
	_elements.erase(_elements.begin() + row);
	generate();
}

void Table::removeColumn(uint column)
{
	if (_columns <= 1)
		return;

	_columns--;
	for (int r = 0; r < _rows; r++)
	{
		_elements[r].erase(_elements[r].begin() + column);
	}
	generate();
}

const GraphicObject* Table::getElement(uint row, uint column) const
{
	if (row < _rows && column < _columns)
		return _elements[row][column].object;

	return nullptr;
}

void Table::addElement(uint row, uint column, GraphicObject* element)
{
	if (row >= _rows || column >= _columns)
		return;

	if (_elements[row][column].object != nullptr)
		return;

	TableElement& tableElement = _elements[row][column];
	tableElement.object = element;
	tableElement.margin = _margin;
	generate();
}

void Table::removeElement(uint row, uint column)
{
	if (row >= _rows || column >= _columns)
		return;

	_elements[row][column].object = nullptr;
	generate();
}

void Table::generate()
{
	clear();
	char corner_tl, corner_tr, corner_bl, corner_br, t_left, t_right, t_top, t_bottom, horizontal, vertical, cross;

	switch (_border)
	{
	case Border::DASHED:
		corner_tl = (char)Symbol::BORDER_DASHED_HORIZONTAL;
		corner_tr = (char)Symbol::BORDER_DASHED_HORIZONTAL;
		corner_bl = (char)Symbol::BORDER_DASHED_HORIZONTAL;
		corner_br = (char)Symbol::BORDER_DASHED_HORIZONTAL;

		t_left = (char)Symbol::BORDER_DASHED_HORIZONTAL;
		t_right = (char)Symbol::BORDER_DASHED_HORIZONTAL;
		t_top = (char)Symbol::BORDER_DASHED_HORIZONTAL;
		t_bottom = (char)Symbol::BORDER_DASHED_HORIZONTAL;

		horizontal = (char)Symbol::BORDER_DASHED_HORIZONTAL;
		vertical = (char)Symbol::BORDER_DASHED_VERTICAL;
		cross = (char)Symbol::BORDER_DASHED_HORIZONTAL;
		break;
	case Border::DOTTED:
		corner_tl = (char)Symbol::BORDER_DOTTED_HORIZONTAL;
		corner_tr = (char)Symbol::BORDER_DOTTED_HORIZONTAL;
		corner_bl = (char)Symbol::BORDER_DOTTED_HORIZONTAL;
		corner_br = (char)Symbol::BORDER_DOTTED_HORIZONTAL;

		t_left = (char)Symbol::BORDER_DOTTED_VERTICAL;
		t_right = (char)Symbol::BORDER_DOTTED_VERTICAL;
		t_top = (char)Symbol::BORDER_DOTTED_HORIZONTAL;
		t_bottom = (char)Symbol::BORDER_DOTTED_HORIZONTAL;

		horizontal = (char)Symbol::BORDER_DOTTED_HORIZONTAL;
		vertical = (char)Symbol::BORDER_DOTTED_VERTICAL;
		cross = (char)Symbol::BORDER_DOTTED_HORIZONTAL;
		break;
	case Border::SOLID:
		corner_tl = (char)Symbol::BORDER_SINGLE_CORNER_TL;
		corner_tr = (char)Symbol::BORDER_SINGLE_CORNER_TR;
		corner_bl = (char)Symbol::BORDER_SINGLE_CORNER_BL;
		corner_br = (char)Symbol::BORDER_SINGLE_CORNER_BR;

		t_left = (char)Symbol::BORDER_SINGLE_T_LEFT;
		t_right = (char)Symbol::BORDER_SINGLE_T_RIGHT;
		t_top = (char)Symbol::BORDER_SINGLE_T_TOP;
		t_bottom = (char)Symbol::BORDER_SINGLE_T_BOTTOM;

		horizontal = (char)Symbol::BORDER_SINGLE_HORIZONTAL;
		vertical = (char)Symbol::BORDER_SINGLE_VERTICAL;
		cross = (char)Symbol::BORDER_SINGLE_CROSS;
		break;
	case Border::DOUBLE:
		corner_tl = (char)Symbol::BORDER_DOUBLE_CORNER_TL;
		corner_tr = (char)Symbol::BORDER_DOUBLE_CORNER_TR;
		corner_bl = (char)Symbol::BORDER_DOUBLE_CORNER_BL;
		corner_br = (char)Symbol::BORDER_DOUBLE_CORNER_BR;

		t_left = (char)Symbol::BORDER_DOUBLE_T_LEFT;
		t_right = (char)Symbol::BORDER_DOUBLE_T_RIGHT;
		t_top = (char)Symbol::BORDER_DOUBLE_T_TOP;
		t_bottom = (char)Symbol::BORDER_DOUBLE_T_BOTTOM;

		horizontal = (char)Symbol::BORDER_DOUBLE_HORIZONTAL;
		vertical = (char)Symbol::BORDER_DOUBLE_VERTICAL;
		cross = (char)Symbol::BORDER_DOUBLE_CROSS;
		break;
	}

	uint* rowHeights = new uint[_rows];
	uint* columnWidths = new uint[_columns];

	uint* rowPos = new uint[_rows];
	uint* columnPos = new uint[_columns];

	for (size_t r = 0; r < _rows; r++)
	{
		rowHeights[r] = 0;
		for (size_t c = 0; c < _columns; c++)
		{
			if (r == 0)
				columnWidths[c] = 0;

			TableElement& tableElement = _elements[r][c];
			uint height = tableElement.object ? tableElement.object->getHeight() + tableElement.margin.top + tableElement.margin.bottom : tableElement.margin.top + tableElement.margin.bottom;
			if (height > rowHeights[r])
				rowHeights[r] = height;

			uint width = tableElement.object ? tableElement.object->getWidth() + tableElement.margin.left + tableElement.margin.right : tableElement.margin.left + tableElement.margin.right;
			if (width > columnWidths[c])
				columnWidths[c] = width;
		}
	}

	uint totalInsideWidth = 0, totalInsideHeight = 0;
	for (size_t r = 0; r < _rows; r++)
	{
		totalInsideHeight += rowHeights[r];
	}

	for (size_t c = 0; c < _columns; c++)
	{
		totalInsideWidth += columnWidths[c];
	}

	ColoredString horizontalTop, horizontalInside, horizontalElement, horizontalBottom;

	horizontalTop += corner_tl;
	horizontalInside += t_left;
	horizontalElement += vertical;
	horizontalBottom += corner_bl;

	for (uint i = 0; i < _columns; i++)
	{
		columnPos[i] = horizontalTop.size() + _margin.left;
		for (uint j = 0; j < columnWidths[i]; j++)
		{
			horizontalTop += horizontal;
			horizontalInside += horizontal;
			horizontalBottom += horizontal;
			horizontalElement += ' ';
		}

		horizontalElement += vertical;

		if (i == _columns - 1)
		{
			horizontalTop += corner_tr;
			horizontalInside += t_right;
			horizontalBottom += corner_br;
		}
		else
		{
			horizontalTop += t_top;
			horizontalInside += cross;
			horizontalBottom += t_bottom;
		}
	}

	addLine(horizontalTop);

	for (uint i = 0; i < _rows; i++)
	{
		rowPos[i] = getHeight() + _margin.top;
		for (uint j = 0; j < rowHeights[i]; j++)
		{
			addLine(horizontalElement);
		}
		if (i == _rows - 1)
			addLine(horizontalBottom);
		else
			addLine(horizontalInside);
	}

	for (uint i = 0; i < _rows; i++)
	{
		for (uint j = 0; j < _columns; j++)
		{
			if (_elements[i][j].object)
			{
				_elements[i][j].object->setMinimumWidth(columnWidths[j] - _margin.left - _margin.right);
				addChild(_elements[i][j].object, columnPos[j], rowPos[i]);
			}
		}
	}

	delete[] rowHeights;
	delete[] columnWidths;
	delete[] rowPos;
	delete[] columnPos;
	GraphicObject::generateObject();
}
