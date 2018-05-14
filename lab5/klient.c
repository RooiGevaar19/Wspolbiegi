#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>

typedef struct XDDD
{
	unsigned int length;
	int id;
	char *homepath;

} MessageInfo;

int getID(char * arg) {
	int id = -1;
	if(arg != NULL)
	{
		sscanf(arg, "%d", &id);
	} else {
		printf("Nie podales id!\n");
    }
	return id;
}

void sendMessage(int client, void * data) {
	unsigned char * buffer;

	buffer = (char *) malloc(sizeof(int) + ((MessageInfo *)data)->length);
	memcpy(buffer, &((MessageInfo *)data)->length, sizeof(int));
	memcpy(buffer + sizeof(int), &((MessageInfo *)data)->id, sizeof(int));
	memcpy(buffer + (2 * sizeof(int)), ((MessageInfo *)data)->homepath, ((MessageInfo *)data)->length - sizeof(int));

	write(client, buffer, ((MessageInfo *)data)->length + sizeof(int));

	free(buffer);
}

void getServerData(int server) {
	unsigned char * surname;
	int length = 0;
	int bytes = 0;

	fsync(server);
	read(server, &length, sizeof(int));
	surname = malloc(length);

	if((bytes=read(server, surname, length)) > 0){
	
	}
	printf("%s\n", surname);
}

int main(int argc, char** argv) {
	MessageInfo data;
	
	data.id = getID(argv[1]);
	data.homepath = getenv("HOME");
	data.length = sizeof(int) + strlen(data.homepath);
	
	if(data.id < 0) return 1;
	int client = open("clientFifo", O_WRONLY);
	int server = open("serverFifo", O_RDONLY);

	sendMessage(client, &data);
	getServerData(server);
}
