#include <memory>
#include <list>
#include <iostream>
#include <exception>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "config.h"
#include "AntiDebug.h"

#include "DebuggerFugitive.h"
#include "Console.h"
#include "interprocess.h"

bool DebuggerFugitive::ParseConfig(const char *szConfig)
{
	namespace pt = boost::property_tree;

	try
	{
		pt::ptree root;
		ReadConfigFile(szConfig, root);

		for (auto &node : root)
		{
			auto name = node.second.get<std::string>("name");
			auto subgroups = node.second.get_child_optional("subgroups");
			if (!subgroups)
				continue;

			auto group = anti_debug_ptr(new TechniqueGroup(name));
			ParseNode(subgroups.get(), group);
			if (!group->IsEmpty())
				m_lstAntiDebugGroups.push_back(group);
		}
	}
	catch (const pt::ptree_error &e)
	{
		Console::SetYellow();
		std::cout << "ERROR: " << e.what() << std::endl;
		Console::SetDefault();
		return false;
	}
	catch (...)
	{
		HandleException(std::current_exception());
		return false;
	}
	return true;
}

void DebuggerFugitive::Execute()
{
	for (auto &pAntiDebugGroup : m_lstAntiDebugGroups)
		pAntiDebugGroup->Check();
}

void DebuggerFugitive::ParseNode(boost::property_tree::ptree &root, anti_debug_ptr &parent)
{
	for (auto &node : root)
	{
		auto name = node.second.get<std::string>("name");
		auto subgroups = node.second.get_child_optional("subgroups");
		auto tags = GetNodeTags(node);
		if (!CheckTags(tags))
			continue;

		auto new_node = !subgroups
			? GetTechniqueByName(name, parent)
			: anti_debug_ptr(new TechniqueGroup(name, parent));
		if (!new_node)
			continue;

		auto params = node.second.get_child_optional("parameters");
		if (params)
		{
			for (auto &param : params.get())
			{
				auto param_name = param.second.get<std::string>("name");
				auto param_type_str = param.second.get<std::string>("type");
				auto param_value_str = param.second.get<std::string>("value");

				auto param_type = ParseParamType(param_type_str);
				auto param_value = ParseParamValue(param_value_str, param_type);
				new_node->AddParameter(param_name, param_value);
			}
		}

		if (subgroups)
			ParseNode(subgroups.get(), new_node);
		else
		{
			PVOID pMethod = GetCheckAddress((PVOID)new_node.get());
			if (UiProxy::GetInstance().bEnabled)
			{
				int nTechniqueId = UiProxy::GetInstance().GetTechniqueId(name);
				UiProxy::GetInstance().SetCheckAddress(nTechniqueId, (DWORD)pMethod);
			}
			else
			{
				std::cout << pMethod << " : " << name << std::endl;
				if (m_bShowInfo && node.second.get_child_optional("info"))
					std::cout << node.second.get<std::string>("info") << std::endl << std::endl;
			}
		}

		parent->AddTechnique(new_node);
	}
}

anti_debug_ptr DebuggerFugitive::GetTechniqueByName(const std::string szName, const anti_debug_ptr &pParent)
{
	auto it = std::find_if(Config::TechniqueToName.cbegin(), Config::TechniqueToName.cend(),
		[&](const std::pair<Config::ETechnique, std::string> &p) {
		return p.second == szName;
	});
	if (Config::TechniqueToName.end() == it)
		return anti_debug_ptr(new TechniqueGroup(szName));
	return CREATE_TECHNIQUE(it->first, szName, pParent);
}

ParamType DebuggerFugitive::ParseParamType(std::string &type)
{
	if (type == "dword")
		return ParamType::Dword;
	if (type == "qword")
		return ParamType::Qword;
	if (type == "real")
		return ParamType::Real;
	return ParamType::String;
}

ParamValue DebuggerFugitive::ParseParamValue(std::string &value, ParamType type)
{
	switch (type)
	{
	case ParamType::Dword:
		return ParamValue{ (std::uint32_t)std::stoul(value) };
	case ParamType::Qword:
		return ParamValue{ (std::uint64_t)std::stoull(value) };
	case ParamType::Real:
		return ParamValue{ (std::double_t)std::stod(value) };
	default:
		return ParamValue{ value };
	}
}

std::list<std::string> DebuggerFugitive::GetNodeTags(std::pair<const std::string, boost::property_tree::ptree> &node)
{
	std::list<std::string> lstTags;
	auto tags = node.second.get_child_optional("tags");
	if (tags)
	{
		for (boost::property_tree::ptree::value_type &tag : node.second.get_child("tags"))
			lstTags.push_back(tag.second.get<std::string>("", ""));
	}
	return lstTags;
}

bool DebuggerFugitive::CheckTags(std::list<std::string> &nodeTags)
{
	if (nodeTags.empty())
		return true;

	for (auto &tag : nodeTags)
	{
		if (m_mExecutionOptions.find(tag) == m_mExecutionOptions.end())
			continue;
		if (!m_mExecutionOptions[tag])
			return false;
	}
	return true;
}

// Dirty Hack: This function is supposed to retrieve the address of Check() method of
//             an object derived from Technique class.
//             We obtain the VTable address of the corresponding class and get the
//             address of Check() method at hardcoded 4*sizeof(DWORD_PTR) byte.
//             Then we check whether it is the method itself or a JMP stub.
//             If it is a JMP to the method, we compute the address from the JMP instruction.
void *DebuggerFugitive::GetCheckAddress(void *pInstance)
{
	DWORD_PTR pVtable = *(DWORD_PTR *)pInstance;
	PVOID pMethod = (PVOID)(*((PDWORD)((DWORD_PTR)pVtable + sizeof(DWORD_PTR) * 4)));;
	if (*(PBYTE)pMethod == 0xE9)
		pMethod = (PVOID)((DWORD)pMethod + 5 + *(PDWORD)((PBYTE)pMethod + 1));
	return pMethod;
}

void DebuggerFugitive::HandleException(std::exception_ptr pException)
{
	try
	{
		if (pException)
			std::rethrow_exception(pException);
	}
	catch(const std::exception &e)
	{
		Console::SetYellow();
		std::cout << "ERROR: " << e.what() << std::endl;
		Console::SetDefault();
	}
}

void DebuggerFugitive::ReadConfigFile(const char *szFilePath, boost::property_tree::ptree &root)
{
	namespace pt = boost::property_tree;
	if (!UiProxy::GetInstance().bEnabled)
	{
		pt::read_json(szFilePath, root);
	}
	else
	{
		interprocess::SharedFile sharedFile = { 0 };
		
		if (!interprocess::InitSharedFile(
			&sharedFile,
			szFilePath,
			strlen(szFilePath),
			UiProxy::GetInstance().dwFileSize))
			throw std::exception("Can not initialize shared file data!");
		
		if (!interprocess::ReadSharedFile(&sharedFile))
			throw std::exception("Can not read a shared file!");

		std::string sFileData{ (LPSTR)sharedFile.pBuffer };
		sFileData.resize(sharedFile.dwFileSize);

		std::stringstream ss;
		ss << sFileData;

		pt::read_json(ss, root);
	}
}
