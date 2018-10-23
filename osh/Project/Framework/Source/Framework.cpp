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