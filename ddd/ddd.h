// $Id$
// DDD application resources

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Dorothea Luetkehaus (luetke@ips.cs.tu-bs.de).
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

#ifndef _ddd_h
#define _ddd_h

#ifdef __GNUG__
#pragma interface
#endif

#include <Xm/Xm.h>
#include <X11/StringDefs.h>
#include <X11/Xlib.h>

#include "strclass.h"
#include "Delay.h"

#define DDD_CLASS_NAME           "Ddd"

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
#define XtNsuppressWarnings      "suppressWarnings"
#define XtCSuppressWarnings      "SuppressWarnings"
#define XtNttyMode               "ttyMode"
#define XtNfullNameMode          "fullNameMode"
#define XtCTTYMode               "TTYMode"
#define XtNdddinitVersion        "dddinitVersion"
#define XtNappDefaultsVersion    "appDefaultsVersion"
#define XtCVersion               "Version"

typedef struct {
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
    String  edit_command;
    Boolean panned_graph_editor;
    Boolean find_words_only;
    Boolean group_iconify;
    Boolean global_tab_completion;
    Boolean save_options_on_exit;
    Boolean save_history_on_exit;
    Boolean cache_source_files;
    Boolean suppress_warnings;
    Boolean tty_mode;
    Boolean full_name_mode;
    String  dddinit_version;
    String  app_defaults_version;
} AppData;

// Some global widgets
extern Widget data_disp_shell, source_view_shell;
extern Widget gdb_w, arg_cmd_w;

// In the following functions, ORIGIN indicates some widget
// which originated the command.  Any subsequent dialogs
// are positioned next to ORIGIN.  If ORIGIN is omitted,
// the widget executing the last command is used.

// Output TEXT in GDB window, unless we've just composed a command
extern void gdb_out(const string& text);

// Output TEXT in GDB window (unconditionally)
extern void _gdb_out(string text);

// Output TEXT on controlling TTY, unless we've just composed a command
extern void tty_out(const string& text);

// Output TEXT on controlling TTY (unconditionally)
extern void _tty_out(const string& text);

// Output TEXT on controlling TTY if we're in full_name_mode
extern void tty_full_name(const string& text);

// Give an artificial prompt
extern void prompt();

// Error handling
extern void post_gdb_busy(Widget origin = 0);
extern void post_gdb_message(string text, Widget origin = 0);
extern void post_gdb_yn(string text, Widget origin = 0);
extern void post_gdb_died(string reason, Widget origin = 0);
extern void post_error(string text, String name = 0, Widget origin = 0);
extern void post_warning(string text, String name = 0, Widget origin = 0);

// Show TEXT in status window
void set_status(const string& text);

// Send COMMAND to GDB, if GDB is ready; queue in COMMAND, otherwise
extern void gdb_command(const string& command, Widget origin = 0);

// Send COMMAND to GDB (unconditionally)
extern void _gdb_command(string command, Widget origin = 0);

// Send COMMAND to GDB; return answer (`string(-1)' if none)
// TIMEOUT is either 0 (= use default timeout), -1 (= no timeout)
// or maximal time in seconds
string gdb_question(const string& command, int timeout = 0);

// Find location of FUNC using DBX
string dbx_lookup(const string& func_name);
void clear_dbx_lookup_cache();

// Pass the COMMAND given in CLIENT_DATA to gdb_command()
void gdbCommandCB(Widget w, XtPointer call_data, XtPointer client_data);

// Command management
extern bool remote_gdb();
extern string sh_command(string command, bool force_local = false);
extern string sh_quote(string s);

// Shell management
void initial_popup_shell(Widget w);

// History management
void process_history_filename(string answer);
void process_history_size(string answer);
void process_history_save(string answer);

class StatusDelay: public Delay {
private:
    string cause;

public:
    StatusDelay(const string& c):
	cause(c)
    {
	set_status(cause + "...");
    }

    ~StatusDelay()
    {
	set_status(cause + "... done.");
    }
};

#endif
