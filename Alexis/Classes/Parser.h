#include <vector>
#include <string>

using namespace std;

class Parser
{
public:
	vector<string> decode(string message, char sep)
	{
		
		vector<string> decoded;

		string temp("");

		for(int i = 0; i<message.length(); i=i+1)
		{
			if(message[i] == sep || i==message.length() -1)
			{
				if(i==message.length()-1)
				{
					temp = temp+message[i];
				}
				decoded.push_back(temp);
				temp = "";
			}

			else
			{
				temp = temp+message[i];
			}
		}
		return decoded;
		
		
	}
	Parser(){};
};

