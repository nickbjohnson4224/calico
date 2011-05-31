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

#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#define DIFFICULTY 4

#define DIFF_Q 10000
#define DIFF_K ((1.0 / DIFF_Q) * pow(2.0, 1.0 - DIFFICULTY))

#define CALICO 0
#define GEN 1
#define AI CALICO

int read_move(void) {
	char buffer[100];
	char letter;
	int number;
	int x, y;

	fgets(buffer, 100, stdin);
	if (!strcmp(buffer, "pass\n")) {
		return PASS;
	}
	sscanf(buffer, "%c %d", &letter, &number);

	letter = toupper(letter) - 'A';

	if (letter >= 8) {
		letter--;
	}

	x = letter + 1;
	y = number;

	return go_get_pos(x, y);
}

int main(void) {
	struct go_board *board;
	int move;

	#if (AI == CALICO)
	struct uct_node *uct;
	double rate;
	int x, y;
	int i;
	#endif

	board = go_new();

	srand(time(NULL));

	while (1) {

		board->player = BLACK;

		#if (AI == CALICO)
		uct = new_uct(board);
		uct->valid = 1;

		for (i = 0; i < DIFF_Q || 
				uct_lcb(uct->child[uct_best_lcb(uct)]) < (1.0 - DIFF_K * i); i++) {
			uct_playout(uct);

			if (i % 10 == 0) {
				printf("progress: %f%%\r", uct_lcb(uct->child[uct_best_lcb(uct)]) / (1.0 - DIFF_K * i) * 100);
			}
		}
		printf("\n");

		printf("playouts: %d\n", i);
		printf("highest confidence: %f\n", uct_lcb(uct->child[uct_best_lcb(uct)]));

		move = uct_best_rate(uct);

		rate = uct_eval_rate(uct, move);
		uct_list(uct);
		free_uct(uct);

		if (rate < .2) {
			printf("black's move: resign");
			go_print(board);
			return 0;
		}

		for (y = GO_DIM - 1; y >= 0; y--) {
			for (x = 0; x < GO_DIM; x++) {
				printf("%s ", (influence[x + y * GO_DIM] > 1000) ? "#" : ((influence[x + y * GO_DIM] < -1000) ? "O" : "-"));
				influence[x + y * GO_DIM] = 0;
			}
			printf("\n");
		}
		#elif (AI == GEN)
		move = gen_move(board);
		#endif

		go_place(board, move, BLACK);
		printf("black's move: %d\n", move);

		go_print(board);

		board->player = WHITE;

		while (1) {
			printf("enter a move: ");
			move = read_move();

			if (!go_check(board, move, WHITE)) {
				go_place(board, move, WHITE);
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
