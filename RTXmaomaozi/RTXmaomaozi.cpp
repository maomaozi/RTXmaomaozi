#include "stdafx.h"
#include "config.h"

#include "tracer.h"


SDL_Surface *initSDL() {
	if (SDL_Init(SDL_INIT_EVERYTHING) == -1) return nullptr;

	return SDL_SetVideoMode(SCR_WIDTH, SCR_HEIGHT, SCR_BPP, SDL_HWSURFACE);
}


void cls(SDL_Surface *screen) 
{
	Uint32 color(SDL_MapRGB(screen->format, SCR_COLOR_R, SCR_COLOR_G, SCR_COLOR_B));
	Draw_FillRect(screen, 0, 0, SCR_WIDTH, SCR_HEIGHT, color);
}


void renderThread(UINT32 *bitmap)
{

	float x1 = 0;
	float x2 = 0;

	while (1)
	{
		
		x1 = (int)(x1 + 10) % 1000;
		x2 = (int)(x2 + 1) % 300;

		Tracer rayTracer;

		rayTracer.addObject(new Sphere({ 800, 500, 800 }, { 255, 0, 0 }, 200.0f, 0.05f, 0.95f, 1.5f));
		rayTracer.addObject(new Sphere({ 1200 + x2, 500, 700 }, { 0, 255, 0 }, 200.0f, 0.5f, 0.5f, 1.5f));

		rayTracer.addObject(new Sphere({ 500 + x1, 540, 1300 }, { 255, 0, 0 }, 200.0f, 0.9f, 0.1f, 1.5f));
		

		rayTracer.addLightSource(new Light({ 810, 800, -100 }, { 255, 255, 255 }, 0.1));
		rayTracer.addLightSource(new Light({ 1000, 1200, 100 }, { 255, 255, 255 }, 0.1));

		rayTracer.trace(SCR_WIDTH, SCR_HEIGHT, Point3(810, 540, -1000), 4, Color(0, 0, 0), bitmap);

	}
}


int _tmain(int argc, _TCHAR* argv[])
{
	ShowWindow(GetConsoleWindow(), SW_HIDE);

	SDL_Surface *screen = nullptr;
	if ((screen = initSDL()) == 0) 
	{
		return -1;
	}

	cls(screen);
	SDL_Flip(screen);

	// Set render FPS to 30
	FPSmanager manager;
	SDL_initFramerate(&manager);
	SDL_setFramerate(&manager, 10);

	// handle keyboard event
	Uint8 *keys;
	// handle event msg
	SDL_Event event;

	// start a render worker thread
	UINT32 *bitmap = new UINT32[SCR_WIDTH * SCR_HEIGHT];
	std::thread t(renderThread, bitmap);
	t.detach();

	// main loop control
	bool RUN = true;
	while (RUN) 
	{
		while (SDL_PollEvent(&event)) 
		{
			keys = SDL_GetKeyState(NULL);
			switch (event.type) 
			{
			case SDL_KEYDOWN:
				if (keys['c']) printf("C\n");
				break;
			case SDL_QUIT:
				RUN = false;
				break;
			}
		}

		cls(screen);
		// =========== draw start ===========

		for(int y = 0; y < SCR_HEIGHT; ++y)
		{
			for (int x = 0; x < SCR_WIDTH; ++x)
			{
				Draw_Pixel(screen, x, y, bitmap[x + y * SCR_WIDTH]);
			}
		}

		// =========== draw end ===========
		SDL_Flip(screen);

		// delay render thread
		SDL_framerateDelay(&manager);		
	}

	SDL_Quit();

	return 0;
}

