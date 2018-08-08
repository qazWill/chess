#include "stdio.h"
#include "board.h"
#include "math.h"
#include "time.h"

/* global variables */
int user_is_white = 1;
int maximum_depth = 16;
int strategic_depth_maximum = 4;
int check_depth_maximum = 16;
int strategic_depth = 0;
int check_depth = 0;
clock_t start;
clock_t current;
int maximum_time;
int remaining_time = 3 * 60;
int time_gain = 5;
int last_rating = 0;
int enableQuietSearch = 1;
int number_of_pieces = 0;


/* function declerations */
void move_user(void);
void move_opponent(void);
int valid(int white, int initial_x, int initial_y, int final_x, int final_y);
int static_rating();
int strategic_rating();
int maximize_rating(int white, int depth, int alpha, int beta);
int minimize_rating(int white, int depth, int alpha, int beta);
int quiet_maximize_rating(int white, int alpha, int beta);
int quiet_minimize_rating(int white, int alpha, int beta);
int move_order_rating(int white, int initial, int final);

void move_user(void) {

    /* displays chess board */
    printf("\n-------Current Board-------\n");
    print_board();
    
    /* prompts user to enter move */
    printf("\nWhere will you move? >>> ");
    
    /* recieves and converts input into coordinates */
    int initial_x = getchar() - 'a';
    int initial_y = getchar() - '0' - 1;
    getchar();
    int final_x = getchar() - 'a';
    int final_y = getchar() - '0' - 1;
    getchar();
    
    /* tests to see if the move is valid */
    if (valid(user_is_white, initial_x, initial_y, final_x, final_y)) {
    
        /* performs move */
        xy_move(initial_x, initial_y, final_x, final_y);
        
    /* if invalid */
    } else {
    
        /* informs user of problem */
        printf("\nThat move is invalid.\n");
        
        /* asks user for a different move */
        move_user();
    }
}

