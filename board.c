#include "stdio.h"
#include "math.h"

/* bitboard custom type */
typedef unsigned long int bitboard;

/* board representation */
bitboard pieces;
bitboard whites;
bitboard blacks;
bitboard kings;
bitboard queens;
bitboard bishops;
bitboard knights;
bitboard rooks;
bitboard pawns;

/* will contain all possible moves for one color */
bitboard options[64];

/* function declarations */
void initialize_board(void);
void print_board(void);
int get(bitboard b, int index);
int xy_get(bitboard b, int x, int y);
void set(int white, char type, int index);
void xy_set(int white, char type, int x, int y);
void remove_piece(int index);
void move(int initial, int final);
void xy_move(int initial_x, int initial_y, int final_x, int final_y);
void update_options(int white);
void check(int white);
int check_mate(int white);
void options_king(bitboard color, int index);
void options_queen(bitboard color, int index);
void options_bishop(bitboard color, int index);
void options_knight(bitboard color, int index);
void options_rook(bitboard color, int index);
void options_pawn(bitboard color, int index);
int valueOf(int index);

/*
This function sets the pieces in their
initial positions.
*/
void initialize_board(void) {

    /* standard setup */
    xy_set(1, 'k', 4, 0);
    xy_set(0, 'k', 4, 7);
    xy_set(1, 'q', 3, 0);
    xy_set(0, 'q', 3, 7);
    xy_set(1, 'b', 2, 0);
    xy_set(1, 'b', 5, 0);
    xy_set(0, 'b', 2, 7);
    xy_set(0, 'b', 5, 7);
    xy_set(1, 'n', 1, 0);
    xy_set(1, 'n', 6, 0);
    xy_set(0, 'n', 1, 7);
    xy_set(0, 'n', 6, 7);
    xy_set(1, 'r', 0, 0);
    xy_set(1, 'r', 7, 0);
    xy_set(0, 'r', 0, 7);
    xy_set(0, 'r', 7, 7);
    int i;
    for (i = 0; i < 8; i++) {
        xy_set(1, 'p', i, 1);
        xy_set(0, 'p', i, 6);
    }
    
    /* custom setup */
    /*int i;
    for (i = 5; i < 8; i++) {
        xy_set(1, 'p', i, 1);
        xy_set(0, 'p', i, 6);
    }*/
    /*xy_set(0, 'p', 0, 3);
    xy_set(0, 'p', 1, 3);
    xy_set(1, 'p', 7, 4);
    xy_set(1, 'p', 7, 3);
    xy_set(1, 'k', 5, 0);
    xy_set(0, 'k', 3, 2);*/
    /*xy_set(0, 'r', 3, 7);
    xy_set(0, 'r', 3, 6);*/
    
    /* input setup */
    
}

/* This function outputs the board as text. */
void print_board(void) {
    int i;
    int j = 7;
    while (j >= 0) {
        i = 0;
        while (i < 8) {
            char c = ']';
            if (xy_get(pieces, i, j)) {
                if (xy_get(kings, i, j)) {
                    c = 'k';
                } else if (xy_get(queens, i, j)) {
                    c = 'q';
                } else if (xy_get(bishops, i, j)) {
                    c = 'b';
                } else if (xy_get(knights, i, j)) {
                    c = 'n';
                } else if (xy_get(rooks, i, j)) {
                    c = 'r';
                } else {
                    c = 'p';
                }
                if (xy_get(whites, i, j)) {
                    printf("-");
                } else {
                    printf("~");
                }
            } else {
                printf("[");
            }
            printf("%c ", c);
            i++;
        }
        printf("\n");
        j--;
    }
}

/*
This function returns whether
there is a piece in the given
bitboard at the given index.
*/
int get(bitboard b, int index) {
    return (int)((b>>index) % 2);
}

/*
This function is the same as
the one above except that it accepts
x and y cordinates instead of an index.
*/
int xy_get(bitboard b, int x, int y) {
    return get(b, x + (y * 8));
}


