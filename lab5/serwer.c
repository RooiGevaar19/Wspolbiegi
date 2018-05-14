#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#define DB_SIZE 10
#define MAX_LINE 70
#define DB_DATA_FILE "data"

typedef struct xddd {
	int id;
	char * surname;
} ClientData;

typedef struct xdddd {
	int id;
	char * homepath;
} MessageData;

int initiallizeDatabase(void * data) {
	FILE *fp;

	char buffer[MAX_LINE];
	int i = 0;
	if (fp = fopen(DB_DATA_FILE, "r")) {
		while((fgets(buffer, sizeof(buffer), fp) != NULL) && (i<DB_SIZE)) {
			((ClientData *)data)[i].surname = (char *) malloc(MAX_LINE);
			sscanf(buffer, "%d\t%20s", &((ClientData *)data)[i].id, ((ClientData *)data)[i].surname);
			i++;
		}
		fclose(fp);
    } else {
		printf("Nie moge otworzyc pliku");
        return 1;
    }
    return 0;
}

char * getSurnameByID(void * data, int id) {
	for (int i = 0; i < DB_SIZE; i++) {
		if (((ClientData *)data)[i].id == id) {
        	return ((ClientData *)data)[i].surname;
        }
	}
    return "User doesn't exist.";
}


MessageData getClientData(int client, int size) {
	MessageData data;

	unsigned char * buffer = (char *) malloc(size);

	read(client, buffer, size);

	memcpy(&data.id, buffer, sizeof(int));
	data.homepath = (char *) malloc(size - sizeof(int));
	memcpy(data.homepath, (buffer + sizeof(int)), size - sizeof(int));
        
	free(buffer);
	return data;
}

void sendMessage(int server, void * db, void * data) {
 
	int length = 0;
	unsigned char * message;
    unsigned char * surname = getSurnameByID(db, ((MessageData *)data)->id);

    message = (char *) malloc(strlen(surname) + sizeof(int) + 1);
    length = strlen(surname);
    memcpy(message, &length , sizeof(int));
    memcpy(message + sizeof(int), surname, length);

    write(server, message, length + sizeof(int));

	free(message);
}

int main() {
	ClientData dbdata[DB_SIZE];
	MessageData data;

	initiallizeDatabase(dbdata);

	mkfifo("clientFifo", 0666);
	mkfifo("serverFifo", 0666);

	int client = open("clientFifo", O_RDONLY);
	int server = open("serverFifo", O_WRONLY);

	int messageLength = 0;
	int bytes = 0;

	while(2137==2137) {
		if ((bytes = read(client, &messageLength, sizeof(int))) > 0) {
			data = getClientData(client, messageLength);
			sendMessage(server, &dbdata, &data);
    	}
	}

	close(client);
	close(server);

	unlink("clientFifo");
	unlink("serverFifo");
}
