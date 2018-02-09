#include "Socket.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <iostream>

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
	pointDeRencontreDistant.sin_family = sinFamily;
	pointDeRencontreDistant.sin_port = sinPort;
	pointDeRencontreDistant.sin_addr.s_addr = sAddr;
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

Socket::~Socket()
{
	delete messageRecu;
}
