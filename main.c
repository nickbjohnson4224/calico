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
#include "playout.h"

#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <time.h>

void print(struct go_board *board) {
	int x, y;
	int libs;
	const char *letters = " ABCDEFGHJKLMNOPQRST";

	printf("\n    ");
	for (x = 1; x <= GO_DIM; x++) {
		printf("%c ", letters[x]);
	}
	printf("\n");

	for (y = GO_DIM; y > 0; y--) {
		printf(" %2d ", y);

		for (x = 1; x <= GO_DIM; x++) {
			
			libs = get_libs(board, get_pos(x, y));
			if (get_color(board, get_pos(x, y)) == WHITE) {
				printf("O ");
			}
			else if (get_color(board, get_pos(x, y)) == BLACK) {
				printf("# ");
			}
			else {
				if ((x - 3) % 6 == 1 && (y - 3) % 6 == 1) {
					printf("+ ");
				}
				else {
					printf("- ");
				}
			}
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
	struct go_board *vboard;
	int x, y, i;
	double wr[GO_DIM * GO_DIM];
	double wr_avg;
	double best;
	int best_move;

	board = new_board();

	srand(time(NULL));
	while (1) {

		board->player = BLACK;
		wr_avg = 0.0;

		// choose move (computer is black)
		for (i = 0; i < GO_DIM * GO_DIM; i++) {
			wr[i] = -1.0;
			if (!check(board, i, BLACK)) {
				vboard = clone_board(board);
				place(vboard, i, BLACK);
				wr[i] = winrate(vboard, BLACK, 100);
				wr_avg += wr[i];
				free(vboard);
			}
		}

		wr_avg /= ((GO_DIM * GO_DIM));

		// refine better moves
		for (i = 0; i < 361; i++) {
			if (wr[i] >= wr_avg) {
				vboard = clone_board(board);
				place(vboard, i, BLACK);
				wr[i] = winrate(vboard, BLACK, 4000);
				free(vboard);
			}
		}

		for (best_move = 60, best = -1.0, i = 0; i < GO_DIM * GO_DIM; i++) {
			if (wr[i] > best) {
				best_move = i;
				best = wr[i];
			}
		}

		place(board, best_move, BLACK);
		printf("black's move: %d (%f%% winrate)\n", best_move, wr[best_move] * 100);

		print(board);

		board->player = WHITE;

		while (1) {
			printf("enter a move: ");
			read_move(&x, &y);

			if (!check(board, get_pos(x, y), WHITE)) {
				place(board, get_pos(x, y), WHITE);
				print(board);
				break;
			}
			else {
				printf("invalid move\n");
			}
		}
	}

	return 0;
}
