#include "stdafx.h"
#include "Utility.h"


Utility::Utility()
{
}


Utility::~Utility()
{
}


void Utility::afficherTitre(string sousTitre)
{
	// Initialisation d'une variable qui vaut la longueur du sous-titre
	// Cette valeur est r้utilis้ plusieurs fois on ้vite donc de toujours
	// repass้ par la fonction length() qui est plus lente que la simple
	// lecture d'un int
	int longueur = sousTitre.length();

	// Affichage du titre principale
	cout << "ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออปบ"
		<< "                                Tetris Console"
		<< "                                บฬออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออน"
		<< "บ";

	// On affiche des espace avant le sous-titre et de fa็on เ ce qu'il soit centr้
	for (int i = 0; i < 39 - (longueur / 2); i++)
	{
		cout << ' ';
	}

	// On affiche le sous-titre
	cout << sousTitre;

	// On v้rifie si le nombre de lettre du sous-titre est pair ou impaire pour ้vit้ un bug d'affichage
	// Car s'il est impair alors le nombre d'espace avant et apr่s le sous-titre sera diff้rent
	if (longueur % 2 != 0)
		longueur++;

	// On affiche des espaces apr่s le sous-titre
	for (int i = 0; i < 39 - (longueur / 2); i++)
	{
		cout << ' ';
	}
	// Puis on affiche la fermeture du cadre
	cout << "บศออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ" << endl;
}