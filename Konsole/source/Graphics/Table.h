#pragma once

#include "GraphicObject.h"

namespace konsole
{
	enum Border {
		DOUBLE,
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

		Spacing(uint le, uint ri, uint to, uint bo)
		{
			left = le;
			right = ri;
			top = to;
			bottom = bo;
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
		TableElement(GraphicObject* obj, Margin mar) 
		{
			object = obj;
			margin = mar;
		}

		GraphicObject* object = nullptr;
		Margin margin;
	};

	class Table : public GraphicObject
	{
	public:
		Table(uint rows, uint columns, Border border, Margin elementsMargin = Margin());
		~Table();

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

		void addElement(uint row, uint column, GraphicObject* element);
		void removeElement(uint row, uint column);

	protected:
		virtual void generateObject() override {}

	private:
		void generate();

	private:
		uint _rows;
		uint _columns;
		Border _border;
		
		Margin _margin;

		std::vector<std::vector<TableElement>> _elements;
	};
}