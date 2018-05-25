#include <iostream>
#include <iomanip>
#include <string>

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <sys/types.h>

#include <cstdio>
#include <cstdlib>

class threadDMX
{
public:
	static void start(Socket *mySocket, EnttecDMXUSB *interfaceDMX)
	{
		if(fichierJSON::ouvrirJSON("config.json")!=NULL)
		{
			json_value* value = fichierJSON::ouvrirJSON("config.json");
			int TAILLEBUSDMX = atoi(fichierJSON::getName(value, "TAILLEBUSDMX").c_str());
			fichierJSON::fermerJSON(value);

			//On initialise le tableau contenant les 512 valeur du bus dmx à 0
			int valeurDMX[TAILLEBUSDMX];
			memset(valeurDMX,0x00, TAILLEBUSDMX);

			DMX *projo;
			DMX *lyre;
			
			while (1)
			{
				if(fichierJSON::ouvrirJSON("test.json")!=NULL)
				{
					//Lire et exploiter ce fichier JSON
					json_value* value = fichierJSON::ouvrirJSON("test.json");
					
					//On regarde si c'est une storyboard ou un changement de couleur qui a été envoyé
					string n = value->u.object.values[0].name;
					
					//Si c'est une storyboard :
					if (n == "storyboard")
					{
						//On crée un vecteur scene qui contiendra chaque élement de la storyboard
						//On crée un objet element de story board que l'on ajoutera au vecteur storyboard
						storyboard *scene = new storyboard();
						elemStoryBoard *elem1 = new elemStoryBoard();
						
						//On recupere le nombre d'element présent dans le fichier json(nombre de sequence de la storyboard)
						//value correspond au fichier entier
						//value->u.object.values[0].value correspond au premier element du fichier, ici : "storyboard"
						//value->u.object.values[0].value->u.object.values[1].value correspond au deuxieme elemnt present dans le premier element, ici "elements"
						//ici c'est un tableau donc applique u.array.length pour recuperer le nombre d'elemnt dans ce tableau
						//value->u.object.values[0].value->u.object.values[1].value->u.array.length
						
						int length = value->u.object.values[0].value->u.object.values[1].value->u.array.length;
						
						cout << "\n length = "<< length << endl;
						
						//Pour chaque elements du json, l'elemnt storyboard recupere la cible, son adresse, les differentes valeures voulues, et le temp qu'il doit durer
						//A la fin de chaque boucle on ajoute l'element de la storyboard dans le vecteur storyboard
						for (int a = 0; a < length; a++) 
						{
							elem1->setCible(fichierJSON::getName(value->u.object.values[0].value->u.object.values[1].value->u.array.values[a],"CIBLE"));
							elem1->setAddr(atoi(fichierJSON::getName(value->u.object.values[0].value->u.object.values[1].value->u.array.values[a],"ADDRCIBLE").c_str()));
							elem1->setBlue(atoi(fichierJSON::getName(value->u.object.values[0].value->u.object.values[1].value->u.array.values[a],"BLUE").c_str()));
							elem1->setRed(atoi(fichierJSON::getName(value->u.object.values[0].value->u.object.values[1].value->u.array.values[a],"RED").c_str()));
							elem1->setGreen(atoi(fichierJSON::getName(value->u.object.values[0].value->u.object.values[1].value->u.array.values[a],"GREEN").c_str()));
							elem1->setIntensity(atoi(fichierJSON::getName(value->u.object.values[0].value->u.object.values[1].value->u.array.values[a],"INTENSITY").c_str()));
							elem1->setTime(atoi(fichierJSON::getName(value->u.object.values[0].value->u.object.values[1].value->u.array.values[a],"TIME").c_str()));
							scene->ajouterElem(*elem1);
						}
						scene->visualiserStoryBoard();
						scene->lireStoryBoard(interfaceDMX);
						
						delete elem1;
						delete scene;
					}
					else if (n == "couleur")
					{
						string cible = fichierJSON::getName(value->u.object.values[0].value, "CIBLE");
						cout << "Cible = " << cible << endl;

						//Si la cible est le projo
						if (cible == "PROJO")
						{
							couleur::setCouleurProjo(interfaceDMX, projo, value, valeurDMX);
						}

						//Si la cible est la lyre
						else if(cible == "LYRE")
						{
							couleur::setCouleurLyre(interfaceDMX, projo, value, valeurDMX);
						}

						else//si il n'y a pas de cible ou qu'elle est incorrecte
						{
							cout<< "Cible incorrecte"<<endl;
						}
						fichierJSON::fermerJSON(value);
					}
				}
			}
			delete projo;
			delete lyre;
		}
		
	}
	
};