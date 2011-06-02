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

#ifndef CALICO_GRAPH_H
#define CALICO_GRAPH_H

#include <SDL/SDL.h>

struct graph_position {
	uint32_t bgcolor;
	int piece_color;
	int symbol;
};

#define SYM_NONE	0
#define SYM_SQUARE	1
#define SYM_CIRCLE	2

struct graph {
	struct graph_position pos[GO_DIM * GO_DIM];
	SDL_Surface *surface;
};

struct graph *graph_new(void);

void graph_set_board  (struct graph *g, struct go_board *b);
void graph_set_bgcolor(struct graph *g, int pos, uint32_t color);
void graph_set_piece  (struct graph *g, int pos, int piece);
void graph_set_symbol (struct graph *g, int pos, int symbol);

void graph_blit(struct graph *g, SDL_Surface *screen);

#endif/*CALICO_GRAPH_H*/
