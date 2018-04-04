#include "DMX.h"
#include <string>
#include <cstdlib>

using namespace std;

//Pour etre construit, l'objet DMX doit OBLIGATOIREMENT posseder une adresse de type DMX et un type
DMX::DMX(int address, string equipement)
{
	addr = address;
	type = equipement;
}

//On rempli le tableau qui contient les 512 valeur DMX selon l'adresse et le type de l'objet
//Le tableau sera rempli à des adresse différente, sur un nombre de canal différent et sur un
//protocol propre à chaque équipement
void DMX::remplirTab(int valeurDMX[], int red, int green, int blue, int intensity=100)
{
	if (type == "LYRE")
	{
		valeurDMX[addr]=red;
		valeurDMX[addr+1]=green;
		valeurDMX[addr+2]=blue;
		valeurDMX[addr+6]=intensity;
	}
	
	else if (type == "PROJO")
	{
		for(int i=0; i<=3;i=i+1)
		{
			valeurDMX[addr + (i*3)] = red;
			valeurDMX[addr + 1 +(i*3)] = green;
			valeurDMX[addr + 2 +(i*3)] = blue;
		}
	}
}

