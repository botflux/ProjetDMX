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

#define PORT IPPORT_USERRESERVED
#define LG_MESSAGE 256


int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        printf("Usage : %s peripherique\n", argv[0]);
        printf("Exemple : %s /dev/ttyUSB0\n", argv[0]);
        return 1;
    }
    printf("Peripherique : %s\n\n", argv[1]);

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
	char messageRecu[LG_MESSAGE];
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



	while(1)
	{
		//On attend un ordre d'un client
		lus = recvfrom(descripteurSocket, messageRecu, sizeof(messageRecu),0,(struct sockaddr*)&pointDeRencontreDistant, &longueurAdresse);

		printf("Message %s reçu avec succes (%d octets)\n\n", messageRecu, lus);

		//fixe et emet la valeur des canaux
		valeur[0] = (int)lus;
		
		for(i=0; i <=511; i=i+1)
		{
			interfaceDMX->SetCanalDMX(i+1, valeur[i]);
		}
		interfaceDMX->SendDMX();
	}
	close(descripteurSocket);
    }
    delete interfaceDMX;
    return 0;
}
