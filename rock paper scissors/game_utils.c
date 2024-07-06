#include "game_utils.h"

#include "headers.h"

void printLines() {
    for (int i = 0; i < 10; i++) printf("-");
    printf("\n");
}

char* getMove(int move) {
    if (move == 0)
        return "rock";
    else if (move == 1)
        return "paper";
    else
        return "scissors";
}

int getUserInput() {
    int input = -1;
    while (1) {
        printf(TCBMAG "Enter 0 for rock, 1 for paper, 2 for scissors: " RESET);
        scanf("%d", &input);

        if (input < 0 || input > 2)
            printf(TCBRED "Invalid, try again\n" RESET);
        else
            break;
    }

    return input;
}

char* getResult(int move1, int move2, int client) {
    int res = (move1 - move2 + 3) % 3;
    if (res == 0) return "\033[1;33mDRAW!\033[0m";

    if (client == 1) {
        if (res == 1) {
            return "\033[1;32mVICTORY!\033[0m";
        } else {
            return "\033[1;31mLOSS!\033[0m";
        }
    } else {
        if (res == 1) {
            return "\033[1;31mLOSS!\033[0m";
        } else {
            return "\033[1;32mVICTORY!\033[0m";
        }
    }
}

char* getServerResult(int move1, int move2) {
    int res = (move1 - move2 + 3) % 3;
    if (res == 0)
        return "DRAW";
    else if (res == 1)
        return "CLIENT 1 WINS";
    else
        return "CLIENT 2 WINS";
}