/*
This function calculates a response to the user's move.
*/
void move_opponent(void) {

    /* calculates remaining time */
    remaining_time += time_gain;
    
    /* calculates how much time to use */
    maximum_time = remaining_time / 30;
    
    /* saves current time */
    start = clock();
    current = start;
    
    /* informs user that program is beginning to think */
    printf("\nI am considering my options...\n");
    
    /* temporarily disables quiet search if needed */
    int desiredEnableQuietSearch = enableQuietSearch;
    enableQuietSearch = 0;
    
    /* counts opponent pieces */
    number_of_pieces = 0;
    int x, y;
    for (x = 0; x < 8; x++) {
        for (y = 0; y < 8; y++) {
            if (xy_get(pieces, x, y)) {
                if (xy_get(whites, x, y)) {
                    number_of_pieces++;
                }
            }
        }
    }
    
    /* generates possible options */
    update_options(!user_is_white);
    
    /* removes moves into check */
    check(!user_is_white);
    
    /* saves options */
    bitboard possible_options[64];
    int index = 0;
    while (index < 64) {
        possible_options[index] = options[index];
        index++;
    }
    
    /* variables and settings needed in the search */
    int depth = 2;
    check_depth = depth - check_depth_maximum;
    strategic_depth = depth - strategic_depth_maximum + 1;
    if (strategic_depth < 1) {
        strategic_depth = 1;
    }
    int alpha = -9999999;
    int beta = 9999999;
    int rating;
    int destination;
    int i;

    /* used for storing the board */
    bitboard old_pieces;
    bitboard old_whites;
    bitboard old_blacks;
    bitboard old_kings;
    bitboard old_queens;
    bitboard old_bishops;
    bitboard old_knights;
    bitboard old_rooks;
    bitboard old_pawns;
    
    /* array used to store order 
    of moves to evaluate */
    int order_size = 0;
    for (index = 0; index < 64; index++) {
        if (possible_options[index] != 0) {
            for (destination = 0; destination < 64; destination++) {
                if (get(possible_options[index], destination)) {
                    order_size++;
                }
            }
        }
    }
    int order_initial[order_size];
    int order_final[order_size];
    int order_ratings[order_size];
    
    /* evaluates at depth 2 with no quiet search */
    int size = 0;
    for (index = 0; index < 64; index++) {
        if (possible_options[index] != 0) {
            for (destination = 0; destination < 64; destination++) {
                if (get(possible_options[index], destination)) {
                    
                    /* save original position */
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
                    
                    /* rates move */
                    rating = maximize_rating(user_is_white, depth - 1, alpha, beta);
                    
                    /* strategicly rates move */
                    if (depth == strategic_depth) {
                        rating += strategic_rating();
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
                    
                    /* updates beta */
                    if (rating < beta) {
                        beta = rating;
                    }
                    
                    /* updates ordered list of moves */
                    int x;
                    for (x = 0; x < size + 1; x++) {
                        if (x == size) {
                            order_initial[x] = index;
                            order_final[x] = destination;
                            order_ratings[x] = rating;
                            break;
                        } else {
                            if (order_ratings[x] > rating) {
                                
                                /* shift elements right at x */
                                int x2;
                                for (int x2 = size; x2 > x; x2--) {
                                    order_initial[x2] = order_initial[x2 - 1];
                                    order_final[x2] = order_final[x2 - 1];
                                    order_ratings[x2] = order_ratings[x2 - 1];
                                }
                                
                                /* insert new element */
                                order_initial[x] = index;
                                order_final[x] = destination;
                                order_ratings[x] = rating;
                                
                                /* exits loop */
                                break;
                            }
                        }
                    }
                    size++;
                }
            }
        }
    }
    
    /* temporary order of evaluation */
    int order_initial2[order_size];
    int order_final2[order_size];
    int order_ratings2[order_size];
    
    /* evaluates at continually greater depths */
    enableQuietSearch = desiredEnableQuietSearch;
    int achieved = maximum_depth;
    int achieved_specific = order_size;
    int depth_change = 0;
    for (depth = 1; depth < maximum_depth + 1; depth += 1) {
        
        /* detmines settings for this search */
        check_depth = depth - check_depth_maximum;
        strategic_depth = depth - strategic_depth_maximum + 1;
        if (strategic_depth < 1) {
            strategic_depth = 1;
        }
        alpha = -9999999;
        beta = 9999999;
        size = 0;
        for (i = 0; i < order_size; i++) {
        
            /* extended depth, not used currently */
            int r = move_order_rating(!user_is_white, order_initial[i], order_final[i]);
            if (r < 100) { // other
                depth_change = 0;
            } else if (r < 200) { // one threat
                depth_change = 0;
            } else if (r < 300) { // two threats
                depth_change = 0;
            } else { // three or more threats or capture
                depth_change = 0;
            }
            depth_change = 0;
        
            /* save original position */
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
            move(order_initial[i], order_final[i]);
            
            /* rates move */
            rating = maximize_rating(user_is_white, depth + depth_change - 1, alpha, beta);
            
            /* 
            rates move strategically,
            this is only considered
            if material ratings are
            the exact same
            */
            if (depth == strategic_depth) {
                rating += strategic_rating();
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
            
            /* updates beta */
            if (rating < beta) {
                beta = rating;
            }
            
            /* rating is unreliable if time has elasped */
            if ((current - start) / CLOCKS_PER_SEC > maximum_time) {
                if (achieved_specific == order_size) {
                    achieved_specific = i;
                }
                rating = order_ratings[size];
            }
            
            /* updates list of moves */
            int x;
            for (x = 0; x < size + 1; x++) {
                if (x == size) {
                    order_initial2[x] = order_initial[i];
                    order_final2[x] = order_final[i];
                    order_ratings2[x] = rating;
                    break;
                } else {
                    if (order_ratings2[x] > rating) {
                        
                        /* shift elements right at x */
                        int x2;
                        for (int x2 = size; x2 > x; x2--) {
                            order_initial2[x2] = order_initial2[x2 - 1];
                            order_final2[x2] = order_final2[x2 - 1];
                            order_ratings2[x2] = order_ratings2[x2 - 1];
                        }
                        
                        /* insert new element */
                        order_initial2[x] = order_initial[i];
                        order_final2[x] = order_final[i];
                        order_ratings2[x] = rating;
                        
                        /* exits loop */
                        break;
                    }
                }
            }
            size++;
            
        }
        
        /* copies finished part of order2 into order1 */
        int x;
        for (x = 0; x < size; x++) {
            order_initial[x] = order_initial2[x];
            order_final[x] = order_final2[x];
            order_ratings[x] = order_ratings2[x];
        }
        
        /* stops search when out of time */
        if ((current - start) / CLOCKS_PER_SEC > maximum_time) {
            achieved = depth;
            break;
        }
    }
    
    /* updates remaining time */
    remaining_time -= (current - start) / CLOCKS_PER_SEC;
    
    /* outputs search results */
    printf("\n----My Search Results----\n");
    printf("time spent >>> %d seconds\n", (int)((current - start) / CLOCKS_PER_SEC));
    printf("time remaining >>> %d minutes\n", remaining_time / 60);
    printf("time remaining >>> %d seconds\n", remaining_time % 60);
    printf("maximum depth achieved >>> %d\n", achieved);
    printf("progress achieved >>> %d of %d\n", achieved_specific, order_size);
    
    /* outputs information about computers percieved advantage, not used currently */
    /*printf("\n----My Estimated Advantage----\n");
    int change = -(order_ratings[0] - last_rating);
    last_rating = order_ratings[0];
    printf("total material >>> %d\n", -static_rating());
    printf("total strategic >>> %d\n", -strategic_rating());
    printf("expected change >>> %d\n", change);*/
    
    /* performs the best move */
    int best_initial = order_initial[0];
    int best_final = order_final[0];
    move(best_initial, best_final);
    
    /* talks to user, I used this for testing */
    /*if (change >= 450000) {
        printf("\nI think that I have found an very good move.\n");
    } else if (change >= 250000) {
        printf("\nI think that I have found a good move.\n");
    } else if (change >= 50000) {
        printf("\nI think that I have found a decent move.\n");
    } else if (change <= -50000) {
        printf("\nI think that I made a mistake.\n");
    } else if (change <= -250000) {
        printf("\nI think that I made a bad mistake.\n");
    } else if (change <= -450000) {
        printf("\nI think that I am made an very bad mistake.\n");
    }*/
    
    /* displays move to user */
    printf("\nI will move from %c%d to %c%d.\n",
           (char)((int)'a' + (best_initial % 8)),
           1 + (best_initial / 8),
           (char)((int)'a' + (best_final % 8)),
           1 + (best_final / 8));
}

/* 
This functions returns whether a user move is valid.
*/
int valid(int white, int initial_x, int initial_y, int final_x, int final_y) {

    /* castling */
    if (white) {
        if (initial_x == 4 && initial_y == 0 && final_y == 0) {
            if (final_x == 6 || final_x == 2) {
                return 1;
            }
        }
    }
    
    /* generates possible moves */
    update_options(white);
    
    /* deletes moves into check */
    check(white);

    /* tests to see if move is possible */
    int initial_index = initial_x + (initial_y * 8);
    int final_index = final_x + (final_y * 8);
    if (get(options[initial_index], final_index)) {
    
        /* returns valid */
        return 1;
    }
    
    /* returns invalid */
    return 0;
}

/*
This functions estimates the material advantage in terms of the user.
*/
int static_rating() {
    int rating = 0;
    int index = 0;
    while (index < 64) {
        if (get(pieces, index)) {
            if (get(whites, index)) {
                if (get(kings, index)) {
                    rating += 0;
                } else if (get(queens, index)) {
                    rating += 900000;
                } else if (get(bishops, index)) {
                    rating += 300000;
                } else if (get(knights, index)) {
                    rating += 300000;
                } else if (get(rooks, index)) {
                    rating += 500000;
                } else {
                    rating += 100000;
                }
            } else {
                if (get(kings, index)) {
                    rating -= 0;
                } else if (get(queens, index)) {
                    rating -= 900000;
                } else if (get(bishops, index)) {
                    rating -= 300000;
                } else if (get(knights, index)) {
                    rating -= 300000;
                } else if (get(rooks, index)) {
                    rating -= 500000;
                } else {
                    rating -= 100000;
                }
            }
        }
        index++;
    }
    
    /* used for checkmating tactics against user */
    int x, y;
    if (number_of_pieces == 1) {
        int king_x = 0;
        int king_y = 0;
        for (x = 0; x < 8; x++) {
            for (y = 0; y < 8; y++) {
                if (xy_get(pieces, x, y)) {
                    if (xy_get(whites, x, y)) {
                        if (xy_get(kings, x, y)) {
                            king_x = x;
                            king_y = y;
                            rating -= (int)(1000 * sqrt(pow(3.5 - x, 2) + pow(3.5 - y, 2)));
                        }
                    }
                }
            }
        }
        for (x = 0; x < 8; x++) {
            for (y = 0; y < 8; y++) {
                if (xy_get(pieces, x, y)) {
                    if (!xy_get(whites, x, y)) {
                        if (!xy_get(pawns, x, y)) {
                            rating += (int)(100 * sqrt(pow(king_x - x, 2) + pow(king_y - y, 2)));
                        } else {
                            rating += y * 200;
                        }
                    }
                }
            }
        }
    }

    /* tests to see if the user is playing as white */
    if (user_is_white) {
    
        /* returns in terms of white */
        return rating;
    }
    
    /* returns in terms of black */
    return -rating;
}

/* estimates the strategic advantage of the user */
int strategic_rating() {
    int rating = 0;
    
    /*
    adjusts rating based on the targets and open
    destinations that white has
    */
    update_options(1);
    check(1);
    int initial;
    int final;
    for (initial = 0; initial < 64; initial++) {
        if (options[initial] != 0) {
            for (final = 0; final < 64; final++) {
                if (get(options[initial], final)) {
                
                    /* it is good to maximize number of options */
                    rating += 1;
                    
                    /* it is good to have as many targets as possible */
                    if (get(pieces, final)) {
                        if (get(kings, final)) {
                            rating += 0;
                        } else if (get(queens, final)) {
                            rating += 18;
                        } else if (get(bishops, final)) {
                            rating += 6;
                        } else if (get(knights, final)) {
                            rating += 6;
                        } else if (get(rooks, final)) {
                            rating += 10;
                        } else {
                            rating += 2;
                        }
                    }
                }
            }
        }
    }
    
    /* 
    adjusts rating based on the targets and open
    destinations that black has
    */
    update_options(0);
    check(0);
    for (initial = 0; initial < 64; initial++) {
        if (options[initial] != 0) {
            for (final = 0; final < 64; final++) {
                if (get(options[initial], final)) {
                    
                    /* it is good to maximize number of options */
                    rating -= 1;
                    
                    /* it is good to have as many targets as possible */
                    if (get(pieces, final)) {
                        if (get(kings, final)) {
                            rating -= 0;
                        } else if (get(queens, final)) {
                            rating -= 18;
                        } else if (get(bishops, final)) {
                            rating -= 6;
                        } else if (get(knights, final)) {
                            rating -= 6;
                        } else if (get(rooks, final)) {
                            rating -= 10;
                        } else {
                            rating -= 2;
                        }
                    }
                }
            }
        }
    }

    
    /* rewards having pieces near the center
    and rewards pawn advancement */
    int x, y;
    for (x = 0; x < 8; x++) {
        for (y = 0; y < 8; y++) {
            if (xy_get(pieces, x, y)) {
                if (xy_get(whites, x, y)) {
                    if (!xy_get(pawns, x, y)) {
                        rating -= (int)(3 * (pow(3.5 - x, 2) + pow(3.5 - y, 2)));
                    } else {
                        rating += 5 * y;
                    }
                } else {
                    if (!xy_get(pawns, x, y)) {
                        rating += (int)(3 * (pow(3.5 - x, 2) + pow(3.5 - y, 2)));
                    } else {
                        rating += 5 * y;
                    }
                }
            }
        }
    }
    
    /* discourages the isolated pawn */
    int h;
    for (x = 0; x < 8; x++) {
        for (y = 0; y < 8; y++) {
            if (xy_get(pawns, x, y)) {
                int adjacent = 0;
                if (xy_get(whites, x, y)) {
                    if (x != 0) {
                        for (h = 0; h < 8; h++) {
                            if (xy_get(pawns, x - 1, h)) {
                                if (xy_get(whites, x - 1, h)) {
                                    adjacent = 1;
                                    break;
                                }
                            }
                        }
                    }
                    if (!adjacent && x != 7) {
                        for (h = 0; h < 8; h++) {
                            if (xy_get(pawns, x + 1, h)) {
                                if (xy_get(whites, x + 1, h)) {
                                    adjacent = 1;
                                    break;
                                }
                            }
                        }
                    }
                    if (!adjacent) {
                        rating -= 25000;
                    }
                } else {
                    if (x != 0) {
                        for (h = 0; h < 8; h++) {
                            if (xy_get(pawns, x - 1, h)) {
                                if (xy_get(blacks, x - 1, h)) {
                                    adjacent = 1;
                                    break;
                                }
                            }
                        }
                    }
                    if (!adjacent && x != 7) {
                        for (h = 0; h < 8; h++) {
                            if (xy_get(blacks, x + 1, h)) {
                                if (xy_get(blacks, x + 1, h)) {
                                    adjacent = 1;
                                    break;
                                }
                            }
                        }
                    }
                    if (!adjacent) {
                        rating += 25000;
                    }
                }
            }
        }
    }
    
    /* discourages doubled pawns */
    int white_count;
    int black_count;
    for (x = 0; x < 8; x++) {
        white_count = 0;
        black_count = 0;
        for (y = 0; y < 8; y++) {
            if (xy_get(pawns, x, y)) {
                if (xy_get(whites, x, y)) {
                    white_count++;
                } else {
                    black_count++;
                }
            }
        }
        if (white_count > 1) {
            rating -= 12500 * white_count;
        }
        if (black_count > 1) {
            rating += 12500 * black_count;
        }
    }
    
    /* increasing percentage of your main pieces is rated as very benificial */
    double white_primary = 0;
    double black_primary = 0;
    for (x = 0; x < 8; x++) {
        for (y = 0; y < 8; y++) {
            if (xy_get(pieces, x, y)) {
                if (!xy_get(pawns, x, y)) {
                    if (xy_get(whites, x, y)) {
                        if (xy_get(kings, x, y)) {
                            white_primary += 0;
                        } else if (xy_get(queens, x, y)) {
                            white_primary += 9;
                        } else if (xy_get(bishops, x, y)) {
                            white_primary += 3;
                        } else if (xy_get(knights, x, y)) {
                            white_primary += 3;
                        } else if (xy_get(rooks, x, y)) {
                            white_primary += 5;
                        } else {
                            white_primary += 0;
                        }
                    } else {
                        if (xy_get(kings, x, y)) {
                            black_primary += 0;
                        } else if (xy_get(queens, x, y)) {
                            black_primary += 9;
                        } else if (xy_get(bishops, x, y)) {
                            black_primary += 3;
                        } else if (xy_get(knights, x, y)) {
                            black_primary += 3;
                        } else if (xy_get(rooks, x, y)) {
                            black_primary += 5;
                        } else {
                            black_primary += 0;
                        }
                    }
                }
            }
        }
    }
    rating += (int)(12500 * (double)(white_primary / (double)(white_primary + black_primary)));
    
    
    /* tests to see if the user is playing as white */
    if (user_is_white) {
        
        /* returns in terms of white */
        return rating;
    }
    
    /* returns in terms of black */
    return -rating;
}

/*
This function searches for the move that maximizes the user's advantage.
It uses the alpha beta pruning method and move ordering.
*/
int maximize_rating(int white, int depth, int alpha, int beta) {

    /* loads current time */
    current = clock();
    
    /* returns from search if time has expired */
    if ((current - start) / CLOCKS_PER_SEC > maximum_time) {
        return 0;
    }
    
    /* ends the main search at depth 0 */
    if (depth <= 0) {
        
        /* tests to see if quiet search is enabled */
        if (enableQuietSearch) {
        
            /* searches for a quiet position and uses that as the score */
            return quiet_maximize_rating(white, alpha, beta);
            
        /* if disabled */
        } else {
        
            /* returns the score of this position */
            return static_rating();
        }
    }
    
    /* variable declarations and some initial values */
    int best_rating = -9999999;
    int rating;
    int destination;
    int i;
    int size = 0;
    int index = 0;
    
    /* generates moves */
    update_options(white);
    
    /* deletes moves into check */
    check(white);
    
    /* saves possible moves */
    bitboard possible_options[64];
    while (index < 64) {
        possible_options[index] = options[index];
        index++;
    }
    
    /* creates array used to store order in which to evaluate moves */
    int order_size = 0;
    for (index = 0; index < 64; index++) {
        if (possible_options[index] != 0) {
            for (destination = 0; destination < 64; destination++) {
                if (get(possible_options[index], destination)) {
                    order_size++;
                }
            }
        }
    }
    int order_initial[order_size];
    int order_final[order_size];
    int order_ratings[order_size];


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
    
    /* guesses which moves will be best */
    for (index = 0; index < 64; index++) {
        if (possible_options[index] != 0) {
            for (destination = 0; destination < 64; destination++) {
                if (get(possible_options[index], destination)) {
                    
                    /* rates move */
                    rating = move_order_rating(white, index, destination); // replace!
                    
                    /* updates list of moves */
                    int x;
                    for (x = 0; x < size + 1; x++) {
                        if (x == size) {
                            order_initial[x] = index;
                            order_final[x] = destination;
                            order_ratings[x] = rating;
                            break;
                        } else {
                            if (order_ratings[x] < rating) {
                                
                                /* shift elements right at x */
                                int x2;
                                for (int x2 = size; x2 > x; x2--) {
                                    order_initial[x2] = order_initial[x2 - 1];
                                    order_final[x2] = order_final[x2 - 1];
                                    order_ratings[x2] = order_ratings[x2 - 1];
                                }
                                
                                /* insert new element */
                                order_initial[x] = index;
                                order_final[x] = destination;
                                order_ratings[x] = rating;
                                
                                /* exits loop */
                                break;
                            }
                        }
                    }
                    size++;
                }
            }
        }
    }
    
    /* iterates through all moves */
    for (i = 0; i < order_size; i++) {
        
        /* save original position */
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
        move(order_initial[i], order_final[i]);
        
        /* rates move in terms of material */
        rating = minimize_rating(!white, depth - 1, alpha, beta);
        
        /* rates move strategically */
        if (strategic_depth == depth) {
            rating += strategic_rating();
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
        
        /* updates alpha and beta as well as the best rating */
        if (rating > best_rating) {
            best_rating = rating;
        }
        if (rating > alpha) {
            alpha = rating;
        }
        if (alpha + 5000 >= beta) { // may need to add to eliminate strategic influence
            return 9999999;
        }
    }
    
    /* tests for check mate and stale mate */
    if (order_size == 0) {
        if (check_mate(!white)) {
            return -8888888 - (depth * 10000);
        } else {
            return 7777777;
        }
    }
    return best_rating;
}

/*
This function searches for the move that minimizes the user's advantage.
It uses the alpha beta pruning method and move ordering.
*/
int minimize_rating(int white, int depth, int alpha, int beta) {
    current = clock();
    if ((current - start) / CLOCKS_PER_SEC > maximum_time) {
        return 0;
    }
    if (depth <= 0) {
        if (enableQuietSearch) {
            return quiet_minimize_rating(white, alpha, beta);
        } else {
            return static_rating();
        }
    }
    
    /* variable declarations */
    int best_rating = 9999999;
    int rating;
    int destination;
    int i;
    int size = 0;
    int index = 0;;
    
    /* updates possible options */
    update_options(white);
    
    /* deletes moves into check */
    check(white);
    
    /* saves all possible options */
    bitboard possible_options[64];
    while (index < 64) {
        possible_options[index] = options[index];
        index++;
    }
    
    /* creates arrays in which the order of the moves to evaluate is stored */
    int order_size = 0;
    for (index = 0; index < 64; index++) {
        if (possible_options[index] != 0) {
            for (destination = 0; destination < 64; destination++) {
                if (get(possible_options[index], destination)) {
                    order_size++;
                }
            }
        }
    }
    int order_initial[order_size];
    int order_final[order_size];
    int order_ratings[order_size];
    
    bitboard old_pieces;
    bitboard old_whites;
    bitboard old_blacks;
    bitboard old_kings;
    bitboard old_queens;
    bitboard old_bishops;
    bitboard old_knights;
    bitboard old_rooks;
    bitboard old_pawns;
    for (index = 0; index < 64; index++) {
        if (possible_options[index] != 0) {
            for (destination = 0; destination < 64; destination++) {
                if (get(possible_options[index], destination)) {
                    
                    /* rates move */
                    rating = move_order_rating(white, index, destination); // replace!
                    
                    /* updates list of moves */
                    int x;
                    for (x = 0; x < size + 1; x++) {
                        if (x == size) {
                            order_initial[x] = index;
                            order_final[x] = destination;
                            order_ratings[x] = rating;
                            break;
                        } else {
                            if (order_ratings[x] < rating) {
                                
                                /* shift elements right at x */
                                int x2;
                                for (int x2 = size; x2 > x; x2--) {
                                    order_initial[x2] = order_initial[x2 - 1];
                                    order_final[x2] = order_final[x2 - 1];
                                    order_ratings[x2] = order_ratings[x2 - 1];
                                }
                                
                                /* insert new element */
                                order_initial[x] = index;
                                order_final[x] = destination;
                                order_ratings[x] = rating;
                                
                                /* exits loop */
                                break;
                            }
                        }
                    }
                    size++;
                }
            }
        }
    }
    
    for (i = 0; i < order_size; i++) {
        
        /* save original position */
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
        move(order_initial[i], order_final[i]);
        
        /* rates move in terms of material */
        rating = maximize_rating(!white, depth - 1, alpha, beta);
        
        /* rates move strategically */
        if (strategic_depth == depth) {
            rating += strategic_rating();
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
        
        /* updates alpha and beta as well as the best rating */
        if (rating < best_rating) {
            best_rating = rating;
        }
        if (rating < beta) {
            beta = rating;
        }
        if (alpha + 5000 >= beta) {
            return -9999999;
        }
    }
    
    /* tests for check mate and stale mate */
    if (order_size == 0) {
        if (check_mate(!white)) {
            return 8888888 + (depth * 10000);
        } else {
            return -7777777;
        }
    }
    return best_rating;
}

int quiet_maximize_rating(int white, int alpha, int beta) {

    /* updates time */
    current = clock();
    
    /* ensures that there is still time to search */
    if ((current - start) / CLOCKS_PER_SEC > maximum_time) {
        return 0;
    }
    
    /* evaluates advantage of not capturing */
    int best_rating = static_rating();
    if (best_rating > alpha) {
        alpha = best_rating;
    }
    if (alpha >= beta) {
        return 9999999;
    }
    
    /* updates possible moves */
    update_options(white);
    //check(white);
    bitboard possible_options[64];
    int index = 0;
    while (index < 64) {
        possible_options[index] = options[index];
        index++;
    }
    
    /* creates arrays used to store order of evaluation */
    bitboard old_pieces;
    bitboard old_whites;
    bitboard old_blacks;
    bitboard old_kings;
    bitboard old_queens;
    bitboard old_bishops;
    bitboard old_knights;
    bitboard old_rooks;
    bitboard old_pawns;
    int rating;
    int destination;
    int order_size = 0;
    for (index = 0; index < 64; index++) {
        if (possible_options[index] != 0) {
            for (destination = 0; destination < 64; destination++) {
                if (get(possible_options[index], destination)) {
                    if (get(pieces, destination)) {
                        order_size++;
                    }
                }
            }
        }
    }
    int order_initial[order_size];
    int order_final[order_size];
    int order_ratings[order_size];
    
    /* move ordering */
    int size = 0;
    for (index = 0; index < 64; index++) {
        if (possible_options[index] != 0) {
            for (destination = 0; destination < 64; destination++) {
                if (get(possible_options[index], destination)) {
                    if (get(pieces, destination)) {
                        
                        /* material rating */
                        rating = valueOf(destination) - valueOf(index);
                        
                        /* updates list of moves */
                        int x;
                        for (x = 0; x < size + 1; x++) {
                            if (x == size) {
                                order_initial[x] = index;
                                order_final[x] = destination;
                                order_ratings[x] = rating;
                                break;
                            } else {
                                if (order_ratings[x] < rating) {
                                    
                                    /* shift elements right at x */
                                    int x2;
                                    for (int x2 = size; x2 > x; x2--) {
                                        order_initial[x2] = order_initial[x2 - 1];
                                        order_final[x2] = order_final[x2 - 1];
                                        order_ratings[x2] = order_ratings[x2 - 1];
                                    }
                                    
                                    /* insert new element */
                                    order_initial[x] = index;
                                    order_final[x] = destination;
                                    order_ratings[x] = rating;
                                    
                                    /* exits loop */
                                    break;
                                }
                            }
                        }
                        size++;
                    }
                }
            }
        }
    }
    
    /* iterates through all moves */
    int i;
    for (i = 0; i < order_size; i++) {
        
        /* save old position */
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
        move(order_initial[i], order_final[i]);
        
        /* rates move in terms of material */
        rating = quiet_minimize_rating(!white, alpha, beta);
        
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
        
        /* updates alpha and beta as well as the best rating */
        if (rating > best_rating) {
            best_rating = rating;
        }
        if (rating > alpha) {
            alpha = rating;
        }
        if (alpha >= beta) {
            return 9999999;
        }
    }

    return best_rating;
}

int quiet_minimize_rating(int white, int alpha, int beta) {
    
    /* updates time */
    current = clock();
    
    /* ensures that there is still time to search */
    if ((current - start) / CLOCKS_PER_SEC > maximum_time) {
        return 0;
    }
    
    /* evaluates advantage of not capturing */
    int best_rating = static_rating();
    if (best_rating < beta) {
        beta = best_rating;
    }
    if (alpha >= beta) {
        return -9999999;
    }
    
    /* updates possible moves */
    update_options(white);
    //check(white);
    bitboard possible_options[64];
    int index = 0;
    while (index < 64) {
        possible_options[index] = options[index];
        index++;
    }
    
    /* creates arrays that store move order */
    bitboard old_pieces;
    bitboard old_whites;
    bitboard old_blacks;
    bitboard old_kings;
    bitboard old_queens;
    bitboard old_bishops;
    bitboard old_knights;
    bitboard old_rooks;
    bitboard old_pawns;
    int rating;
    int destination;
    int order_size = 0;
    for (index = 0; index < 64; index++) {
        if (possible_options[index] != 0) {
            for (destination = 0; destination < 64; destination++) {
                if (get(possible_options[index], destination)) {
                    if (get(pieces, destination)) {
                        order_size++;
                    }
                }
            }
        }
    }
    int order_initial[order_size];
    int order_final[order_size];
    int order_ratings[order_size];

    /* move ordering */
    int size = 0;
    for (index = 0; index < 64; index++) {
        if (possible_options[index] != 0) {
            for (destination = 0; destination < 64; destination++) {
                if (get(possible_options[index], destination)) {
                    if (get(pieces, destination)) {
                        
                        rating = valueOf(destination) - valueOf(index);
                        
                        /* updates list of moves */
                        int x;
                        for (x = 0; x < size + 1; x++) {
                            if (x == size) {
                                order_initial[x] = index;
                                order_final[x] = destination;
                                order_ratings[x] = rating;
                                break;
                            } else {
                                if (order_ratings[x] < rating) {
                                    
                                    /* shift elements right at x */
                                    int x2;
                                    for (int x2 = size; x2 > x; x2--) {
                                        order_initial[x2] = order_initial[x2 - 1];
                                        order_final[x2] = order_final[x2 - 1];
                                        order_ratings[x2] = order_ratings[x2 - 1];
                                    }
                                    
                                    /* insert new element */
                                    order_initial[x] = index;
                                    order_final[x] = destination;
                                    order_ratings[x] = rating;
                                    
                                    /* exits loop */
                                    break;
                                }
                            }
                        }
                        size++;
                    }
                }
            }
        }
    }
    
    /* iterates through every move */
    int i;
    for (i = 0; i < order_size; i++) {
        
        /* save old position */
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
        move(order_initial[i], order_final[i]);
        
        /* rates move in terms of material */
        rating = quiet_maximize_rating(!white, alpha, beta);
        
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
        
        /* updates alpha and beta as well as the best rating */
        if (rating < best_rating) {
            best_rating = rating;
        }
        if (rating < beta) {
            beta = rating;
        }
        if (alpha >= beta) {
            return -9999999;
        }
    }
    
    return best_rating;
}


/* 
This function rates a moves potential to be good without doing
a full search.  It is used to improve move ordering.
*/
int move_order_rating(int white, int initial, int final) {
    
    /* will store the final rating */
    int rating = 0;
    
    /* value of the piece being moved */
    int initialValue = valueOf(initial);

    /* finds and iterates through all threats */
    bitboard old_pieces = pieces;
    bitboard old_whites = whites;
    bitboard old_blacks = blacks;
    bitboard old_kings = kings;
    bitboard old_queens = queens;
    bitboard old_bishops = bishops;
    bitboard old_knights = knights;
    bitboard old_rooks = rooks;
    bitboard old_pawns = pawns;
    move(initial, final);
    update_options(white);
    check(white);
    int index;
    int destination;
    int targets = 0;
    int value = 0;
    for (index = 0; index < 64; index++) {
        if (index == final) {
            if (options[index] != 0) {
                for (destination = 0; destination < 64; destination++) {
                    if (get(options[index], destination)) {
                        if (get(pieces, destination)) {
                            targets++;
                            value += valueOf(destination);
                        }
                    }
                }
            }
        }
    }
    pieces = old_pieces;
    whites = old_whites;
    blacks = old_blacks;
    kings = old_kings;
    queens = old_queens;
    bishops = old_bishops;
    knights = old_knights;
    rooks = old_rooks;
    pawns = old_pawns;
    
    /* rates captures */
    if (get(pieces, final)) {
        rating += valueOf(final) / 100000;
        rating += 1000;
        
    /* rates threats */
    } else if (targets > 0) {
        rating += targets * 100;
        rating += value / 100000;
        
    /* rates other moves */
    } else {
        if (get(kings, initial)) {
            rating += 0;
        } else {
           rating += initialValue / 100000;
        }
    }

    /* returns final rating */
    return rating;
}
















