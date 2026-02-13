#include "Overseer.h"

Tetris* Overseer::_tetris = nullptr;

void Overseer::setTetris(Tetris* tetris)
{
	_tetris = tetris;
}

Tetris& Overseer::getTetris()
{
	return *_tetris;
}

Overseer::Overseer()
{
}


Overseer::~Overseer()
{
}
