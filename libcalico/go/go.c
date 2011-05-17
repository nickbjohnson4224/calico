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

#include <calico.h>

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define get_opponent(p) (-(p))

// adjacent postions
static int adj_genned = 0;

struct go_board *go_new(void) {
	struct go_board *board;
	int i;

	board = calloc(sizeof(struct go_board), 1);
	board->ko = PASS;
	board->player = BLACK;

	for (i = 0; i < GO_DIM * GO_DIM; i++) {
		board->pos[i].group = i;
	}

	if (!adj_genned) go_gen_adj();

	return board;
}

struct go_board *go_clone(const struct go_board *board) {
	struct go_board *new;

	new = malloc(sizeof(struct go_board));
	memcpy(new, board, sizeof(struct go_board));

	return new;
}

int go_get_pos(int x, int y) {
	
	// bounds go_check
	if (x <= 0 || x > GO_DIM || y <= 0 || y > GO_DIM) {
		return PASS;
	}

	return (((y - 1) * GO_DIM) + (x - 1));
}

int go_place(struct go_board *board, int pos, int player) {
	int libs;
	int i;
	int color;
	int adj[4];
	
	board->ko = PASS;

	// reduce liberties of all adjacent groups
	for (i = 0; i < 4; i++) {
		adj[i] = go_get_adj(pos, i);
		go_add_libs(board, adj[i], -1);
	}

	for (libs = 0, i = 0; i < 4; i++) {
		color = go_get_color(board, adj[i]);
		// capture all adjacent enemy groups with no liberties
		if (color == get_opponent(player)) {
			if (go_get_libs(board, adj[i]) <= 0) {
				go_capture_group(board, adj[i]);
				board->ko = pos;
				libs++;
			}
		}
		// count liberties of added piece
		else if (color == EMPTY) {
			libs++;
		}
	}

	board->pos[pos].libs  = libs;
	board->pos[pos].color = player;
	board->pos[pos].group = pos;
	board->pos[pos].rank  = 0;

	// merge with adjacent allied groups
	for (i = 0; i < 4; i++) {
		if (go_get_color(board, adj[i]) == player) {
			go_merge_group(board, pos, adj[i]);
			board->ko = PASS;
		}
	}

	board->last = pos;

	return 0;
}

int go_check(struct go_board *board, int pos, int player) {
	int i, j;
	int group;
	int libs;

	// make sure space is open
	if (go_get_color(board, pos) != EMPTY) {
		return 1;
	}
	
	// make sure there are no ko captures
	if (board->ko && go_get_libs(board, board->ko) == 1){
		for (i = 0; i < 4; i++) {
			if (go_get_adj(pos, i) != PASS && go_get_adj(pos, i) == board->ko) {
				return 1;
			}
		}
	}

	// make sure there is no suicide
	for (i = 0; i < 4; i++) {
		libs = 0;
		group = go_get_group(board, go_get_adj(pos, i));
		if (go_get_color(board, go_get_adj(pos, i)) == EMPTY) {
			return 0;
		}
		for (j = 0; j < 4; j++) {
			if (go_get_group(board, go_get_adj(pos, j)) == group) {
				libs++;
			}
		}
		if (go_get_color(board, go_get_adj(pos, i)) == player) {
			if (libs != go_get_libs(board, go_get_adj(pos, i))) {
				return 0;
			}
		}
		else if (go_get_color(board, go_get_adj(pos, i)) == get_opponent(player)) {
			if (libs >= go_get_libs(board, go_get_adj(pos, i))) {
				return 0;
			}
		}
	}

	return 1;
}

static int _adj_map[4][GO_DIM * GO_DIM];

static int _go_get_adj(int pos, int direction) {
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

	return go_get_pos(x, y);
}

int go_get_adj(int pos, int direction) {

	if (pos == PASS) {
		return PASS;
	}

	return _adj_map[direction][pos];
}

int go_gen_adj(void) {
	int i, j;

	for (i = 0; i < GO_DIM * GO_DIM; i++) {
		for (j = 0; j < 4; j++) {
			_adj_map[j][i] = _go_get_adj(i, j);
		}
	}

	return 0;
}

int go_get_color(const struct go_board *board, int pos) {
	
	if (pos == PASS || pos >= GO_DIM * GO_DIM) {
		return INVAL;
	}

	return board->pos[pos].color;
}

int go_score(struct go_board *board) {
	int b, w, i, j;

	b = 0;
	w = 0;
	for (i = 0; i < GO_DIM * GO_DIM; i++) {
		switch (go_get_color(board, i)) {
		case WHITE: w++; break;
		case BLACK: b++; break;
		case EMPTY:
			for (j = 0; j < 4; j++) {
				if (go_get_color(board, go_get_adj(i, j)) == BLACK) {
					b++;
					break;
				}
				else if (go_get_color(board, go_get_adj(i, j)) == WHITE) {
					w++;
					break;
				}
			}
		}
	}

	return (b - w);
}
