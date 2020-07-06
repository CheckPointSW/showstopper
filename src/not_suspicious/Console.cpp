#include "Console.h"
#include <Windows.h>

namespace Console
{
	ConsoleHander::ConsoleHander()
		: m_hConsole(GetStdHandle(STD_OUTPUT_HANDLE))
	{
		SetDefault();
	}

	ConsoleHander &ConsoleHander::GetInstance()
	{
		static ConsoleHander console;
		return console;
	}

	void ConsoleHander::SetDefault()
	{
		SetConsoleTextAttribute(m_hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	}

	void ConsoleHander::SetRed()
	{
		SetConsoleTextAttribute(m_hConsole, FOREGROUND_RED);
	}

	void ConsoleHander::SetGreen()
	{
		SetConsoleTextAttribute(m_hConsole, FOREGROUND_GREEN);
	}

	void ConsoleHander::SetYellow()
	{
		SetConsoleTextAttribute(m_hConsole, FOREGROUND_RED | FOREGROUND_GREEN);
	}

	void ConsoleHander::SetColor(Colors color)
	{
		switch (color)
		{
		case Console::Colors::Green:
			SetGreen();
			break;
		case Console::Colors::Red:
			SetRed();
			break;
		case Console::Colors::Yellow:
			SetYellow();
			break;
		case Console::Colors::Default:
		default:
			SetDefault();
		}
	}

	void SetDefault() { ConsoleHander::GetInstance().SetDefault(); }
	void SetRed() { ConsoleHander::GetInstance().SetRed(); }
	void SetGreen() { ConsoleHander::GetInstance().SetGreen(); }
	void SetYellow() { ConsoleHander::GetInstance().SetYellow(); }

	void SetColor(Colors color) { ConsoleHander::GetInstance().SetColor(color); }
}
