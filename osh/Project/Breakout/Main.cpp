#include <Windows.h>
#include <iostream>

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

void main(void)
{
	float lastCountTime = GetTime();
	unsigned int frameCount = 0;
	float lastTime = 0.0F;
	float totalFrameTime = 0.0F;
	float avgFPS = 0.0F;

	while (true)
	{
		float nowTime = GetTime();

		// Clear Back-Buffer

		// Render

		// Swap Buffers

		// Poll Events


		for (int i = 0; i < 999; ++i)
		{

		}

		//
		// TODO : Debug FPS and Frame-Time calculation
		//
		totalFrameTime += nowTime - lastTime;
		++frameCount;
		lastTime = nowTime;

		if (nowTime - lastCountTime >= 1.0F)
		{
			avgFPS = totalFrameTime / frameCount;

			std::cout << "Average FPS : " << avgFPS << " Average Frame Time : " << ((nowTime - lastCountTime) / avgFPS) << std::endl;

			frameCount = 0;
			lastCountTime = nowTime;
			totalFrameTime = 0.0F;
		}
	}
}