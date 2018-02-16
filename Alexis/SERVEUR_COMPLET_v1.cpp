//Programme serveur commande projo et lyre

#include <iostream>
#include <iomanip>
#include <cstring>
#include <vector>

#include "Socket.h"
#include "Parser.h"
#include "enttecdmxusb.h"

using namespace std;

#define DMXDEVICE "/dev/ttyUSB0"

#define PORT 5000
#define LG_MESSAGE 256
#define ADRESSEDEBUTLYRE 22
#define ADRESSEDEBUTPROJO 1
#define NBCANAUXPROJO 12
#define NBCANAUXLYRE 7
#define TAILLEBUSDMX 512

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

    if(test)//Si l'interface DMX est OK
    {
		//affiche la config
		string configurationDMX = interfaceDMX->GetConfiguration();
		cout << "Interface DMX USB PRO detectee " << std::endl << configurationDMX << std::endl;

		//On initialise le tableau des valeur a envoyer par voie DMX à 0
		int valeurDMX[TAILLEBUSDMX];//tableau contenant les Valeur des 512 canaux du bus DMX
		memset(valeurProjo, 0, TAILLEBUSDMX);

		//On crée et configure le socket en UDP avec le bon port et n'importe qu'elle interface reseau du raspberry(ici on a la clé wi fi sur un réseau et la carte reseau sur un autre)
		Socket *mySocket = new Socket();
		mysocket->setDescription(PF_INET,SOCK_DGRAM,0);
		mySocket->setPdrLocal(PF_INET, htons(PORT), htonl(INADDDR_ANY));
		mySocket->bindSocket();

		vector<scene> storyboard;//On prepare le tableau dynamique qui contiendra les storyboard

		char message[LG_MESSAGE];//On initialise le message que le serveur va recevoir

		while(1)
		{
			memset(message, 0x00, LG_MESSAGE*sizeof(char));//On le remet à 0 à chaque fois
			//On attend un ordre d'un client
			mySocket->setLus(recvfrom(mySocket->getDescripteur(), message, sizeof(message),0,(struct sockaddr*)mySocket->getPdrDistant(), mySocket->getLgAddr2()));
			//On affiche le message recu
			cout << "Message recu : " << message << endl;
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

			int valueRed=0;
			int valueGreen=0;
			int valueBlue=0;
			int valueIntensity=0;

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
			if (cible == "PROJO")
			{
			for(int i=0; i < fullDecoded.size();i=i+1)
				{
					if(fullDecoded[i][0]=="RED")
					{
						valueRed = atoi(fullDecoded[i][1].c_str());
					}
					if(fullDecoded[i][0]=="BLUE")
					{
						valueBlue = atoi(fullDecoded[i][1].c_str());
					}
					if(fullDecoded[i][0]=="GREEN")
					{
						valueGreen = atoi(fullDecoded[i][1].c_str());
					}
				}
				for(int i=0; i<=3;i=i+1)//La lyre est divisée en 4 quart, on veut allumer la barre entierrement donc on fait une boucle qui allume chaque quart
				{
					valeurDMX[ADRESSEDEBUTPROJO + (i*3)] = valueRed;
					valeurDMX[ADRESSEDEBUTPROJO + 1 + (i*3)] = valueGreen;
					valeurDMX[ADRESSEDEBUTPROJO + 2 + (i*3)] = valueBlue;
				}

				for(int i=ADRESSEDEBUTPROJO; i <=ADRESSEDEBUTPROJO+NBCANAUXPROJO - 1; i=i+1)
				{
					interfaceDMX->SetCanalDMX(i, valeurDMX[i]);
				}
				interfaceDMX->SendDMX();//on envoie la trame DMX au boitier
			}

			//Si la cible est la lyre
			else if(cible == "LYRE")
			{
				for(int i=0; i < fullDecoded.size(); i=i+1)
				{
					if (fullDecoded[i][0]=="RED")
					{
						valueRed = atoi(fullDecoded[i][1].c_str());
					}

					if (fullDecoded[i][0]=="BLUE")
					{
						valueBlue = atoi(fullDecoded[i][1].c_str());
					}

					if (fullDecoded[i][0]=="GREEN")
					{
						valueGreen = atoi(fullDecoded[i][1].c_str());
					}

					if(fullDecoded[i][0]=="INTENSITY")
					{
						valueIntensity = atoi(fullDecoded[i][1].c_str());
					}
				}
				valeurDMX[ADRESSEDEBUTLYRE]=valueRed;
				valeurDMX[ADRESSEDEBUTLYRE+1]=valueGreen;
				valeurDMX[ADRESSEDEBUTLYRE+2]=valueBlue;
				valeurDMX[ADRESSEDEBUTLYRE+6]=valueIntensity;

				for(int i=ADRESSEDEBUTLYRE; i<=ADRESSEDEBUTLYRE+NBCANAUXLYRE -1; i=i+1)
				{
					interfaceDMX->SetCanalDMX(i, valeurProjo[i]);
				}
				interfaceDMX->SendDMX();
			}
			
			else//si il n'y a pas de cible ou qu'elle est incorrecte
			{
				cout<< "Cible incorrecte"<<endl;
			}
		}
		close(descripteurSocket);
    }
    delete interfaceDMX;
    return 0;
}
