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

	rayTracer.addObject(new Sphere({ 600, 500, 500 }, { 255.0f, 0, 0 }, 300.0f, 0.1f, 0.1f, 1.5f));
	//rayTracer.addLightSource(new Light({ 900,900,100 }, { 255, 255, 255 }, 0.1));

	rayTracer.trace(SCR_WIDTH, SCR_HEIGHT, Point3(0, 0, -100), 5, Color(0, 0, 0), bitmap);
}


int _tmain(int argc, _TCHAR* argv[])
{
	//ShowWindow(GetConsoleWindow(), SW_HIDE);


	/*
	Sphere s1({ 10.0f, 10.0f, 10.0f }, { 255.0f, 0, 0 }, 8.0f, 0.1f, 0.1f, 1.5f);
	Intersection in1;
	bool f = s1.getIntersection(Point3{ 10, 10, 3 }, Vec3{ 0, 0, 1 }, in1, true);
	Vec3 outRay(0, 0, 0);
	float strength = in1.obj->calcReflectionRay(in1.entryPoint, Vec3{ 1, 0, 3 }, outRay);
	float strength = in1.obj->calcRefractionRay(in1.entryPoint, Vec3{ 1, 0, 3 }, outRay, false);
	*/

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

