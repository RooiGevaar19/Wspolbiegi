#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <signal.h>

#define key 1488
#define sem1 0
#define sem2 1

#define DICTSIZE 130
#define MEMSIZE 77
// 0-35  tile on the board 
// 36-71 if the tile is used
// 72-73 score
// 74-75 tiles
// 76    which player won

// =========== GLOBAL VARIABLES
// Semaphores and shared memory



 char dict[DICTSIZE][3] = {
    "CAT", "DOG", "BUG", "PIG", "BEE",
    "COW", "HEN", "FOX", "CAR", "TED",
    "AND", "NOR", "LET", "SET", "GET",
    "MET", "DAD", "MUM", "MOM", "MAD",
    "BAD", "SAD", "HUD", "LED", "CUT",
    "RUN", "LIT", "BIT", "BUT", "HAD",
    "FAN", "BAN", "CAN", "DOT", "DID",
    "LOT", "HAT", "HAS", "OAT", "SIT",
    "MIA", "SIA", "PIA", "ERA", "SIP",
    "REP", "ARM", "ACE", "ATE", "APE",
    // ^50
    "PEN", "KEN", "LAN", "ZAP", "ZIP",
    "OFF", "OLD", "EVE", "BIG", "RIG",
    "ADD", "SUB", "YOU", "MUD", "SUN",
    "GIG", "GEM", "MUG", "FOG", "PAL",
    "FAD", "LAD", "DAM", "LAB", "SQL",
    "FAQ", "WTF", "WEB", "BOW", "RIB",
    "CAB", "COP", "ICE", "AIR", "BID",
    "CIG", "GIN", "JEN", "JAR", "PAR",
    "RED", "GIT", "TAP", "EAR", "EYE",
    "LIP", "LEG", "BED", "CIA", "NSA",
    // ^100
    "NOT", "FUN", "GAS", "BUS", "BOT",
    "CUB", "NET", "VET", "HAM", "ROD",
    "MAY", "WAY", "DAY", "DRY", "JAM",
    "SHE", "WET", "TIP", "REA", "SEA",
    "TEN", "SIX", "TWO", "ONE", "ELF",
    "SEX", "USA", "ASS", "GAY", "BAY"
 };

int memory;
int semaphores;
int board[36] = {
    0,0,0,0,0,0,
    0,0,0,0,0,0,
    0,0,0,0,0,0,
    0,0,0,0,0,0,
    0,0,0,0,0,0,
    0,0,0,0,0,0
};
int used[36] = {
    0,0,0,0,0,0,
    0,0,0,0,0,0,
    0,0,0,0,0,0,
    0,0,0,0,0,0,
    0,0,0,0,0,0,
    0,0,0,0,0,0
};
int score1 = 0;
int score2 = 0;
int tilesleft1 = 18;
int tilesleft2 = 18;
int whowon = 0;
char *playername;
int Gracz1 = 1;
int Gracz2 = 2;
int *shm; // shm = shared memory
bool endgame = false;

struct sembuf Gracz1_lock = {sem2,-1,0};  
struct sembuf Gracz1_unlock = {sem1,1,0}; 
struct sembuf Gracz2_lock = {sem1,-1,0}; 
struct sembuf Gracz2_unlock = {sem2,1,0};


// XLIB Global variables

Display* display;
int screen_num;
static char* appname;

Window win;
int x, y;
unsigned int width, height;
unsigned int border_width;
char* window_name = "Scrabble Game!";
char* icon_name   = "ScrabbleX";

// Display variables
char * display_name = NULL;
unsigned int display_width, display_height;
    
// Miscellaneous X variables
XSizeHints* size_hints;
XWMHints* wm_hints;
XClassHint* class_hints;
XTextProperty windowName, iconName;
XEvent report;
XFontStruct* font_info;
XGCValues values;
GC gc;
GC gc2;
Colormap mycolormap;


// =========== STRUCTURES

struct ItemsBoard {
    char item[6];
    int DisplayX;
    int DisplayY;
} itemsboard;

// =========== FUNCTIONS AND PROCEDURES

void charToString(char ch, char* out) {
    out[0] = ch;
}

void cleanMemory(){
    printf("\nKończenie gry.\n");
    semctl(semaphores,0,IPC_RMID,0);
    shmdt(shm);
    shmctl(memory,IPC_RMID,0);
    exit(0);
}

