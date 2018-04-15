#include "Utility.h"

using namespace konsole;
using namespace std;

Utility::Utility()
{
}


Utility::~Utility()
{
}

void Utility::afficherTitre(string sousTitre)
{
	// Initialisation d'une variable qui vaut la longueur du sous-titre
	// Cette valeur est r�utilis� plusieurs fois on �vite donc de toujours
	// repass� par la fonction length() qui est plus lente que la simple
	// lecture d'un int
	int longueur = sousTitre.length();

	// Affichage du titre principale
	cout << "������������������������������������������������������������������������������ͻ�"
		<< "                                Tetris Console"
		<< "                                �������������������������������������������������������������������������������͹"
		<< "�";

	// On affiche des espace avant le sous-titre et de fa�on � ce qu'il soit centr�
	for (int i = 0; i < 39 - (longueur / 2); i++)
	{
		cout << ' ';
	}

	// On affiche le sous-titre
	cout << sousTitre;

	// On v�rifie si le nombre de lettre du sous-titre est pair ou impaire pour �vit� un bug d'affichage
	// Car s'il est impair alors le nombre d'espace avant et apr�s le sous-titre sera diff�rent
	if (longueur % 2 != 0)
		longueur++;

	// On affiche des espaces apr�s le sous-titre
	for (int i = 0; i < 39 - (longueur / 2); i++)
	{
		cout << ' ';
	}
	// Puis on affiche la fermeture du cadre
	cout << "�������������������������������������������������������������������������������ͼ" << endl;
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

	char buffer[10];
	if (value > 0)
	{
		_itoa_s(value, buffer, 10, 10);
		result += buffer;
	}

	return result;
}