/* ensures BOARD_H is only defined once */
#ifndef BOARD_H
#define BOARD_H

/* type definition for a bit board */
typedef unsigned long int bitboard;

/* global variables */
extern bitboard options[64];
extern bitboard pieces;
extern bitboard whites;
extern bitboard blacks;
extern bitboard kings;
extern bitboard queens;
extern bitboard bishops;
extern bitboard knights;
extern bitboard rooks;
extern bitboard pawns;

/* global functions */
void initialize_board(void);
void print_board(void);
int get(bitboard b, int index);
int xy_get(bitboard b, int x, int y);
void move(int initial, int final_y);
void xy_move(int initial_x, int initial_y, int final_x, int final_y);
void update_options(int white);
void check(int white);
int check_mate(int white);
int valueOf(int index);

#endif