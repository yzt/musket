#pragma once

namespace GameFramework
{
	class __declspec(dllexport) Framework
	{
	public:
		Framework(void);

		bool Initialize(void);
		bool Uninitialize(void);

		bool MakeWindow(const char *Title, int Width, int Height);

		void Run(void);

	private:
		void *m_Window;
	};
}