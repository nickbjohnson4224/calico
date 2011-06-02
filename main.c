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

#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#define PLAYOUTS 100000
#define TIME_PER_MOVE 10.0
#define THREADS 4

#define CALICO 0
#define GEN 1
#define AI CALICO

int read_move(void) {
	char buffer[100];
	char *s;
	char letter;
	int number;
	int x, y;

	s = fgets(buffer, 100, stdin);
	if (!strcmp(buffer, "pass\n")) {
		return PASS;
	}
	sscanf(buffer, "%c %d", &letter, &number);

	letter = toupper(letter) - 'A';

	if (letter >= 8) {
		letter--;
	}

	x = letter + 1;
	y = number;

	return go_get_pos(x, y);
}


void *calico_thread(void *uct_ptr) {
	struct uct_node *uct;
	struct uct_node **uct_tbl = uct_ptr;
	double playout_time;
	volatile int i;

	playout_time = clock();

	uct = *uct_tbl;

	printf("thread starting on UCT %p\n", (void*) uct);

	for (i = 0; i < PLAYOUTS / THREADS; i++) {
		uct_playout(uct);

		if (i % 100 == 0) {
			printf("progress: %f%%\r", 100 * (i / (double) (PLAYOUTS / THREADS)));
		}
	}

	*uct_tbl = uct;

	return NULL;
}

struct uct_node *thread_uct[THREADS];
pthread_t thread[THREADS];

int main(void) {
	SDL_Surface *screen;
	struct go_board *board;
	double playout_time;
	int move;

	#if (AI == CALICO)
	struct uct_node *uct;
	double rate;
	int x, y;
	int i;
	#endif

	board = go_new();

//	SDL_Init(SDL_INIT_VIDEO);
//	screen = SDL_SetVideoMode(640, 480, 32, SDL_SWSURFACE);

	srand(time(NULL));

	while (1) {

		board->player = BLACK;

		#if (AI == CALICO)
		playout_time = clock();

		for (i = 0; i < THREADS; i++) {
			thread_uct[i] = new_uct(board);
			thread_uct[i]->valid = 1;
			if (pthread_create(&thread[i], NULL, calico_thread, (void*) &thread_uct[i])) {
				fprintf(stderr, "could not create thread %d\n", i);
				abort();
			}
		}

		for (i = 0; i < THREADS; i++) {
			pthread_join(thread[i], NULL);

			printf("thread %d terminated with %d playouts\n", i, thread_uct[i]->plays);
		}

		uct = thread_uct[0];
		for (i = 1; i < THREADS; i++) {
			merge_uct(uct, thread_uct[i]);
		}

		printf("playouts: %d\n", uct->plays);
		printf("playouts per second: %f\n", uct->plays / ((clock() - playout_time) / CLOCKS_PER_SEC));
		printf("highest confidence: %f\n", uct_rate(uct->child[uct_best_rate(uct)]));

		move = uct_best_rate(uct);

		rate = uct_eval_rate(uct, move);
		uct_list(uct);
		free_uct(uct);

		if (rate < .2) {
			printf("black's move: resign");
			go_print(board);
			return 0;
		}

		#elif (AI == GEN)
		move = gen_move(board);
		#endif

		printf("\n");
		printf("influence map:\n");
		for (y = GO_DIM - 1; y >= 0; y--) {
			for (x = 0; x < GO_DIM; x++) {
				if (influence[x + y * GO_DIM] > 1000) {
					printf("# ");
				}
				else if (influence[x + y * GO_DIM] < -1000) {
					printf("O ");
				}
				else {
					printf("- ");
				}
			}
			printf("\n");
		}

		go_place(board, move, BLACK);
		printf("black's move: %d\n", move);

		go_print(board);

		board->player = WHITE;

		while (1) {
			printf("enter a move: ");
			move = read_move();

			if (!go_check(board, move, WHITE)) {
				go_place(board, move, WHITE);
				go_print(board);
				break;
			}
			else {
				printf("invalid move\n");
			}
		}
	}

	SDL_FreeSurface(screen);
	SDL_Quit();
	return 0;
}
