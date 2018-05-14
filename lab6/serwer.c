#include <stdio.h>
#include <stdlib.h>
#include <linux/msg.h>
#include <linux/ipc.h>
#include <string.h>

#define DBCOUNT 10

typedef struct XDDDDD {
	long typ;
	char dane[256];
} Message;

typedef struct XDDD {
	char* polWord;
	char* engWord;
} Dictionary;

void initializeDatabase(Dictionary* db) {
	((Dictionary *) db)[0].polWord = "Polska";  ((Dictionary *) db)[0].engWord = "Poland";
	((Dictionary *) db)[1].polWord = "Izrael";  ((Dictionary *) db)[1].engWord = "Israel";
	((Dictionary *) db)[2].polWord = "Dania";   ((Dictionary *) db)[2].engWord = "Denmark";
	((Dictionary *) db)[3].polWord = "Francja"; ((Dictionary *) db)[3].engWord = "France";
	((Dictionary *) db)[4].polWord = "Kanada";  ((Dictionary *) db)[4].engWord = "Canada";
	((Dictionary *) db)[5].polWord = "Japonia"; ((Dictionary *) db)[5].engWord = "Japan";
	((Dictionary *) db)[6].polWord = "Katar";   ((Dictionary *) db)[6].engWord = "Qatar";
	((Dictionary *) db)[7].polWord = "Maroko";  ((Dictionary *) db)[7].engWord = "Morocco";
	((Dictionary *) db)[8].polWord = "Austria"; ((Dictionary *) db)[8].engWord = "Austria";
	((Dictionary *) db)[9].polWord = "Niemcy";  ((Dictionary *) db)[9].engWord = "Germany";
}

int compare(Dictionary* db, int length, const char* word) {
	char temp[256];
	strcpy(temp, word);
	temp[strlen(word)] = '\0';
	for(int i = 0; i < length; i++) {
		if(strcmp(((Dictionary *) db)[i].polWord, temp) == 0) {
			printf("%s", ((Dictionary *) db)[i].engWord);
			return i;
		}
	}
	return 0;
}

int main()
{
	Message message;
	Dictionary dictionary[255];

	initializeDatabase(dictionary);

    int input, output, *p;
	int key = 2018;
	int key2 = 2137;

    input = msgget(key, 0777|IPC_CREAT);
	output = msgget(key2, 0777|IPC_CREAT);
	
	while(2137==2137) {
      	if(msgrcv(input, &message, 255, 0, 0) > 0) {
			int result = compare(dictionary, DBCOUNT, message.dane);
			memset(message.dane, 0x00, 256);
			if(result > 0) {
				strcpy(message.dane, dictionary[result].engWord);
        	} else {
				strcpy(message.dane, "nothing, because we don\'t have that word in a database");
			}
      		msgsnd(output, &message, 255, 0);
		}
	}
	msgctl(input, IPC_RMID, 0);
	msgctl(output, IPC_RMID, 0);
}   
