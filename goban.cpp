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

#include "goban.h"

#include <cstdlib>
#include <cstring>
#include <cstdio>

using namespace calico;

/*****************************************************************************
 * goban
 *
 * Summary *******************************************************************
 *
 * The goban class contains all of the necessary logic to enforce and enact
 * the rules of Go on a Go board representation.
 *
 * Interface Notes ***********************************************************
 *
 * Most methods of the goban class have two ways of addressing board locations
 * and moves. 
 *
 * The first (type A) is of the form (..., int col, int row, ...), and 
 * addresses the board with coordinates from the lower left hand corner that 
 * are one-indexed. For example, the lower left star point is at (4,4) in this 
 * scheme. This is the preferred interface for use by user interface classes, 
 * with the exception of the passing move, which it cannot express.
 *
 * The second (type B) addressing scheme numbers the board locations with one 
 * integer, from left to right then bottom to top, and is zero indexed. For 
 * example, the lower left star point is numbered 60. This scheme also can 
 * accept the value PASS, which is -1, to indicate that a player is passing. 
 * This is the more common interface for the AI itself.
 *
 * Implementation Notes ******************************************************
 *
 * The goban internal state is completely contained within the memory
 * allocated to the class, i.e. there are no pointers within the class to
 * memory outside of the class. This should help to improve memory locality
 * and therefore reduce cache misses, and allows the fast memcpy() function
 * to be used for cloning the goban (which is a frequent operation in MCTS).
 *
 * Groups of pieces are implemented as a disjoint-set forest. Each piece
 * points to a "group leader" (or points to another piece in the group), which
 * stores all metadata about the group (e.g. liberty count). This makes 
 * finding and merging groups a nearly constant-time operation, and deleting
 * groups nearly a linear time operation with respect to the number of board
 * spaces. By nearly constant time, I mean inverse Ackermann function time,
 * which is less than 5 for all sane values.
 */

/*****************************************************************************
 * Cloning constructor
 *
 * The constructed goban is an exact, independent copy of the goban <src>.
 */

goban::goban(const class goban *src) {
	memcpy(this, src, sizeof(src));
}

/*****************************************************************************
 * Dimension constructor A
 *
 * The constructed goban is empty, and has <cols> columns and <rows> rows.
 */

goban::goban(int cols, int rows) {
	this->init(cols, rows);
}

/*****************************************************************************
 * Dimension constructor B
 *
 * The constructed goban is empty, and has <dim> columns and <dim> rows.
 */

goban::goban(int dim) {
	this->init(dim, dim);
}

/*****************************************************************************
 * Default constructor
 *
 * The constructed goban is empty, and has 19 columns and 19 rows.
 */

goban::goban(void) {
	this->init(19, 19);
}

/*****************************************************************************
 * Sets up the board state for a goban. This method is used internally by
 * most goban constructors, and does their heavy lifting.
 */

void goban::init(int cols, int rows) {

	// set dimensions
	this->cols = cols;
	this->rows = rows;

	// construct board
	for (int i = 0; i < cols; i++) {
		for (int j = 0; j < rows; j++) {
			this->board[i][j].player = EMPTY;
			this->board[i][j].group  = &this->board[i][j];
			this->board[i][j].libs   = 0;
			this->board[i][j].rank   = 0;
		}
	}

	// link pieces rightward
	for (int i = 0; i < cols - 1; i++) {
		for (int j = 0; j < rows; j++) {
			this->board[i][j].link[0] = &this->board[i+1][j];
		}
	}
	for (int j = 0; j < rows; j++) {
		this->board[cols - 1][j].link[0] = NULL;
	}

	// link pieces upward
	for (int i = 0; i < cols; i++) {
		for (int j = 0; j < rows - 1; j++) {
			this->board[i][j].link[1] = &this->board[i][j+1];
		}
	}
	for (int i = 0; i < cols; i++) {
		this->board[i][rows - 1].link[1] = NULL;
	}

	// link pieces leftward
	for (int i = 1; i < cols; i++) {
		for (int j = 0; j < rows; j++) {
			this->board[i][j].link[2] = &this->board[i-1][j];
		}
	}
	for (int j = 0; j < rows; j++) {
		this->board[0][j].link[2] = NULL;
	}

	// link pieces downward
	for (int i = 0; i < cols; i++) {
		for (int j = 1; j < rows; j++) {
			this->board[i][j].link[3] = &this->board[i][j-1];
		}
	}
	for (int i = 0; i < cols; i++) {
		this->board[i][0].link[3] = NULL;
	}

	// set other metadata
	this->turn = BLACK;
	this->ko   = NULL;
	this->pass = false;
}

