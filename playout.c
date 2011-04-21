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

#include "playout.h"

#include <stdlib.h>
#include <stdio.h>

static int is_bad_move(struct go_board *board, int move, int player);
//static int is_finished(struct go_board *board);
static int score(struct go_board *board);

int playout(const struct go_board *board_init) {
	struct go_board *board;
	int i, j, move, winner, pass;

	board = clone_board(board_init);

	i = 0;
	j = 0;
	pass = 0;
	while (1) {
		move = rand() % (GO_DIM * GO_DIM);

		if (is_bad_move(board, move, board->player)) {
			j++;

			if (j >= 100) {
				pass++;
				board->player = -board->player;
				if (pass >= 2) {
					winner = score(board);
					free(board);
					return (winner > 0) ? BLACK : WHITE;
				}
			}

			continue;
		}
		
		j = 0;
		place(board, move, board->player);
		board->player = -board->player;
		i++;
	}

	free(board);
	return 0;
}

static int is_bad_move(struct go_board *board, int move, int player) {
	int adj, i;
	
	if (check(board, move, player)) {
		return 1;
	}

	adj = 0;
	for (i = 0; i < 4; i++) {
		if (get_color(board, get_adj(move, i)) == player) adj++;
	}

	if (adj == 4) {
		return 1;
	}

	return 0;
}

/*static int is_finished(struct go_board *board) {
	int i;

	for (i = 0; i < 361; i++) {
		if (!is_bad_move(board, i, board->player)) {
			return 0;
		}
	}

	return 1;
}*/

static int score(struct go_board *board) {
	int i, w, b;

	w = b = 0;
	for (i = 0; i < GO_DIM * GO_DIM; i++) {
		if (get_color(board, i) == WHITE) {
			w++;
		}
		if (get_color(board, i) == BLACK) {
			b++;
		}
		if (get_color(board, i) == EMPTY) {
			for (int j = 0; j < 4; j++) {
				if (get_color(board, get_adj(i, j)) == WHITE) {
					w++;
					break;
				}
				if (get_color(board, get_adj(i, j)) == BLACK) {
					b++;
					break;
				}
			}
		}
	}

	return (b - w);
}

double winrate(const struct go_board *board, int player, int tries) {
	int i;
	int wins;

	for (wins = 0, i = 0; i < tries; i++) {
		if (playout(board) == player) wins++;
	}

	return ((double) wins / (double) tries);
}
