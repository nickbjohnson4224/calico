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

#ifndef UCT_H
#define UCT_H

#include <calico.h>

struct uct_node {
	struct go_board *state;

	int move;
	int wins;
	int plays;
	int valid;

	struct uct_node *child[GO_DIM * GO_DIM];
	struct uct_node *parent;
};

struct uct_node *new_uct(const struct go_board *state);
void free_uct(struct uct_node *uct);

double uct_ucb(struct uct_node *uct);
double uct_lcb(struct uct_node *uct);
double uct_rate(struct uct_node *uct);

int uct_best_lcb(struct uct_node *uct);
int uct_best_ucb(struct uct_node *uct);
int uct_best_rate(struct uct_node *uct);

double uct_eval_rate(struct uct_node *uct, int move);

int uct_playout(struct uct_node *root, pat_matcher p, struct pat_weight *w);

int uct_list(struct uct_node *uct);

#endif/*UCT_H*/
