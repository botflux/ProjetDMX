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
	static void start(Socket *mySocket, EnttecDMXUSB *interfaceDMX, int *accesFichier)
	{
		if(fichierJSON::ouvrirJSON("config.json")!=NULL)
		{
			int TAILLEBUSDMX = 512;
			json_value* value = fichierJSON::ouvrirJSON("config.json");
			if (value != NULL)
			{
				TAILLEBUSDMX = atoi(fichierJSON::getName(value, "TAILLEBUSDMX").c_str());
			}
			fichierJSON::fermerJSON(value);

			//On initialise le tableau contenant les 512 valeur du bus dmx à 0
			int valeurDMX[TAILLEBUSDMX];
			memset(valeurDMX,0x00, TAILLEBUSDMX);
			
			string n;
			
			storyboard *scene;
			scene = new storyboard();
			while (1)
			{
				if (*accesFichier == 0)
				{
					if(fichierJSON::ouvrirJSON("test.json")!=NULL)
					{
						//Lire et exploiter ce fichier JSON
						json_value* value = fichierJSON::ouvrirJSON("test.json");
						
						//On regarde si c'est une storyboard ou un changement de couleur qui a été envoyé
						n = value->u.object.values[0].name;
						cout << "n = " << n << endl;
						//scene = new storyboard();
						//Si c'est une storyboard :
						if (n == "storyboard")
						{
							//On crée un vecteur scene qui contiendra chaque élement de la storyboard
							//On crée un objet element de story board que l'on ajoutera au vecteur storyboard
							
							elemStoryBoard *elem1 = new elemStoryBoard();
							
							//On recupere le nombre d'element présent dans le fichier json(nombre de sequence de la storyboard)
							//value correspond au fichier entier
							//value->u.object.values[0].value correspond au premier element du fichier, ici : "storyboard"
							//value->u.object.values[0].value->u.object.values[1].value correspond au deuxieme elemnt present dans le premier element, ici "elements"
							//ici c'est un tableau donc applique u.array.length pour recuperer le nombre d'elemnt dans ce tableau
							//value->u.object.values[0].value->u.object.values[1].value->u.array.length
							
							int length = value->u.object.values[0].value->u.object.values[0].value->u.array.length;
							
							cout << "\n length = "<< length << endl;
							
							//Pour chaque elements du json, l'elemnt storyboard recupere la cible, son adresse, les differentes valeures voulues
							//et le temp qu'il doit durer
							//A la fin de chaque boucle on ajoute l'element de la storyboard dans le vecteur storyboard
							for (int a = 0; a < length; a++) 
							{
								elem1->setCible(fichierJSON::getName(value->u.object.values[0].value->u.object.values[0].value->u.array.values[a],"target"));
								cout << "target";
								elem1->setAddr(fichierJSON::getNameInt(value->u.object.values[0].value->u.object.values[0].value->u.array.values[a],"targetAddress"));
								cout << "targetAddr";
								elem1->setBlue(fichierJSON::getNameInt(value->u.object.values[0].value->u.object.values[0].value->u.array.values[a],"blue"));
								cout << "blue";
								elem1->setRed(fichierJSON::getNameInt(value->u.object.values[0].value->u.object.values[0].value->u.array.values[a],"red"));
								cout << "red";
								elem1->setGreen(fichierJSON::getNameInt(value->u.object.values[0].value->u.object.values[0].value->u.array.values[a],"green"));
								cout << "green";
								elem1->setWhite(fichierJSON::getNameInt(value->u.object.values[0].value->u.object.values[0].value->u.array.values[a],"white"));
								cout << "intensity";
								elem1->setTime(fichierJSON::getNameInt(value->u.object.values[0].value->u.object.values[0].value->u.array.values[a],"time"));
								cout << "time";

								scene->ajouterElem(*elem1);
							}
							scene->visualiserStoryBoard();
							delete elem1;
						}
						
						else if (n == "couleur")
						{
							cout << "--------------test-----------------" << endl;
							string cible = fichierJSON::getName(value->u.object.values[0].value, "target");
							cout << "Cible = " << cible << endl;
							
							if (cible == "PROJO" || cible == "LYRE")
							{
								couleur::setCouleur(interfaceDMX, cible, value);
							}
						}
						fichierJSON::fermerJSON(value);
						*accesFichier = 1;
					}
				}
				//On joue la sotryboard apres avoir fermé le fichier et libéré son acces pour le threadSocket
				if (n == "storyboard")
				{
					scene->lireStoryBoard(interfaceDMX);
				}
				
			}
			delete scene;
		}
		
	}
	
};