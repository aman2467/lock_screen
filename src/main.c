/* ==========================================================================
 * @file    : main.c
 *
 * @description : This file contains main program.
 *
 * @author  : Aman Kumar (2015)
 *
 * @copyright   : The code contained herein is licensed under the GNU General
 *		Public License. You may obtain a copy of the GNU General
 *		Public License Version 2 or later at the following locations:
 *              http://www.opensource.org/licenses/gpl-license.html
 *              http://www.gnu.org/copyleft/gpl.html
 * ========================================================================*/

#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include <SDL/SDL.h>
#include <lock_screen.h>

SDL_Surface *screen;
SDL_Surface *lock;

extern void *captureThread(void *);

VIDEO_CONFIG config = {
	"/dev/video0",
	640,
	480,
	640*480*2,
	0};

VIDEO_CONFIG *getConfig(void)
{
	return &config;
}

void display_screen(void)
{
	int alpha = 0;
	SDL_Rect src, dest;

	src.x = 0;
	src.y = 0;
	src.w = lock->w;
	src.h = lock->h;
	dest.x = (screen->w - lock->w)/2;
	dest.y = (screen->h - lock->h)/2;
	dest.w = lock->w;
	dest.h = lock->h;

	while(alpha++ < 200) {
		SDL_SetAlpha(lock, SDL_SRCALPHA, alpha);
		SDL_BlitSurface(lock, &src, screen, &dest);
		SDL_Flip(screen);
		usleep(500);
	}
	alarm(7);
}

void handle_alrm(int sig)
{
	SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
	SDL_Flip(screen);
}

int main(int argc, char **argv)
{
	SDL_Event event;
	SDL_Surface *tmp;
	char passwd[20] = {0};
	char pwd[20] = "aman";
	char ch;
	int i = 0, display = 0;
	int kill = 0, mouse = 0;
	pthread_t tCaptureThread;
	VIDEO_CONFIG *config = getConfig();

	if(argc == 2) {
		if(strlen(argv[1]) < 20)
			strcpy(pwd, argv[1]);
	}
	SDL_Init(SDL_INIT_VIDEO);
	screen = SDL_SetVideoMode(SCREEN_W, SCREEN_H, 32,
				  SDL_SWSURFACE | SDL_FULLSCREEN);

	SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
	SDL_Flip(screen);

	SDL_ShowCursor(SDL_DISABLE);
	tmp = SDL_LoadBMP(UTILS_DIR"/lock_screen.bmp");
	lock = SDL_DisplayFormat(tmp);
	SDL_FreeSurface(tmp);
	signal(SIGALRM, handle_alrm);
	config->running = 1;
	if(pthread_create(&tCaptureThread, NULL, captureThread, NULL)) {
		printf("Capture Thread create fail\n");
		exit(0);
	}
	for(ever) {
		usleep(10000);
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_KEYDOWN:
					ch = event.key.keysym.sym;
					if(ch == 'q') {
						memset(passwd, 0, 20);
						i = 0;
					} else {
						passwd[i++] = ch;
						if(i > 19) {
							memset(passwd, 0, 20);
							i = 0;
						}
					}
					if(strcmp(passwd, pwd) == 0)
						kill = 1;
				case SDL_MOUSEBUTTONDOWN:
					display = 1;
					break;
				case SDL_MOUSEMOTION:
					mouse++;
					break;
				}
		}
		if(kill) {
			break;
		} else if(display || mouse > 20) {
			display_screen();
			config->save = 1;
			display = 0;
			mouse = 0;
		}
	}
	config->running = 0;
	pthread_join(tCaptureThread, NULL);

	SDL_Quit();
	return 0;
}
