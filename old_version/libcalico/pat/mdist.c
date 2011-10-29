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

void mdist_add(struct mdist *dest, struct mdist *src, double factor) {
	int i;

	for (i = 0; i < GO_DIM * GO_DIM; i++) {
		dest->value[i] += src->value[i] * factor;
	}

	dest->total += src->total * factor;
}

int mdist_sel(struct mdist *m) {
	double r;
	int i;

	r = (rand() / ((double) RAND_MAX)) * m->total;

	for (i = 0; i < GO_DIM * GO_DIM; i++) {
		r -= m->value[i];
		if (r <= 0.0) {
			return i;
		}
	}

	return PASS;
}
