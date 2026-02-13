#include "Utility.h"

#include <iostream>


Utility::Utility()
= default;


Utility::~Utility()
= default;

void Utility::showTitle(const string& sousTitre)
{
	// Initialisation d'une variable qui vaut la longueur du sous-titre
	// Cette valeur est réutilisé plusieurs fois on évite donc de toujours
	// repassé par la fonction length() qui est plus lente que la simple
	// lecture d'un int
	int longueur = static_cast<int>(sousTitre.length());

	// Affichage du titre principale
	cout << "╔══════════════════════════════════════════════════════════════════════════════╗║"
		<< "                                Tetris Console"
		<< "                                ║╠══════════════════════════════════════════════════════════════════════════════╣"
		<< "║";

	// On affiche des espace avant le sous-titre et de façon à ce qu'il soit centré
	for (int i = 0; i < 39 - (longueur / 2); i++)
	{
		cout << ' ';
	}

	// On affiche le sous-titre
	cout << sousTitre;

	// On vérifie si le nombre de lettre du sous-titre est pair ou impaire pour évité un bug d'affichage
	// Car s'il est impair alors le nombre d'espace avant et après le sous-titre sera différent
	if (longueur % 2 != 0)
		longueur++;

	// On affiche des espaces après le sous-titre
	for (int i = 0; i < 39 - (longueur / 2); i++)
	{
		cout << ' ';
	}
	// Puis on affiche la fermeture du cadre
	cout << "║╚══════════════════════════════════════════════════════════════════════════════╝" << endl;
}

string Utility::valueToString(int value, int length)
{
	string result;
	int decimal = 1;
	for (int i = 0; i < length; i++)
	{
		if (value < decimal)
			result += "0";

		decimal *= 10;
	}

	if (value > 0)
	{
		result += std::to_string(value);
	}

	return result;
}