void exportMemory(){
    for (int i = 0; i < 36; i++) shm[i] = board[i];
    for (int i = 0; i < 36; i++) shm[i+36] = used[i];
    shm[72] = score1;
    shm[73] = score2;
    shm[74] = tilesleft1;
    shm[75] = tilesleft2;
    shm[76] = whowon;
    //printf("exported\n");
}

void importMemory(){
    for (int i = 0; i < 36; i++) board[i] = shm[i];
    for (int i = 0; i < 36; i++) used[i] = shm[i+36];
    score1 = shm[72];
    score2 = shm[73];
    tilesleft1 = shm[74];
    tilesleft2 = shm[75];
    whowon = shm[76];
    int x = 0;
    int y = 0;
    for (int i = 0; i < 36; i++) 
        if (shm[i] != 0)
            x = shm[i];
    for (int i = 0; i < 36; i++) 
        if (board[i] != 0)
            y = board[i];
    //printf("imported %i %i\n", x, y);
}


void setFont(char* fname) {
    if ( (font_info = XLoadQueryFont(display, fname)) == NULL ) {
       fprintf(stderr, "%s: cannot open %s font.\n", appname, fname);
       exit(EXIT_FAILURE);
    }
}

void drawRectangle(int x, int y, int width, int height) {
    XFillRectangle(display, win, gc2, x, y, width, height);
}

void clearArea(int x, int y, int width, int height) {
    XClearArea(display, win, x, y, width, height, true);
}

void drawText(char* text, int x, int y) {
    XDrawString(display, win, gc, x, y, text, strlen(text));
}

void drawEmptyBoard(int x, int y) {
    for (int i = 0; i < 600; i+=100) {
        for (int j = 0; j < 600; j+=100) {
            XDrawRectangle(display, win, gc, x+i, y+j, 100, 100);
        }
    }
}

void hideBoard() {
    clearArea(100, 100, 600, 600);
}

void drawBoard() {
    drawEmptyBoard(100, 100);
    for (int i = 0; i < 36; i++) {
        char letter[1];
        charToString(board[i], letter);
        drawText(letter, 150+(i%6*100), 150+(i/6*100));
    }
}

void drawFinalScreen(char* text, int player) {
    XDrawRectangle(display, win, gc, 100, 100, 600, 600);
    drawText(text, 350, 400);
    if (score1 > score2) drawText("Player 1 won.", 350, 420);
    if (score1 < score2) drawText("Player 2 won.", 350, 420);
    if (score1 == score2) drawText("Stalemate!", 350, 420);
    
}

void drawItemsBoard() {
    int x = itemsboard.DisplayX;
    int y = itemsboard.DisplayY;
    for (int i = 0; i < 600; i+=100) {
        XDrawRectangle(display, win, gc, x+i, y, 100, 100);
    }
    for (int i = 0; i < 6; i++) {
        char letter[1];
        charToString(itemsboard.item[i], letter);
        drawText(letter, x+50+100*i, y+50);
    }
}

void destroyWindow() {
    XUnloadFont(display, font_info->fid);
    XFreeGC(display, gc);
    XCloseDisplay(display);
    cleanMemory();
    exit(EXIT_SUCCESS);
}

void endTheGame(int player) {
    while (2137 == 2137) {
        XNextEvent(display, &report);
        switch (report.type) {
            case Expose : {
                if ( report.xexpose.count != 0 ) break;
                break;
            }
            case ConfigureNotify : {
                width  = report.xconfigure.width;
                height = report.xconfigure.height;
                break;
            }
            case DestroyNotify : {
                destroyWindow();
            }      
            case KeyPress : {
                if (report.xkey.keycode == 0x09) destroyWindow();
            }
        }
    }
}

// =========== Game Mechanisms

int randomLetter() {
    if ((rand()%4) == 0) {
        char vowels[6] = {'A', 'E', 'I', 'O', 'U', 'Y'};
        return (int) vowels[(rand()%6)];
    } else {
        return (rand()%26)+65;
    }
}

