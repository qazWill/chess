#include "stdio.h"
#include "board.h"
#include "game.h"

/* entry point of the program */
int main(void) {

    /* places pieces at their initial positions */
    initialize_board();
    
    /* alternates between user and opponent */
    while (1) {
        move_user();
        move_opponent();
    }
    
    /* end of program */
    return 0;
}