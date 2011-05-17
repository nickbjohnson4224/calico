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

#include <pattern.h>

#include <stdlib.h>
#include <math.h>

double pattern_weight[65536];
double total_weight;

void pattern_save(FILE *file) {
	int i;
	
	fprintf(file, "PATTERN FILE\n");
	fprintf(file, "%f\n", total_weight);

	for (i = 0; i < 65536; i++) {
		if (pattern_weight[i] != 0.0) {
			fprintf(file, "0x%x\t%f\n", i, pattern_weight[i]);
		}
	}
}

void pattern_load(FILE *file) {
	int i, n;
	double val;

	pattern_init();

	fscanf(file, "PATTERN FILE\n");
	fscanf(file, "%f", &val);

	total_weight = val;

	for (i = 0; i < 65536; i++) {
		fscanf(file, "%i %f", &n, &val);
		pattern_weight[n] = val;
	}
}

void pattern_init(void) {
	int i;

	for (i = 0; i < 65536; i++) {
		pattern_weight[i] = 0.0;
	}

	total_weight = 65536.0;
}

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

uint16_t pattern_at(const struct go_board *board, int pos, int player) {
	uint16_t adj[8];
	uint16_t pattern;
	int x, y;
	int i;
	
	x = (pos % GO_DIM) + 1;
	y = (pos / GO_DIM) + 1;

	if (get_color(board, get_pos(x, y)) != EMPTY) {
		return -1;
	}

	adj[0] = color_code(get_color(board, get_pos(x+1, y+0)), player);
	adj[1] = color_code(get_color(board, get_pos(x+1, y+1)), player);
	adj[2] = color_code(get_color(board, get_pos(x+0, y+1)), player);
	adj[3] = color_code(get_color(board, get_pos(x-1, y+1)), player);
	adj[4] = color_code(get_color(board, get_pos(x-1, y+0)), player);
	adj[5] = color_code(get_color(board, get_pos(x-1, y-1)), player);
	adj[6] = color_code(get_color(board, get_pos(x+0, y-1)), player);
	adj[7] = color_code(get_color(board, get_pos(x+1, y-1)), player);

	pattern = 0;
	for (i = 0; i < 8; i++) {
		pattern |= ((adj[i] & 0x3) << (i * 2));
	}

	return pattern;
}

double pattern_value(uint16_t pattern) {
	if (pattern == -1) return 0.0;
	return atan(pattern_weight[pattern] / (total_weight / 65536.0)) + 2.0;
}

void pattern_reward(uint16_t pattern, double value) {
	uint16_t rot0, rot1, rot2, rot3;

	rot0 = pattern;
	rot1 = ((pattern >> 4)  | (pattern << 12)) & 0xFFFF;
	rot2 = ((pattern >> 8)  | (pattern << 8))  & 0xFFFF;
	rot3 = ((pattern >> 12) | (pattern << 4))  & 0xFFFF;

	pattern_weight[rot0] += value;
	pattern_weight[rot1] += value;
	pattern_weight[rot2] += value;
	pattern_weight[rot3] += value;
	total_weight += fabs(value) * 4.0;
}

void weight_add(double *w, double *w2, double factor) {
	int i;

	for (i = 0; i < GO_DIM * GO_DIM; i++) {
		w[i] += w2[i] * factor;
	}
}

int weight_sel(double *w) {
	double sum;
	double r;
	int i;

	for (sum = 0.0, i = 0; i < GO_DIM * GO_DIM; i++) {
		sum += w[i];
	}

	r = (rand() / ((double) RAND_MAX)) * sum;

	for (i = 0; i < GO_DIM * GO_DIM; i++) {
		r -= w[i];
		if (r <= 0.0) {
			return i;
		}
	}

	return PASS;
}