void initItemsboard() {
    // (rand()%26)+65;
    itemsboard.item[0] = randomLetter();
    itemsboard.item[1] = randomLetter();
    itemsboard.item[2] = randomLetter();
    itemsboard.item[3] = randomLetter();
    itemsboard.item[4] = randomLetter();
    itemsboard.item[5] = randomLetter();
    itemsboard.DisplayX = 100;
    itemsboard.DisplayY = 800;
}

void renderItemsboard(int player) {
    char buffer[100];
    if (player == Gracz1) sprintf(buffer, "Moves left: %i", tilesleft1);
    else sprintf(buffer, "Moves left: %i", tilesleft2);
    drawText(buffer, itemsboard.DisplayX, itemsboard.DisplayY-40);
    drawItemsBoard();
}

void hideItemsboard() {
    clearArea(100, 720, 620, 180);
}

void shuffleItemsboard(int index) {
    itemsboard.item[index] = (rand()%26)+65;
}

int is_ready(){
    int i = 0;
    if (board[0] == 0 && board[1] == 0 && board[2] == 0 && board[3] == 0 && board[4] == 0 && board[5] == 0 && 
        board[6] == 0 && board[7] == 0 && board[8] == 0 && board[9] == 0 && board[10] == 0 && board[11] == 0 &&
        board[12] == 0 && board[13] == 0 && board[14] == 0 && board[15] == 0 && board[16] == 0 && board[17] == 0 &&
        board[18] == 0 && board[19] == 0 && board[20] == 0 && board[21] == 0 && board[22] == 0 && board[23] == 0 &&
        board[24] == 0 && board[25] == 0 && board[26] == 0 && board[27] == 0 && board[28] == 0 && board[29] == 0 &&
        board[30] == 0 && board[31] == 0 && board[32] == 0 && board[33] == 0 && board[34] == 0 && board[35] == 0) {
        return 1;
    } else {
        return 0;
    }
}

int checkScore(int x, int y) {
    int score = 0;
    // for each word
    for (int i = 0; i < DICTSIZE; i++) {
        // check the X axis
        int left = y*6;
        for (int j = 0; j < 4; j++) {
            if ((dict[i][0] == board[left+j]) && (dict[i][1] == board[left+j+1]) && (dict[i][2] == board[left+j+2])
                && (used[left+j] != 2 || used[left+j+1] != 2 || used[left+j+2] != 2)) {
                used[left+j] = 2;
                used[left+j+1] = 2;
                used[left+j+2] = 2;
                score++;
            }
        }
        // check the Y axis
        int up = x;
        for (int j = 0; j < 4; j++) {
            if ((dict[i][0] == board[up+6*j]) && (dict[i][1] == board[up+6*(j+1)]) && (dict[i][2] == board[up+6*(j+2)])
                && (used[up+6*j] != 2 || used[up+6*(j+1)] != 2 || used[up+6*(j+2)] != 2)) {
                used[up+6*j] = 2;
                used[up+6*(j+1)] = 2;
                used[up+6*(j+2)] = 2;
                score++;
            }
        }
    }
    return score;
}


void showScoreboard(int player) {
    char buf[100];
    clearArea(100, 45, 600, 55);
    if (player == Gracz1) sprintf(buf, "Points: %i", score1);
    if (player == Gracz2) sprintf(buf, "Points: %i", score2);
    drawText(buf, 100, 60);
}

