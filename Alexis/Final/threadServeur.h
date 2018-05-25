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
			
			json_value* value = fichierJSON::ouvrirJSON("test.json");
			//On regarde si c'est une storyboard ou un changement de couleur qui a été envoyé
			string n = value->u.object.values[0].name;
			
			if (n == "storyboard" || n == "couleur")
			{
				mySocket->sendACK(true);
			}
			else
			{
				mySocket->sendACK(false);
			}
			fichierJSON::fermerJSON(value);
		}
	}
	
};