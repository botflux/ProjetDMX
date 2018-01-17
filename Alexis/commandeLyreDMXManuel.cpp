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

#include "enttecdmxusb.h"

using namespace std;

#define DMXDEVICE "/dev/ttyUSB0"

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
	cout << "Interface DMX USB PRO detectee " << std::endl << configurationDMX << std::endl ;

	//remet a 0 les 512 canaux
	interfaceDMX->ResetCanauxDMX();
	//emet les valeurs des 512 canaux
	interfaceDMX->SendDMX();

	//fixe et emet la valeur du canal 1
	int canal = 1;
	int valeur = 127;
	interfaceDMX->SetCanalDMX(canal, valeur);
	interfaceDMX->SendDMX();
	canal = 3;
	valeur = 127;
	interfaceDMX->SetCanalDMX(canal, valeur);
	interfaceDMX->SendDMX();
    }

    delete interfaceDMX;
    return 0;
}