/*
This function sets a piece onto the
board given a color, type, and index.
*/
void set(int white, char type, int index) {
    if (!(get(pieces, index))) {
        bitboard value = pow(2, index);
        pieces += value;
        if (white) {
            whites += value;
        } else {
            blacks += value;
        }
        if (type == 'k') {
            kings += value;
        } else if (type == 'q') {
            queens += value;
        } else if (type == 'b') {
            bishops += value;
        } else if (type == 'n') {
            knights += value;
        } else if (type == 'r') {
            rooks += value;
        } else {
            pawns += value;
        }
    } else {
        remove_piece(index);
        set(white, type, index);
    }
}

/*
This function is the same as the
one above except that it accepts
x and y cordinates instead of an
index.
*/
void xy_set(int white, char type, int x, int y) {
    set(white, type, x + (y * 8));
}

/*
This function removes a piece
from the given index.
*/
void remove_piece(int index) {
    bitboard value = pow(2, index);
    if (get(pieces, index)) {
        pieces -= value;
    }
    if (get(whites, index)) {
        whites -= value;
    }
    if (get(blacks, index)) {
        blacks -= value;
    }
    if (get(kings, index)) {
        kings -= value;
    }
    if (get(queens, index)) {
        queens -= value;
    }
    if (get(bishops, index)) {
        bishops -= value;
    }
    if (get(knights, index)) {
        knights -= value;
    }
    if (get(rooks, index)) {
        rooks -= value;
    }
    if (get(pawns, index)) {
        pawns -= value;
    }
}

/* This function moves a piece. */
void move(int initial, int final) {

    /* castling */
    if (get(kings, initial)) {
    
        /* castling for white */
        if (initial == 4) {
         
            /* queenside */
            if (final == 2) {
                remove_piece(0);
                set(1, 'k', 2);
                set(1, 'r', 3);
            
            /* kingside */
            } else if (final == 6) {
                remove_piece(7);
                set(1, 'k', 6);
                set(1, 'r', 5);
            }
        }
    
        /* black does not castle */
        /*if (initial == 60) {
            
            if (final == 58) {
                remove_piece(56);
                set(0, 'k', 58);
                set(0, 'r', 59);
                
            } else if (final == 62) {
                remove_piece(7);
                set(0, 'k', 62);
                set(0, 'r', 61);
            }
        }*/
    }
    
    /* performs all other moves */
    int white = 0;
    char type = 'k';
    if (get(whites, initial)) {
        white = 1;
    }
    char c;
    if (get(kings, initial)) {
        c = 'k';
    } else if (get(queens, initial)) {
        c = 'q';
    } else if (get(bishops, initial)) {
        c = 'b';
    } else if (get(knights, initial)) {
        c = 'n';
    } else if (get(rooks, initial)) {
        c = 'r';
    } else {
        c = 'p';
        if (final >= 56 || final < 8) {
            c = 'q';
        }
    }
    set(white, c, final);
    remove_piece(initial);
}

/*
This function is the same as
the one above except that it accepts
x and y cordinates instead of an index.
*/
void xy_move(int initial_x, int initial_y, int final_x, int final_y) {
    move(initial_x + (8 * initial_y), final_x + (8 * final_y));
}

/* This functions updates possible moves for a particular color. */
void update_options(int white) {
    bitboard color = blacks;
    if (white) {
        color = whites;
    }
    int index = 0;
    while (index < 64) {
        if (get(pieces, index) && get(color, index)) {
            if (get(kings, index)) {
                options_king(color, index);
            } else if (get(queens, index)) {
                options_queen(color, index);
            } else if (get(bishops, index)) {
                options_bishop(color, index);
            } else if (get(knights, index)) {
                options_knight(color, index);
            } else if (get(rooks, index)) {
                options_rook(color, index);
            } else {
                options_pawn(color, index);
            }
        } else {
            options[index] = 0;
        }
        index++;
    }
}

