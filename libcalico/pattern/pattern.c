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
#include <math.h>

double pattern_weight[65536];
double height_weight[GO_DIM / 2 + 2];

void pattern_save() {
	FILE *pfile, *hfile;
	int i;
	
	pfile = fopen("patterns.txt", "w+");
	hfile = fopen("heights.txt", "w+");

	for (i = 0; i < 65536; i++) {
		if (pattern_weight[i] != 1.0) {
			fprintf(pfile, "0x%x\t%f\n", i, pattern_weight[i]);
		}
	}

	for (i = 0; i <= GO_DIM / 2 + 1; i++) {
		if (height_weight[i] != 1.0) {
			fprintf(hfile, "0x%x\t%f\n", i, height_weight[i]);
		}
	}

	fclose(pfile);
	fclose(hfile);
}

void pattern_load() {
	FILE *pfile, *hfile;
	char buffer[100];
	double val;
	int n;
	
	pfile = fopen("patterns.txt", "w+");
	hfile = fopen("heights.txt", "w+");

	pattern_init();

	while (fgets(buffer, 100, pfile)) {
		sscanf(buffer, "%i %f", &n, &val);
		pattern_weight[n] = val;
	}

	while (fgets(buffer, 100, hfile)) {
		sscanf(buffer, "%i %f", &n, &val);
		height_weight[n] = val;
	}

	fclose(pfile);
	fclose(hfile);
}

void pattern_init(void) {
	int i;

	for (i = 0; i < 65536; i++) {
		pattern_weight[i] = 1.0;
	}

	for (i = 0; i <= GO_DIM / 2 + 1; i++) {
		height_weight[i] = 1.0;
	}
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

	if (go_get_color(board, go_get_pos(x, y)) != EMPTY) {
		return -1;
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

	return pattern;
}

int height_at(int pos) {
	int height, x, y;

	x = (pos % GO_DIM) + 1;
	y = (pos / GO_DIM) + 1;

	if (x > GO_DIM / 2) x = GO_DIM - x + 1;
	if (y > GO_DIM / 2) y = GO_DIM - y + 1;
	height = (x < y) ? x : y;

	return height;
}

double pattern_value(const struct go_board *board, int pos, int player) {
	uint16_t pattern;
	double value;
	int height;

	pattern = pattern_at(board, pos, player);

	if (pattern == -1) {
		return 0.0;
	}

	height = height_at(pos);

	value = 0;
	value += pattern_weight[pattern];//(atan(pattern_weight[pattern]) / 3.1416) + .5;
	value += height_weight[height];//(atan(height_weight[height]) / 3.1416) + .5;

	return exp(value / 2.0);
}

void pattern_reward(const struct go_board *board, int pos, int player, double value) {
	uint16_t rot0, rot1, rot2, rot3;
	uint16_t pattern;
	int height;

	pattern = pattern_at(board, pos, player);

	rot0 = pattern;
	rot1 = ((pattern >> 4)  | (pattern << 12)) & 0xFFFF;
	rot2 = ((pattern >> 8)  | (pattern << 8))  & 0xFFFF;
	rot3 = ((pattern >> 12) | (pattern << 4))  & 0xFFFF;

	pattern_weight[rot0] += value;
	pattern_weight[rot1] += value;
	pattern_weight[rot2] += value;
	pattern_weight[rot3] += value;

	height = height_at(pos);

	height_weight[height] += value;
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
