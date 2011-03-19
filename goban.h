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

#ifndef CALICO_GOBAN_H
#define CALICO_GOBAN_H

namespace calico {

	const int MAX_DIM = 19;	
	const int PASS    = -1;

	enum player {
		WHITE = -1,
		EMPTY = 0,
		BLACK = 1,
	};

	struct piece {
		struct piece *link[4]; // right, up, left, down
		enum player player;

		struct piece *group;
		int libs;
		int rank;
	};

	class goban {
		struct piece board[MAX_DIM][MAX_DIM];
		struct piece *ko;
		
	public:
		enum player turn;
		bool pass;
		int cols;
		int rows;

		// group operations
		struct piece *get_group(int col, int row);
		struct piece *get_group(int pos);
		struct piece *get_group(struct piece *piece);
		void merge_group(struct piece *p1, struct piece *p2);
		void delete_group(struct piece *group);
		int get_libs(int col, int row);
		int get_libs(int pos);
		int get_libs(struct piece *piece);

		// constructors
		goban(const class goban *src);
		goban(int cols, int rows);
		goban(int dim);
		goban(void);
		void init(int cols, int rows);

		// direct board access
		struct piece *get(int col, int row);
		struct piece *get(int pos);

		// rule enforcement
		bool check_move(int col, int row);
		bool check_move(int move);
		bool can_move(void);

		// gameplay
		bool move(int col, int row);
		bool move(int move);
		void move_unchecked(int col, int row);
		void move_unchecked(int move);

		// scoring
		int score(enum player player);
		enum player winner(void);

		#ifdef TEST
		// test driver
		int test(void);
		#endif
	};
}

#endif/*CALICO_GOBAN_H*/
