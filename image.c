
#include "image.h"

SDL_Surface* loadImageAsSurface(const char* filename) {
	SDL_Surface *img = IMG_Load(filename);
	if (NULL == img) {
		fprintf(stderr, "Error loading image: %s - %s\n", filename, IMG_GetError());
		exit(EXIT_FAILURE);
	}
	return img;
}
