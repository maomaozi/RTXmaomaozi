#include "stdafx.h"
#include "config.h"

#include "tracer.h"


Camera camera({ 0, 0, 0 }, { 0,1,0 }, { 1, 0, 0 }, SCR_WIDTH, SCR_HEIGHT, 1400);


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

	Object *obj1 = new Sphere({ 200, 200, 500 }, 200.0f, { 0.2f, 0, 0 }, 0.75f, 1.6f, 1.0f);
	Object *obj2 = new Sphere({ 250, 300, 1200 }, 300.0f, {0.2f, 0.55f, 0 }, 0.4f, 1.5f, 1.0f);
	Object *obj3 = new Sphere({ 500, 200, 700 }, 200.0f, { 0.4f, 0, 0.5f }, 0.35f, 1.4f, 1.0f);
	Object *obj4 = new Sphere({ 1000, 400, 1100 }, 400.0f, { 0.99f, 0.99f, 0.99f }, 0.0f, 1.0f, 0.0f);
	Object *obj5 = new Sphere({ 900, 200, 500 }, 200.0f, { 0.5f, 0.3f, 0.4f }, 0.2f, 1.5f, 1.0f);

	Object *plane1 = new Plane(Vec3(0, 1, 0), Point3(0, 0, 0), Color(0.7, 0.7, 0.7), 1.0f);

	rayTracer.addObject(obj1);
	rayTracer.addObject(obj2);
	rayTracer.addObject(obj3);
	rayTracer.addObject(obj4);
	rayTracer.addObject(obj5);
	rayTracer.addObject(plane1);

	Light *light1 = new Light({ 100, 300, 100 }, { 255, 255, 255 }, 0.5);
	Light *light2 = new Light({ 1400, 500, 200 }, { 255, 255, 255 }, 0.6);

	rayTracer.addLightSource(light1);
	rayTracer.addLightSource(light2);

	while (1)
	{
		rayTracer.trace(camera, 10, Color(20, 20, 20), bitmap);
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

	// Set render FPS to 10
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
				if (keys['w']) camera.moveY(50);
				if (keys['s']) camera.moveY(-50);
				if (keys['a']) camera.moveX(-50);
				if (keys['d']) camera.moveX(50);

				if (keys['q']) camera.turnByY(0.05);
				if (keys['e']) camera.turnByY(-0.05);
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

