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

#ifndef GO_H
#define GO_H

#define PASS (-1)

#define WHITE (-1)
#define BLACK 1
#define EMPTY 0
#define INVAL 2

#define GO_DIM 19

struct go_piece {
	int group;
	int color;
	int rank;
	int libs;
};

struct go_board {
	struct go_piece pos[361];
	int ko;
	int player;
};

struct go_board *new_board  (void);
struct go_board *clone_board(const struct go_board *board);
void fix_board(struct go_board *board);

int get_pos(int x, int y);
int get_color(const struct go_board *board, int pos);
int get_libs(struct go_board *board, int pos);

int gen_adj(void);

int place(struct go_board *board, int pos, int player);
int check(struct go_board *board, int pos, int player);

void print(struct go_board *board);

#endif/*GO_H*/
