#include "Utils.h"
#include "AntiDebug.h"
#include "Technique.h"

#include <iostream>
#include <Windows.h>

#include "Console.h"


bool TechniqueGroup::Check() const
{
	bool result = false;
	auto sParentName = m_wpParent.expired() 
		? std::string{ "" } 
		: m_wpParent.lock()->GetName();

	if (IsGroup() && IsEmpty())
		return result;
		
	Console::SetDefault();

	if (!sParentName.empty())
		std::cout << "[" << sParentName << "]";
	if (!m_lstChildren.empty() && !m_lstChildren.front()->IsGroup())
		std::cout << "[" << GetName() << "]" <<  std::endl;

	for (auto &technique : m_lstChildren)
	{
		bool bDebuggerDetected = technique->Check();
		
		if (!technique->IsGroup())
		{
			Console::SetColor(bDebuggerDetected 
				? Console::Colors::Red 
				: Console::Colors::Green);

			std::cout << (bDebuggerDetected ? " true : " : "false : ") 
				      << technique->GetName() << std::endl;

			Console::SetDefault();
		}
		
		if (bDebuggerDetected & !result)
			result = true;
	}

	return result;
}
