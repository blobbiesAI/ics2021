#include <NDL.h>
#include <SDL.h>
#include <string.h>
#define keyname(k) #k,


static const char *keyname[] = {
  "NONE",
  _KEYS(keyname)
};

int SDL_PushEvent(SDL_Event *ev) {
  return 0;
}

int SDL_PollEvent(SDL_Event *ev) {
	return SDL_WaitEvent(ev);
}

int SDL_WaitEvent(SDL_Event *event) {
	char buf[64] = {0};
	if(NDL_PollEvent(buf, sizeof(buf))==0){
		event->type = SDL_USEREVENT;
		(event->key).keysym.sym = SDLK_NONE;
	}
	else{
		size_t keynums = sizeof(keyname)/sizeof(keyname[0]);
		event->type = strncmp(buf, "kd", 2)==0 ? SDL_KEYDOWN : SDL_KEYUP;
		for(size_t i = 0; i<keynums; i++){
			if(strcmp(buf+3, keyname[i])==0){
				(event->key).keysym.sym = i;
				return 1;
			}
		}
	}
	return 0;
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
  return 0;
}

uint8_t* SDL_GetKeyState(int *numkeys) {
  return NULL;
}
