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

#include "uct.h"
#include "playout.h"
#include "pattern.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

// Split at N plays
#define N (GO_DIM * GO_DIM)

#define CONF .02

#define ERR(s, n, p) (CONF * (log(s) / log(N)) * sqrt((p) * (1.0 - (p)) / (n)))

struct uct_node *new_uct(const struct go_board *state) {
	struct uct_node *node;

	node = calloc(sizeof(struct uct_node), 1);
	node->state = clone_board(state);
	node->player = state->player;

	return node;
}

void free_uct(struct uct_node *uct) {
	int i;

	free(uct->state);
	
	for (i = 0; i < GO_DIM * GO_DIM; i++) {
		if (uct->child[i]) {
			free_uct(uct->child[i]);
		}
	}

	free(uct);
}

double uct_ucb(struct uct_node *uct) {
	double winrate;

	if (!uct || uct->plays == 0) {
		return 2.0;
	}

	if (!uct->valid) {
		return -1.0;
	}

	winrate = (double) (uct->wins + 1) / (uct->plays + 1);

	return winrate + ERR(uct->parent->plays, uct->plays, winrate);
}

double uct_lcb(struct uct_node *uct) {
	double winrate;

	if (!uct || uct->plays == 0) {
		return 0.0;
	}

	if (!uct->valid) {
		return -1.0;
	}

	winrate = (double) (uct->wins + 1) / (uct->plays + 1);

	return winrate - ERR(uct->parent->plays, uct->plays, winrate);
}

double uct_rate(struct uct_node *uct) {
	double winrate;

	if (!uct || uct->plays == 0) {
		return 0.0;
	}

	if (!uct->valid) {
		return -1.0;
	}

	winrate = (double) (uct->wins + 1) / (uct->plays + 1);

	return winrate;
}

int uct_best_lcb(struct uct_node *uct) {
	double best_lcb;
	int best_move;
	int i;

	best_lcb  = -1.0;
	best_move = -1;
	for (i = 0; i < GO_DIM * GO_DIM; i++) {
		if (uct_lcb(uct->child[i]) >= best_lcb) {
			best_move = i;
			best_lcb  = uct_lcb(uct->child[i]);
		}
	}

	return best_move;
}

int uct_best_ucb(struct uct_node *uct) {
	double best_ucb;
	int best_move;
	int i;

	best_ucb  = -1.0;
	best_move = -1;
	for (i = 0; i < GO_DIM * GO_DIM; i++) {
		if (uct_ucb(uct->child[i]) >= best_ucb) {
			best_move = i;
			best_ucb  = uct_ucb(uct->child[i]);
		}
	}

	return best_move;
}

int uct_best_rate(struct uct_node *uct) {
	double best_rate;
	int best_move;
	int i;

	best_rate = -1.0;
	best_move = -1;
	for (i = 0; i < GO_DIM * GO_DIM; i++) {
		if (uct_rate(uct->child[i]) >= best_rate) {
			best_move = i;
			best_rate = uct_rate(uct->child[i]);
		}
	}

	return best_move;
}

int uct_play(struct uct_node *uct, int quantum) {
	int move;
	
	while (1) {
		move = uct_best_ucb(uct);

		if (!uct->child[move]) {
			uct->child[move] = new_uct(uct->state);
			uct->child[move]->parent = uct;

			if (!check(uct->child[move]->state, move, uct->state->player)) {
				place(uct->child[move]->state, move, uct->state->player);
				uct->child[move]->state->player = -uct->state->player;
				uct->child[move]->player = -uct->state->player;
				uct->child[move]->valid = 1;
			}
			else {
				uct->child[move]->valid = 0;
				continue;
			}
		}

		for (int i = 0; i < quantum; i++) {
			if (playout(uct->child[move]->state) == -uct->child[move]->player) {
				uct->child[move]->wins++;
			}
			uct->child[move]->plays++;
			uct->plays++;
		}

		break;
	}

	return 0;
}

int uct_list(struct uct_node *uct) {
	int i;

	for (i = 0; i < GO_DIM * GO_DIM; i++) {
		printf("move %d: ", i);
		printf("\tplays = %d", (uct->child[i]) ? uct->child[i]->plays : 0);
		printf("\twins = %d", (uct->child[i]) ? uct->child[i]->wins : 0);
		printf("\tCI = [%f, %f]\n", uct_lcb(uct->child[i]), uct_ucb(uct->child[i]));
	}

	return 0;
}

double uct_eval_rate(struct uct_node *uct, int move) {
	return uct_rate(uct->child[move]);
}

int uct_reward_patterns(struct uct_node *uct) {
	int i;

	for (i = 0; i < GO_DIM * GO_DIM; i++) {
		if (uct->child[i]) {
			pattern_reward(pattern_at(uct->state, i, uct->player), uct_ucb(uct->child[i]) - .5);
		}
	}

	return 0;
}
