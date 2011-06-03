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

int go_dist(int move0, int move1) {
	int x0, y0, x1, y1, dx, dy;

	if (move0 == PASS || move1 == PASS) {
		return 1;
	}

	x0 = move0 % GO_DIM;
	y0 = move0 / GO_DIM;
	x1 = move1 % GO_DIM;
	y1 = move1 / GO_DIM;

	dx = (x0 < x1) ? x1 - x0 : x0 - x1;
	dy = (y0 < y1) ? y1 - y0 : y0 - y1;

	return (dx + dy + ((dx < dy) ? dy : dx));
}

int go_height(int move) {
	int x, y;

	x = (move % GO_DIM) + 1;
	y = (move / GO_DIM) + 1;

	if (x > GO_DIM / 2 + 1) x = GO_DIM - x + 1;
	if (y > GO_DIM / 2 + 1) y = GO_DIM - y + 1;

	return (x < y) ? x : y;
}

int go_is_atari(struct go_board *board, int move, int player) {
	int group;
	int i;

	for (i = 0; i < 4; i++) {
		if (go_get_color(board, go_get_adj(move, i)) == -player) {
			group = go_get_group(board, go_get_adj(move, i));

			if (go_get_libs(board, go_get_adj(move, i)) == 2) {
				return 1;
			}
		}
	}

	return 0;
}

int go_is_extend(struct go_board *board, int move, int player) {
	int group;
	int i;

	for (i = 0; i < 4; i++) {
		if (go_get_color(board, go_get_adj(move, i)) == player) {
			group = go_get_group(board, go_get_adj(move, i));

			if (go_get_libs(board, go_get_adj(move, i)) == 1) {
				return 1;
			}
		}
	}

	return 0;
}

int go_is_capture(struct go_board *board, int move, int player) {
	int group;
	int i;

	for (i = 0; i < 4; i++) {
		if (go_get_color(board, go_get_adj(move, i)) == -player) {
			group = go_get_group(board, go_get_adj(move, i));

			if (go_get_libs(board, go_get_adj(move, i)) == 1) {
				return 1;
			}
		}
	}

	return 0;
}
