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

#include <go.h>

#include <stdio.h>

/* weighting ****************************************************************/

void weight_add(double *w, double *w2, double factor);
int  weight_sel(double *w);

/* pattern matching *********************************************************/

void     pattern_init  (void);
void     pattern_load  (void);
void     pattern_save  (void);

int      height_at     (int pos);
uint16_t pattern_at    (const struct go_board *board, int pos, int player);
double   pattern_value (const struct go_board *board, int pos, int player);
void     pattern_reward(const struct go_board *board, int pos, int player, double value);

#endif/*PATTERN_H*/
