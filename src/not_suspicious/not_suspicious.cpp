#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <cxxopts.hpp>

#include <filesystem>
#include <Windows.h>
#include "globals.h"
#include "DebuggerFugitive.h"
#include "ProcessHelper.h"

cxxopts::Options g_opts{ CMD_OPTIONS_DESCRIPTION_NAME };

cxxopts::Options GetOptionsParser()
{
	try
	{
		g_opts.add_options()
			(CMD_OPTION_HELP_VARIANTS, "Show Help")
			(CMD_OPTION_CONFIG_PATH_VARIANTS, "Path to the JSON config", cxxopts::value<std::string>())
			(CMD_OPTION_SHOW_INFO_VARIANTS, "Show the description for each technique")
			(CMD_OPTION_AUXILIARY_VARIANTS, "Execute auxiliary function", cxxopts::value<std::vector<std::string>>());
		g_opts.add_options(CMD_TECHNIQUE_OPTION_GROUP)
			(CMD_OPTION_ADMIN, "Allow techniques that require Admin privilege")
			(CMD_OPTION_CRASH, "Allow techniques that may crash the process")
			(CMD_OPTION_TRACE, "Allow techniques that work only when the technique's code is traced")
			(CMD_OPTION_BREAK, "Allow techniques that require setting a breakpoint")
			(CMD_OPTION_ATTACH, "Allow techniques that require a debugger to be attached after the program was run");
		return g_opts;
	}
	catch (const cxxopts::OptionException &e)
	{
		std::cout << "[ERROR] Error parsing options: " << e.what() << std::endl;
		exit(1);
	}
}

int HandleCmdOptions(const cxxopts::ParseResult &args)
{
	if (args.count(CMD_OPTION_HELP))
	{
		std::cout << g_opts.help({"", CMD_TECHNIQUE_OPTION_GROUP}) << std::endl;
		return 1;
	}

	if (args.count(CMD_OPTION_AUXILIARY))
	{
		auto &aux_opts = args[CMD_OPTION_AUXILIARY].as<std::vector<std::string>>();
		if (aux_opts.size() < 2)
		{
			std::cout << "[ERROR] Wrong amout of arguments for \"" << CMD_OPTION_AUXILIARY << "\" option." << std::endl;
			return 1;
		}

		if (aux_opts[0] == CMD_OPTION_AUXILIARY_SELFDEBUGGING)
		{
			DWORD dwParentPid = std::stoul(aux_opts[1]);
			HANDLE hEvent = OpenEventW(EVENT_MODIFY_STATE, FALSE, EVENT_SELFDBG_EVENT_NAME);
			if (hEvent && process_helper::EnableDebugPrivilege(true))
			{
				if (FALSE == DebugActiveProcess(dwParentPid))
					SetEvent(hEvent);
				else
					DebugActiveProcessStop(dwParentPid);
			}
			ExitProcess(ERROR_SUCCESS);
		}
		else if (aux_opts[0] == CMD_OPTION_AUXILIARY_UIPROXY)
		{
			HANDLE hListView = (HANDLE)std::stoul(aux_opts[1]);
			UiProxy::GetInstance().bEnabled = true;
			UiProxy::GetInstance().hListView = hListView;

			if (aux_opts.size() > 3)
			{
				memcpy_s(UiProxy::GetInstance().szFileName, 256, aux_opts[2].c_str(), aux_opts[2].size());
				UiProxy::GetInstance().dwFileSize = std::stoul(aux_opts[3]);
			}
		}
	}

	std::map<std::string, bool> mParams;
#define ADD_OPTION(m, opt_str, default_value) m.insert({ opt_str, args.count(opt_str) ? true : default_value })
	ADD_OPTION(mParams, CMD_OPTION_ADMIN, false);
	ADD_OPTION(mParams, CMD_OPTION_CRASH, false);
	ADD_OPTION(mParams, CMD_OPTION_TRACE, true);
	ADD_OPTION(mParams, CMD_OPTION_BREAK, true);
	ADD_OPTION(mParams, CMD_OPTION_ATTACH, true);

	std::string sConfigPath = UiProxy::GetInstance().bEnabled
		? UiProxy::GetInstance().szFileName
		: (!args.count(CMD_OPTION_CONFIG_PATH))
			? DAFAULT_CONFIG_FILE_NAME 
			: args[CMD_OPTION_CONFIG_PATH].as<std::string>();
	if (!UiProxy::GetInstance().bEnabled && !std::filesystem::exists(sConfigPath))
	{
		std::cout << g_opts.help({"", CMD_TECHNIQUE_OPTION_GROUP}) << std::endl;
		return 1;
	}

	DebuggerFugitive worker(mParams, args.count(CMD_OPTION_SHOW_INFO));
	if (!worker.ParseConfig(sConfigPath.c_str()))
		return 1;

	std::cout << std::endl;
	std::cout << "Press any key to start checks..." << std::endl;
	std::cin.get();

	worker.Execute();

	return ERROR_SUCCESS;
}

int main(int argc, char **argv)
{
	auto result = HandleCmdOptions(GetOptionsParser().parse(argc, argv));
	system("pause");
	return result;
}