/*****************************************************************************
 * Get Group A
 *
 * Returns the "group" at coordinates (row, col), which is a piece contained
 * within the group that is chosen to represent the group. The <libs> field
 * of the returned piece is a count of the liberties of the group. Returns
 * NULL if the selected position is empty or invalid.
 */

struct piece *goban::get_group(int col, int row) {
	struct piece *piece;

	piece = this->get(col, row);
	
	if (!piece || piece->player == EMPTY) {
		return NULL;
	}

	return this->get_group(piece);
}

/*****************************************************************************
 * Get Group B
 *
 * Returns the "group" at position <pos>, which is a piece contained within
 * the group that is chosen to represent the group. The <libs> field of the
 * returned piece is a count of the liberties of the group. Returns NULL if
 * the selected position is empty or invalid.
 */

struct piece *goban::get_group(int pos) {
	struct piece *piece;

	piece = this->get(pos);

	if (!piece || piece->player == EMPTY) {
		return NULL;
	}

	return this->get_group(piece);
}

/*****************************************************************************
 * Get Group (Internal)
 *
 * Returns <piece>'s "group", which is a piece contained within the group that
 * is chosen to represent the group. The <libs> field of the returned piece
 * is a count of the liberties of the group.
 */

struct piece *goban::get_group(struct piece *piece) {
	
	if (!piece) {
		return NULL;
	}

	// correct null groups
	if (piece->group == NULL) {
		piece->group = piece;
	}

	if (piece->group == piece) {
		// piece is group leader
		return piece;
	}
	else {
		piece->group = this->get_group(piece->group);
		return piece->group;
	}
}

/*****************************************************************************
 * Merge Group
 *
 * Combines the groups containing <p1> and <p2> into one group.
 */

void goban::merge_group(struct piece *p1, struct piece *p2) {
	struct piece *group1;
	struct piece *group2;

	group1 = this->get_group(p1);
	group2 = this->get_group(p2);

	// refuse to merge a group with itself
	if (group1 == group2) return;

	if (group1->rank < group2->rank) {
		// group 1 is added to group 2
		group1->group = group2;
		group2->libs += group1->libs;
	}
	else if (group1->rank > group2->rank) {
		// group 2 is added to group 1
		group2->group = group1;
		group1->libs += group2->libs;
	}
	else {
		// group 2 is added to group 1 and group 1 is promoted
		group2->group = group1;
		group1->rank++;
		group1->libs += group2->libs;
	}
}

/*****************************************************************************
 * Delete Group
 *
 * Deletes a group from the board, updating liberty information for 
 * surrounding stones.
 */