/*
This function removes all moves
into check for a particular color.
*/
void check(int white) {

    /* will be used to save original position */
    bitboard old_pieces;
    bitboard old_whites;
    bitboard old_blacks;
    bitboard old_kings;
    bitboard old_queens;
    bitboard old_bishops;
    bitboard old_knights;
    bitboard old_rooks;
    bitboard old_pawns;
    
    /* copies array of options */
    bitboard possible_options[64];
    int index = 0;
    while (index < 64) {
        possible_options[index] = options[index];
        index++;
    }
    
    /* iterates through available options */
    int destination;
    for (index = 0; index < 64; index++) {
        if (possible_options[index] != 0) {
            for (destination = 0; destination < 64; destination++) {
                if (get(possible_options[index], destination)) {
                
                    /* saves original position */
                    old_pieces = pieces;
                    old_whites = whites;
                    old_blacks = blacks;
                    old_kings = kings;
                    old_queens = queens;
                    old_bishops = bishops;
                    old_knights = knights;
                    old_rooks = rooks;
                    old_pawns = pawns;
                    
                    /* performs move */
                    move(index, destination);
                    
                    /* generates possible responses */
                    update_options(!white);

                    /* iterates through those responses and deletes moves that lead to a king capture */
                    int done = 0;
                    int initial, final;
                    for (initial = 0; initial < 64; initial++) {
                        if (options[initial] != 0) {
                            for (final = 0; final < 64; final++) {
                                if (get(options[initial], final)) {
                                    if (get(kings, final)) {
                                        if (!done) {
                                            possible_options[index] -= (bitboard)pow(2, destination);
                                            done = 1;
                                        }
                                    }
                                }
                            }
                        }
                    }
                    
                    /* undoes last move */
                    pieces = old_pieces;
                    whites = old_whites;
                    blacks = old_blacks;
                    kings = old_kings;
                    queens = old_queens;
                    bishops = old_bishops;
                    knights = old_knights;
                    rooks = old_rooks;
                    pawns = old_pawns;
                }
            }
        }
    }
    
    /* reloads original possibilities */
    index = 0;
    while (index < 64) {
        options[index] = possible_options[index];
        index++;
    }
}

/*
This function returns whether or not the king is
in check and is used to find checkmate opportunities.
*/
int check_mate(int white) {
    update_options(white);
    int initial, final;
    for (initial = 0; initial < 64; initial++) {
        if (options[initial] != 0) {
            for (final = 0; final < 64; final++) {
                if (get(options[initial], final)) {
                    if (get(kings, final)) {
                        return 1;
                    }
                }
            }
        }
    }
    return 0;
}

/* updates options for a king on the board */
void options_king(bitboard color, int index) {

    /* old method */
    /*bitboard moves = 460039; //7+256+1024+65536+131072+262144;
    int right_shifts = 9 - index;
    if (right_shifts >= 0) {
        moves = moves>>right_shifts;
    } else {
        moves = moves<<(-right_shifts);
    }
    moves = moves & (~color);
    options[index] = moves;*/
    
    bitboard moves = 0;
    int king_x = (index % 8);
    int king_y = (index / 8);
    int x;
    int y;
    for (x = king_x - 1; x < king_x + 2; x++) {
        for (y = king_y - 1; y < king_y + 2; y++) {
            if (x >= 0 && x < 8 && y >= 0 && y < 8) {
                int i = (y * 8) + x;
                moves += (bitboard)pow(2, i);
            }
        }
    }
    moves = moves & (~color);
    options[index] = moves;
}

/* updates options for a queen */
void options_queen(bitboard color, int index) {
    options_rook(color, index);
    bitboard value = options[index];
    options_bishop(color, index);
    options[index] += value;
}

/* updates options for a bishop */
void options_bishop(bitboard color, int index) {
    bitboard value;
    bitboard moves = 0;
    int i = index - 9;
    while ((i % 8) < (index % 8) && i >= 0) {
        value = (bitboard)pow(2, i);
        if ((value & pieces) != 0) {
            if ((value & color) != 0) {
                break;
            } else {
                moves += value;
                break;
            }
        } else {
            moves += value;
        }
        i -= 9;
    }
    i = index + 7;
    while ((i % 8) < (index % 8) && i < 64) {
        value = (bitboard)pow(2, i);
        if ((value & pieces) != 0) {
            if ((value & color) != 0) {
                break;
            } else {
                moves += value;
                break;
            }
        } else {
            moves += value;
        }
        i += 7;
    }
    i = index - 7;
    while ((i % 8) > (index % 8) && i >= 0) {
        value = (bitboard)pow(2, i);
        if ((value & pieces) != 0) {
            if ((value & color) != 0) {
                break;
            } else {
                moves += value;
                break;
            }
        } else {
            moves += value;
        }
        i -= 7;
    }
    i = index + 9;
    while ((i % 8) > (index % 8) && i < 64) {
        value = (bitboard)pow(2, i);
        if ((value & pieces) != 0) {
            if ((value & color) != 0) {
                break;
            } else {
                moves += value;
                break;
            }
        } else {
            moves += value;
        }
        i += 9;
    }

    options[index] = moves;
}

