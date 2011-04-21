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
	for (x = 1; x <= 19; x++) {
		printf("%c ", letters[x]);
	}
	printf("\n");

	for (y = 19; y > 0; y--) {
		printf(" %2d ", y);

		for (x = 1; x <= 19; x++) {
			
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
	for (x = 1; x <= 19; x++) {
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
	int x, y;

	board = new_board();

	srand(time(NULL));
	for (y = 0, x = 0; x < 10000; x++) playout(board);
/*	while (1) {
		print(board);
		printf("enter a move: ");
		read_move(&x, &y);

		if (!check(board, get_pos(x, y), board->player)) {
			place(board, get_pos(x, y), board->player);
			board->player = -board->player;
		}
		else {
			printf("invalid move\n");
		}
	} */

	return 0;
}
