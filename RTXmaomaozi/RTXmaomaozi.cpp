#include "stdafx.h"
#include "config.h"

#include "tracer.h"

Camera camera({ 0.0f, 300, -300 }, { 0, 1, 0.0f }, { 1, 0, 0 }, SCR_WIDTH, SCR_HEIGHT, 500);


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



	////=====
	//rayTracer.addObject(new Triangle({ 100, 1000, 1200 }, { 200, 500, 700 }, { 500, 500, 1800 }, { 0.7f, 0.7f, 0.7f }, { 0.0f, 0.0f, 0.0f }, 1.4f, 0.99f));
	//rayTracer.addObject(new Triangle({ -600, 500, 1500 }, { 100, 1000, 1200 }, { 500, 500, 1800 }, { 0.7f, 0.7f, 0.7f }, { 0.0f, 0.0f, 0.0f }, 1.4f, 0.99f));
	//rayTracer.addObject(new Triangle({ -600, 500, 1500 }, { 200, 500, 700 }, { 100, 1000, 1200 }, { 0.7f, 0.7f, 0.7f }, { 0.0f, 0.0f, 0.0f }, 1.4f, 0.99f));

	//rayTracer.addObject(new Triangle({ 100, 0, 1200 }, { 200, 500, 700 }, { 500, 500, 1800 }, { 0.7f, 0.7f, 0.7f }, { 0.0f, 0.0f, 0.0f }, 1.4f, 0.99f));
	//rayTracer.addObject(new Triangle({ -600, 500, 1500 }, { 100, 0, 1200 }, { 500, 500, 1800 }, { 0.7f, 0.7f, 0.7f }, { 0.0f, 0.0f, 0.0f }, 1.4f, 0.99f));
	//rayTracer.addObject(new Triangle({ -600, 500, 1500 }, { 200, 500, 700 }, { 100, 0, 1200 }, { 0.7f, 0.7f, 0.7f }, { 0.0f, 0.0f, 0.0f }, 1.4f, 0.99f));
	////=====

	
	rayTracer.addObject(new CheesePlane(Vec3(0, 1, 0), Point3(0, 0, 0), Color(1, 1, 1), 1.0f));
	rayTracer.addObject(new Plane(Vec3(0, -1, 0), Point3(0, 2000, 0), Color(1, 1, 1), 1.0f));

	rayTracer.addObject(new Plane(Vec3(0, 0, -1), Point3(0, 0, 4000), Color(1, 1, 1), 1.0f));
	rayTracer.addObject(new Plane(Vec3(0, 0, 1), Point3(0, 0, -3000), Color(1, 1, 1), 1.0f));

	rayTracer.addObject(new Plane(Vec3(1, 0, 0), Point3(-3000, 0, 0), Color(1, 1, 1), 1.0f));
	rayTracer.addObject(new Plane(Vec3(-1, 0, 0), Point3(3000, 0, 0), Color(1, 1, 1), 1.0f));


	rayTracer.addObject(new Sphere({ 200, 350, 1000 }, 350, { 0.5f, 0.5f, 0.5f }, { 0.0f, 0.0f, 0.0f }, 1.5f, 0.85f));

	rayTracer.addLightSource(new SphereSpotLight({ 200, 1500, 1000 }, { 255, 255, 255 }, 3.0f, { 0.0f, -1, 0.0f }, 0.03f, 80.0f));


	rayTracer.trace(camera, 1, Color(0, 0, 0), Color(0, 0, 0), 1, bitmap); 
	
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

				if (keys['q']) camera.turnByY(-0.05);
				if (keys['e']) camera.turnByY(0.05);

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
				Draw_Pixel(screen, x, y, bitmap[x + (SCR_HEIGHT - y - 1) * SCR_WIDTH]);
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

