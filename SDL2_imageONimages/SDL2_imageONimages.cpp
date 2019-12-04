/*
 *   Author:  Michael McGuire
 *  Purpose:  SDL2, openning the direct media layer and collating an image on the background, then centering a different image on top
 * Language:  c++
 */

// headers
#include <iostream>
#include "SDL.h"

// custom header to help deallocate SDL2 memory usage
#include "Cleanup.h"

// allows us to implicitly call the std namespace from iostream
using namespace std;

// function to stream error messages 
void logSDLError(ostream& os, const string& msg) 
{
	os << msg << " error: " << SDL_GetError() << std::endl;
}

// function to load image file onto surface, then copy to and return texture
SDL_Texture* loadTexture(const string& file, SDL_Renderer* ren) {
	//Initialize to nullptr to avoid dangling pointer issues
	SDL_Texture* texture = nullptr;
	//Load the image
	SDL_Surface* loadedImage = SDL_LoadBMP(file.c_str());
	//If the loading went ok, convert to texture and return the texture
	if (loadedImage != nullptr) {
		texture = SDL_CreateTextureFromSurface(ren, loadedImage);
		SDL_FreeSurface(loadedImage);
		//Make sure converting went ok too
		if (texture == nullptr) {
			logSDLError(std::cout, "CreateTextureFromSurface");
		}
	}
	else {
		logSDLError(std::cout, "LoadBMP");
	}
	return texture;
}

// function to place image on renderer
void renderTexture(SDL_Texture* tex, SDL_Renderer* ren, int x, int y) {
	// create rectangle
	SDL_Rect dst;
	// position rectangle
	dst.x = x;
	dst.y = y;
	// place image on texture
	SDL_QueryTexture(tex, NULL, NULL, &dst.w, &dst.h);
	// copy rectangle with textured image on renderer
	SDL_RenderCopy(ren, tex, NULL, &dst);
}

int main(int argc, char* argv[])
{
	// initialize SDL2, end program if error initializing SDL2
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) 
	{
		// send error message to console
		logSDLError(cout, "SDL_Init");
		// end main
		return 1;
	}

	// create window using SDL2
	int window_width = 640; int window_height = 480;
	SDL_Window* window = SDL_CreateWindow("Lesson 2", 100, 100, window_width, window_height, SDL_WINDOW_SHOWN);
	// if error creating window
	if (window == nullptr) 
	{
		// send error message to console
		logSDLError(cout, "CreateWindow");
		// destroy SDL2
		SDL_Quit();
		// end main
		return 1;
	}
	
	// create graphics renderer for SDL2
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	// if error creating renderer
	if (renderer == nullptr) 
	{
		// send error message to console
		logSDLError(cout, "CreateRenderer");
		// deallocate memory from window creation
		cleanup(window);
		// destroy SDL2
		SDL_Quit();
		// end main
		return 1;
	}


	//create 2 texture holders: 1 with background image and 1 with foreground image
	SDL_Texture* background = loadTexture("background.bmp", renderer);
	SDL_Texture* foreground = loadTexture("image.bmp", renderer);
	// if error creating either texture
	if (background == nullptr || foreground == nullptr) 
	{
		// deallocate memory from background, foreground, renderer, and the window
		cleanup(background, foreground, renderer, window);
		// destroy SDL2
		SDL_Quit();
		// end main
		return 1;
	}

	// clear the renderer
	SDL_RenderClear(renderer);

	// setup the background
	int bW, bH;
	// resize the background texture to the size of background image
	SDL_QueryTexture(background, NULL, NULL, &bW, &bH);
	// collate the background texture onto the renderer for the bacground 
	renderTexture(background, renderer, 0, 0);
	renderTexture(background, renderer, bW, 0);
	renderTexture(background, renderer, 0, bH);
	renderTexture(background, renderer, bW, bH);

	// place foreground image on top
	int iW, iH;
	// resize the foreground texture to the size of foreground image
	SDL_QueryTexture(foreground, NULL, NULL, &iW, &iH);
	// draw the foreground image on top of and centered on the window
	int x = window_width / 2 - iW / 2;
	int y = window_height / 2 - iH / 2;
	renderTexture(foreground, renderer, x, y);

	// copy the rendered image to the window
	SDL_RenderPresent(renderer);

	// wait 10 seconds
	SDL_Delay(10000);
	// deallocate memory from the Textures, the Renderer, and the window
	cleanup(background, foreground, renderer, window);
	// destroy SDL2
	SDL_Quit();

	// end main
	return 0;
}
