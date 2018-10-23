#pragma once

#include "Common.h"


namespace GameFramework
{
	class Mesh;

	struct Vector2
	{
	public:
		float X, Y;
	};

	struct Vector3
	{
	public:
		float X, Y, Z;
	};

	class MeshCreateInfo
	{
		//Vertex
		// Position : x, y, z
		// UV : x, y
		// Vertex [x, y, z|x, y]

	public:
		Vector3 *Positions;
		Vector2 *UVs;

		unsigned int VertexCount;

		unsigned int *Indices;
		unsigned int IndexCount;
	};

	class FRAMEWORK_API Framework
	{
	public:
		Framework(void);

		bool Initialize(void);
		bool Uninitialize(void);

		bool MakeWindow(const char *Title, int Width, int Height);

		Mesh *CreateMesh(MeshCreateInfo &Info);

		void Run(void);

	private:
		void *m_Window;
	};
}