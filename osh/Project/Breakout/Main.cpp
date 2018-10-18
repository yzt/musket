#include <Framework.h>

using namespace GameFramework;

const int WIDTH = 800;
const int HEIGHT = 600;
const char *GAME_NAME = "Breakout Game";

void main()
{
	Framework framework;

	framework.Initialize();

	framework.MakeWindow(GAME_NAME, WIDTH, HEIGHT);

	framework.Run();

	framework.Uninitialize();
}