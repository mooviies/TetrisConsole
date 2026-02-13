#pragma once

struct Vector2i
{
	Vector2i()
	{
		row = 0;
		column = 0;
	}

	Vector2i(const Vector2i& other)
	{
		row = other.row;
		column = other.column;
	}

	Vector2i(int r, int c)
	{
		row = r;
		column = c;
	}

	Vector2i& operator+=(const Vector2i& right)
	{
		this->row += right.row;
		this->column += right.column;
		return *this;
	}

	Vector2i operator+(const Vector2i& right) const
	{
		return {row + right.row, column + right.column};
	}

	Vector2i operator-(const Vector2i& right) const
	{
		return {row - right.row, column - right.column};
	}

	bool operator==(const Vector2i& right) const
	{
		return row == right.row && column == right.column;
	}

	int row;
	int column;
};

