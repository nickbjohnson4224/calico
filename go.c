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

#include "go.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define get_opponent(p) (-(p))

// adjacent postions
#define ADJ_R 0
#define ADJ_U 1
#define ADJ_L 2
#define ADJ_D 3
static int get_adj(int pos, int direction);
static int adj_genned = 0;

// groups
static int  get_group(struct go_board *board, int pos);
static void merge    (struct go_board *board, int g1, int g2);
static void capture  (struct go_board *board, int pos);
static void add_libs (struct go_board *board, int pos, int value);

struct go_board *new_board(void) {
	struct go_board *board;
	int i;

	board = calloc(sizeof(struct go_board), 1);
	board->ko = PASS;
	board->player = BLACK;

	for (i = 0; i < GO_DIM * GO_DIM; i++) {
		board->pos[i].group = i;
	}

	if (!adj_genned) gen_adj();

	return board;
}

struct go_board *clone_board(const struct go_board *board) {
	struct go_board *new;

	new = malloc(sizeof(struct go_board));
	memcpy(new, board, sizeof(struct go_board));

	return new;
}

void fix_board(struct go_board *board) {
	int i, j;

	for (i = 0; i < 361; i++) {
		board->pos[i].libs = 0;
	}

	for (i = 0; i < 361; i++) {
		if (board->pos[i].color == EMPTY) {
			for (j = 0; j < 4; j++) {
				add_libs(board, get_adj(i, j), 1);
			}
		}
	}
}

int get_pos(int x, int y) {
	
	// bounds check
	if (x <= 0 || x > GO_DIM || y <= 0 || y > GO_DIM) {
		return PASS;
	}

	return (((y - 1) * GO_DIM) + (x - 1));
}

int place(struct go_board *board, int pos, int player) {
	int libs;
	int i;
	
	board->ko = PASS;

	// reduce liberties of all adjacent groups
	for (i = 0; i < 4; i++) {
		add_libs(board, get_adj(pos, i), -1);
	}

	// capture all adjacent enemy groups with no liberties
	for (i = 0; i < 4; i++) {
		if (get_color(board, get_adj(pos, i)) == get_opponent(player) 
			&& get_libs(board, get_adj(pos, i)) <= 0) {
			
			capture(board, get_adj(pos, i));
			board->ko = pos;
		}
	}

	// count liberties of added piece
	for (libs = 0, i = 0; i < 4; i++) {
		if (get_color(board, get_adj(pos, i)) == EMPTY) {
			libs++;
		}
	}

	board->pos[pos].libs  = libs;
	board->pos[pos].color = player;
	board->pos[pos].group = pos;
	board->pos[pos].rank  = 0;

	// merge with adjacent allied groups
	for (i = 0; i < 4; i++) {
		if (get_color(board, get_adj(pos, i)) == player) {
			merge(board, pos, get_adj(pos, i));
			board->ko = PASS;
		}
	}

	return 0;
}

int check(struct go_board *board, int pos, int player) {
	int i, j;
	int group;
	int libs;

	// make sure space is open
	if (get_color(board, pos) != EMPTY) {
		return 1;
	}
	
	// make sure there are no ko captures
	if (board->ko && get_libs(board, board->ko) == 1){
		for (i = 0; i < 4; i++) {
			if (get_adj(pos, i) != PASS && get_adj(pos, i) == board->ko) {
				return 1;
			}
		}
	}

	// make sure there is no suicide
	for (i = 0; i < 4; i++) {
		libs = 0;
		group = get_group(board, get_adj(pos, i));
		if (get_color(board, get_adj(pos, i)) == EMPTY) {
			return 0;
		}
		for (j = 0; j < 4; j++) {
			if (get_group(board, get_adj(pos, j)) == group) {
				libs++;
			}
		}
		if (get_color(board, get_adj(pos, i)) == player) {
			if (libs != get_libs(board, get_adj(pos, i))) {
				return 0;
			}
		}
		else if (get_color(board, get_adj(pos, i)) == get_opponent(player)) {
			if (libs >= get_libs(board, get_adj(pos, i))) {
				return 0;
			}
		}
	}

	return 1;
}

