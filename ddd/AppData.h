// $Id$ -*- C++ -*-
// DDD application data

// Copyright (C) 1996 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller (zeller@ips.cs.tu-bs.de).
// 
// This file is part of the DDD Library.
// 
// The DDD Library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// The DDD Library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Library General Public License for more details.
// 
// You should have received a copy of the GNU Library General Public
// License along with the DDD Library -- see the file COPYING.LIB.
// If not, write to the Free Software Foundation, Inc.,
// 675 Mass Ave, Cambridge, MA 02139, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

#ifndef _DDD_AppData_h
#define _DDD_AppData_h

#include <Xm/Xm.h>
#include <X11/StringDefs.h>
#include <X11/Xlib.h>

#include "version.h"

// The DDD application class name
#define DDD_CLASS_NAME           Ddd_NAME

// For a documentation of these resources, see `Ddd.in'.
#define XtNdebugger              "debugger"
#define XtCDebugger              "Debugger"
#define XtNdebuggerCommand       "debuggerCommand"
#define XtCDebuggerCommand       "DebuggerCommand"
#define XtNtraceDialog           "traceDialog"
#define XtNtraceShellCommands    "traceShellCommands"
#define XtCTrace                 "Trace"
#define XtNgdbInitCommands       "gdbInitCommands"
#define XtCGDBInitCommands       "GDBInitCommands"
#define XtNdbxInitCommands       "dbxInitCommands"
#define XtCDBXInitCommands       "DBXInitCommands"
#define XtNvslPath               "vslPath"
#define XtCVSLPath               "VSLPath"
#define XtNvslLibrary            "vslLibrary"
#define XtCVSLLibrary            "VSLLibrary"
#define XtNmaxNameLength         "maxNameLength"
#define XtCMaxNameLength         "MaxNameLength"
#define XtNindentAmount          "indentAmount"
#define XtCIndentAmount          "IndentAmount"
#define XtNcommandButtons        "commandButtons"
#define XtCCommandButtons        "CommandButtons"
#define XtNsourceButtons         "sourceButtons"
#define XtCSourceButtons         "sourceButtons"
#define XtNseparateDataWindow    "separateDataWindow"
#define XtNseparateSourceWindow  "separateSourceWindow"
#define XtNseparateExecWindow    "separateExecWindow"
#define XtCSeparate              "Separate"
#define XtNtermCommand           "termCommand"
#define XtCTermCommand           "TermCommand"
#define XtNtermType              "termType"
#define XtCTermType              "TermType"
#define XtNtermTimeout           "termTimeout"
#define XtCTermTimeout           "TermTimeout"
#define XtNuseTTYCommand         "useTTYCommand"
#define XtCUseTTYCommand         "UseTTYCommand"
#define XtNtermStartProcess      "termStartProcess"
#define XtCTermStartProcess      "TermStartProcess"
#define XtNquestionTimeout       "questionTimeout"
#define XtCQuestionTimeout       "QuestionTimeout"
#define XtNsynchronousDebugger   "synchronousDebugger"
#define XtCSynchronousDebugger   "SynchronousDebugger"
#define XtNdebuggerHost          "debuggerHost"
#define XtCDebuggerHost          "DebuggerHost"
#define XtNdebuggerHostLogin     "debuggerHostLogin"
#define XtCDebuggerHostLogin     "DebuggerHostLogin"
#define XtNrshCommand            "rshCommand"
#define XtCRshCommand            "RshCommand"
#define XtNlistExecCommand       "listExecCommand"
#define XtCListExecCommand       "ListExecCommand"
#define XtNlistSourceCommand     "listSourceCommand"
#define XtCListSourceCommand     "ListSourceCommand"
#define XtNlistCoreCommand       "listCoreCommand"
#define XtCListCoreCommand       "ListCoreCommand"
#define XtNlistDirCommand        "listDirCommand"
#define XtCListDirCommand        "ListDirCommand"
#define XtNwwwPage               "wwwPage"
#define XtCWWWPage               "WWWPage"
#define XtNwwwCommand            "wwwCommand"
#define XtCWWWCommand            "WWWCommand"
#define XtNshowInvocation        "showInvocation"
#define XtCShowInvocation        "ShowInvocation"
#define XtNshowVersion           "showVersion"
#define XtCShowVersion           "ShowVersion"
#define XtNshowConfiguration     "showConfiguration"
#define XtCShowConfiguration     "ShowConfiguration"
#define XtNshowResources         "showResources"
#define XtCShowResources         "ShowResources"
#define XtNshowManual            "showManual"
#define XtCShowManual            "ShowManual"
#define XtNcheckConfiguration    "checkConfiguration"
#define XtCCheckConfiguration    "CheckConfiguration"
#define XtNprintCommand          "printCommand"
#define XtCPrintCommand          "PrintCommand"
#define XtNpaperSize             "paperSize"
#define XtCPaperSize             "PaperSize"
#define XtNeditCommand           "editCommand"
#define XtCEditCommand           "EditCommand"
#define XtNpannedGraphEditor     "pannedGraphEditor"
#define XtCPannedGraphEditor     "PannedGraphEditor"
#define XtNfindWordsOnly         "findWordsOnly"
#define XtCFindWordsOnly         "FindWordsOnly"
#define XtNgroupIconify          "groupIconify"
#define XtCGroupIconify          "GroupIconify"
#define XtNglobalTabCompletion   "globalTabCompletion"
#define XtCGlobalTabCompletion   "GlobalTabCompletion"
#define XtNsaveOptionsOnExit     "saveOptionsOnExit"
#define XtCSaveOptionsOnExit     "SaveOptionsOnExit"
#define XtNsaveHistoryOnExit     "saveHistoryOnExit"
#define XtCSaveHistoryOnExit     "SaveHistoryOnExit"
#define XtNcacheSourceFiles      "cacheSourceFiles"
#define XtCCacheSourceFiles      "CacheSourceFiles"
#define XtNcacheMachineCode      "cacheMachineCode"
#define XtCCacheMachineCode      "CacheMachineCode"
#define XtNsuppressWarnings      "suppressWarnings"
#define XtCSuppressWarnings      "SuppressWarnings"
#define XtNttyMode               "ttyMode"
#define XtNfullNameMode          "fullNameMode"
#define XtCTTYMode               "TTYMode"
#define XtNdisplayGlyphs         "displayGlyphs"
#define XtCDisplayGlyphs         "DisplayGlyphs"
#define XtNdisassemble           "disassemble"
#define XtCDisassemble           "Disassemble"
#define XtNdddinitVersion        "dddinitVersion"
#define XtNappDefaultsVersion    "appDefaultsVersion"
#define XtCVersion               "Version"

