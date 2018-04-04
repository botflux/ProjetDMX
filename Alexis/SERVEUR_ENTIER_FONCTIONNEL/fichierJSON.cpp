#include "fichierJSON.h"
#include <string>
#include <cstring>
#include <iostream>
#include "json.h"

using namespace std;

//Rechercher la valeur associer à un nom donné.
string fichierJSON::getName(json_value* value, string name)
{
	int length, x;
	
	if (value == NULL) 
	{
		cout << "value = NULL";
		return "";
	}
	length = value->u.object.length;

	for (x = 0; x < length; x++) 
	{
		string n = value->u.object.values[x].name;
		string v = value->u.object.values[x].value->u.string.ptr;
		
		if(n == name)
		{
			return v;
		}
	}
	return "pas  de valeur coresepondante a ce nom";
}

//Appliquer la configuration du serveur et des équipement
void fichierJSON::setConfig(json_value* value, int &port, int &portDistant, int &nbCanauxProjo, int &nbCanauxLyre, int &tailleBusDMX)
{
	if(getName(value, "PORT")!="")
	{
		port = atoi(getName(value, "PORT").c_str());
	}
	
	if(getName(value, "PORTDISTANT")!="")
	{
		portDistant = atoi(getName(value, "PORTDISTANT").c_str());
	}
	
	if(getName(value, "NBCANAUXPROJO")!="")
	{
		nbCanauxProjo = atoi(getName(value, "NBCANAUXPROJO").c_str());
	}
	
	if(getName(value, "NBCANAUXLYRE")!="")
	{
		nbCanauxLyre = atoi(getName(value, "NBCANAUXLYRE").c_str());
	}
	
	if(getName(value, "TAILLEBUSDMX")!="")
	{
		tailleBusDMX = atoi(getName(value, "TAILLEBUSDMX").c_str());
	}
}

//Ouvrir le fichier JSOn avant de pouvoir l'utiliser
//On vérifie qu'il est correct et si c'est le cas on affiche son contenu
json_value* fichierJSON::ouvrirJSON(string fileName)
{
        char* filename = new char[fileName .length() + 1];
		strcpy(filename , fileName .c_str());
	
        FILE* fp;
        struct stat filestatus;
        int file_size;
        char* file_contents;
        json_char* json;
        json_value* value;

        if ( stat(filename, &filestatus) != 0) {
			cout << "File "<< filename <<" not found\n";
			exit(0);
        }
        file_size = filestatus.st_size;
        file_contents = (char*)malloc(filestatus.st_size);
        if ( file_contents == NULL) {
			cout << "Memory error: unable to allocate "<<file_size<<" bytes\n";
			exit(0);
        }

        fp = fopen (filename, "r");
        if (fp == NULL) {
			cout << "Unable to open "<<filename<<endl;
			fclose (fp);
			free(file_contents);
			exit(0);
        }
        if ( fread(file_contents, file_size, 1, fp) != 1 ) {
			cout << "Unable t read content of "<<filename<<endl;
			fclose (fp);
			free(file_contents);
			exit(0);
        }
        fclose (fp);

        cout << file_contents << endl;

        cout << "--------------------------------\n\n";

        json = (json_char*)file_contents;

        value = json_parse(json,file_size);

        if (value == NULL) {
                cout << "Unable to parse data\n";
                free(file_contents);
                exit(1);
        }
		free(file_contents);
	return value;
}

//Une fois que le fichier à fini d'être utilisé,
//Il ne faut pas oubleir de le fermer
void fichierJSON::fermerJSON(json_value* value)
{
	json_value_free(value);
}
