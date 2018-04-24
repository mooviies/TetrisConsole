#include "Table.h"

using namespace std;
using namespace konsole;

Table::Table(uint rows, uint columns, Margin tableMargin, Margin elementsMargin, Alignement alignement)
{
	if (rows == 0)
		rows = 1;

	if (columns == 0)
		columns = 1;

	_rows = rows;
	_columns = columns;
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
	
}
