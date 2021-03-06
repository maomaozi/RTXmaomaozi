#include "stdafx.h"
#include "config.h"

#include "tracer.h"
#include "scence.h"

#include "kdTree.h"

Camera camera({ 0.0f, 800.0f, -1000 }, { -0.06f, 1.0f, 0.3f }, { 1.0f, 0.0f, 0.2f }, SCR_WIDTH, SCR_HEIGHT, 800.0f);


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
	Scence scence;

	scence.addPlane(new Plane(Vec3(0, 1, 0), Point3(0, 0, 0), Color(1, 1, 1), 1.0f));

	scence.addTriangle(new Triangle({ 2200, 2500, 1000 }, { 2200, 2500, 4000 }, { -4000, 2500, 4000 }, { 1, 1, 1 }, { 0.0f, 0.0f, 0.0f }, 1.4f, 1.0f));
	scence.addTriangle(new Triangle({ 2200, 2500, 1000 }, { -3000, 2500, 4000 }, { -3000, 2500, -3000 }, { 1, 1, 1 }, { 0.0f, 0.0f, 0.0f }, 1.4f, 1.0f));
	scence.addTriangle(new Triangle({ -4000, 2501, 4000 }, { 2200, 2501, 4000 }, { 2200, 2501, 1000 }, { 1, 1, 1 }, { 0.0f, 0.0f, 0.0f }, 1.4f, 1.0f));
	scence.addTriangle(new Triangle({ -3000, 2501, -3000 }, { -3000, 2501, 4000 }, { 2200, 2501, 1000 }, { 1, 1, 1 }, { 0.0f, 0.0f, 0.0f }, 1.4f, 1.0f));

	scence.addPlane(new Plane(Vec3(0, 0, -1), Point3(0, 0, 3000), Color(1, 1, 1), 1.0f));
	scence.addPlane(new Plane(Vec3(0, 0, 1), Point3(0, 0, -3000), Color(1, 1, 1), 1.0f));

	scence.addPlane(new Plane(Vec3(1, 0, 0), Point3(-2000, 0, 0), Color(1, 1, 1), 1.0f));


	scence.addTriangle(new Triangle({ 2200, 0, 1100 }, { 2200, 10000, 1100 }, { 2200, 0, -1000 }, { 1, 1, 1 }, { 0.0f, 0.0f, 0.0f }, 1.4f, 1.0f));
	scence.addTriangle(new Triangle({ 2200, 0, 10000 }, { 2200, 10000, 1600 }, { 2200, 0, 1600 }, { 1, 1, 1 }, { 0.0f, 0.0f, 0.0f }, 1.4f, 1.0f));

	scence.addTriangle(new Triangle({ 2250, 0, -1000 }, { 2250, 10000, 1100 }, { 2250, 0, 1100 }, { 1, 1, 1 }, { 0.0f, 0.0f, 0.0f }, 1.4f, 1.0f));
	scence.addTriangle(new Triangle({ 2250, 0, 1600 }, { 2250, 10000, 1600 }, { 2250, 0, 10000 }, { 1, 1, 1 }, { 0.0f, 0.0f, 0.0f }, 1.4f, 1.0f));

	scence.addSphere(new Sphere({ 500, 800, 1000 }, 400, { 0.01f, 0.01f, 0.01f }, { 0.99f, 0.99f, 0.98f }, 1.6f, 0.01f));
	scence.addSphere(new Sphere({ -200, 400, 1900 }, 400, { 0.99f, 0.94f, 0 }, { 0.00f, 0.00f, 0.00f }, 1.0f, 0.05f));

	//for (int i = 0; i < 7; ++i) {
	//	for (int j = 0; j < 4; ++j) {
	//		for (int k = 0; k < 4; ++k) {
	//			scence.addSphere(new Sphere({ -800.0f + 400 * i, 200.0f + 300 * j, 300.0f + 300 * k }, 120, { 0.01f, 0.01f, 0.01f }, { 0.99f, 0.99f, 0.99f }, 1.5f, 0.01f));
	//		}
	//	}
	//}


	scence.addLight(new SphereSpotLight({ 500, 2000, 1000 }, { 255, 230, 202 }, 30.0f, { 0.0f, -1, 0.0f }, 0.01f, 300.0f));
	scence.addLight(new SphereDotLight({ 500, 2200, 1000 }, { 255, 230, 202 }, 20.0f, 80.0f));
	scence.addLight(new SphereSpotLight({ 5000, 3000, 1500 }, { 135,206,250 }, 30.0f, { -1.0f, -0.8f, 0.0f }, 0.01f, 350.0f));

	scence.build();
	rayTracer.setSence(&scence);


	rayTracer.trace(camera, 8, Color(0, 0, 0), Color(0, 0, 0), 2, bitmap);

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

		for (int y = 0; y < SCR_HEIGHT; ++y)
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

