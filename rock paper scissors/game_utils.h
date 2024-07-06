#ifndef __GAME_UTILS_H
#define __GAME_UTILS_H

void printLines();
char *getMove(int move);
char *getResult(int move1, int move2, int client);
char *getServerResult(int move1, int move2);

int getUserInput();

#endif