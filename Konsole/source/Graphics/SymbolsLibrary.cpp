#include "SymbolsLibrary.h"

using namespace konsole;

char SymbolsLibrary::get(Symbol symbol)
{
	return (char)symbol - FACTOR_ASCII_TO_ANSI;
}