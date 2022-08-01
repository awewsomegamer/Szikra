#include <util.h>

void uppercaseString(char* string){
	for (int i = 0; i < strlen(string); i++)
		*(string + i) = toupper(*(string + i));
}

void lowercaseString(char* string){
	for (int i = 0; i < strlen(string); i++)
		*(string + i) = tolower(*(string + i));
}
