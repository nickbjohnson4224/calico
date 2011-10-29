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

static int color_code(int color, int player) {
	
	if (color == EMPTY) {
		return 0;
	}
	if (color == INVAL) {
		return 3;
	}
	if (color == player) {
		return 1;
	}
	
	return 2;
}

int neighbor_matcher(const struct go_board *board, int pos, int player) {
	uint16_t rot0, rot1, rot2, rot3;
	uint16_t adj[8];
	uint16_t pattern;
	int x, y;
	int i;
	
	x = (pos % GO_DIM) + 1;
	y = (pos / GO_DIM) + 1;

	if (go_get_color(board, go_get_pos(x, y)) != EMPTY) {
		return 0x10000;
	}

	adj[0] = color_code(go_get_color(board, go_get_pos(x+1, y+0)), player);
	adj[1] = color_code(go_get_color(board, go_get_pos(x+1, y+1)), player);
	adj[2] = color_code(go_get_color(board, go_get_pos(x+0, y+1)), player);
	adj[3] = color_code(go_get_color(board, go_get_pos(x-1, y+1)), player);
	adj[4] = color_code(go_get_color(board, go_get_pos(x-1, y+0)), player);
	adj[5] = color_code(go_get_color(board, go_get_pos(x-1, y-1)), player);
	adj[6] = color_code(go_get_color(board, go_get_pos(x+0, y-1)), player);
	adj[7] = color_code(go_get_color(board, go_get_pos(x+1, y-1)), player);

	pattern = 0;
	for (i = 0; i < 8; i++) {
		pattern |= ((adj[i] & 0x3) << (i * 2));
	}

	rot0 = pattern;
	rot1 = ((pattern >> 4)  | (pattern << 12)) & 0xFFFF;
	rot2 = ((pattern >> 8)  | (pattern << 8))  & 0xFFFF;
	rot3 = ((pattern >> 12) | (pattern << 4))  & 0xFFFF;

	if (rot0 < rot1 || rot0 < rot2 || rot0 < rot3) {
		return rot0;
	}
	if (rot1 < rot2 || rot1 < rot3) {
		return rot1;
	}
	if (rot2 < rot3) {
		return rot2;
	}
	return rot3;
}
