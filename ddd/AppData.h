// $Id$ -*- C++ -*-
// DDD application data

// Copyright (C) 1996 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@ips.cs.tu-bs.de>.
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
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
#define XtNgdbSettings           "gdbSettings"
#define XtCGDBSettings           "GDBSettings"
#define XtNdbxInitCommands       "dbxInitCommands"
#define XtCDBXInitCommands       "DBXInitCommands"
#define XtNdbxSettings           "dbxSettings"
#define XtCDBXSettings           "DBXSettings"
#define XtNxdbInitCommands       "xdbInitCommands"
#define XtCXDBInitCommands       "XDBInitCommands"
#define XtNxdbSettings           "xdbSettings"
#define XtCXDBSettings           "XDBSettings"
#define XtNvslPath               "vslPath"
#define XtCVSLPath               "VSLPath"
#define XtNvslLibrary            "vslLibrary"
#define XtCVSLLibrary            "VSLLibrary"
#define XtNvslDefs               "vslDefs"
#define XtCVSLDefs               "VSLDefs"
#define XtNtabWidth              "tabWidth"
#define XtCTabWidth              "TabWidth"
#define XtNlinesAboveCursor      "linesAboveCursor"
#define XtCLinesAboveCursor      "LinesAboveCursor"
#define XtNlinesBelowCursor      "linesBelowCursor"
#define XtCLinesBelowCursor      "LinesBelowCursor"
#define XtNindentSource          "indentSource"
#define XtNindentCode            "indentCode"
#define XtCIndent                "Indent"
#define XtNlineNumberWidth       "lineNumberWidth"
#define XtCLineNumberWidth       "LineNumberWidth"
#define XtNconsoleButtons        "consoleButtons"
#define XtCConsoleButtons        "ConsoleButtons"
#define XtNsourceButtons         "sourceButtons"
#define XtCSourceButtons         "SourceButtons"
#define XtNdataButtons           "dataButtons"
#define XtCDataButtons           "DataButtons"
#define XtNtoolButtons           "toolButtons"
#define XtCToolButtons           "ToolButtons"
#define XtNtoolBar               "toolBar"
#define XtCToolBar               "ToolBar"
#define XtNseparateDataWindow    "separateDataWindow"
#define XtNseparateSourceWindow  "separateSourceWindow"
#define XtNseparateExecWindow    "separateExecWindow"
#define XtCSeparate              "Separate"
#define XtNdataWindow            "dataWindow"
#define XtNsourceWindow          "sourceWindow"
#define XtNdebuggerConsole       "debuggerConsole"
#ifndef XtCWindow
#define XtCWindow                "Window"
#endif
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
#define XtNdebuggerRHost         "debuggerRHost"
#define XtCDebuggerRHost         "DebuggerRHost"
#define XtNdebuggerHostLogin     "debuggerHostLogin"
#define XtCDebuggerHostLogin     "DebuggerHostLogin"
#define XtNrHostInitCommands     "rHostInitCommands"
#define XtCRHostInitCommands     "RHostInitCommands"
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
#define XtNuncompressCommand     "uncompressCommand"
#define XtCUncompressCommand     "UncompressCommand"
#define XtNpsCommand             "psCommand"
#define XtCPsCommand             "PsCommand"
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
#define XtNshowManual            "showManual"
#define XtCShowManual            "ShowManual"
#define XtNshowLicense           "showLicense"
#define XtCShowLicense           "ShowLicense"
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
#define XtNdisplayLineNumbers    "displayLineNumbers"
#define XtCDisplayLineNumbers    "DisplayLineNumbers"
#define XtNdisassemble           "disassemble"
#define XtCDisassemble           "Disassemble"
#define XtNuseSourcePath         "useSourcePath"
#define XtCUseSourcePath         "UseSourcePath"
#define XtNallRegisters          "allRegisters"
#define XtCAllRegisters          "AllRegisters"
#define XtNbuttonTips            "buttonTips"
#define XtNvalueTips             "valueTips"
#define XtCTips                  "Tips"
#define XtNbuttonDocs            "buttonDocs"
#define XtNvalueDocs             "valueDocs"
#define XtCDocs                  "Docs"
#define XtNstatusAtBottom        "statusAtBottom"
#define XtCStatusAtBottom        "StatusAtBottom"
#define XtNmaxDisplayTitleLength "maxDisplayTitleLength"
#define XtNmaxPopupExprLength    "maxPopupExprLength"
#define XtNmaxValueTipLength     "maxValueTipLength"
#define XtNmaxValueDocLength     "maxValueDocLength"
#define XtCMaxLength             "MaxLength"
#define XtNbuttonTipDelay        "buttonTipDelay"
#define XtNvalueTipDelay         "valueTipDelay"
#define XtCTipDelay              "TipDelay"
#define XtNbuttonDocDelay        "buttonDocDelay"
#define XtNvalueDocDelay         "valueDocDelay"
#define XtCDocDelay              "DocDelay"
#define XtNclearDocDelay         "clearDocDelay"
#define XtNclearTipDelay         "clearTipDelay"
#define XtCClearDelay            "ClearDelay"
#define XtNblinkWhileBusy        "blinkWhileBusy"
#define XtCBlinkWhileBusy        "BlinkWhileBusy"
#define XtNbusyBlinkRate         "busyBlinkRate"
#define XtNungrabMousePointer    "ungrabMousePointer"
#define XtCUngrabMousePointer    "UngrabMousePointer"
#define XtNdetectAliases         "detectAliases"
#define XtCDetectAliases         "DetectAliases"
#define XtNstatusHistorySize     "statusHistorySize"
#define XtCStatusHistorySize     "StatusHistorySize"
#define XtNverifyButtons         "verifyButtons"
#define XtCVerifyButtons         "VerifyButtons"
#define XtNstickyTool            "stickyTool"
#define XtCStickyTool            "StickyTool"
#define XtNtoolRightOffset       "toolRightOffset"
#define XtNtoolTopOffset         "toolTopOffset"
#define XtCOffset                "Offset"
#define XtNautoCommands          "autoCommands"
#define XtCAutoCommands          "AutoCommands"
#define XtNautoCommandPrefix     "autoCommandPrefix"
#define XtCAutoCommandPrefix     "AutoCommandPrefix"
#define XtNlessTifHacks          "lessTifHacks"
#define XtCLessTifHacks          "LessTifHacks"
#define XtNdddinitVersion        "dddinitVersion"
#define XtNappDefaultsVersion    "appDefaultsVersion"
#define XtCVersion               "Version"

