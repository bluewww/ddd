// $Id$ -*- C++ -*-
// DDD application data

// Copyright (C) 1996-1998 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@ips.cs.tu-bs.de>.
// 
// This file is part of DDD.
// 
// DDD is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// DDD is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with DDD -- see the file COPYING.
// If not, write to the Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers <ddd@ips.cs.tu-bs.de>.

#ifndef _DDD_AppData_h
#define _DDD_AppData_h

#include <Xm/Xm.h>
#include <X11/StringDefs.h>
#include <X11/Xlib.h>

#include "version.h"
#include "OnOff.h"

// The DDD application class name
#define DDD_CLASS_NAME           Ddd_NAME

// For a documentation of these resources, see `Ddd.in'.
#define XtNsession               "session"
#define XtNinitialSession        "initialSession"
#ifndef XtCSessionID
#define XtCSessionID             "SessionID"
#endif
#define XtNdebugger              "debugger"
#define XtCDebugger              "Debugger"
#define XtNdebuggerCommand       "debuggerCommand"
#define XtCDebuggerCommand       "DebuggerCommand"
#define XtNtraceDialog           "traceDialog"
#define XtNtraceShellCommands    "traceShellCommands"
#define XtCTrace                 "Trace"
#define XtNgdbInitCommands       "gdbInitCommands"
#define XtNgdbSettings           "gdbSettings"
#define XtNdbxInitCommands       "dbxInitCommands"
#define XtNdbxSettings           "dbxSettings"
#define XtNxdbInitCommands       "xdbInitCommands"
#define XtNxdbSettings           "xdbSettings"
#define XtNjdbInitCommands       "jdbInitCommands"
#define XtNjdbSettings           "jdbSettings"
#define XtNrestartCommands       "restartCommands"
#define XtCInitCommands          "InitCommands"
#define XtCSettings              "Settings"
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
#define XtNlineBufferedConsole   "lineBufferredConsole"
#define XtCLineBuffered          "LineBuffered"
#define XtNdisplayShortcuts      "displayShortcuts"
#define XtCDisplayShortcuts      "DisplayShortcuts"
#define XtNconsoleButtons        "consoleButtons"
#define XtNsourceButtons         "sourceButtons"
#define XtNdataButtons           "dataButtons"
#define XtNtoolButtons           "toolButtons"
#define XtCButtons               "Buttons"
#define XtNlabelDelimiter        "labelDelimiter"
#define XtCLabelDelimiter        "LabelDelimiter"
#define XtNcommandToolBar        "commandToolBar"
#define XtNargToolBar            "argToolBar"
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
#define XtNshowNews              "showNews"
#define XtCShowNews              "ShowNews"
#define XtNcheckConfiguration    "checkConfiguration"
#define XtCCheckConfiguration    "CheckConfiguration"
#define XtNprintCommand          "printCommand"
#define XtCPrintCommand          "PrintCommand"
#define XtNpaperSize             "paperSize"
#define XtCPaperSize             "PaperSize"
#define XtNeditCommand           "editCommand"
#define XtCEditCommand           "EditCommand"
#define XtNgetCoreCommand        "getCoreCommand"
#define XtCGetCoreCommand        "GetCoreCommand"
#define XtNpannedGraphEditor     "pannedGraphEditor"
#define XtCPannedGraphEditor     "PannedGraphEditor"
#define XtNfindWordsOnly         "findWordsOnly"
#define XtCFindWordsOnly         "FindWordsOnly"
#define XtNfindCaseSensitive     "findCaseSensitive"
#define XtCFindCaseSensitive     "FindCaseSensitive"
#define XtNgroupIconify          "groupIconify"
#define XtCGroupIconify          "GroupIconify"
#define XtNuniconifyWhenReady    "uniconifyWhenReady"
#define XtCUniconifyWhenReady    "UniconifyWhenReady"
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
#define XtNblockTTYInput         "blockTTYInput"
#define XtCBlockTTYInput         "BlockTTYInput"
#define XtNdisplayGlyphs         "displayGlyphs"
#define XtCDisplayGlyphs         "DisplayGlyphs"
#define XtNmaxGlyphs             "maxGlyphs"
#define XtCMaxGlyphs             "MaxGlyphs"
#define XtNcacheGlyphImages      "cacheGlyphImages"
#define XtCCacheGlyphImages      "CacheGlyphImages"
#define XtNdisplayLineNumbers    "displayLineNumbers"
#define XtCDisplayLineNumbers    "DisplayLineNumbers"
#define XtNdisassemble           "disassemble"
#define XtCDisassemble           "Disassemble"
#define XtNmaxDisassemble        "maxDisassemble"
#define XtCMaxDisassemble        "MaxDisassemble"
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
#define XtNstartupTips           "startupTips"
#define XtCStartupTips           "StartupTips"
#define XtNstartupTipCount       "startupTipCount"
#define XtCStartupTipCount       "StartupTipCount"
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
#define XtNalign2dArrays         "align2dArrays"
#define XtCAlign2dArrays         "Align2dArrays"
#define XtNbumpDisplays          "bumpDisplays"
#define XtCBumpDisplays          "BumpDisplays"
#define XtNhideInactiveDisplays  "hideInactiveDisplays"
#define XtCHideInactiveDisplays  "HideInactiveDisplays"
#define XtNstatusHistorySize     "statusHistorySize"
#define XtCStatusHistorySize     "StatusHistorySize"
#define XtNverifyButtons         "verifyButtons"
#define XtCVerifyButtons         "VerifyButtons"
#define XtNautoRaiseTool         "autoRaiseTool"
#define XtCAutoRaiseTool         "AutoRaiseTool"
#define XtNdecorateTool          "decorateTool"
#define XtCDecorateTool          "DecorateTool"
#define XtNstickyTool            "stickyTool"
#define XtCStickyTool            "StickyTool"
#define XtNtoolRightOffset       "toolRightOffset"
#define XtNtoolTopOffset         "toolTopOffset"
#define XtCToolOffset            "ToolOffset"
#define XtNbuttonCaptions        "buttonCaptions"
#define XtCCaptions              "Captions"
#define XtNbuttonCaptionHeight   "buttonCaptionHeight"
#define XtCCaptionHeight         "CaptionHeight"
#define XtNautoCommands          "autoCommands"
#define XtCAutoCommands          "AutoCommands"
#define XtNautoCommandPrefix     "autoCommandPrefix"
#define XtCAutoCommandPrefix     "AutoCommandPrefix"
#define XtNmaxBreakpointNumber   "maxBreakpointNumber"
#define XtNmaxDisplayNumber      "maxDisplayNumber"
#define XtCMaxNumber             "MaxNumber"
#define XtNshowStartupLogo       "showStartupLogo"
#define XtCShowStartupLogo       "ShowStartupLogo"
#define XtNcolorWMIcons          "colorWMIcons"
#define XtCColorWMIcons          "ColorWMIcons"
#define XtNlessTifVersion        "lessTifVersion"
#define XtCLessTifVersion        "LessTifVersion"
#define XtNdddinitVersion        "dddinitVersion"
#define XtNappDefaultsVersion    "appDefaultsVersion"
#define XtCVersion               "Version"

