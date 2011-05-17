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

void go_print(struct go_board *board) {
	int x, y;
	int libs;
	const char *letters = " ABCDEFGHJKLMNOPQRST";

	printf("\n    ");
	for (x = 1; x <= GO_DIM; x++) {
		printf("%c ", letters[x]);
	}
	printf("\n");

	for (y = GO_DIM; y > 0; y--) {
		printf(" %2d", y);

		if (board->last == go_get_pos(1, y)) {
			printf("[");
		}
		else printf(" ");

		for (x = 1; x <= GO_DIM; x++) {
			
			libs = go_get_libs(board, go_get_pos(x, y));
			if (go_get_color(board, go_get_pos(x, y)) == WHITE) {
				printf("O");
			}
			else if (go_get_color(board, go_get_pos(x, y)) == BLACK) {
				printf("#");	
			}
			else {
				if ((x - 3) % 6 == 1 && (y - 3) % 6 == 1) {
					printf("+");
				}
				else {
					printf("-");
				}
			}

			if (board->last == go_get_pos(x, y)) {
				printf("]");
			}
			else if (board->last == go_get_pos(x + 1, y)) {
				printf("[");
			}
			else printf(" ");
		}

		printf("%-2d\n", y);
	}

	printf("    ");
	for (x = 1; x <= GO_DIM; x++) {
		printf("%c ", letters[x]);
	}
	printf("\n\n");
}

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
	struct uct_node *uct;
	int x, y, i;
	int best_move;
	double rate;
	FILE *patterns;

	patterns = fopen("patterns.txt", "r");
	if (patterns) {
		pattern_load(patterns);
	}
	else {
		fprintf(stderr, "warning: no pattern file\n");
		pattern_init();
	}
	fclose(patterns);

	printf("pattern file loaded.\n");

	board = go_new();

	srand(time(NULL));

	while (1) {

		board->player = BLACK;

		uct = new_uct(board);
		for (i = 0; i < 2000; i++) {
			uct_play(uct, 1);
		}

		best_move = uct_best_rate(uct);
		rate = uct_eval_rate(uct, best_move);
		uct_reward_patterns(uct);
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

		patterns = fopen("patterns.txt", "w+");
		if (patterns) {
			pattern_save(patterns);
		}
		else {
			fprintf(stderr, "error: could not open pattern file for writing\n");
		}
		fclose(patterns);

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