struct AppData {
    String  debugger;
    String  debugger_command;
    Boolean trace_dialog;
    Boolean trace_shell_commands;
    String  gdb_initial_cmds;
    String  gdb_settings;
    String  dbx_initial_cmds;
    String  dbx_settings;
    String  xdb_initial_cmds;
    String  xdb_settings;
    String  vsl_path;
    String  vsl_library;
    String  vsl_defs;
    int     tab_width;
    int     lines_above_cursor;
    int     lines_below_cursor;
    int     indent_source;
    int     indent_code;
    int     line_number_width;
    String  console_buttons;
    String  source_buttons;
    String  data_buttons;
    String  tool_buttons;
    Boolean tool_bar;
    Boolean separate_data_window;
    Boolean separate_source_window;
    Boolean separate_exec_window;
    Boolean data_window;
    Boolean source_window;
    Boolean debugger_console;
    String  term_command;
    String  term_type;
    Boolean use_tty_command;
    String  term_start_process;
    int     question_timeout;
    Boolean synchronous_gdb;
    String  debugger_host;
    String  debugger_rhost;
    String  debugger_host_login;
    String  rhost_init_commands;
    String  rsh_command;
    String  list_exec_command;
    String  list_source_command;
    String  list_core_command;
    String  list_dir_command;
    String  uncompress_command;
    String  ps_command;
    String  www_page;
    String  www_command;
    Boolean show_invocation;
    Boolean show_version;
    Boolean show_configuration;
    Boolean show_manual;
    Boolean show_license;
    Boolean check_configuration;
    String  print_command;
    String  paper_size;
    String  edit_command;
    Boolean panned_graph_editor;
    Boolean find_words_only;
    Boolean group_iconify;
    Boolean global_tab_completion;
    Boolean save_history_on_exit;
    Boolean cache_source_files;
    Boolean cache_machine_code;
    Boolean suppress_warnings;
    Boolean tty_mode;
    Boolean full_name_mode;
    Boolean display_glyphs;
    Boolean display_line_numbers;
    Boolean disassemble;
    Boolean use_source_path;
    Boolean all_registers;
    Boolean button_tips;
    Boolean value_tips;
    Boolean button_docs;
    Boolean value_docs;
    Boolean status_at_bottom;
    int     max_display_title_length;
    int     max_popup_expr_length;
    int     max_value_tip_length;
    int     max_value_doc_length;
    int     button_tip_delay;
    int     value_tip_delay;
    int     button_doc_delay;
    int     value_doc_delay;
    int     clear_doc_delay;
    int     clear_tip_delay;
    Boolean blink_while_busy;
    int     busy_blink_rate;
    Boolean ungrab_mouse_pointer;
    Boolean detect_aliases;
    int     status_history_size;
    Boolean verify_buttons;
    Boolean sticky_tool;
    int     tool_right_offset;
    int     tool_top_offset;
    Boolean auto_commands;
    String  auto_command_prefix;
    Boolean lesstif_hacks;
    String  dddinit_version;
    String  app_defaults_version;
};

extern AppData app_data;

#endif // _DDD_AppData_h
// DON'T ADD ANYTHING BEHIND THIS #endif
