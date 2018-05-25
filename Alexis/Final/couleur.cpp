#include "fichierJSON.h"
#include "storyboard.h"
#include "DMX.h"
#include "enttecdmxusb.h"
#include "Socket.h"
#include "couleur.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <iostream>

using namespace std;

void couleur::setCouleurProjo(EnttecDMXUSB *interfaceDMX, DMX *projo, json_value *value, int valeurDMX[])
{
	json_value* vConfig = fichierJSON::ouvrirJSON("config.json");
	int NBCANAUXPROJO = atoi(fichierJSON::getName(vConfig, "NBCANAUXPROJO").c_str());
	fichierJSON::fermerJSON(vConfig);
	
	int ADRESSEDEBUTPROJO;	
	
	ADRESSEDEBUTPROJO = atoi(fichierJSON::getName(value->u.object.values[0].value, "ADDRCIBLE").c_str());//On cherche avant tout la cible du message
	if(ADRESSEDEBUTPROJO<1 || ADRESSEDEBUTPROJO >512)
	{
		cout << "Adresse invalide, arret du traitement";
	}
	
	projo = new DMX(ADRESSEDEBUTPROJO, "PROJO");
	
	int red = atoi(fichierJSON::getName(value->u.object.values[0].value, "RED").c_str());
	if(red<0 || red >255)
	{
		red = 0;
		cout <<" Valeur red invalide, mise par defaut à 0" << endl;
	}
	int green = atoi(fichierJSON::getName(value->u.object.values[0].value, "GREEN").c_str());
	if(green<0 || green >255)
	{
		green =0;
		cout <<" Valeur green invalide, mise par defaut à 0" << endl;
	}
	int blue = atoi(fichierJSON::getName(value->u.object.values[0].value, "BLUE").c_str());
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

void couleur::setCouleurLyre(EnttecDMXUSB *interfaceDMX, DMX *lyre, json_value *value, int valeurDMX[])
{
	json_value* vConfig = fichierJSON::ouvrirJSON("config.json");
	int NBCANAUXLYRE = atoi(fichierJSON::getName(vConfig, "NBCANAUXLYRE").c_str());
	fichierJSON::fermerJSON(vConfig);
	
	int ADRESSEDEBUTLYRE;
	
	ADRESSEDEBUTLYRE = atoi(fichierJSON::getName(value->u.object.values[0].value, "ADDRCIBLE").c_str());
	if(ADRESSEDEBUTLYRE<1 || ADRESSEDEBUTLYRE >512)
	{
		cout << "Adresse invalide, arret du traitement";
	}
	lyre = new DMX(ADRESSEDEBUTLYRE, "LYRE");
	
	int red = atoi(fichierJSON::getName(value->u.object.values[0].value, "RED").c_str());
	if(red<0 || red >255)
	{
		red = 0;
		cout <<" Valeur red invalide, mise par defaut à 0" << endl;
	}
	int green = atoi(fichierJSON::getName(value->u.object.values[0].value, "GREEN").c_str());
	if(green<0 || green >255)
	{
		green = 0;
		cout <<" Valeur green invalide, mise par defaut à 0" << endl;
	}
	int blue = atoi(fichierJSON::getName(value->u.object.values[0].value, "BLUE").c_str());
	if(blue<0 || blue >255)
	{
		blue = 0;
		cout <<" Valeur blue invalide, mise par defaut à 0" << endl;
	}
	int intensity = atoi(fichierJSON::getName(value->u.object.values[0].value, "INTENSITY").c_str());
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