void goban::delete_group(struct piece *group) {
	struct piece *ng[4]; // neighboring groups
	struct piece *piece;
	int pos;

	// make sure this is the group leader
	group = this->get_group(group);

	for (pos = 0; pos < this->rows * this->cols; pos++) {
		piece = this->get(pos);

		if (this->get_group(piece) == group) {

			// delete piece
			piece->player = EMPTY;
			piece->group  = piece;
			piece->rank   = 0;
			piece->libs   = 0;

			// get neighboring groups
			for (int l = 0; l < 4; l++) {
				ng[l] = this->get_group(piece->link[l]);
			}

			// prune neighbors (no repeats wanted)
			if (ng[1] == ng[0]) ng[1] = NULL;
			if (ng[2] == ng[0] || ng[2] == ng[1]) ng[2] = NULL;
			if (ng[3] == ng[0] || ng[3] == ng[1] || ng[3] == ng[2]) ng[3] = NULL;

			// add liberties to neighboring groups
			for (int l = 0; l < 4; l++) {
				if (ng[l]) ng[l]->libs++;
			}
		}
	}
}

/*****************************************************************************
 * Get A
 *
 * Returns a pointer to the piece at coordinates (row, col), or NULL on error.
 * Note that the <libs> field of the returned piece is not a valid count of
 * liberties.
 */

struct piece *goban::get(int col, int row) {
	
	// bounds check
	if (row > this->rows || col > this->cols || row < 1 || col < 1) {
		return NULL;
	}

	return &this->board[row - 1][col - 1];
}

/*****************************************************************************
 * Get B
 *
 * Returns a pointer to the piece at position number <pos>, or NULL on error.
 * Note that the <libs> field of the returned piece is not a valid count of
 * liberties.
 */

struct piece *goban::get(int pos) {
	int col;
	int row;
	
	col = (pos % this->cols) + 1;
	row = (pos / this->cols) + 1;
	
	return this->get(col, row);
}

/*****************************************************************************
 * Get Libs A
 *
 * Returns the number of liberties of the group at coordinates (row, col). If
 * there is no group present, this method returns -1.
 */

int goban::get_libs(int col, int row) {
	struct piece *group;

	group = this->get_group(col, row);

	// reject empty space groups
	if (group->player == EMPTY) {
		return -1;
	}

	return group->libs;
}

/*****************************************************************************
 * Get Libs B
 *
 * Returns the number of liberties of the group at position number <pos>. If
 * there is no group present, this method returns -1.
 */

int goban::get_libs(int pos) {
	struct piece *group;

	group = this->get_group(pos);

	// reject empty space groups
	if (group->player == EMPTY) {
		return -1;
	}

	return group->libs;
}

/*****************************************************************************
 * Get Libs (Internal)
 *
 * Returns the number of liberties of the group containing <piece>.
 */

int goban::get_libs(struct piece *piece) {
	struct piece *group;

	group = this->get_group(piece);
	return group->libs;
}

/*****************************************************************************
 * Check Move A
 *
 * Checks whether a play at position (col, row) follows the rules of Go. If
 * it does, this method returns true, and otherwise, this method returns 
 * false.
 */

bool goban::check_move(int col, int row) {
	enum player opponent = (this->turn == WHITE) ? BLACK : WHITE;
	struct piece *piece;

	// check if space is occupied or invalid
	piece = this->get(col, row);
	if (!piece || piece->player != EMPTY) {
		return false;
	}

	// check for liberties (if so, succeeed)
	if ((piece->link[0] && piece->link[0]->player == EMPTY) ||
		(piece->link[1] && piece->link[1]->player == EMPTY) ||
		(piece->link[2] && piece->link[2]->player == EMPTY) ||
		(piece->link[3] && piece->link[3]->player == EMPTY)) {
			return true;
	}

	// check for captures (if so: if ko, fail, otherwise succeed)
	for (int l = 0; l < 4; l++) {
		if (piece->link[l] && piece->link[l]->player == opponent) {
			if (this->get_libs(piece->link[l]) == 1) {
				// there is a capture; check for ko
				if (piece->link[l] == this->ko) {
					// capturing ko piece; fail
					return false;
				}
				else {
					// regular capture; succeed
					return true;
				}
			}
		}
	}

	// check for suicide (if so, fail)
	for (int l = 0; l < 4; l++) {
		if (piece->link[l] && piece->link[l]->player == this->turn) {
			if (this->get_libs(piece->link[l]) > 1) {
				return true; // no suicide
			}
		}
	}
	return false; // not no suicide => suicide
}

