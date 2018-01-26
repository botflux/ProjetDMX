//Programme serveur commande projo et lyre

#include <iostream>
#include <iomanip>
#include <string>

#include <vector>

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include "enttecdmxusb.h"

using namespace std;

#define DMXDEVICE "/dev/ttyUSB0"

#define PORT 5000
#define LG_MESSAGE 256
#define ADRESSEDEBUTPROJO 22
#define ADRESSEDEBUTLYRE 1
#define TAILLEBUSDMX 512


vector<string> decode (string message, char separator)
{
vector<string> decoded;

string temp("");

for(int i = 0; i<message.length(); i=i+1)
{
	if(message[i] == separator || i==message.length() -1)
	{
		if(i==message.length()-1)
		{
			temp = temp+message[i];
		}
		decoded.push_back(temp);
		temp = "";
	}

	else
	{
		temp = temp+message[i];
	}
}
return decoded;
}



int main(int argc, char *argv[])
{

    string test1="cible=LYRE;red=255;green=100;blue=50";

	vector<string> vectorStr = decode(test1,';');

   vector< vector<string> > fullDecoded;

	for(int i = 0; i<vectorStr.size(); i=i+1)
	{
		vector<string> tempo = decode(vectorStr[i], '=');
		fullDecoded.push_back(tempo);
	}
	for(int i=0; i < fullDecoded.size(); i=i+1)
	{
		cout << "Nom : " << fullDecoded[i][0] << "\tValeur : " << fullDecoded[i][1]<<endl;
	}


    printf("Peripherique : %s\n\n", DMXDEVICE);

    EnttecDMXUSB *interfaceDMX;

    interfaceDMX = new EnttecDMXUSB(DMX_USB_PRO, DMXDEVICE);

    string configurationDMX;

    if(interfaceDMX->IsAvailable())//Si l'interface DMX est OK
    {
	//affiche la config
	configurationDMX = interfaceDMX->GetConfiguration();
	cout << "Interface DMX USB PRO detectee " << std::endl << configurationDMX << std::endl;

	int valeur[TAILLEBUSDMX];//tableau contenant les Valeur des 512 canaux du bus DMX
	int i;
	memset(valeur, 0, TAILLEBUSDMX);//On initialise le tableaux en le remplissant de 0


	//Variables pour le socket
	int descripteurSocket;
	struct sockaddr_in pointDeRencontreLocal;
	struct sockaddr_in pointDeRencontreDistant;
	socklen_t longueurAdresse;
	char messageRecu[LG_MESSAGE];
	int lus;
	int retour;

	descripteurSocket = socket(PF_INET, SOCK_DGRAM, 0);//On definit le protocole du socket sur UDP

	if(descripteurSocket <0) //Si echec on arrete le programme
	{
		perror("socket");
		exit(-1);
	}
	printf("Socket cree avec succes (%d)\n", descripteurSocket);

	longueurAdresse = sizeof(struct sockaddr_in);
	memset(&pointDeRencontreLocal, 0x00, longueurAdresse);
	pointDeRencontreLocal.sin_family = PF_INET;

	pointDeRencontreLocal.sin_addr.s_addr = htonl(INADDR_ANY);
	pointDeRencontreLocal.sin_port = htons(PORT);

	if((bind(descripteurSocket, (struct sockaddr *)&pointDeRencontreLocal, longueurAdresse)) <0)
	{
		perror("bind");
		exit(-2);
	}

	printf("Socket attachee avec succes ! \n");

	longueurAdresse = sizeof(pointDeRencontreDistant);

	memset(&pointDeRencontreDistant, 0x00, longueurAdresse);

	memset(messageRecu, 0x00, LG_MESSAGE*sizeof(char));//On initialise messageRecu comme une chaine vide

	int j;

	while(1)
	{
		memset(messageRecu, 0x00, LG_MESSAGE*sizeof(char));
		//On attend un ordre d'un client
		lus = recvfrom(descripteurSocket, messageRecu, sizeof(messageRecu),0,(struct sockaddr*)&pointDeRencontreDistant, &longueurAdresse);
		//On affiche le message recu
		printf("Message %s reçu avec succes (%d octets)\n\n", messageRecu, lus);

		//On le parse pour separer les differents champs
		char *pch = strtok(messageRecu,";");
		cout<<"cible : "<<pch<<endl;
		string cible = pch;//On definit la cible
		j=0;
		pch = strtok(NULL,";");//On atteint la premiere valeur a vouloir entrer dans le tableau de valeur
		while(pch!= NULL)
		{
			valeur[j] = atoi(pch);//on rentre la valeur dans le tableau
			pch = strtok(NULL,";");//On passe a la valeur suivante
			j=j+1;//on indente l'index du tableau
		}

		//Pour la lyre  :
		if (cible == "LYRE")
		{
			int canalLyre = ADRESSEDEBUTLYRE;
			for(i=0; i <=2; i=i+1)
			{
				//De base la lyre se configure quart par quart
				//ici on veut l'allumer entierement couleur par couleur
				//On fait 3 tour de boucle pour les 3 couleurs
				//et on met chaque quart a la meme valeur
				interfaceDMX->SetCanalDMX(canalLyre, valeur[i]);
				interfaceDMX->SetCanalDMX(canalLyre+3, valeur[i]);
				interfaceDMX->SetCanalDMX(canalLyre+6, valeur[i]);
				interfaceDMX->SetCanalDMX(canalLyre+9,valeur[i]);
				canalLyre = canalLyre + 1;//A chaque tour on passe a la couleur suivante en indentant les canaux
			}
			interfaceDMX->SendDMX();//on envoie la trame DMX au boitier
		}

		//Pour le projo
		else if(cible == "PROJO")
		{
			int canalProjo = ADRESSEDEBUTPROJO;

			for (i=0; i <=6; i=i+1)// 7 canaux à definir 1 par 1
			{
				interfaceDMX->SetCanalDMX(canalProjo, valeur[i]);//On definit le canal avec sa valeur
				canalProjo = canalProjo +1;//On apsse au canal suivant à chaque tour
			}
			interfaceDMX->SendDMX();// On enovie la trame DMX
		}

		else// Si la cible n'existe pas
		{
			cout<< "Cible incorrecte"<<endl;
		}

	}
	close(descripteurSocket);
    }
    delete interfaceDMX;
    return 0;
}