static int _adj_map[4][GO_DIM * GO_DIM];

static int _get_adj(int pos, int direction) {
	int x, y;

	if (pos == PASS) {
		return PASS;
	}

	x = (pos % GO_DIM) + 1;
	y = (pos / GO_DIM) + 1;

	switch (direction) {
	case ADJ_R: x++; break;
	case ADJ_U: y++; break;
	case ADJ_L: x--; break;
	case ADJ_D: y--; break;
	default: return PASS;
	}

	return get_pos(x, y);
}

static int get_adj(int pos, int direction) {

	if (pos == PASS) {
		return PASS;
	}

	return _adj_map[direction][pos];
}

int gen_adj(void) {
	int i, j;

	for (i = 0; i < GO_DIM * GO_DIM; i++) {
		for (j = 0; j < 4; j++) {
			_adj_map[j][i] = _get_adj(i, j);
		}
	}

	return 0;
}

static int get_group(struct go_board *board, int pos) {

	if (pos == PASS) {
		return PASS;
	}

	if (board->pos[pos].group == pos) {
		return pos;
	}
	else {

		/*
		 * Note:
		 * The following commented-out code guarantees much better asymptotic
		 * performance. However, in practice, it slows down the whole program
		 * by a significant margin (~35%). This is likely because the set tree
		 * will rarely become more than one layer deep, unless multiple large
		 * groups are merged, which is much less common than a single piece
		 * being merged with a large group.
		 */

//		board->pos[pos].group = get_group(board, board->pos[pos].group);
//		return board->pos[pos].group;
		return get_group(board, board->pos[pos].group);
	}
}

static void merge(struct go_board *board, int g1, int g2) {
	int libs;
	int g3;

	g1 = get_group(board, g1);
	g2 = get_group(board, g2);

	if (g1 == g2 || g1 == PASS || g2 == PASS) return;

	libs = board->pos[g1].libs + board->pos[g2].libs;

	if (board->pos[g1].rank < board->pos[g2].rank) {
		board->pos[g1].group = g2;
		g3 = g2;
	}
	else if (board->pos[g1].rank > board->pos[g2].rank) {
		board->pos[g2].group = g1;
		g3 = g1;
	}
	else {
		board->pos[g1].group = g2;
		board->pos[g2].rank++;
		g3 = g2;
	}

	board->pos[g3].libs = libs;
}

static void capture(struct go_board *board, int pos) {
	int group;
	int i, j;

	uint32_t map[GO_DIM * GO_DIM / 32 + 1];

	if (pos == PASS) return;

	group = get_group(board, pos);

	for (i = 0; i < (GO_DIM * GO_DIM / 32 + 1); i++) {
		map[i] = 0;
	}

	for (i = 0; i < GO_DIM * GO_DIM; i++) {
		if (get_group(board, i) == group) {
			map[i >> 5] |= (1 << (i % 32));
		}
	}

	for (i = 0; i < GO_DIM * GO_DIM; i++) {
		if (map[i >> 5] & (1 << (i % 32))) {
			board->pos[i].group = PASS;
			board->pos[i].libs  = 0;
			board->pos[i].color = EMPTY;
			board->pos[i].rank  = 0;

			for (j = 0; j < 4; j++) {
				add_libs(board, get_adj(i, j), 1);
			}
		}
	}
}

int get_libs(struct go_board *board, int pos) {
	
	if (pos == PASS) {
		return PASS;
	}

	return board->pos[get_group(board, pos)].libs;
}

static void add_libs(struct go_board *board, int pos, int value) {

	pos = get_group(board, pos);

	if (pos == PASS) {
		return;
	}

	board->pos[pos].libs += value;
}

int get_color(const struct go_board *board, int pos) {
	
	if (pos == PASS) {
		return INVAL;
	}

	return board->pos[pos].color;
}
