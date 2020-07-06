#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include <Windows.h>

namespace Console
{
	enum class Colors
	{
		Default,
		Green,
		Red,
		Yellow,
	};

	class ConsoleHander
	{
	public:
		ConsoleHander(const ConsoleHander &) = delete;
		void operator=(const ConsoleHander &) = delete;

		static ConsoleHander &GetInstance();

		void SetDefault();
		void SetRed();
		void SetGreen();
		void SetYellow();

		void SetColor(Colors color);

	private:
		ConsoleHander();

	private:
		HANDLE m_hConsole;
	};

	void SetDefault();
	void SetRed();
	void SetGreen();
	void SetYellow();

	void SetColor(Colors color);
}

#endif // _CONSOLE_H_
