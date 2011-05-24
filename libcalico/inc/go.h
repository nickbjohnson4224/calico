/*
 * Copyright (C) 2011 Nick Johnson <nickbjohnson4224 at gmail.com>
 * 
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef CALICO_GO_H
#define CALICO_GO_H

#include <stdint.h>

/* board representation *****************************************************/

struct go_piece {
	int16_t group;
	int16_t libs;
	int8_t color;
	int8_t rank;
};

struct go_board {
	struct go_piece pos[GO_DIM * GO_DIM];
	int ko;
	int player;
	int last;
};

/*****************************************************************************
 * Colors
 *
 * Each constant represents a possible "color" of a position on the Go board.
 * This represents, in general, which player has a piece at that position.
 *
 * WHITE - white, obviously
 * BLACK - black, obviously
 * EMPTY - no player
 * INVAL - off of the board
 *
 * Notes:
 *
 * BLACK and WHITE are guaranteed to be additive inverses of each other, so
 * that the opponent of a given player can be calculated with a simple
 * negation. EMPTY is guaranteed to be zero.
 *
 * All four values are guaranteed to fit in a 8-bit signed integer.
 */

#define WHITE (-1)
#define BLACK 1
#define EMPTY 0
#define INVAL 2

/*****************************************************************************
 * PASS
 *
 * Represents a universally invalid move. This value is guaranteed to be an
 * integer outside of the range [0, GO_DIM * GO_DIM].
 */

#define PASS (-1)

/* board operations (board.c) ***********************************************/
struct go_board *go_new  (void);
struct go_board *go_clone(const struct go_board *board);

/* basic operations (go.c) **************************************************/
int    go_get_pos  (int x, int y);
int    go_get_color(const struct go_board *board, int pos);

/* group operations (group.c) ***********************************************/
int go_get_group    (struct go_board *board, int pos);
int go_merge_group  (struct go_board *board, int g1, int g2);
int go_capture_group(struct go_board *board, int pos);
int go_get_libs     (struct go_board *board, int pos);
int go_add_libs     (struct go_board *board, int pos, int value);

/* adjacenct position calculation (adj.c) ***********************************/
#define ADJ_R 0 // Right
#define ADJ_U 1	// Up
#define ADJ_L 2	// Left
#define ADJ_D 3	// Down
int go_gen_adj(void);
int go_get_adj(int pos, int direction);

/* rule application (rules.c) ***********************************************/
int go_place(struct go_board *board, int pos, int player);
int go_check(struct go_board *board, int pos, int player);
int go_score(struct go_board *board);

/* output (print.c) *********************************************************/
void go_print(struct go_board *board);

/* analysis (analysis.c) ****************************************************/
int go_dist      (int move0, int move1);
int go_height    (int move);
int go_is_atari  (const struct go_board *board, int move, int player);
int go_is_extend (const struct go_board *board, int move, int player);
int go_is_capture(const struct go_board *board, int move, int player);

#endif/*CALICO_GO_H*/
