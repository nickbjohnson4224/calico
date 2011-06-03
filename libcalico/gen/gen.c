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

#include <stdlib.h>
#include <math.h>

static int is_bad_move(struct go_board *board, int move, int player);

static double move_weight(const struct go_board *board, int move) {
	double w;
	double i;
	int d, h, d2;
	double wd, wd2, wh, wi;
	int a, e, c;

	if (is_bad_move((struct go_board *) board, move, board->player)) {
		return 0.0;
	}

	a   = go_is_atari  ((struct go_board *) board, move, board->player);
	e   = go_is_extend ((struct go_board *) board, move, board->player);
	c   = go_is_capture((struct go_board *) board, move, board->player);

	if (a || e || c) return 1.0;
	
	d = go_dist(move, board->last);
	d2 = go_dist(move, board->llast);

	if (d <= 5 || d2 <= 5) return 0.8;

	return 0.5;
}

int gen_move(const struct go_board *board) {
	static unsigned int seed;
	int move, i;
	double x;

	i = 0;
	while (1) {
		move = rand_r(&seed) % (GO_DIM * GO_DIM);
		x = rand_r(&seed) / ((double) RAND_MAX);

		if (x > move_weight(board, move)) {
			i++;
			if (i > 10) {
				for (move = 0; move < GO_DIM * GO_DIM; move++) {
					if (move_weight(board, move) > 0.0) {
						return move;
					}
				}
				return PASS;
			}
			continue;
		}

		return move;
	}
}

int gen_move_light(const struct go_board *board) {
	static unsigned int seed;
	int move, i;

	i = 0;
	while (1) {
		move = rand_r(&seed) % (GO_DIM * GO_DIM);

		if (is_bad_move((struct go_board *) board, move, board->player)) {
			i++;
			if (i > 100) return PASS;
			continue;
		}

		return move;
	}
}

static int is_bad_move(struct go_board *board, int move, int player) {
	int adj, i;
	
	if (go_check(board, move, player)) {
		return 1;
	}

	adj = 0;
	for (i = 0; i < 4; i++) {
		if (go_get_color(board, go_get_adj(move, i)) == player 
				|| go_get_color(board, go_get_adj(move, i)) == INVAL) {
			adj++;
		}
		if (go_get_libs(board, go_get_adj(move, i)) == 1) {
			return 0;
		}
	}

	if (adj == 4) {
		return 1;
	}

	return 0;
}