int makeMove(int player) {
    bool picked = false;
    bool chosen = false;
    char currenttile;
    if (tilesleft1 == 0 && tilesleft2 == 0) {
        endgame = true;
        clearArea(100, 100, 600, 600);
        drawFinalScreen("Game Over", player);
        endTheGame(player);
    } else if (endgame) {
    } else {
        showScoreboard(player);
        drawText("Your turn.", 100, 80);
        drawText("Pick a tile you want to place:", itemsboard.DisplayX, itemsboard.DisplayY-20);
        while (!picked) {
            renderItemsboard(player);
            XNextEvent(display, &report);
            switch (report.type) {
                case ButtonPress : {
                    switch (report.xbutton.button) {
                        case Expose : {
                            if ( report.xexpose.count != 0 ) break;
                            break;
                        }
                        case ConfigureNotify : {
                            width  = report.xconfigure.width;
                            height = report.xconfigure.height;
                            break;
                        }
                        case Button1 : {
                            int x = report.xbutton.x;
                            int y = report.xbutton.y;
                            //printf("%i %i\n", x, y);
                            if (x >= 100 && x <= 700 && y >= 800 && y <= 900) {
                                int addr = (x-100)/100;
                                currenttile = itemsboard.item[addr];
                                printf("%i\n", currenttile);
                                itemsboard.item[addr] = randomLetter();
                                if (player == Gracz1) tilesleft1--;
                                if (player == Gracz2) tilesleft2--;
                                picked = true;
                                XFlush(display);
                            }
                        }
                        default : break;
                    }
                    break;
                }   
                case DestroyNotify : {
                    destroyWindow();
                }      
                case KeyPress : {
                    if (report.xkey.keycode == 0x09) destroyWindow();
                }
            }
        }
        hideItemsboard();
        clearArea(100, 760, 620, 40);
        char buffer[100];
        sprintf(buffer, "You picked \"%c\"! Place it on a free spot on the board.", currenttile);
        drawText(buffer, itemsboard.DisplayX, itemsboard.DisplayY-20);
        while (!chosen) {
            drawBoard();
            XNextEvent(display, &report);
            switch (report.type) {
                case ButtonPress : {
                    switch (report.xbutton.button) {
                        case Expose : {
                            if ( report.xexpose.count != 0 ) break;
                            break;
                        }
                        case ConfigureNotify : {
                            width  = report.xconfigure.width;
                            height = report.xconfigure.height;
                            break;
                        }
                        case Button1 : {
                            int x = report.xbutton.x;
                            int y = report.xbutton.y;
                            int addr = (y-100)/100*6 + (x-100)/100%6;
                            if (x >= 100 && x <= 700 && y >= 100 && y <= 700 && used[addr] == 0) {
                                //printf("%i at %i\n", currenttile, addr);
                                board[addr] = (int) currenttile;
                                used[addr] = 1;

                                chosen = true;

                                if (player == Gracz1) score1 += checkScore(((x-100)/100), ((y-100)/100));
                                if (player == Gracz2) score2 += checkScore(((x-100)/100), ((y-100)/100));
                                showScoreboard(player);
                                hideBoard();
                                drawBoard();
                                hideItemsboard();
                                if (tilesleft1 == 0 && tilesleft2 == 0 && player == Gracz1) {
                                    endgame = true;
                                    clearArea(100, 100, 600, 600);
                                    drawFinalScreen("Game Over", player);
                                }
                                if (tilesleft2 == 0 && player == Gracz2) {
                                    endgame = true;
                                    clearArea(100, 100, 600, 600);
                                    drawFinalScreen("Game Over", player);
                                }
                                clearArea(100, 60, 600, 20);
                                if (player == Gracz1) drawText("Waiting for the turn of the player 2.", 100, 80);
                                if (player == Gracz2) drawText("Waiting for the turn of the player 1.", 100, 80);
                                XFlush(display);
                            }
                        }
                        default : break;
                    }
                    break;
                }   
                case DestroyNotify : {
                    destroyWindow();
                }      
                case KeyPress : {
                    if (report.xkey.keycode == 0x09) destroyWindow();
                }
            }
        }
    }
    return 0;
}



// =========== MAIN