struct AppData {
    String    session;
    String    initial_session;
    String    debugger;
    String    debugger_command;
    Boolean   trace_dialog;
    Boolean   trace_shell_commands;
    String    gdb_init_commands;
    String    gdb_settings;
    String    dbx_init_commands;
    String    dbx_settings;
    String    xdb_init_commands;
    String    xdb_settings;
    String    jdb_init_commands;
    String    jdb_settings;
    String    restart_commands;
    String    vsl_path;
    String    vsl_library;
    String    vsl_defs;
    Cardinal  tab_width;
    Cardinal  lines_above_cursor;
    Cardinal  lines_below_cursor;
    Cardinal  indent_source;
    Cardinal  indent_code;
    Cardinal  line_number_width;
    Boolean   line_buffered_console;
    String    display_shortcuts;
    String    console_buttons;
    String    source_buttons;
    String    data_buttons;
    String    tool_buttons;
    String    label_delimiter;
    Boolean   command_tool_bar;
    Boolean   arg_tool_bar;
    Boolean   separate_data_window;
    Boolean   separate_source_window;
    Boolean   separate_exec_window;
    Boolean   data_window;
    Boolean   source_window;
    Boolean   debugger_console;
    String    term_command;
    String    term_type;
    Boolean   use_tty_command;
    String    term_start_process;
    int       question_timeout;
    Boolean   synchronous_gdb;
    String    debugger_host;
    String    debugger_rhost;
    String    debugger_host_login;
    String    rhost_init_commands;
    String    rsh_command;
    String    list_exec_command;
    String    list_source_command;
    String    list_core_command;
    String    list_dir_command;
    String    uncompress_command;
    String    ps_command;
    String    www_page;
    String    www_command;
    Boolean   show_invocation;
    Boolean   show_version;
    Boolean   show_configuration;
    Boolean   show_manual;
    Boolean   show_license;
    Boolean   show_news;
    Boolean   check_configuration;
    String    print_command;
    String    paper_size;
    String    edit_command;
    String    get_core_command;
    Boolean   panned_graph_editor;
    Boolean   find_words_only;
    Boolean   find_case_sensitive;
    Boolean   group_iconify;
    Boolean   uniconify_when_ready;
    Boolean   global_tab_completion;
    Boolean   save_history_on_exit;
    Boolean   cache_source_files;
    Boolean   cache_machine_code;
    Boolean   suppress_warnings;
    Boolean   tty_mode;
    OnOff     block_tty_input;
    Boolean   full_name_mode;
    Boolean   display_glyphs;
    Cardinal  max_glyphs;
    Boolean   cache_glyph_images;
    Boolean   display_line_numbers;
    Boolean   disassemble;
    Cardinal  max_disassemble;
    Boolean   use_source_path;
    Boolean   all_registers;
    Boolean   button_tips;
    Boolean   value_tips;
    Boolean   button_docs;
    Boolean   value_docs;
    Boolean   startup_tips;
    int       startup_tip_count;
    Boolean   status_at_bottom;
    Cardinal  max_display_title_length;
    Cardinal  max_popup_expr_length;
    Cardinal  max_value_tip_length;
    Cardinal  max_value_doc_length;
    Cardinal  button_tip_delay;
    Cardinal  value_tip_delay;
    Cardinal  button_doc_delay;
    Cardinal  value_doc_delay;
    Cardinal  clear_doc_delay;
    Cardinal  clear_tip_delay;
    Boolean   blink_while_busy;
    Cardinal  busy_blink_rate;
    Boolean   ungrab_mouse_pointer;
    Boolean   detect_aliases;
    Boolean   align_2d_arrays;
    Boolean   bump_displays;
    Boolean   hide_inactive_displays;
    Cardinal  status_history_size;
    Boolean   verify_buttons;
    Boolean   auto_raise_tool;
    OnOff     decorate_tool;
    Boolean   sticky_tool;
    Position  tool_right_offset;
    Position  tool_top_offset;
    Boolean   button_captions;
    Dimension button_caption_height;
    Boolean   auto_commands;
    String    auto_command_prefix;
    int       max_breakpoint_number;
    int       max_display_number;
    String    show_startup_logo;
    Boolean   color_wm_icons;
    int       lesstif_version;
    String    dddinit_version;
    String    app_defaults_version;
};

extern AppData app_data;

#endif // _DDD_AppData_h
// DON'T ADD ANYTHING BEHIND THIS #endif
