#include "stdafx.h"
#include "config.h"

#include "tracer.h"

//Camera camera({ 100.0f, 200, 0 }, { 0, 1, 0.2f }, { 1, 0, 0.2f }, SCR_WIDTH, SCR_HEIGHT, 800);

Camera camera({ 0.0f, 200, 0 }, { 0, 1, 0.0f }, { 1, 0, 0.0f }, SCR_WIDTH, SCR_HEIGHT, 1000);

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

	/*
	rayTracer.addObject(new Sphere({ 140, 200, 500 }, 200.0f, { 0.3f, 0, 0 }, { 0.50f, 0.4f, 0.4f }, 1.5f, 0.0f));
	rayTracer.addObject(new Sphere({ 250, 300, 1200 }, 300.0f, { 0.4f, 0.55f, 0.1f }, { 0.1f, 0.1f, 0.1f }, 1.5f, 1.0f));
	rayTracer.addObject(new Sphere({ 500, 200, 700 }, 200.0f, { 0.4f, 0, 0.5f }, { 0.05f, 0.1f, 0.05f }, 1.4f, 0.6f));
	rayTracer.addObject(new Sphere({ 1000, 400, 1100 }, 400.0f, { 0.95f, 0.95f, 0.99f }, { 0.0f, 0.0f, 0.0f }, 1.0f, 0.0f));
	rayTracer.addObject(new Sphere({ 900, 200, 500 }, 200.0f, { 0.8f, 0.7f, 0.1f }, { 0.0f, 0.0f, 0.0f }, 1.5f, 0.0f));

	rayTracer.addObject(new Plane(Vec3(0, -1, 0), Point3(0, 3000, 0), Color(0.25, 0.25, 0.25), 1.0f));				// up
	rayTracer.addObject(new CheesePlane(Vec3(0, 1, 0), Point3(0, 0, 0), Color(0.25, 0.25, 0.25), 0.0001f));			// down

	rayTracer.addObject(new Plane(Vec3(0, 0, -1), Point3(0, 0, 3000), Color(0.25, 0.25, 0.25), 1.0f));				//front
	rayTracer.addObject(new Plane(Vec3(0, 0, 1), Point3(0, 0, -1000), Color(0.25, 0.25, 0.25), 1.0f));				//back

	rayTracer.addObject(new Plane(Vec3(1, 0, 0), Point3(-1000, 0, 0), Color(0.25, 0.25, 0.25), 1.0f));					//left
	rayTracer.addObject(new Plane(Vec3(-1, 0, 0), Point3(3000, 0, 0), Color(0.25, 0.25, 0.25), 1.0f));				//right

	rayTracer.addLightSource(new DotLight({ 800, 1500, 2900 }, { 255, 245, 250 }, 0.8));
	rayTracer.addLightSource(new DotLight({ 800, 1500, 1000 }, { 255, 245, 250 }, 0.8));
	rayTracer.addLightSource(new DotLight({ 1600, 1500, 100 }, { 255, 245, 250 }, 0.8));
	*/

	/*
	rayTracer.addObject(new CheesePlane(Vec3(0, 1, 0), Point3(0, 0, 0), Color(0.25, 0.25, 0.25), 0.3f));
	rayTracer.addObject(new Sphere({ 400, 350, 1400 }, 350, { 0.5f, 0.5f, 0.5f }, { 0.0f, 0.0f, 0.0f }, 1.5f, 0.8f));

	rayTracer.addLightSource(new SpotLight({ 1000.0f, 1500, 1800 }, { 255.0f, 0, 0 }, 3.5f, {-0.7f, -1, -0.40f}, 0.04f));
	rayTracer.addLightSource(new SpotLight({ -100, 1500, 1800 }, { 0, 255, 0 }, 3.5f, { 0.7f, -1, -0.40f }, 0.04f));
	rayTracer.addLightSource(new SpotLight({ 700, 1500, 1800 }, { 0, 0, 255 }, 3.5f, { -0.2f, -1, -0.80f }, 0.04f));
	*/

	/*
	rayTracer.addObject(new Plane(Vec3(0, 1, 0), Point3(0, 0, 0), Color(0.9, 0.9, 0.9), 1.0f));
	rayTracer.addObject(new Sphere({ 640, 400, 1450 }, 300, { .05f, .05f, .05f }, { 0.95f, 0.95f, 0.95f }, 1.4f, 0.9f));

	rayTracer.addLightSource(new DotLight({ 640, 800, 1450 }, { 255, 255, 255 }, 1.0));
	*/

	/*
	rayTracer.addObject(new CheesePlane(Vec3(0, 1, 0), Point3(0, 0, 0), Color(0.25, 0.25, 0.25), 0.5f));
	rayTracer.addObject(new Sphere({ 400, 500, 1400 }, 350, { 0.9f, 0.9f, 0.9f }, { 0.1f, 0.1f, 0.1f }, 1.5f, 1.0f));


	rayTracer.addObject(new Sphere({ 400, 1300, 1400 }, 100, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, 1.5f, 1.0f));
	rayTracer.addLightSource(new SpotLight({ 400, 1100, 1400 }, { 255.0f, 255.0f, 255.0f }, 2.0f, {0,1,0}, 0.4f));
	*/

	
	rayTracer.addObject(new Plane(Vec3(0, 1, 0), Point3(0, 0, 0), Color(0.25, 0.25, 0.25), 0.9f));

	rayTracer.addObject(new Sphere({ 400, 350, 1400 }, 350, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, 1.0f, 0.1f));

	rayTracer.addLightSource(new SphereLight({ 1000.0f, 1500, 1800 }, { 255.0f, 255.0f, 255.0f }, 1.5f, 70.0f));
	

	rayTracer.trace(camera, 6, Color(0, 0, 0), Color(0, 0, 0), 1, bitmap); 
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

