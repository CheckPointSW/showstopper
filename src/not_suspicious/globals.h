#ifndef _GLOBALS_H_
#define _GLOBALS_H_

//
// Global Object Names
//
#define CMD_OPTIONS_DESCRIPTION_NAME "not_suspicious.exe"
// Common arguments
#define CMD_OPTION_HELP                 "help"
#define CMD_OPTION_HELP_VARIANTS        "h,help"
#define CMD_OPTION_CONFIG_PATH          "config"
#define CMD_OPTION_CONFIG_PATH_VARIANTS "c,config"
#define CMD_OPTION_SHOW_INFO            "info"
#define CMD_OPTION_SHOW_INFO_VARIANTS   "i,info"
#define CMD_OPTION_AUXILIARY            "auxiliary"
#define CMD_OPTION_AUXILIARY_VARIANTS   "a,auxiliary"
// Techniques options arguments
#define CMD_TECHNIQUE_OPTION_GROUP      "Techniques"
#define CMD_OPTION_ADMIN                "admin"
#define CMD_OPTION_CRASH                "crash"
#define CMD_OPTION_TRACE                "trace"
#define CMD_OPTION_BREAK                "break"
#define CMD_OPTION_ATTACH               "attach"

#define EVENT_SELFDBG_EVENT_NAME L"SelfDebugging"

//
// Project Command Line Arguments Oprions
//
#define CMD_OPTION_AUXILIARY_SELFDEBUGGING    "SelfDebugging"
#define CMD_OPTION_AUXILIARY_UIPROXY          "UiProxy"
#define CMD_PARAMETERS_COUNT_AUXILIARY        4

#define DAFAULT_CONFIG_FILE_NAME              "config.json"

#endif // _GLOBALS_H_
