#include <vector>
#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <sys/types.h>

#include "fichierJSON.h"
#include "storyboard.h"
#include "DMX.h"
#include "enttecdmxusb.h"
#include "Socket.h"
#include <iostream>

using namespace std;

//On ajoute un élement au vecteur qui contient la storyboard
void storyboard::ajouterElem(elemStoryBoard elem)
{
	scene.push_back(elem);
}

//On affiche de façon ecrite la liste des élément de la story board qui vont se dérouler dans l'ordre
void storyboard::visualiserStoryBoard()
{
	int length = scene.size();
	for(int i=0; i < length; i++)
	{
		cout << "elem : " << i << " de la story board : " << endl; 
		cout << "cible = " << scene[i].getCible() << endl;
		cout << "adrdCible = " <<scene[i].getAddr() << endl;
		cout << "Red = "<<scene[i].getRed() << endl;
		cout << "Green = "<<scene[i].getGreen() << endl;
		cout << "Blue = "<<scene[i].getBlue() << endl;
		cout << "Intensity = "<<scene[i].getIntensity() << endl;
		cout << "Time = "<<scene[i].getTime() << endl;
		cout << endl;
	}
}

//On lance la storyboard, c'est à dire qu'on va jouer les élement qu'elle contient sur les projecteurs
void storyboard::lireStoryBoard(EnttecDMXUSB *interfaceDMX)
{
	if(fichierJSON::ouvrirJSON("config.json")!= NULL)
	{
		json_value* v1 = fichierJSON::ouvrirJSON("config.json");
		int NBCANAUXPROJO = atoi(fichierJSON::getName(v1, "NBCANAUXPROJO").c_str());
		int NBCANAUXLYRE = atoi(fichierJSON::getName(v1, "NBCANAUXLYRE").c_str());
		int TAILLEBUSDMX = atoi(fichierJSON::getName(v1, "TAILLEBUSDMX").c_str());
		fichierJSON::fermerJSON(v1);
		
		int valeurDMX[TAILLEBUSDMX];//tableau contenant les Valeur des 512 canaux du bus DMX
		memset(valeurDMX,0x00, TAILLEBUSDMX);
		
		int ADRESSEDEBUTLYRE;
		int ADRESSEDEBUTPROJO;
		
		DMX *projo;
		DMX *lyre;
		
		int length = scene.size();
		for(int i=0; i < length; i++)
		{
			if (scene[i].getCible() == "PROJO")
			{
				
				ADRESSEDEBUTPROJO = scene[i].getAddr();//On cherche avant tout la cible du message
				if(ADRESSEDEBUTPROJO<1 || ADRESSEDEBUTPROJO >512)
				{
					cout << "Adresse invalide, arret du traitement";
					break;
				}
				
				projo = new DMX(ADRESSEDEBUTPROJO, "PROJO");
				
				projo->remplirTab(valeurDMX, scene[i].getRed(), scene[i].getGreen(), scene[i].getBlue(), 0);//On rempli le tableau valeurDMX à partir de l'adresse du projo et des valeurs qu'on lui defini

				for(int j=ADRESSEDEBUTPROJO; j <=ADRESSEDEBUTPROJO+NBCANAUXPROJO - 1; j=j+1)
				{
					interfaceDMX->SetCanalDMX(j, valeurDMX[j]);//On prepare la trame DMX avec les valeur a envoyer au boitier DMX a partir des valeurs du tableau
				}
				interfaceDMX->SendDMX();//on envoie la trame DMX au boitier
				usleep(scene[i].getTime()*1000);
			}

			//Si la cible est la lyre
			else if(scene[i].getCible() == "LYRE")
			{
				ADRESSEDEBUTLYRE = scene[i].getAddr();//On cherche avant tout la cible du message
				if(ADRESSEDEBUTLYRE<1 || ADRESSEDEBUTLYRE >512)
				{
					cout << "Adresse invalide, arret du traitement";
					break;
				}
				lyre = new DMX(ADRESSEDEBUTLYRE, "LYRE");
				
				lyre->remplirTab(valeurDMX, scene[i].getRed(), scene[i].getGreen(), scene[i].getBlue(), scene[i].getIntensity());
				
				for(int j=ADRESSEDEBUTLYRE; j<=ADRESSEDEBUTLYRE+NBCANAUXLYRE-1; j=j+1)
				{
					cout << "j = " << j << " ; valeurDMX = " << valeurDMX[j] << endl;
					interfaceDMX->SetCanalDMX(j, valeurDMX[j]);
				}
				
				interfaceDMX->SendDMX();
				usleep(scene[i].getTime()*1000);
			}

			else//si il n'y a pas de cible ou qu'elle est incorrecte
			{
				cout<< "Cible incorrecte"<<endl;
			}
		}
	}
}