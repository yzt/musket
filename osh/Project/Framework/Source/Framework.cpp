#include <Framework.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <Windows.h>
#include <iostream>
#include "Mesh.h"

namespace GameFramework
{
	long GetFrequency(void)
	{
		LARGE_INTEGER var;
		QueryPerformanceFrequency(&var);
		return var.QuadPart;
	}

	float GetTimestamp(void)
	{
		static long frequency = GetFrequency();

		LARGE_INTEGER clock;
		QueryPerformanceCounter(&clock);

		return (float(clock.QuadPart * 1000000) / frequency) / 1000000;
	}

	float GetTime(void)
	{
		static float start = GetTimestamp();

		return GetTimestamp() - start;
	}

	Framework::Framework(void) :
		m_Window(nullptr)
	{
	}

	bool Framework::Initialize(void)
	{
		glfwInit();
		glfwWindowHint(GLFW_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_VERSION_MINOR, 3);

		return true;
	}

	bool Framework::Uninitialize(void)
	{
		glfwTerminate();

		return true;
	}

	bool Framework::MakeWindow(const char * Title, int Width, int Height)
	{
		GLFWwindow *window = glfwCreateWindow(Width, Height, Title, nullptr, nullptr);

		if (window == nullptr)
			return false;

		glfwMakeContextCurrent(window);

		glewInit();

		m_Window = window;

		return true;
	}

	Mesh * Framework::CreateMesh(MeshCreateInfo & Info)
	{
		unsigned int vao;
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		unsigned int elementSize = sizeof(Vector3) + sizeof(Vector2);
		unsigned int bufferSize = Info.VertexCount * elementSize;
		float *buffer = reinterpret_cast<float*>(malloc(bufferSize));

		for (int i = 0; i < Info.VertexCount; ++i)
		{
			int bufferIndex = i * elementSize;
			memcpy(buffer + bufferIndex, Info.Positions + i, sizeof(Vector3));

			bufferIndex += sizeof(Vector3);
			memcpy(buffer + bufferIndex, Info.Positions + i, sizeof(Vector2));
		}

		unsigned int vbo;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_VERTEX_ARRAY, vbo);
		glBufferData(GL_VERTEX_ARRAY, bufferSize, buffer, GL_STATIC_DRAW);

		unsigned int ebo;
		glGenBuffers(1, &ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, Info.IndexCount * sizeof(float), Info.Indices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 2, GL_FLOAT, false, 0, (void*)sizeof(Vector3));
		glEnableVertexAttribArray(0);



			return nullptr;
	}

	void Framework::Run(void)
	{
		GLFWwindow *window = reinterpret_cast<GLFWwindow*>(m_Window);

		float lastCountTime = GetTime();
		unsigned int frameCount = 0;
		float lastTime = 0.0F;
		float totalFrameTime = 0.0F;
		float avgFPS = 0.0F;

		while (true)
		{
			float nowTime = GetTime();

			// Update

			glClear(GL_DEPTH_BUFFER_BIT);

			// Render

			glfwSwapBuffers(window);
			glfwPollEvents();

			totalFrameTime += nowTime - lastTime;
			++frameCount;
			lastTime = nowTime;

			if (nowTime - lastCountTime >= 1.0F)
			{
				avgFPS = frameCount / totalFrameTime;

				std::cout << "Average FPS : " << avgFPS << " Average Frame Time : " << ((nowTime - lastCountTime) / avgFPS) << std::endl;

				frameCount = 0;
				lastCountTime = nowTime;
				totalFrameTime = 0.0F;
			}
		}
	}
}