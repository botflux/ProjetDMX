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

#include <stdio.h>
#include <stdlib.h>

#include "Socket.h"
#include "Parser.h"
#include "enttecdmxusb.h"

using namespace std;

#define DMXDEVICE "/dev/ttyUSB0"

const int PORT = 5000;
#define LG_MESSAGE 256
const int ADRESSEDEBUTLYRE = 22;
const string ADDRLYRE = "22";
const int ADRESSEDEBUTPROJO = 1;
const string ADDRPROJO = "1";
const int NBCANAUXPROJO = 12;
const int NBCANAUXLYRE = 7;
const int TAILLEBUSDMX = 512;

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
	//Interface DMX
    cout << "Peripherique : " << DMXDEVICE << endl;
    EnttecDMXUSB *interfaceDMX;
    interfaceDMX = new EnttecDMXUSB(DMX_USB_PRO, DMXDEVICE);
    bool test = interfaceDMX->IsAvailable();
    cout << "Hello world\n";
    if(test)//Si l'interface DMX est OK
    {
		//affiche la config
		string configurationDMX = interfaceDMX->GetConfiguration();
		cout << "Interface DMX USB PRO detectee " << std::endl << configurationDMX << std::endl;

		//On initialise le tableau des valeur a envoyer par voie DMX à 0
		int valeurDMX[TAILLEBUSDMX];//tableau contenant les Valeur des 512 canaux du bus DMX
		memset(valeurDMX,0x00, TAILLEBUSDMX);

		//On crée et configure le socket en UDP avec le bon port et n'importe qu'elle interface reseau du raspberry(ici on a la clé wi fi sur un réseau et la carte reseau sur un autre)
		Socket *mySocket = new Socket();
		mySocket->setDescripteur(PF_INET,SOCK_DGRAM,0);
		mySocket->setPdrLocal(PF_INET, htons(PORT), htonl(INADDR_ANY));
		mySocket->bindSocket();

		vector<scene> storyboard;//On prepare le tableau dynamique qui contiendra les storyboard

		char message[LG_MESSAGE];//On initialise le message que le serveur va recevoir
		
		while(1)
		{
			memset(message, 0x00, LG_MESSAGE*sizeof(char));//On le remet à 0 à chaque fois
			//memset(reponse, 0x00, LG_MESSAGE*sizeof(char));
			//On attend un ordre d'un client
			mySocket->setLus(recvfrom(mySocket->getDescripteur(), message, sizeof(message),0,(struct sockaddr*)mySocket->getPdrDistant(), mySocket->getLgAddr2()));
			//On affiche le message recu
			cout << "Message recu : " << message << endl;
			mySocket->setPdrDistant(0, htons(15000), 0);
			mySocket->affichePdrDistant();
			
			string m = message;

			//On creer un parser et on l'utilise pour decoder le message recu
			Parser *p = new Parser();
			vector<string> vectorStr = p->decode(m,';');
			vector< vector<string> > fullDecoded;

			for(int i = 0; i < vectorStr.size(); i=i+1)
			{
				vector<string> tempo = p->decode(vectorStr[i],'=');
				fullDecoded.push_back(tempo);
			}
			string cible = "";

			//On rempli un tableau en 2 dimension avec les information decodées du message qu'on trie
			for(int i=0; i< fullDecoded.size(); i=i+1)
			{
				cout <<"Nom : " << fullDecoded[i][0]  <<"\tValeur : " << fullDecoded[i][1] << endl; 
				if(fullDecoded[i][0]=="CIBLE")
				{
					cible = fullDecoded[i][1];//On cherche avant tout la cible du message
				}
			}
			//Si la cible est le projo
			if (cible == ADDRPROJO)
			{
				mySocket->sendACK(true);
				for(int i=0; i < fullDecoded.size();i=i+1)
				{
					mySocket->remplirTab(valeurDMX, ADRESSEDEBUTPROJO, "PROJO", fullDecoded[i][0], fullDecoded[i][1]);
				}

				for(int i=ADRESSEDEBUTPROJO; i <=ADRESSEDEBUTPROJO+NBCANAUXPROJO - 1; i=i+1)
				{
					interfaceDMX->SetCanalDMX(i, valeurDMX[i]);
				}
				interfaceDMX->SendDMX();//on envoie la trame DMX au boitier
			}

			//Si la cible est la lyre
			else if(cible == ADDRLYRE)
			{
				mySocket->sendACK(true);
				for(int i=0; i < fullDecoded.size();i=i+1)
				{
					mySocket->remplirTab(valeurDMX, ADRESSEDEBUTLYRE, "LYRE", fullDecoded[i][0], fullDecoded[i][1]);
				}
				
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
		}
		mySocket->closeDescripteur();
    }
    delete interfaceDMX;
    return 0;
}
