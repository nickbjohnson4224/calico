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
#include <string.h>

struct go_board *go_new(void) {
	struct go_board *board;
	int i;

	board = calloc(sizeof(struct go_board), 1);
	board->ko = PASS;
	board->player = BLACK;

	for (i = 0; i < GO_DIM * GO_DIM; i++) {
		board->pos[i].group = i;
	}

	go_gen_adj();

	return board;
}

struct go_board *go_clone(const struct go_board *board) {
	struct go_board *new;

	new = malloc(sizeof(struct go_board));
	memcpy(new, board, sizeof(struct go_board));

	return new;
}
