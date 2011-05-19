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
#include <stdio.h>

static int is_bad_move(struct go_board *board, int move, int player);
static int can_play(struct go_board *board, int player);

int playout(const struct go_board *board_init, pat_matcher p, struct pat_weight *w) {
	struct go_board *board;
	struct mdist *mdist;
	int i, j, move, winner, pass;

	board = go_clone(board_init);
	
	i = 0;
	j = 0;
	pass = 0;
	while (1) {

		if (1 || !p || !w) {
			move = rand() % (GO_DIM * GO_DIM);
		}
		else {
			mdist = pat_gen_mdist(board, board->player, w, p);
			move = mdist_sel(mdist);
			free(mdist);
		}

		if (is_bad_move(board, move, board->player)) {
			j++;

			if (j > 100) {
				pass++;
				board->player = -board->player;
				j = 0;
				if (pass >= 2) {
					winner = go_score(board);
					free(board);
					return (winner > 0) ? BLACK : WHITE;
				}
			}

			continue;
		}

		pass = 0;
		j = 0;
		go_place(board, move, board->player);
		board->player = -board->player;
	}

	free(board);
	return 0;
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

static int can_play(struct go_board *board, int player) {
	int i;

	for (i = 0; i < GO_DIM * GO_DIM; i++) {
		if (!is_bad_move(board, i, player)) {
			return 1;
		}
	}

	return 0;
}
