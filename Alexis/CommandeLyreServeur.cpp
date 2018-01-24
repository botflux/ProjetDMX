/*
 * Programme de test de l'interface Enttec DMX USB PRO
 * (c) 2009 tv
 */

#include <iostream>
#include <iomanip>
#include <string>

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
#define ADRESSEDEBUTPROJO 15

int main(int argc, char *argv[])
{
   /* if(argc != 2)
    {
        printf("Usage : %s peripherique\n", argv[0]);
        printf("Exemple : %s /dev/ttyUSB0\n", argv[0]);
        return 1;
    }*/
    printf("Peripherique : %s\n\n", DMXDEVICE);

/*    EnttecDMXUSB interfaceDMX(DMX_USB_PRO, DMXDEVICE);
    //EnttecDMXUSB interfaceDMX(DMX_USB_PRO, argv[1]);
    string configurationDMX;

    if(interfaceDMX.IsAvailable())
    {
        configurationDMX = interfaceDMX.GetConfiguration();
        cout << "Interface " << interfaceDMX.GetNomInterface() << " detectee" << std::endl << configurationDMX << std::endl;
    }
    else
        cout << "Interface non detectee !" << endl;
*/

    EnttecDMXUSB *interfaceDMX;

    interfaceDMX = new EnttecDMXUSB(DMX_USB_PRO, DMXDEVICE);

    string configurationDMX;

    if(interfaceDMX->IsAvailable())//Si l'interface DMX est OK
    {
	//affiche la config
	configurationDMX = interfaceDMX->GetConfiguration();
	cout << "Interface DMX USB PRO detectee " << std::endl << configurationDMX << std::endl;
	int valeur[512];
	int i;
	memset(valeur, 0, 512);

	int descripteurSocket;
	struct sockaddr_in pointDeRencontreLocal;
	struct sockaddr_in pointDeRencontreDistant;
	socklen_t longueurAdresse;
	char messageRecu[LG_MESSAGE]="";
	int lus;
	int retour;

	descripteurSocket = socket(PF_INET, SOCK_DGRAM, 0);

	if(descripteurSocket <0) //echec
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

	memset(messageRecu, 0x00, LG_MESSAGE*sizeof(char));


	//char *pch;
	int j;

	while(1)
	{
		memset(messageRecu, 0x00, LG_MESSAGE*sizeof(char));
		//On attend un ordre d'un client
		lus = recvfrom(descripteurSocket, messageRecu, sizeof(messageRecu),0,(struct sockaddr*)&pointDeRencontreDistant, &longueurAdresse);

		printf("Message %s reçu avec succes (%d octets)\n\n", messageRecu, lus);

		char *pch = strtok(messageRecu,";");
		cout<<"cible : "<<pch<<endl;
		string cible = pch;
		j=0;
		pch = strtok(NULL,";");
		while(pch!= NULL)
		{
			valeur[j] = atoi(pch);
			pch = strtok(NULL,";");
			//cout << "test numero : " << j << " apres premiere ligne"<< endl;
			//cout << pch <<  endl;
			//valeur[j] = atoi(pch);
			//cout <<"test apres deuxieme ligne"<<endl;
			j=j+1;
		}

		//fixe et emet la valeur des canaux
		//valeur[0] = atoi( messageRecu);


		//Pour la lyre  :
		if (cible == "LYRE")
		{
		for(i=0; i <=2; i=i+1)
		{
			interfaceDMX->SetCanalDMX(i+1, valeur[i]);
			interfaceDMX->SetCanalDMX(i+4, valeur[i]);
			interfaceDMX->SetCanalDMX(i+7, valeur[i]);
			interfaceDMX->SetCanalDMX(i+10,valeur[i]);
		}
		interfaceDMX->SendDMX();
		}

		//Pour le projo
		if(cible == "PROJO");
		{
		int canalProjo = ADRESSEDEBUTPROJO;

		for (i=0; i <=6; i=i+1)
		{
			interfaceDMX->SetCanalDMX(canalProjo, valeur[i]);
			canalProjo = canalProjo +1;
		}
		interfaceDMX->SendDMX();
		}

		if(cible != "PROJO" && cible !="LYRE")
		{
			cout<< "Cible incorrecte"<<endl;
		}

	}
	close(descripteurSocket);
    }
    delete interfaceDMX;
    return 0;
}
