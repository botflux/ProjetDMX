/*
	http://docs.openlighting.org/ola/doc/latest/client_tutorial.html
	Programme d'exemple pour transmettre des donnée DMX avec OLA
*/
#include <ola/DmxBuffer.h>
#include <ola/io/SelectServer.h>
#include <ola/Logging.h>
#include <ola/client/ClientWrapper.h>
#include <ola/Callback.h>

using std::cout;
using std::endl;

bool SendData(ola::client::OlaClientWrapper *wrapper) //Fonction pour envoyer des données
{
	 static unsigned int universe = 1;
	 static unsigned int i = 0;

	 ola::DmxBuffer buffer;
	 buffer.Blackout();
	 buffer.SetChannel(0, i);
	 wrapper->GetClient()->SendDMX(universe, buffer, ola::client::SendDMXArgs());//Envoi des données

	 if (++i == 100) //Une fois qu'on atteint les 100 images envoyées on termine le programme
	 {
		wrapper->GetSelectServer()->Terminate();
	 }

	 return true;
}


int main(int, char *[]) 
{
	//initialisation
	 ola::InitLogging(ola::OLA_LOG_WARN, ola::OLA_LOG_STDERR);
	 ola::client::OlaClientWrapper wrapper;

	 if (!wrapper.Setup()) //On initialise la variable wrapper avec la fonction Setup, si ça ne fonctionne pas on termine le programme
	 {
		 std::cerr << "Setup failed" << endl;
		 exit(1);
	 }

	 // Creer un timer et l'enregistrer sur le serveur
	 ola::io::SelectServer *ss = wrapper.GetSelectServer(); // Selectionner le serveur
	 ss->RegisterRepeatingTimeout(25, ola::NewCallback(&SendData, &wrapper));//repeter toutes les 25 ms ;    la donnée SendData est un appel de la fonction définie plus haut

	 // Start the main loop
	 ss->Run();
}