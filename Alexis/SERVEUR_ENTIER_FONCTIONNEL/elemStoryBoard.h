#include <string>

using namespace std;

class elemStoryBoard
{
	string cible;
	int addrCible;
	int blue;
	int red;
	int green;
	int intensity;
	int time;
public:

	string getCible();
	int getAddr();
	int getBlue();
	int getRed();
	int getGreen();
	int getIntensity();
	int getTime();
	
	void setCible(string);
	void setAddr(int);
	void setBlue(int);
	void setRed(int);
	void setGreen(int);
	void setIntensity(int);
	void setTime(int);
	
};