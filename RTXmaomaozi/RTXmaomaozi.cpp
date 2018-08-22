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

	Tracer rayTracer;

	Object *obj1 = new Sphere({ 300, 200, 800 }, 200.0f, { 0.1f, 0, 0 }, 0.9f, 1.5f);
	Object *obj2 = new Sphere({ 450, 250, 1500 }, 300.0f, {0, 0.75f, 0 }, 0.2f, 1.5f);
	Object *obj3 = new Sphere({ 700, 200, 800 }, 200.0f, { 0, 0, 0.3f }, 0.7f, 1.4f);

	rayTracer.addObject(obj1);
	rayTracer.addObject(obj2);
	rayTracer.addObject(obj3);

	Light *light1 = new Light({ 100, 300, 100 }, { 255, 255, 255 }, 0.4);
	Light *light2 = new Light({ 1200, 500, 200 }, { 255, 255, 255 }, 0.4);

	rayTracer.addLightSource(light1);
	rayTracer.addLightSource(light2);

	rayTracer.trace(SCR_WIDTH, SCR_HEIGHT, Point3(810, 540, -2000), 15, Color(50, 50, 50), bitmap);

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

