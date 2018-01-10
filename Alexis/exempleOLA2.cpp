#include <stdlib.h>
#include <unistd.h>
#include <ola/DmxBuffer.h>
#include <ola/Logging.h>
#include <ola/client/StreamingClient.h>
#include <iostream>

using std::cout;
using std::endl;

int main(int, char *[]) 
{
	unsigned int universe = 1; // varaible utilisée pour envoyer des donnée

	ola::InitLogging(ola::OLA_LOG_WARN, ola::OLA_LOG_STDERR);//Initialisation du loggin
	ola::DmxBuffer buffer; //Un buffer pour garder la donnée
	buffer.Blackout(); // On initialise le buffer à 0

	// Créer un nouveau client
	ola::client::StreamingClient ola_client((ola::client::StreamingClient::Options()));

	// configurer le client pour se connecter au serveur
	if (!ola_client.Setup()) //Si ça ne fonctionne pas on arrete le programme
	{
		std::cerr << "Setup failed" << endl;
		exit(1);
	}

	//Envoi de 100 données au serveur
	for (unsigned int i = 0; i < 100; i++) 
	{
		buffer.SetChannel(0, i);//On remet le channel à 0 à chaque fois qu'une trame est envoyée
		if (!ola_client.SendDmx(universe, buffer)) //Si l'envoie échoue on arrete le programme
		{
			cout << "Send DMX failed" << endl;
			exit(1);
		}
		usleep(25000); // pause de 25ms entre chaque envoi de données
	}
 return 0;
}