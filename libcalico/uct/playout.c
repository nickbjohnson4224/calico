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

int influence[GO_DIM * GO_DIM];

int playout(const struct go_board *board_init) {
	struct go_board *board;
	int move, winner, pass;

	board = go_clone(board_init);
	
	pass = 0;
	while (1) {
		move = gen_move(board);

		if (move == PASS) {
			pass++;
			board->player = -board->player;
			if (pass >= 2) {
				winner = go_score(board);

				for (int i = 0; i < GO_DIM * GO_DIM; i++) {
					influence[i] += go_get_color(board, i);
				}

				free(board);
				return (winner > 0) ? BLACK : WHITE;
			}

			continue;
		}

		pass = 0;
		go_place(board, move, board->player);
		board->player = -board->player;
	}

	free(board);
	return 0;
}

int playout_light(const struct go_board *board_init) {
	struct go_board *board;
	int move, winner, pass;

	board = go_clone(board_init);
	
	pass = 0;
	while (1) {
		move = gen_move_light(board);

		if (move == PASS) {
			pass++;
			board->player = -board->player;
			if (pass >= 2) {
				winner = go_score(board);

				for (int i = 0; i < GO_DIM * GO_DIM; i++) {
					influence[i] += go_get_color(board, i);
				}

				free(board);
				return (winner > 0) ? BLACK : WHITE;
			}

			continue;
		}

		pass = 0;
		go_place(board, move, board->player);
		board->player = -board->player;
	}

	free(board);
	return 0;
}
