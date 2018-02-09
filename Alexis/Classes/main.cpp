#include <iostream>
#include "Socket.h"
#include <cstring>
#include "Parser.h"
#include <cstdlib>

using namespace std;

#define PORT 5000
#define LG_MESSAGE 256

int main ()
{
	Socket *mySocket = new Socket();

	mySocket->setDescripteur(PF_INET, SOCK_DGRAM,0);

	mySocket->setPdrLocal(PF_INET, htons(PORT), htonl(INADDR_ANY));

	mySocket->bindSocket();

	char message[LG_MESSAGE];

	while (true)
	{
		memset(message, 0x00, LG_MESSAGE*sizeof(char));
		mySocket->setLus(recvfrom(mySocket->getDescripteur(), message, sizeof(message), 0, (struct sockaddr*)mySocket->getPdrDistant(), mySocket->getLgAddr2()));
		cout << "Message recu : " << message << endl;
	}
	return 0;
}
