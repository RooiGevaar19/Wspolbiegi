#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <signal.h>

#define key 1488
#define sem1 0
#define sem2 1

int memory;
int semaphores;
int board[9] = {0,0,0,0,0,0,0,0,0};
char *playername;
int Gracz1 = 1;
int Gracz2 = 2;
int *shm; // shm = shared memory

struct sembuf Gracz1_lock = {sem2,-1,0};  
struct sembuf Gracz1_unlock = {sem1,1,0}; 
struct sembuf Gracz2_lock = {sem1,-1,0}; 
struct sembuf Gracz2_unlock = {sem2,1,0};


void intro(){
    system("clear");
    printf("Kółko i Krzyżyk\n");
}

int makeMove(int player){
    int pole;
    int status = 1;
    int x,y;
    while(status == 1){
        printf("\nPodaj numer pola w postaci wiersz-spacja-kolumna [1..3], np. \'1 2\': ");
        scanf("%i %i", &x, &y);
        pole = 3*(x-1)+(y-1);
        if(pole < 10 && y <= 3 && x <= 3 && x > 0 && y > 0 && pole >= 0 ){
            if(board[pole] == 0){
                board[pole] = player;
                status = 0;
                return 0;
            }
            else{
                printf("\nTo pole jest akurat zajete.\n");
                status = 1;
            }
        }
        else{
            printf("\nZly numer pola.\n");
            status = 1;
        }
    }
}

void cleanMemory(){
    printf("\nKończenie gry.\n");
    semctl(semaphores,0,IPC_RMID,0);
    shmdt(shm);
    shmctl(memory,IPC_RMID,0);
    exit(0);
}

int is_ready(){
    int i = 0;
    if(board[0] == 0 && board[1] == 0 && board[2] == 0 && board[3] == 0 && board[4] == 0 && board[5] == 0 && board[6] == 0 && board[7] == 0 && board[8] == 0){
        return 1;
    }
    else{
        return 0;
    }
}

void exportMemory(){
    for (int i = 0; i < 10; i++) shm[i] = board[i];
}

void importMemory(){
    for (int i = 0; i < 10; i++) board[i] = shm[i];
}

void drawBoard(){
    int i = 0;
    int k = 0;
    printf("\n");
    while(k<9){
        for(i;i<3;i++){
            int j = 0;
            for(j;j<3;j++){
                if(board[k] == 0) printf("  "); 
                if(board[k] == 1) printf(" O"); 
                if(board[k] == 2) printf(" X");
                if(j==0 || j==1) printf(" |");
                k++;
            }
            if(i==0 || i==1) printf("\n---+---+---\n");
        }
    }
    printf("\n\n");
}

void checkBoard(int player){
    if(board[0] != 0 && board[1] != 0 && board[2] != 0 && board[3] != 0 && board[4] != 0 && board[5] != 0 && board[6] != 0 && board[7] != 0 && board[8] != 0){
        board[9] = 3; // kwestia remisu
    } else {
        if(board[0] == player && board[1] == player && board[2] == player) board[9] = player; 
        else if(board[3] == player && board[4] == player && board[5] == player) board[9] = player; 
        else if(board[6] == player && board[7] == player && board[8] == player) board[9] = player;
        else if(board[0] == player && board[3] == player && board[6] == player) board[9] = player; 
        else if(board[1] == player && board[4] == player && board[7] == player) board[9] = player; 
        else if(board[2] == player && board[5] == player && board[8] == player) board[9] = player; 
        else if(board[0] == player && board[4] == player && board[8] == player) board[9] = player; 
        else if(board[2] == player && board[4] == player && board[6] == player) board[9] = player; 
    }
}

void checkResult(){
    if(board[9] == 1 && strcmp(playername,"Gracz1") == 0){
        printf("\nWygrales!\n");
        cleanMemory();
    }
    else if(board[9] == 1 && strcmp(playername,"Gracz2") == 0){
        printf("\nPrzegrales! Wygral Gracz 1\n");
        cleanMemory();
    }
    else if(board[9] == 2 && strcmp(playername,"Gracz2") == 0){
        printf("\nWygrales!\n");
        cleanMemory();
    }
    else if(board[9] == 2 && strcmp(playername,"Gracz1") == 0){
        printf("\nPrzegrales! Wygral Gracz 2\n");
        cleanMemory();
    }
    else if(board[9] == 3){
        printf("\nRemis!\n");
        cleanMemory();
    }
}
int main()
{
    intro();
    signal(SIGINT, cleanMemory);

    if((memory = shmget(key,sizeof(int)*39,IPC_CREAT|IPC_EXCL|0777)) == -1){
        if((memory = shmget(key,sizeof(int)*39,0)) == -1){
            printf("\nNie mozna połączyć z grą - bląd pamieci wspoldzielonej!\n");
            exit(1);
        }
        else{
            printf("\nTwoja nazwa gracza: ");
        }
    }
    else{
        printf("\nTwoja nazwa gracza: ");
    }

    semaphores = semget(key,2,0777|IPC_CREAT|IPC_EXCL);

    if(semaphores == -1){
        semaphores = semget(key,2,0);
        playername = "Gracz2";
        printf("Gracz 2 - grasz krzyżykiem\n");
    }
    else{
        playername = "Gracz1";
        printf("Gracz 1 - grasz kółkiem\n");
    }

    shm = shmat(memory,(void*)0,0);
    exportMemory();

    if(strcmp(playername,"Gracz1") == 0){
        semctl(semaphores,sem1,SETVAL,0);
        semctl(semaphores,sem2,SETVAL,1);
        if(makeMove(Gracz1) == 0){
            system("clear");
            exportMemory();
            drawBoard();
            while(1){
                printf("\nOczekiwanie na ruch Gracza 2\n");
                semop(semaphores,&Gracz2_lock,1);
                system("clear");
                importMemory();
                drawBoard();
                checkResult();
                if(makeMove(Gracz1) == 0){
                    system("clear");
                    checkBoard(Gracz1);
                    exportMemory();
                    drawBoard();
                    checkResult();
                }
                semop(semaphores,&Gracz2_unlock,1);
            }
        }
    }
    else{
        printf("\nOczekiwanie na ruch Gracza 1 \n");
        while(1){
            importMemory();
            if(is_ready() == 0){
                while(1){
                    printf("\nOczekiwanie na ruch Gracza 1\n");
                    semop(semaphores,&Gracz1_lock,1);
                    system("clear");
                    importMemory();
                    drawBoard();
                    checkResult();
                    if(makeMove(Gracz2) == 0){
                        system("clear");
                        checkBoard(Gracz2);
                        exportMemory();
                        drawBoard();
                        checkResult();
                    }
                    semop(semaphores,&Gracz1_unlock,1);
                }
            }
        }
    }

   return 0;
}
