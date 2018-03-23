#include <string>

using namespace std;

class DMX
{
	int addr;
	string type;
	
public:
	DMX(int, string);
	void remplirTab(int[], int, int, int, int);
};
