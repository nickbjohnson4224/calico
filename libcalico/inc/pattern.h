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

#ifndef PATTERN_H
#define PATTERN_H

#include <calico.h>

#include <stdio.h>

/* general pattern matching API *********************************************/

typedef int (*pat_matcher)(const struct go_board *board, int move, int player);

struct pat_weight {
	int count;
	double *weight;
};

struct mdist *pat_gen_mdist(const struct go_board *board, 
	int player, struct pat_weight *w, pat_matcher p);

struct pat_weight *pat_weight_reward(struct pat_weight *w, int pattern, double value);
void pat_weight_save(struct pat_weight *w, const char *path);
void pat_weight_load(struct pat_weight **w, const char *path);

/* move distributions *******************************************************/

struct mdist {
	double value[GO_DIM * GO_DIM];
	double total;
};

void mdist_add(struct mdist *dest, struct mdist *src, double factor);
int  mdist_sel(struct mdist *m);

/* specific pattern matchers ************************************************/

int neighbor_matcher(const struct go_board *board, int move, int player);
int height_matcher  (const struct go_board *board, int move, int player);
int distance_matcher(const struct go_board *board, int move, int player);
int atari_matcher   (const struct go_board *board, int move, int player);
int region_matcher  (const struct go_board *board, int move, int player);

#endif/*PATTERN_H*/
