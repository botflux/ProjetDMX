#include <iostream>
#include <iomanip>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <sys/types.h>

#define LG_MESSAGE 1024

class threadServeur
{
public:
	static void start(Socket *mySocket)
	{
		while (1)
		{
			char message[LG_MESSAGE];//On initialise le message que le serveur va recevoir
			memset(message, 0x00, LG_MESSAGE*sizeof(char));//On le remet à 0 à chaque fois
			
			//On attend un ordre d'un client
			mySocket->setLus(recvfrom(mySocket->getDescripteur(), message, sizeof(message),0,(struct sockaddr*)mySocket->getPdrDistant(), mySocket->getLgAddr2()));
			cout << "message recu : " << message << endl;
			
			//Stocker le message dans un fichier json
			string m = message;
			FILE* fichier = NULL;
			fichier = fopen("test.json", "w");
			fprintf(fichier, m.c_str());
			fclose(fichier);
		}
	}
	
};