/*****************************************************************************
 * Check Move B
 *
 * Checks whether a play at position (move) follows the rules of Go. If it
 * does, this method returns true, and otherwise, this method returns false.
 */

bool goban::check_move(int move) {
	int col;
	int row;

	// passing is always acceptable
	if (move == PASS) {
		return true;
	}
	
	col = (move % this->cols) + 1;
	row = (move / this->cols) + 1;

	return this->check_move(col, row);
}

/*****************************************************************************
 * Can Move
 *
 * Checks whether there are any valid moves remaining.
 */

bool goban::can_move(void) {
	for (int i = 0; i < this->rows * this->cols; i++) {
		if (this->check_move(i)) {
			return true;
		}
	}

	return false;
}

/*****************************************************************************
 * Move A
 *
 * Performs a move at coordinates (col, row) on the goban. If this move is
 * illegal, no change occurs, and this function returns false.
 */

bool goban::move(int col, int row) {
	
	// check move
	if (!this->check_move(col, row)) {
		return false;
	}

	// move
	this->move_unchecked(col, row);
	return true;
}

/*****************************************************************************
 * Move B
 *
 * Performs a move at the position <move> (or passes if move is PASS). If this
 * move is illegal, no change occurs, and this function returns false.
 */

bool goban::move(int move) {
	
	// check move
	if (!this->check_move(move)) {
		return false;
	}

	// move
	this->move_unchecked(move);
	return true;
}

/*****************************************************************************
 * Move Unchecked A
 *
 * Performs a move at coordinates (col, row) without checking legality. If the
 * move is in fact illegal, behavior is undefined.
 */

void goban::move_unchecked(int col, int row) {
	enum player opponent = (this->turn == WHITE) ? BLACK : WHITE;

	// find piece to set
	struct piece *piece = this->get(col, row);

	// place piece
	piece->player = this->turn;
	piece->libs   = 0;

	// count initial liberties
	for (int l = 0; l < 4; l++) {
		if (piece->link[l] && piece->link[l]->player == EMPTY) {
			piece->libs++;
		}
	}

	// reduce liberties of adjacent groups
	for (int l = 0; l < 4; l++) {
		if (piece->link[l] && piece->link[l]->player != EMPTY) {
			this->get_group(piece->link[l])->libs--;
		}
	}

	// merge with adjacent allies
	for (int l = 0; l < 4; l++) {
		if (piece->link[l] && piece->link[l]->player == this->turn) {
			this->merge_group(piece, piece->link[l]);
		}
	}

	// capture opponent if possible
	bool caps = false; // did we capture anything?
	for (int l = 0; l < 4; l++) {
		if (piece->link[l] && piece->link[l]->player == opponent) {
			
			// capture if no liberties remain
			if (this->get_libs(piece->link[l]) == 0) {
				caps = true;
				this->delete_group(piece->link[l]);
			}
		}
	}

	// set ko
	if (caps && piece->group == piece) {
		this->ko = piece;
	}
	else {
		this->ko = NULL;
	}

	// switch players
	this->turn = opponent;
	this->pass = false;
}

/*****************************************************************************
 * Move Unchecked B
 *
 * Performs a move a position <move> (or pass if <move> is PASS) without
 * checking legality. If the move is in fact illegal, behavior is undefined.
 */

void goban::move_unchecked(int move) {
	enum player opponent = (this->turn == WHITE) ? BLACK : WHITE;
	int col;
	int row;

	// perform a pass
	if (move == PASS) {
		this->turn = opponent;
		this->ko   = NULL;
		this->pass = true;
		return;
	}
	
	// perform a move
	col = (move % this->cols) + 1;
	row = (move / this->cols) + 1;

	return this->move_unchecked(col, row);
}
