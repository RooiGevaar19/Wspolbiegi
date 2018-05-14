#include <stdio.h>
#include <stdlib.h>
#include <linux/msg.h>
#include <linux/ipc.h>
#include <string.h>

typedef struct XDDDDDD
{
      	long typ;
     	char dane[256];
} Message;

int main(int argc, char** argv) {
	if(argc > 1) {
      	Message message;
      	int input, output, *p;
      	long ident = getpid();
		int key = 2018;
		int key2 = 2137;
      
      	input = msgget(key, 0777);
		output = msgget(key2, 0777);

		int length = strlen(argv[1]);
      	strcpy(message.dane, argv[1]);
		message.dane[strlen(argv[1])] = '\0';

      	message.typ = ident;
      	msgsnd(input, &message, 255, 0);
      	printf("Client %5ld sent %s\n", ident, message.dane);
      	msgrcv(output, &message, 255, ident, 0);
      	printf("Client %5ld got  %s\n", ident, message.dane);
		return 0;
	} else {
		printf("You sent no word.");
		return 1;
	}
}      
