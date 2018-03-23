//Programme serveur commande projo et lyre

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <sys/types.h>

#include <cstdio>
#include <cstdlib>

#include "Socket.h"
#include "DMX.h"
#include "enttecdmxusb.h"
#include "json.h"
#include "fichierJSON.h"

using namespace std;

#define DMXDEVICE "/dev/ttyUSB0"
#define LG_MESSAGE 1024

const int NBCANAUXPROJO = 12;
const int NBCANAUXLYRE = 7;
const int TAILLEBUSDMX = 512;

int main()
{
	//Application de la config des projo et des ports
	json_value* value = fichierJSON::ouvrirJSON("config.json");
	
	int PORTDISTANT;
	int PORT;
	
	int NBCANAUXPROJO;
	int NBCANAUXLYRE;
	int TAILLEBUSDMX;
	
	fichierJSON::setConfig(value, PORT, PORTDISTANT, NBCANAUXPROJO, NBCANAUXLYRE, TAILLEBUSDMX);
	
	cout << "Port serveur : " << PORT << endl;
	cout << "Port client : " << PORTDISTANT << endl;
	
	fichierJSON::fermerJSON(value);
	
	//delete value;
	
	//Interface DMX
    cout << "Peripherique : " << DMXDEVICE << endl;
    EnttecDMXUSB *interfaceDMX;
    interfaceDMX = new EnttecDMXUSB(DMX_USB_PRO, DMXDEVICE);
    bool test = interfaceDMX->IsAvailable();
	
    if(test)//Si l'interface DMX est OK
    {
		//On initialise le tableau des valeur a envoyer par voie DMX à 0
		int valeurDMX[TAILLEBUSDMX];//tableau contenant les Valeur des 512 canaux du bus DMX
		memset(valeurDMX,0x00, TAILLEBUSDMX);

		//On crée et configure le socket en UDP avec le bon port et n'importe qu'elle interface reseau du raspberry(ici on a la clé wi fi sur un réseau et la carte reseau sur un autre)
		Socket *mySocket = new Socket();
		mySocket->setDescripteur(PF_INET,SOCK_DGRAM,0);
		mySocket->setPdrLocal(PF_INET, htons(PORT), htonl(INADDR_ANY));
		mySocket->bindSocket();

		//vector<scene> storyboard;//On prepare le tableau dynamique qui contiendra les storyboard

		char message[LG_MESSAGE];//On initialise le message que le serveur va recevoir
		
		int ADRESSEDEBUTLYRE;
		int ADRESSEDEBUTPROJO;
		
		DMX *projo;
		DMX *lyre;
		
		while(1)
		{
			memset(message, 0x00, LG_MESSAGE*sizeof(char));//On le remet à 0 à chaque fois
			//memset(reponse, 0x00, LG_MESSAGE*sizeof(char));
			//On attend un ordre d'un client
			mySocket->setLus(recvfrom(mySocket->getDescripteur(), message, sizeof(message),0,(struct sockaddr*)mySocket->getPdrDistant(), mySocket->getLgAddr2()));
			//On affiche le message recu
			cout << "Message recu : " << message << endl;
			mySocket->setPdrDistant(0, htons(PORTDISTANT), 0);
			//Stocker le message dans un fichier json
			string m = message;
			FILE* fichier = NULL;
			fichier = fopen("trame.json", "w");
			fprintf(fichier, m.c_str());
			fclose(fichier);
			

			//Lire et exploiter ce fichier JSON
			json_value* value = fichierJSON::ouvrirJSON("trame.json");
			
			string cible = fichierJSON::getName(value, "CIBLE");
			
			cout << "Cible = " << cible << endl;

			//Si la cible est le projo
			if (cible == "PROJO")
			{
				
				ADRESSEDEBUTPROJO = atoi(fichierJSON::getName(value, "ADDRCIBLE").c_str());//On cherche avant tout la cible du message
				if(ADRESSEDEBUTPROJO<1 || ADRESSEDEBUTPROJO >512)
				{
					cout << "Adresse invalide, arret du traitement";
					break;
				}
				
				projo = new DMX(ADRESSEDEBUTPROJO, "PROJO");
					
				mySocket->sendACK(true); //On envoie un accusé de réception au client pour le prévenir que le message a bien été reçu
				
				int red = atoi(fichierJSON::getName(value, "RED").c_str());
				if(red<0 || red >255)
				{
					red = 0;
					cout <<" Valeur red invalide, mise par defaut à 0" << endl;
				}
				int green = atoi(fichierJSON::getName(value, "GREEN").c_str());
				if(green<0 || green >255)
				{
					green =0;
					cout <<" Valeur green invalide, mise par defaut à 0" << endl;
				}
				int blue = atoi(fichierJSON::getName(value, "BLUE").c_str());
				if(blue<0 || blue >255)
				{
					blue = 0;
					cout <<" Valeur blue invalide, mise par defaut à 0" << endl;
				}
				
				projo->remplirTab(valeurDMX, red, green, blue, 0);//On rempli le tableau valeurDMX à partir de l'adresse du projo et des valeurs qu'on lui defini

				for(int i=ADRESSEDEBUTPROJO; i <=ADRESSEDEBUTPROJO+NBCANAUXPROJO - 1; i=i+1)
				{
					interfaceDMX->SetCanalDMX(i, valeurDMX[i]);//On prepare la trame DMX avec les valeur a envoyer au boitier DMX a partir des valeurs du tableau
				}
				interfaceDMX->SendDMX();//on envoie la trame DMX au boitier
			}

			//Si la cible est la lyre
			else if(cible == "LYRE")
			{
				ADRESSEDEBUTLYRE = atoi(fichierJSON::getName(value, "ADDRCIBLE").c_str());
				if(ADRESSEDEBUTLYRE<1 || ADRESSEDEBUTLYRE >512)
				{
					cout << "Adresse invalide, arret du traitement";
					break;
				}
				lyre = new DMX(ADRESSEDEBUTLYRE, "LYRE");
				
				mySocket->sendACK(true);
				
				int red = atoi(fichierJSON::getName(value, "RED").c_str());
				if(red<0 || red >255)
				{
					red = 0;
					cout <<" Valeur red invalide, mise par defaut à 0" << endl;
				}
				int green = atoi(fichierJSON::getName(value, "GREEN").c_str());
				if(green<0 || green >255)
				{
					green = 0;
					cout <<" Valeur green invalide, mise par defaut à 0" << endl;
				}
				int blue = atoi(fichierJSON::getName(value, "BLUE").c_str());
				if(blue<0 || blue >255)
				{
					blue = 0;
					cout <<" Valeur blue invalide, mise par defaut à 0" << endl;
				}
				int intensity = atoi(fichierJSON::getName(value, "INTENSITY").c_str());
				if(intensity<0 || intensity >255)
				{
					intensity = 0;
					cout <<" Valeur intensity invalide, mise par defaut à 0" << endl;
				}
				
				lyre->remplirTab(valeurDMX, red, green, blue, intensity);
				
				for(int i=ADRESSEDEBUTLYRE; i<=ADRESSEDEBUTLYRE+NBCANAUXLYRE-1; i=i+1)
				{
					interfaceDMX->SetCanalDMX(i, valeurDMX[i]);
				}
				
				interfaceDMX->SendDMX();
			}

			else//si il n'y a pas de cible ou qu'elle est incorrecte
			{
				cout<< "Cible incorrecte"<<endl;
				mySocket->sendACK(false);
			}
			fichierJSON::fermerJSON(value);
		}
		mySocket->closeDescripteur();
		delete mySocket;
		delete projo;
		delete lyre;
    }
    delete interfaceDMX;
	
	return 0;
}
