#include "Table.h"

#include "SymbolsLibrary.h"

using namespace std;
using namespace konsole;

Table::Table(uint rows, uint columns, Border border, Margin tableMargin, Margin elementsMargin, Alignement alignement)
{
	if (rows == 0)
		rows = 1;

	if (columns == 0)
		columns = 1;

	_rows = rows;
	_columns = columns;
	_border = border;
	_margin = tableMargin;
	_defaultElementMargin = elementsMargin;
	_defaultElementAlignement = alignement;

	for (int r = 0; r < _rows; r++)
	{
		_elements.push_back(vector<TableElement>());
		for (int c = 0; c < _columns; c++)
		{
			_elements[r].push_back(TableElement(nullptr, elementsMargin, alignement));
		}
	}
	generate();
}

Table::~Table()
{
}

void Table::setAlignement(Alignement alignement)
{
	_defaultElementAlignement = alignement;
	for (int r = 0; r < _rows; r++)
	{
		for (int c = 0; c < _columns; c++)
		{
			_elements[r][c].alignement = alignement;
		}
	}
	generate();
}

void Table::setAlignement(uint row, uint column, Alignement alignement)
{
	_elements[row][column].alignement = alignement;
	generate();
}

void Table::setAlignementRow(uint row, Alignement alignement)
{
	for (int c = 0; c < _columns; c++)
	{
		_elements[row][c].alignement = alignement;
	}
	generate();
}

void Table::setAlignementColumn(uint column, Alignement alignement)
{
	for (int r = 0; r < _rows; r++)
	{
		_elements[r][column].alignement = alignement;
	}
	generate();
}

void Table::setTableMargin(Margin margin)
{
	_margin = margin;
	generate();
}

void Table::setMargin(Margin margin)
{
	_defaultElementMargin = margin;
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
		_elements[_rows - 1].push_back(TableElement(nullptr, _defaultElementMargin, _defaultElementAlignement));
	}
	generate();
}

void Table::addColumn()
{
	_columns++;
	for (int r = 0; r < _rows; r++)
	{
		_elements[r].push_back(TableElement(nullptr, _defaultElementMargin, _defaultElementAlignement));
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
		_elements[row].push_back(TableElement(nullptr, _defaultElementMargin, _defaultElementAlignement));
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
		_elements[r].insert(_elements[r].begin() + column, TableElement(nullptr, _defaultElementMargin, _defaultElementAlignement));
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

void Table::addElement(uint row, uint column, const GraphicObject* element)
{
	if (row >= _rows || column >= _columns)
		return;

	if (_elements[row][column].object != nullptr)
		return;

	TableElement& tableElement = _elements[row][column];
	tableElement.object = element;
	tableElement.alignement = _defaultElementAlignement;
	tableElement.margin = _defaultElementMargin;
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
	char corner_tl, corner_tr, corner_bl, corner_br, t_left, t_right, t_top, t_bottom, horizontal, vertical, cross;

	switch (_border)
	{
	case Border::DASHED:
		corner_tl = (char)Symbol::BORDER_SINGLE_CORNER_TL;
		corner_tr = (char)Symbol::BORDER_SINGLE_CORNER_TR;
		corner_bl = (char)Symbol::BORDER_SINGLE_CORNER_BL;
		corner_br = (char)Symbol::BORDER_SINGLE_CORNER_BR;

		t_left = (char)Symbol::BORDER_SINGLE_T_LEFT;
		t_right = (char)Symbol::BORDER_SINGLE_T_RIGHT;
		t_top = (char)Symbol::BORDER_SINGLE_T_TOP;
		t_bottom = (char)Symbol::BORDER_SINGLE_T_BOTTOM;

		horizontal = (char)Symbol::BORDER_DASHED_HORIZONTAL;
		vertical = (char)Symbol::BORDER_DASHED_VERTICAL;
		cross = (char)Symbol::BORDER_SINGLE_CROSS;
		break;
	case Border::DOTTED:
		corner_tl = (char)Symbol::BORDER_DOTTED;
		corner_tr = (char)Symbol::BORDER_DOTTED;
		corner_bl = (char)Symbol::BORDER_DOTTED;
		corner_br = (char)Symbol::BORDER_DOTTED;

		t_left = (char)Symbol::BORDER_DOTTED;
		t_right = (char)Symbol::BORDER_DOTTED;
		t_top = (char)Symbol::BORDER_DOTTED;
		t_bottom = (char)Symbol::BORDER_DOTTED;

		horizontal = (char)Symbol::BORDER_DOTTED;
		vertical = (char)Symbol::BORDER_DOTTED;
		cross = (char)Symbol::BORDER_DOTTED;
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

	uint* rowHeights = new uint[_elements.size()];
	uint* columnWidths = new uint[_elements[0].size()];

	for (size_t r = 0; r < _elements.size(); r++)
	{
		rowHeights[r] = 0;
		for (size_t c = 0; c < _elements[r].size(); c++)
		{
			if (r == 0)
				columnWidths[c] = 0;

			TableElement& tableElement = _elements[r][c];
			uint height = tableElement.object ? tableElement.object->getHeight() + tableElement.margin.top + tableElement.margin.bottom : 0;
			if (height > rowHeights[r])
				rowHeights[r] = height;

			uint width = tableElement.object ? tableElement.object->getWidth() + tableElement.margin.left + tableElement.margin.right : 0;
			if (width > columnWidths[c])
				columnWidths[c] = width;
		}
	}

	uint totalInsideWidth = 0, totalInsideHeight = 0;
	for (size_t r = 0; r < _elements.size(); r++)
	{
		totalInsideHeight += rowHeights[r];
	}

	for (size_t c = 0; c < _elements[0].size(); c++)
	{
		totalInsideWidth += columnWidths[c];
	}

	ColoredString horizontalTop, horizontalInside, horizontalElement, horizontalBottom;

	horizontalTop += corner_tl;
	horizontalInside += t_left;
	horizontalElement += vertical;
	horizontalBottom += corner_bl;

	for (int i = 0; i < _columns; i++)
	{
		for (int j = 0; j < columnWidths[i]; j++)
		{
			horizontalTop += horizontal;
			horizontalInside += horizontal;
			horizontalElement += ' ';
			horizontalBottom += horizontal;

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

	for (int i = 0; i < _rows; i++)
	{
		for (int j = 0; j < rowHeights[i]; j++)
		{
			addLine(horizontalElement);
		}
		if (i == _rows - 1)
			addLine(horizontalBottom);
		else
			addLine(horizontalInside);
	}

	delete[] rowHeights;
	delete[] columnWidths;
	GraphicObject::generateObject();
}
