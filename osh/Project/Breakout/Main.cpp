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

	Vector3 positions[] =
	{
		{-0.5F, -0.5F, 0.0F},
		{0.0F, 0.5F, 0.0F},
		{0.5F, -0.5F, 0.0F}
	};

	Vector2 uvs[] = 
	{
		{0.0F, 0.0F},
		{0.5F, 1.0F},
		{1.0F, 0.0F}
	};

	unsigned int indices[] =
	{
		0, 1, 2
	};

	MeshCreateInfo info;
	info.Positions = positions;
	info.UVs = uvs;
	info.VertexCount = 3;
	info.Indices = indices;
	info.IndexCount = 3;

	framework.CreateMesh(info);

	framework.Run();

	framework.Uninitialize();
}