int main(int argc, char* argv[]) {

    srand(time(NULL));
    //loadDictionary();
    signal(SIGINT, cleanMemory);

    appname = argv[0];
    /*  Allocate memory for our structures  */
    if ( !( size_hints = XAllocSizeHints() ) || !( wm_hints    = XAllocWMHints()   ) ||
	 !( class_hints = XAllocClassHint() )    ) {
        fprintf(stderr, "%s: couldn't allocate memory.\n", appname);
        exit(EXIT_FAILURE);
    }


    /*  Connect to X server  */
    if ( (display = XOpenDisplay(display_name)) == NULL ) {
	   fprintf(stderr, "%s: couldn't connect to X server %s\n",
	   appname, display_name);
        exit(EXIT_FAILURE);
    }


    /*  Get screen size from display structure macro  */

    screen_num = DefaultScreen(display);
    display_width = DisplayWidth(display, screen_num);
    display_height = DisplayHeight(display, screen_num);


    /*  Set initial window size and position, and create it  */

    x = 0;
    y = 0;
    //width = display_width / 3;
    //height = display_width / 3;
    width = 800;
    height = 900;

    win = XCreateSimpleWindow(display, RootWindow(display, screen_num),
	   x, y, width, height, border_width,
	   BlackPixel(display, screen_num),
	   WhitePixel(display, screen_num));


    /*  Set hints for window manager before mapping window  */

    if (XStringListToTextProperty(&window_name, 1, &windowName) == 0) {
        fprintf(stderr, "%s: structure allocation for windowName failed.\n", appname);
        exit(EXIT_FAILURE);
    }

    if (XStringListToTextProperty(&icon_name, 1, &iconName) == 0) {
        fprintf(stderr, "%s: structure allocation for iconName failed.\n", appname);
        exit(EXIT_FAILURE);
    }

    size_hints->flags = PPosition | PSize | PMinSize;
    size_hints->min_width = 200;
    size_hints->min_height = 100;

    wm_hints->flags = StateHint | InputHint;
    wm_hints->initial_state = NormalState;
    wm_hints->input = True;

    class_hints->res_name = appname;
    class_hints->res_class = "hellox";

    XSetWMProperties(display, win, &windowName, &iconName, argv, argc, size_hints, wm_hints, class_hints);

    /*  Choose which events we want to handle  */
    XSelectInput(display, win, ExposureMask | KeyPressMask | ButtonPressMask | StructureNotifyMask);


    /*  Load a font called "9x15"  */
    setFont("10x20");
    /*  Create graphics context  */

    gc = XCreateGC(display, win, 0, &values);
    XSetFont(display, gc, font_info->fid);
    XSetBackground(display, gc, WhitePixel(display, screen_num));
    XSetForeground(display, gc, BlackPixel(display, screen_num));

    /*  Display Window  */
    XMapWindow(display, win);
    
    //  Enter event loop 

    //  LET THE SHOW BEGIN!

    initItemsboard();

    bool start = true;


    while (2137 == 2137) {

        XNextEvent(display, &report);

        if (start) {
            drawText("Welcome to Micro-Scrabble!", 100, 20);
            printf("Bring it on!\n");
            if((memory = shmget(key,sizeof(int)*MEMSIZE,IPC_CREAT|IPC_EXCL|0777)) == -1){
                if((memory = shmget(key,sizeof(int)*MEMSIZE,0)) == -1){
                    printf("\nERROR: Cannot connect with the game - no shared memory!\n");
                    getchar();
                    exit(1);
                } else {
                    //printf("\nTwoja nazwa gracza: ");
                }
            } else {
                //printf("\nTwoja nazwa gracza: ");
            }
            semaphores = semget(key,2,0777|IPC_CREAT|IPC_EXCL);
            if(semaphores == -1){
                semaphores = semget(key,2,0);
                playername = "Gracz2";
                printf("You start as a player 2\n");
            }
            else {
                playername = "Gracz1";
                printf("You start as a player 1\n");
            }
            shm = shmat(memory,(void*)0,0);
            start = false;
        }

        ///*
        if (strcmp(playername,"Gracz1") == 0) {
            semctl(semaphores,sem1,SETVAL,0);
            semctl(semaphores,sem2,SETVAL,1);
            clearArea(100, 60, 600, 20);
            drawText("You're player 1.", 100, 40);
            if(makeMove(Gracz1) == 0){
                exportMemory();
                hideBoard();
                drawBoard();
                while(1){
                    drawBoard();
                    semop(semaphores,&Gracz2_lock,1);
                    importMemory();
                    drawBoard();
                    if(makeMove(Gracz1) == 0){
                        exportMemory();
                        drawBoard();
                    }
                    semop(semaphores,&Gracz2_unlock,1);
                }
            }
        } else {
            clearArea(100, 60, 600, 20);
            drawText("You're player 2.", 100, 40);
            while (2137==2137) {
                importMemory();
                if(is_ready() == 0){
                    while(1){
                        drawBoard();
                        semop(semaphores,&Gracz1_lock,1);
                        importMemory();
                        drawBoard();
                        if(makeMove(Gracz2) == 0){
                            exportMemory();
                            drawBoard();
                        }
                        semop(semaphores,&Gracz1_unlock,1);
                    }
                }
            }
        }
        //*/
        

        // trash

        //makeMoveBeta();
    }
    return EXIT_SUCCESS; 
}