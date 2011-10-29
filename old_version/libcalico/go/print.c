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
#include <stdio.h>

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
				if ((x - 3) % 6 == 1 && (y - 3) % 6 == 1 && (GO_DIM == 13 || GO_DIM == 19)) {
					printf("+");
				}
				else {
					printf("+");
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

void go_print_sdl(struct go_board *board, SDL_Surface *surface, SDL_Rect *off) {
	static SDL_Surface *black_bmp = NULL;
	static SDL_Surface *white_bmp = NULL;
	static SDL_Surface *board_bmp = NULL;
	SDL_Rect off1;
	int x, y;

	if (!board_bmp) board_bmp = SDL_LoadBMP("board_blank.bmp");
	if (!black_bmp) black_bmp = SDL_LoadBMP("black.bmp");
	if (!white_bmp) white_bmp = SDL_LoadBMP("white.bmp");

	SDL_BlitSurface(board_bmp, NULL, surface, off);

	for (x = 1; x <= GO_DIM; x++) {
		for (y = 1; y <= GO_DIM; y++) {
			off1.x = off->x + (x - 1) * 23 + 2;
			off1.y = off->y + (GO_DIM - y) * 23 + 2;
			switch (go_get_color(board, go_get_pos(x, y))) {
			case WHITE: SDL_BlitSurface(white_bmp, NULL, surface, &off1); break;
			case BLACK: SDL_BlitSurface(black_bmp, NULL, surface, &off1); break;
			}
		}
	}

	SDL_Flip(surface);
}
