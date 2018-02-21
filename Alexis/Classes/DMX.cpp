#include "DMX.h"
#include <string>
#include <cstdlib>

using namespace std;

DMX::DMX(int address, string equipement)
{
	addr = address;
	type = equipement;
}


void DMX::remplirTab(int valeurDMX[], string fullDecoded, string a)
{
	if (type == "LYRE")
	{
		if (fullDecoded=="RED")
		{
			int valueRed = atoi(a.c_str());
			valeurDMX[addr]=valueRed;
		}

		if (fullDecoded=="BLUE")
		{
			int valueBlue = atoi(a.c_str());
			valeurDMX[addr+2]=valueBlue;
		}

		if (fullDecoded=="GREEN")
		{
			int valueGreen = atoi(a.c_str());
			valeurDMX[addr+1]=valueGreen;
		}

		if(fullDecoded=="INTENSITY")
		{
			int valueIntensity = atoi(a.c_str());
			valeurDMX[addr+6]=valueIntensity;
		}
	}
	
	else if (type == "PROJO")
	{
		if (fullDecoded=="RED")
		{
			int valueRed = atoi(a.c_str());
			
			for(int i=0; i<=3;i=i+1)
			{
				valeurDMX[addr + (i*3)] = valueRed;
			}
		}

		if (fullDecoded=="BLUE")
		{
			int valueBlue = atoi(a.c_str());
			
			for(int i=0; i<=3;i=i+1)
			{
				valeurDMX[addr + 2 +(i*3)] = valueBlue;
			}
		}

		if (fullDecoded=="GREEN")
		{
			int valueGreen = atoi(a.c_str());
			
			for(int i=0; i<=3;i=i+1)
			{
				valeurDMX[addr + 1 +(i*3)] = valueGreen;
			}
		}
	}
}

