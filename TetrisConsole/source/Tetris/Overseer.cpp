#include "Overseer.h"

#include <cassert>

Tetris* Overseer::_tetris = nullptr;

void Overseer::setTetris(Tetris* tetris)
{
	_tetris = tetris;
}

Tetris& Overseer::getTetris()
{
	assert(_tetris != nullptr);
	return *_tetris;
}

Overseer::Overseer()
= default;


Overseer::~Overseer()
= default;