/* updates options for a knight */
void options_knight(bitboard color, int index) {
    bitboard moves = 43234889994; //for index 18;
    if ((index % 8) == 0) {
        moves -= 2;
        moves -= pow(2, 33);
    }
    if ((index % 8) <= 1) {
        moves -= pow(2, 8);
        moves -= pow(2, 24);
    }
    if ((index % 8) == 7) {
        moves -= pow(2, 3);
        moves -= pow(2, 35);
    }
    if ((index % 8) >= 6) {
        moves -= pow(2, 12);
        moves -= pow(2, 28);
    }
    int right_shifts = 18 - index;
    if (right_shifts >= 0) {
        moves = moves>>right_shifts;
    } else {
        moves = moves<<(-right_shifts);
    }
    moves = moves & (~color);
    options[index] = moves;
}

/* updates options for a rook */
void options_rook(bitboard color, int index) {
    bitboard value;
    bitboard moves = 0;
    int i = index - 1;
    while (i != -1 && (i % 8) < (index % 8)) {
        value = (bitboard)pow(2, i);
        if ((value & pieces) != 0) {
            if ((value & color) != 0) {
                break;
            } else {
                moves += value;
                break;
            }
        } else {
            moves += value;
        }
        i--;
    }
    i = index + 1;
    while (i != 64 && i % 8 > index % 8) {
        value = (bitboard)pow(2, i);
        if ((value & pieces) != 0) {
            if ((value & color) != 0) {
                break;
            } else {
                moves += value;
                break;
            }
        } else {
            moves += value;
        }
        i++;
    }
    i = index - 8;
    while (i >= 0) {
        value = (bitboard)pow(2, i);
        if ((value & pieces) != 0) {
            if ((value & color) != 0) {
                break;
            } else {
                moves += value;
                break;
            }
        } else {
            moves += value;
        }
        i -= 8;
    }
    i = index + 8;
    while (i < 64) {
        value = (bitboard)pow(2, i);
        if ((value & pieces) != 0) {
            if ((value & color) != 0) {
                break;
            } else {
                moves += value;
                break;
            }
        } else {
            moves += value;
        }
        i += 8;
    }
    options[index] = moves;
}

/*
This function updates options for a pawn.
Note that en passant is not allowed yet.
*/
void options_pawn(bitboard color, int index) {
    bitboard value = (bitboard)pow(2, index);
    if (get(whites, index)) {
        bitboard moves_forward = value<<8;
        moves_forward = moves_forward & (~pieces);
        if (index >= 8 && index < 16 && moves_forward != 0) {
            moves_forward += (value<<16) & (~pieces);
        }
        bitboard moves_slant = 0;
        if (index % 8 != 0) {
            moves_slant += value<<7;
        }
        if (index % 8 != 7) {
            moves_slant += value<<9;
        }
        moves_slant = moves_slant & pieces & (~color);
        options[index] = moves_forward + moves_slant;
    } else {
        bitboard moves_forward = value>>8;
        moves_forward = moves_forward & (~pieces);
        if (index < 56 && index >= 48 && moves_forward != 0) {
            moves_forward += (value>>16) & (~pieces);
        }
        bitboard moves_slant = 0;
        if (index % 8 != 0) {
            moves_slant += value>>9;
        }
        if (index % 8 != 7) {
            moves_slant += value>>7;
        }
        moves_slant = moves_slant & pieces & (~color);
        options[index] = moves_forward + moves_slant;
    }
}

/* 
This function returns a pieces value.  
Note that in some parts of the program
a value of 0 is given to the king instead
of 1000000.
*/
int valueOf(int index) {
    if (get(kings, index)) {
        return 1000000;
    } else if (get(queens, index)) {
        return 900000;
    } else if (get(bishops, index)) {
        return 300000;
    } else if (get(knights, index)) {
        return 300000;
    } else if (get(rooks, index)) {
        return 500000;
    } else {
        return 100000;
    }
}













