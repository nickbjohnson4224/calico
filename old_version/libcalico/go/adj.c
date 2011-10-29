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

static int _adj_map[4][GO_DIM * GO_DIM];

static int _go_get_adj(int pos, int direction) {
	int x, y;

	if (pos == PASS) {
		return PASS;
	}

	x = (pos % GO_DIM) + 1;
	y = (pos / GO_DIM) + 1;

	switch (direction) {
	case ADJ_R: x++; break;
	case ADJ_U: y++; break;
	case ADJ_L: x--; break;
	case ADJ_D: y--; break;
	default: return PASS;
	}

	return go_get_pos(x, y);
}

int go_get_adj(int pos, int direction) {

	if (pos == PASS) {
		return PASS;
	}

	return _adj_map[direction][pos];
}

int go_gen_adj(void) {
	int i, j;

	for (i = 0; i < GO_DIM * GO_DIM; i++) {
		for (j = 0; j < 4; j++) {
			_adj_map[j][i] = _go_get_adj(i, j);
		}
	}

	return 0;
}
