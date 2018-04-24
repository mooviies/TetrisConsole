#pragma once

#include "GraphicObject.h"

namespace konsole
{
	enum Border {
		SOLID,
		DASHED,
		DOTTED
	};

	struct Spacing {

		Spacing()
		{
			left = 0;
			right = 0;
			top = 0;
			bottom = 0;
		}

		Spacing(uint value)
		{
			*this = value;
		}

		Spacing& operator=(uint value)
		{
			left = right = top = bottom = value;
			return *this;
		}

		uint left;
		uint right;
		uint top;
		uint bottom;
	};

	typedef Spacing Padding;
	typedef Spacing Margin;

	struct TableElement
	{
		TableElement() {}
		TableElement(const GraphicObject* obj, Margin mar, Alignement align) 
		{
			object = obj;
			margin = mar;
			alignement = align;
		}

		const GraphicObject* object = nullptr;
		Margin margin;
		Alignement alignement;
	};

	class Table : public GraphicObject
	{
	public:
		Table(uint rows, uint columns, Margin tableMargin = Margin(), Margin elementsMargin = Margin(), Alignement alignement = Alignement::LEFT);
		~Table();

		void setAlignement(Alignement alignement);
		void setAlignement(uint row, uint column, Alignement alignement);
		void setAlignementRow(uint row, Alignement alignement);
		void setAlignementColumn(uint column, Alignement alignement);

		void setTableMargin(Margin margin);
		void setMargin(Margin margin);
		void setMargin(uint row, uint column, Margin margin);
		void setMarginRow(uint row, Margin margin);
		void setMarginColumn(uint column, Margin margin);

		uint getNbRows() const { return _rows; }
		uint getNbColumns() const { return _columns; }

		void addRow();
		void addColumn();

		void insertRowBefore(uint row);
		void insertColumnBefore(uint column);

		void removeRow(uint row);
		void removeColumn(uint column);

		const GraphicObject* getElement(uint row, uint column) const;

		void addElement(uint row, uint column, const GraphicObject* element);
		void removeElement(uint row, uint column);

	private:
		void generate();

	private:
		uint _rows;
		uint _columns;
		Margin _margin;
		
		Margin _defaultElementMargin;
		Alignement _defaultElementAlignement;

		std::vector<std::vector<TableElement>> _elements;
	};
}