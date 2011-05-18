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

#include <stdlib.h>
#include <stdio.h>

struct mdist *pat_gen_mdist(const struct go_board *board, 
		int player, struct pat_weight *w, pat_matcher p) {
	struct mdist *m;
	int i, pattern;
	
	m = malloc(sizeof(struct mdist));
	m->total = 0.0;
	
	for (i = 0; i < GO_DIM * GO_DIM; i++) {
		pattern = p(board, i, player);
		if (pattern < w->count) {
			m->value[i] = w->weight[pattern];
			m->total += m->value[i];
		}
		else {
			m->value[i] = 0.0;
		}
	}

	return m;
}

struct pat_weight *pat_weight_reward(struct pat_weight *w, int pattern, double value) {
	int i;
	
	if (!w) {
		w = malloc(sizeof(struct pat_weight));
		w->count = 0;
		w->weight = NULL;
	}

	if (pattern >= w->count) {
		w->weight = realloc(w->weight, sizeof(double) * pattern);
		for (i = w->count; i <= pattern; i++) {
			w->weight[i] = 0.0;
		}
		w->count = pattern;
	}

	w->weight[pattern] += value;

	return w;
}

void pat_weight_save(struct pat_weight *w, const char *path) {
	FILE *file;
	int i;

	file = fopen(path, "w+");
	if (!file) return;

	for (i = 0; i < w->count; i++) {
		fprintf(file, "%d\t%f\n", i, w->weight[i]);
	}

	fclose(file);
}

void pat_weight_load(struct pat_weight **w, const char *path) {
	char buffer[100];
	double value;
	FILE *file;
	int i;

	file = fopen(path, "r");
	if (!file) return;

	while (fgets(buffer, 100, file)) {
		sscanf(buffer, "%i %f", &i, &value);
		(*w) = pat_weight_reward(*w, i, value);
	}

	fclose(file);
}
