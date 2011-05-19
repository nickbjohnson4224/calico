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
#include <ctype.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

int read_move(int *x, int *y) {
	char buffer[100];
	char letter;
	int number;

	fgets(buffer, 100, stdin);
	sscanf(buffer, "%c %d", &letter, &number);

	letter = toupper(letter) - 'A';

	if (letter >= 8) {
		letter--;
	}

	*x = letter + 1;
	*y = number;

	return 0;
}

int main(void) {
	struct go_board *board;
	struct pat_weight *w;
	struct uct_node *uct;
	int x, y, i;
	int best_move;
	double rate;

	board = go_new();

	w = NULL;
	pat_weight_load(&w, "height.pat");
	pat_weight_list(w);

	srand(time(NULL));
	
	board->player = BLACK;

	while (1) {

		board->player = BLACK;

		uct = new_uct(board);
		uct->valid = 1;

		for (i = 0; i < 10000 || 
				uct_lcb(uct->child[uct_best_lcb(uct)]) < (1.0 - .1 * log10((double) i + 1.0)); i++) {
			uct_playout(uct, height_matcher, w);

			if (i > 50000) break;
		}

		printf("playouts: %d\n", i);

		best_move = uct_best_rate(uct);
		rate = uct_eval_rate(uct, best_move);
		uct_list(uct);
		free_uct(uct);

		if (rate < .2) {
			printf("black's move: resign");
			go_print(board);
			return 0;
		}

		go_place(board, best_move, BLACK);
		printf("black's move: %d\n", best_move);

		go_print(board);

		board->player = WHITE;

		while (1) {
			printf("enter a move: ");
			read_move(&x, &y);

			if (!go_check(board, go_get_pos(x, y), WHITE)) {
				go_place(board, go_get_pos(x, y), WHITE);
				go_print(board);
				break;
			}
			else {
				printf("invalid move\n");
			}
		}
	}

	return 0;
}
