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

struct uct_node *thread_uct[THREADS];
pthread_t thread[THREADS];
pthread_t refresh;

SDL_Surface *screen;
SDL_Surface *board_bmp;
SDL_Surface *white_bmp;
SDL_Surface *black_bmp;
SDL_Rect board1_off;
SDL_Rect board2_off;
SDL_Rect board3_off;

struct go_board *board;

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

void *refresh_thread(void *mutex_ptr) {
	SDL_mutex *mutex = mutex_ptr;
	struct uct_node *uct;
	SDL_Rect off1;
	uint32_t color;
	double hue, value, r, g, b;
	int plays, wins;
	int x, y;
	int i;
	
	while (1) {
		SDL_mutexP(mutex);
		// draw influence board
		SDL_BlitSurface(board_bmp, NULL, screen, &board2_off);

		for (x = 1; x <= GO_DIM; x++) {
			for (y = 1; y <= GO_DIM; y++) {
				off1.x = board2_off.x + (x - 1) * 23 + 2;
				off1.y = board2_off.y + (GO_DIM - y) * 23 + 2;
				off1.w = 23;
				off1.h = 23;
				color = ((atan(influence[go_get_pos(x, y)] / 5000.0) + M_PI_2) / M_PI) * 255;
				color = 255 - color;
				color = (color << 16 | color << 8 | color);
				SDL_FillRect(screen, &off1, color);
			}
		}

		// draw winrate board
		if (thread_uct[0]) {
		SDL_BlitSurface(board_bmp, NULL, screen, &board3_off);

		uct = thread_uct[0];
		for (x = 1; x <= GO_DIM; x++) {
			for (y = 1; y <= GO_DIM; y++) {
				off1.x = board3_off.x + (x - 1) * 23 + 2;
				off1.y = board3_off.y + (GO_DIM - y) * 23 + 2;
				off1.w = 23;
				off1.h = 23;
				
				plays = 0;
				wins = 0;
				for (i = 0; i < THREADS; i++) {
					uct = thread_uct[i];
					if (uct->child[go_get_pos(x, y)]) {
						plays += uct->child[go_get_pos(x, y)]->plays;
						wins += uct->child[go_get_pos(x, y)]->wins;
					}
				}

				hue = (double) wins / plays;
				value = (double) (atan(sqrt(plays / 1000.0)) / (M_PI_2));

				r = (value + 1) / 2 * (1.0 - hue);
				g = value;// * atan((hue)) / M_PI + .5;
				b = 0.0;

				color = ((uint32_t) (r * 255)) << 16 | ((uint32_t) (g * 255)) << 8 | ((uint32_t) (b * 255));


				if (board && (!thread_uct[0]->child[go_get_pos(x, y)] || thread_uct[0]->child[go_get_pos(x, y)]->valid == 0)) {
					switch (go_get_color(board, go_get_pos(x, y))) {
					case WHITE: SDL_BlitSurface(white_bmp, NULL, screen, &off1); break;
					case BLACK: SDL_BlitSurface(black_bmp, NULL, screen, &off1); break;
					}
				}
				else {
					SDL_FillRect(screen, &off1, color);
				}
			}
		}
		}

		SDL_Flip(screen);
		SDL_mutexV(mutex);

		SDL_Delay(200);
	}

}

int main(void) {
	SDL_mutex *mutex;
	double playout_time;
	int move;

	#if (AI == CALICO)
	struct uct_node *uct;
	double rate;
	int x, y;
	int i;
	int plays;
	#endif

	board = go_new();
	mutex = SDL_CreateMutex();

	SDL_Init(SDL_INIT_VIDEO);
	screen = SDL_SetVideoMode(673, 231, 32, SDL_SWSURFACE);

	board1_off.x = 10;
	board1_off.y = 10;

	board2_off.x = 20 + 211;
	board2_off.y = 10;

	board3_off.x = 30 + 422;
	board3_off.y = 10;

	board_bmp = SDL_LoadBMP("board_blank.bmp");
	white_bmp = SDL_LoadBMP("white.bmp");
	black_bmp = SDL_LoadBMP("black.bmp");

	pthread_create(&refresh, NULL, refresh_thread, mutex);

	srand(time(NULL));
	plays = 0;

	while (1) {

		board->player = BLACK;

		for (y = GO_DIM - 1; y >= 0; y--) {
			for (x = 0; x < GO_DIM; x++) {
				influence[x + y * GO_DIM] /= 1000;
			}
		}

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

		SDL_mutexP(mutex);
		uct = thread_uct[0];
		for (i = 1; i < THREADS; i++) {
			merge_uct(uct, thread_uct[i]);
		}
		thread_uct[0] = NULL;
		SDL_mutexV(mutex);

		printf("playouts: %d\n", uct->plays);
		printf("playouts per second: %f\n", uct->plays / ((clock() - playout_time) / CLOCKS_PER_SEC));
		printf("highest confidence: %f\n", uct_rate(uct->child[uct_best_rate(uct)]));

		move = uct_best_rate(uct);

		rate = uct_eval_rate(uct, move);
		uct_list(uct);
		free_uct(uct);

		if (rate < .3) {
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
		SDL_mutexP(mutex);
		go_print_sdl(board, screen, &board1_off);
		SDL_mutexV(mutex);

		board->player = WHITE;

		while (1) {
			printf("enter a move: ");
			move = read_move();

			if (move == PASS) {
				goto exit;
			}
			if (!go_check(board, move, WHITE)) {
				go_place(board, move, WHITE);
				go_print(board);
				SDL_mutexP(mutex);
				go_print_sdl(board, screen, &board1_off);
				SDL_mutexV(mutex);
				break;
			}
			else {
				printf("invalid move\n");
			}
		}
	}

	exit:

	SDL_FreeSurface(screen);
	SDL_Quit();
	return 0;
}
