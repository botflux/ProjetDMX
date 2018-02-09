//Programme serveur commande projo et lyre

#include <iostream>
#include <iomanip>
#include <string>

#include <vector>

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

class scene
{
	string cible ;
	int red ;
	int green ;
	int blue ;
	int intensity ;
	int duree ;
	int numero ;
};

int main(int argc, char *argv[])
{


    printf("Peripherique : %s\n\n", DMXDEVICE);

    EnttecDMXUSB *interfaceDMX;
    cout << "test 0" << endl;
    interfaceDMX = new EnttecDMXUSB(DMX_USB_PRO, DMXDEVICE);
	cout << "test 1" << endl;
    string configurationDMX;

    bool test = interfaceDMX->IsAvailable();
    cout << "test : " << endl;

    if(test)//Si l'interface DMX est OK
    {
	//affiche la config
	configurationDMX = interfaceDMX->GetConfiguration();
	cout << "Interface DMX USB PRO detectee " << std::endl << configurationDMX << std::endl;

	int valeurLyre[TAILLEBUSDMX];//tableau contenant les Valeur des 512 canaux du bus DMX
	int valeurProjo[TAILLEBUSDMX];
	memset(valeurLyre, 0, TAILLEBUSDMX);//On initialise le tableaux en le remplissant de 0
	memset(valeurProjo, 0, TAILLEBUSDMX);

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
	vector<scene> storyboard;
	while(1)
	{
		memset(messageRecu, 0x00, LG_MESSAGE*sizeof(char));
		//On attend un ordre d'un client
		lus = recvfrom(descripteurSocket, messageRecu, sizeof(messageRecu),0,(struct sockaddr*)&pointDeRencontreDistant, &longueurAdresse);
		//On affiche le message recu
		printf("Message %s reçu avec succes (%d octets)\n\n", messageRecu, lus);

		string message = messageRecu;

		vector<string> vectorStr = decode(message,';');
		vector< vector<string> > fullDecoded;

		for(int i = 0; i < vectorStr.size(); i=i+1)
		{
			vector<string> tempo = decode(vectorStr[i],'=');
			fullDecoded.push_back(tempo);
		}
		string cible = "";

		char strValueRed[10]="0";
		char strValueGreen[10]="0";
		char strValueBlue[10]="0";
		char strValueIntensity[10]="0";

		int valueRed=0;
		int valueGreen=0;
		int valueBlue=0;
		int valueIntensity=0;

		string testRed;
		string testBlue;
		string testGreen;
		string testIntensity;

		for(int i=0; i< fullDecoded.size(); i=i+1)
		{
			cout <<"Nom : " << fullDecoded[i][0]  <<"\tValeur : " << fullDecoded[i][1] << endl; 
			if(fullDecoded[i][0]=="CIBLE")
			{
				cible = fullDecoded[i][1];
			}
		}

		//Pour la lyre  :
		if (cible == "PROJO")
		{
			int canalLyre = ADRESSEDEBUTLYRE;



			for(int i=0; i < fullDecoded.size();i=i+1)
			{
				if(fullDecoded[i][0]=="RED")
				{
					testRed = fullDecoded[i][1];
					strcpy(strValueRed, testRed.c_str());
					valueRed = atoi(strValueRed);
				}
				if(fullDecoded[i][0]=="BLUE")
				{
					testBlue = fullDecoded[i][1];
					strcpy(strValueBlue, testBlue.c_str());
					valueBlue = atoi(strValueBlue);
				}
				if(fullDecoded[i][0]=="GREEN")
				{
					testGreen = fullDecoded[i][1];
					strcpy(strValueGreen, testGreen.c_str());
					valueGreen = atoi(strValueGreen);
				}

			}
			valeurLyre[0] = valueRed;
			valeurLyre[1] = valueGreen;
			valeurLyre[2] = valueBlue;

			for(int i=0; i <=2; i=i+1)
			{
				//De base la lyre se configure quart par quart
				//ici on veut l'allumer entierement couleur par couleur
				//On fait 3 tour de boucle pour les 3 couleurs
				//et on met chaque quart a la meme valeur
				interfaceDMX->SetCanalDMX(canalLyre, valeurLyre[i]);
				interfaceDMX->SetCanalDMX(canalLyre+3, valeurLyre[i]);
				interfaceDMX->SetCanalDMX(canalLyre+6, valeurLyre[i]);
				interfaceDMX->SetCanalDMX(canalLyre+9,valeurLyre[i]);
				canalLyre = canalLyre + 1;//A chaque tour on passe a la couleur suivante en indentant les canaux
			}
			interfaceDMX->SendDMX();//on envoie la trame DMX au boitier
		}

		//Pour le projo
		else if(cible == "LYRE")
		{
			int canalProjo = ADRESSEDEBUTPROJO;

			for(int i=0; i < fullDecoded.size(); i=i+1)
			{
				if (fullDecoded[i][0]=="RED")
				{
					testRed = fullDecoded[i][1];
					strcpy(strValueRed, testRed.c_str());
					valueRed = atoi(strValueRed);
				}

				if (fullDecoded[i][0]=="BLUE")
				{
					testBlue = fullDecoded[i][1];
					strcpy(strValueBlue, testBlue.c_str());
					valueBlue = atoi(strValueBlue);
				}

				if (fullDecoded[i][0]=="GREEN")
				{
					testGreen = fullDecoded[i][1];
					strcpy(strValueGreen, testGreen.c_str());
					valueGreen = atoi(strValueGreen);
				}

				if(fullDecoded[i][0]=="INTENSITY")
				{
					testIntensity = fullDecoded[i][1];
					strcpy(strValueIntensity, testIntensity.c_str());
					valueIntensity = atoi(strValueIntensity);
				}
			}
			valeurProjo[0]=valueRed;
			valeurProjo[1]=valueGreen;
			valeurProjo[2]=valueBlue;
			valeurProjo[6]=valueIntensity;

			for(int i=0; i<=6; i=i+1)
			{
				interfaceDMX->SetCanalDMX(canalProjo, valeurProjo[i]);
				canalProjo = canalProjo + 1;
			}
			interfaceDMX->SendDMX();
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
