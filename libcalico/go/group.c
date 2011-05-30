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

/*****************************************************************************
 * go_get_group
 *
 * Returns the position of the leader of the group at <pos>. Returns PASS on
 * error.
 *
 * Notes:
 *
 * The currently commented-out code guarantees much better asymptotic
 * performance; however, in practice, it slows down the whole program
 * by a significant margin (~35%). This is likely because the set tree
 * will rarely become more than one layer deep, unless multiple large
 * groups are merged, which is much less common than a single piece
 * being merged with a large group. 
 *
 * If that code is put back in, this function runs in inverse Ackermann 
 * function time (effectively O(1)) with respect to group size.
 *
 * This function runs O(lg(n)) time where n is the group size.
 */

int go_get_group(struct go_board *board, int pos) {

	// filter invalid positions
	if (pos < 0 || pos >= GO_DIM * GO_DIM) {
		return PASS;
	}

	if (board->pos[pos].group == pos) {
		return pos;
	}
	else {
//		board->pos[pos].group = get_group(board, board->pos[pos].group);
//		return board->pos[pos].group;
		return go_get_group(board, board->pos[pos].group);
	}
}

/*****************************************************************************
 * go_merge_group
 *
 * Combines the groups that contain the positions <g1> and <g2>. The group
 * leader of this new group is selected from the group leaders of the two
 * constituent groups. Returns the new group leader on success, PASS on error.
 *
 * Notes:
 *
 * This function runs in O(lg(n)) time where n is the larger of the two group
 * sizes. If g1 and g2 are both group leaders, this function runs in O(1) 
 * time.
 */

int go_merge_group(struct go_board *board, int g1, int g2) {
	int libs;
	int g3;

	g1 = go_get_group(board, g1);
	g2 = go_get_group(board, g2);

	if (g1 == g2 || g1 == PASS || g2 == PASS) return PASS;

	libs = board->pos[g1].libs + board->pos[g2].libs;

	if (board->pos[g1].rank < board->pos[g2].rank) {
		board->pos[g1].group = g2;
		g3 = g2;
	}
	else if (board->pos[g1].rank > board->pos[g2].rank) {
		board->pos[g2].group = g1;
		g3 = g1;
	}
	else {
		board->pos[g1].group = g2;
		board->pos[g2].rank++;
		g3 = g2;
	}

	board->pos[g3].libs = libs;

	return g3;
}

/*****************************************************************************
 * go_capture_group
 *
 * Removes all elements of the group containing the position <pos> from the
 * board, updating liberty information. Returns zero on success, nonzero on 
 * error.
 *
 * Notes:
 *
 * This function runs in O(n) time where n is the size of the group.
 */

int go_capture_group(struct go_board *board, int pos) {
	int color;
	int color1;
	int i;

	if (pos < 0 || pos > GO_DIM * GO_DIM) {
		return 1;
	}

	color = go_get_color(board, pos);
	board->pos[pos].color = EMPTY;
	board->pos[pos].group = PASS;
	board->pos[pos].libs  = 0;
	board->pos[pos].rank  = 0;

	for (i = 0; i < 4; i++) {
		color1 = go_get_color(board, go_get_adj(pos, i));
		if (color1 == -color) {
			go_add_libs(board, go_get_adj(pos, i), 1);
		}
		else if (color1 == color) {
			go_capture_group(board, go_get_adj(pos, i));
		}
	}

	return 0;
}

/*****************************************************************************
 * go_get_libs
 *
 * Returns the number of liberties in the group containing the position <pos>.
 * Returns negative on error.
 */

int go_get_libs(struct go_board *board, int pos) {
	int group;
	
	group = go_get_group(board, pos);

	if (group < 0 || group > GO_DIM * GO_DIM) {
		return -1;
	}

	return board->pos[group].libs;
}

/*****************************************************************************
 * go_add_libs
 *
 * Adds <value> to the number of liberties in the group containing the
 * position <pos>. Returns zero on success, nonzero on error.
 */

int go_add_libs(struct go_board *board, int pos, int value) {

	pos = go_get_group(board, pos);

	if (pos < 0 || pos > GO_DIM * GO_DIM) {
		return 1;
	}

	board->pos[pos].libs += value;

	return 0;
}
