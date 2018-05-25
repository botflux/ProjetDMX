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
	static void start(Socket *mySocket, int *accesFichier)
	{
		while (1)
		{
			char message[LG_MESSAGE];//On initialise le message que le serveur va recevoir
			memset(message, 0x00, LG_MESSAGE*sizeof(char));//On le remet à 0 à chaque fois
			
			//On attend un ordre d'un client
			mySocket->setLus(recvfrom(mySocket->getDescripteur(), message, sizeof(message),0,(struct sockaddr*)mySocket->getPdrDistant(), mySocket->getLgAddr2()));
			cout << "message recu : " << message << endl;
			string str = message;
			
			FILE* fichier = NULL;
			fichier = fopen("temp.json", "w");
			fprintf(fichier, str.c_str());
			fclose(fichier);
			
			json_value* value1 = fichierJSON::ouvrirJSON("temp.json");
			bool isFragmented = fichierJSON::getNameBool(value1, "isFragmented");
			if (isFragmented == false)
			{
				while(*accesFichier != 1)
				{
				}
				if (*accesFichier == 1) //Si le fichier est accessible
				{
					string m = fichierJSON::getName(value1, "data");
					FILE* fichier = NULL;
					fichier = fopen("test.json", "w");
					fprintf(fichier, m.c_str());
					fclose(fichier);
					
					json_value* value = fichierJSON::ouvrirJSON("test.json");
					if (value != NULL)//Si le fichier n'est pas vide
					{
						string n = value->u.object.values[0].name;
						if (n == "storyboard" || n == "couleur")//On regarde si c'est une storyboard ou un changement de couleur qui a été envoyé
						{
							mySocket->sendACK("0");//Si c'est le cas alors on renvoie au client que le message reçu est correcte
						}
						else//Si ce n'est pas l'un des 2 alors on renvoie un mesasge d'erreur au client
						{
							mySocket->sendACK("-3");
						}
					}
					else
					{
						mySocket->sendACK("-2");
					}
					fichierJSON::fermerJSON(value);
					*accesFichier = 0;//Le fichier sera accessible par le thread DMX
				}
				fichierJSON::fermerJSON(value1);
			}
			else
			{
				string strData = fichierJSON::getName(value1, "data");
				int nbPacket = fichierJSON::getNameInt(value1, "packetCount");
				fichierJSON::fermerJSON(value1);
				
				for (int i =1; i< nbPacket; i++)
				{
					memset(message, 0x00, LG_MESSAGE*sizeof(char));//On le remet à 0 à chaque fois
					mySocket->setLus(recvfrom(mySocket->getDescripteur(), message, sizeof(message),0,(struct sockaddr*)mySocket->getPdrDistant(), mySocket->getLgAddr2()));
					cout << "message recu : " << message << endl;
					str = message;
					cout << str << endl;
					
					FILE* fichier = NULL;
					fichier = fopen("temp.json", "w");
					fprintf(fichier, str.c_str());
					fclose(fichier);
					
					json_value* value1 = fichierJSON::ouvrirJSON("temp.json");
					if (i ==  fichierJSON::getNameInt(value1, "index"))
					{
						string strTemp = fichierJSON::getName(value1, "data");
						strData.append(strTemp);
						fichierJSON::fermerJSON(value1);		
					}
					else
					{
						mySocket->sendACK("-1");
						fichierJSON::fermerJSON(value1);		
						break;
					}
					cout <<" ----------- \n" <<strData << "--------------- \n" << endl;
				}
				
				//Stocker le message dans un fichier json
				while(*accesFichier != 1)
				{
				}
				if (*accesFichier == 1) //Si le fichier est accessible
				{
					cout << "--------------------------------------" << endl;
					cout << strData << endl;
					cout << "--------------------------------------" << endl;
					FILE* fichier = NULL;
					fichier = fopen("test.json", "w");
					fprintf(fichier, strData.c_str());
					fclose(fichier);
					
					json_value* value = fichierJSON::ouvrirJSON("test.json");
					
					if (value != NULL)//Si le fichier n'est pas vide
					{
						string n = value->u.object.values[0].name;
						if (n == "storyboard" || n == "couleur")//On regarde si c'est une storyboard ou un changement de couleur qui a été envoyé
						{
							mySocket->sendACK("0");//Si c'est le cas alors on renvoie au client que le message reçu est correcte
						}
						else//Si ce n'est pas l'un des 2 alors on renvoie un mesasge d'erreur au client
						{
							mySocket->sendACK("-3");
						}
					}
					else
					{
						mySocket->sendACK("-2");
					}
					fichierJSON::fermerJSON(value);
					
					*accesFichier = 0;//Le fichier sera accessible par le thread DMX
				}
			}
		}
	}
};