struct AppData {
    String  debugger;
    String  debugger_command;
    Boolean trace_dialog;
    Boolean trace_shell_commands;
    String  gdb_initial_cmds;
    String  dbx_initial_cmds;
    String  vslPath;
    String  vslLibrary;
    int     max_name_length;
    int     indent_amount;
    String  command_buttons;
    String  source_buttons;
    Boolean separate_data_window;
    Boolean separate_source_window;
    Boolean separate_exec_window;
    String  term_command;
    String  term_type;
    Boolean use_tty_command;
    String  term_start_process;
    int     question_timeout;
    Boolean synchronous_gdb;
    String  debugger_host;
    String  debugger_host_login;
    String  rsh_command;
    String  list_exec_command;
    String  list_source_command;
    String  list_core_command;
    String  list_dir_command;
    String  www_page;
    String  www_command;
    Boolean show_invocation;
    Boolean show_version;
    Boolean show_configuration;
    Boolean show_resources;
    Boolean show_manual;
    Boolean check_configuration;
    String  print_command;
    String  paper_size;
    String  edit_command;
    Boolean panned_graph_editor;
    Boolean find_words_only;
    Boolean group_iconify;
    Boolean global_tab_completion;
    Boolean save_options_on_exit;
    Boolean save_history_on_exit;
    Boolean cache_source_files;
    Boolean cache_machine_code;
    Boolean suppress_warnings;
    Boolean tty_mode;
    Boolean full_name_mode;
    Boolean display_glyphs;
    Boolean disassemble;
    String  dddinit_version;
    String  app_defaults_version;
};

extern AppData app_data;

#endif // _DDD_AppData_h
// DON'T ADD ANYTHING BEHIND THIS #endif
