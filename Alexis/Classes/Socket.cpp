#include "Socket.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <iostream>
#include "enttecdmxusb.h"
using namespace std;

Socket::Socket()
{
	longueurAdresse = sizeof(struct sockaddr_in);
	messageRecu = new char[256];
}

void Socket::setDescripteur(int a, int b, int c)
{
	descripteurSocket = socket(a, b, c);
	if(descripteurSocket < 0)
	{
		cerr << "socket";
		exit(-1);
	}
	else
	{
		cout <<"Socket cree avec succes" << endl;
	}
}

void Socket::setPdrLocal(short sinFamily, unsigned short sinPort, unsigned long sAddr)
{
	pointDeRencontreLocal.sin_family = sinFamily;
	pointDeRencontreLocal.sin_port = sinPort;
	pointDeRencontreLocal.sin_addr.s_addr = sAddr;
}

void Socket::setPdrDistant(short sinFamily, unsigned short sinPort, unsigned long sAddr)
{
//	pointDeRencontreDistant.sin_family = sinFamily;
	pointDeRencontreDistant.sin_port = sinPort;
//	pointDeRencontreDistant.sin_addr.s_addr = sAddr;
}

void Socket::affichePdrDistant()
{
	cout << "sin_family = " << pointDeRencontreDistant.sin_family << endl;
	cout << "sin_port = " << pointDeRencontreDistant.sin_port << endl;
	cout << "sin_addr.s_addr = " << pointDeRencontreDistant.sin_addr.s_addr << endl;
}

void Socket::setMessage(char *a)
{
	messageRecu = a;
}

void Socket::setLus(int a)
{
	lus = a;
}

void Socket::setRetour(int a)
{
	retour = a;
}

int Socket::getDescripteur()
{
	return descripteurSocket;
}

struct sockaddr_in* Socket::getPdrLocal()
{
	return &pointDeRencontreLocal;
}

struct sockaddr_in* Socket::getPdrDistant()
{
	return &pointDeRencontreDistant;
}

socklen_t* Socket::getLgAddr2()
{
	return &longueurAdresse;
}

socklen_t Socket::getLgAddr()
{
	return longueurAdresse;
}

char* Socket::getMessage()
{
	return messageRecu;
}

int Socket::getLus()
{
	return lus;
}

int Socket::getRetour()
{
	return retour;
}

void Socket::bindSocket()
{
	int a;
	a = bind(descripteurSocket, (struct sockaddr *)&pointDeRencontreLocal, longueurAdresse);
	if(a < 0)
	{
		cerr <<"bind";
		exit(-2);
	}
	else
	{
		cout << "Socket attache avec succes"<< endl;
	}
}

void Socket::closeDescripteur()
{
	close(descripteurSocket);
}

Socket::~Socket()
{
	
	delete messageRecu;
}

void Socket::sendACK(bool a)
{
	char reponse[]= "Message reçu correct";
	char reponseCibleInvalide[] = "Cible INVALIDE";
	
	switch ( a )
	{
		case true : 
			sendto(descripteurSocket, reponse,sizeof(reponse), 0, (struct sockaddr *)&pointDeRencontreDistant, longueurAdresse);
			break;
		case false :
			sendto(descripteurSocket, reponseCibleInvalide,sizeof(reponseCibleInvalide), 0, (struct sockaddr *)&pointDeRencontreDistant, longueurAdresse);
			break;
	}
}


void Socket::remplirTab(int valeurDMX[], int ADRESSEDEBUT, string equipementDMX, string fullDecoded, string a)
{
	if (equipementDMX == "LYRE")
	{
		if (fullDecoded=="RED")
		{
			int valueRed = atoi(a.c_str());
			valeurDMX[ADRESSEDEBUT]=valueRed;
		}

		if (fullDecoded=="BLUE")
		{
			int valueBlue = atoi(a.c_str());
			valeurDMX[ADRESSEDEBUT+2]=valueBlue;
		}

		if (fullDecoded=="GREEN")
		{
			int valueGreen = atoi(a.c_str());
			valeurDMX[ADRESSEDEBUT+1]=valueGreen;
		}

		if(fullDecoded=="INTENSITY")
		{
			int valueIntensity = atoi(a.c_str());
			valeurDMX[ADRESSEDEBUT+6]=valueIntensity;
		}
	}
	
	else if (equipementDMX == "PROJO")
	{
		if (fullDecoded=="RED")
		{
			int valueRed = atoi(a.c_str());
			
			for(int i=0; i<=3;i=i+1)
			{
				valeurDMX[ADRESSEDEBUT + (i*3)] = valueRed;
			}
		}

		if (fullDecoded=="BLUE")
		{
			int valueBlue = atoi(a.c_str());
			
			for(int i=0; i<=3;i=i+1)
			{
				valeurDMX[ADRESSEDEBUT + 2 +(i*3)] = valueBlue;
			}
		}

		if (fullDecoded=="GREEN")
		{
			int valueGreen = atoi(a.c_str());
			
			for(int i=0; i<=3;i=i+1)
			{
				valeurDMX[ADRESSEDEBUT + 1 +(i*3)] = valueGreen;
			}
		}
	}
}






