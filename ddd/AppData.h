// $Id$ -*- C++ -*-
// DDD application data

// Copyright (C) 1996-1998 Technische Universitaet Braunschweig, Germany.
// Copyright (C) 2000 Universitaet Passau, Germany.
// Written by Andreas Zeller <zeller@gnu.org>.
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
// `http://www.gnu.org/software/ddd/',
// or send a mail to the DDD developers <ddd@gnu.org>.

#ifndef _DDD_AppData_h
#define _DDD_AppData_h

#include <Xm/Xm.h>
#include <X11/StringDefs.h>
#include <X11/Xlib.h>

#include "version.h"
#include "OnOff.h"
#include "BindingS.h"

// The DDD application class name
#define DDD_CLASS_NAME           Ddd_NAME

// For a documentation of these resources, see the `Ddd' app-resource
// file or its original source, `Ddd.in.m4'.

#define XtNsession               "session"
#define XtNinitialSession        "initialSession"
#ifndef XtCSessionID
#define XtCSessionID             "SessionID"
#endif
#define XtNdebugger              "debugger"
#define XtCDebugger              "Debugger"
#define XtNdebuggerCommand       "debuggerCommand"
#define XtCDebuggerCommand       "DebuggerCommand"
#define XtNautoDebugger          "autoDebugger"
#define XtCAutoDebugger          "AutoDebugger"
#define XtNopenSelection         "openSelection"
#define XtCOpenSelection         "OpenSelection"
#define XtNtrace                 "trace"
#define XtCTrace                 "Trace"
#define XtNplayLog               "playLog"
#define XtCPlayLog               "PlayLog"
#define XtNgdbInitCommands       "gdbInitCommands"
#define XtNgdbSettings           "gdbSettings"
#define XtNdbxInitCommands       "dbxInitCommands"
#define XtNdbxSettings           "dbxSettings"
#define XtNxdbInitCommands       "xdbInitCommands"
#define XtNxdbSettings           "xdbSettings"
#define XtNjdbInitCommands       "jdbInitCommands"
#define XtNjdbSettings           "jdbSettings"
#define XtNpydbInitCommands      "pydbInitCommands"
#define XtNpydbSettings          "pydbSettings"
#define XtNperlInitCommands      "perlInitCommands"
#define XtNperlSettings          "perlSettings"
#define XtNbashInitCommands      "bashInitCommands"
#define XtNbashSettings          "bashSettings"
#define XtNrestartCommands       "restartCommands"
#define XtCInitCommands          "InitCommands"
#define XtNsourceInitCommands    "sourceInitCommands"
#define XtCSourceInitCommands    "SourceInitCommands"
#define XtCSettings              "Settings"
#define XtNinitSymbols           "initSymbols"
#define XtCInitSymbols           "InitSymbols"
#define XtNvslPath               "vslPath"
#define XtCVSLPath               "VSLPath"
#define XtNvslLibrary            "vslLibrary"
#define XtCVSLLibrary            "VSLLibrary"
#define XtNvslDefs               "vslDefs"
#define XtNvslBaseDefs           "vslBaseDefs"
#define XtCVSLDefs               "VSLDefs"
#define XtNdefaultFont           "defaultFont"
#define XtNvariableWidthFont     "variableWidthFont"
#define XtNfixedWidthFont        "fixedWidthFont"
#define XtNdataFont              "dataFont"
#ifndef XtCFont
#define XtCFont                  "Font"
#endif
#define XtNdefaultFontSize       "defaultFontSize"
#define XtNvariableWidthFontSize "variableWidthFontSize"
#define XtNfixedWidthFontSize    "fixedWidthFontSize"
#define XtNdataFontSize          "dataFontSize"
#define XtCFontSize              "FontSize"
#define XtNfontSelectCommand     "fontSelectCommand"
#define XtCFontSelectCommand     "FontSelectCommand"
#define XtNtabWidth              "tabWidth"
#define XtCTabWidth              "TabWidth"
#define XtNlinesAboveCursor      "linesAboveCursor"
#define XtCLinesAboveCursor      "LinesAboveCursor"
#define XtNlinesBelowCursor      "linesBelowCursor"
#define XtCLinesBelowCursor      "LinesBelowCursor"
#define XtNindentScript          "indentScript"
#define XtNindentSource          "indentSource"
#define XtNindentCode            "indentCode"
#define XtCIndent                "Indent"
#define XtNlineNumberWidth       "lineNumberWidth"
#define XtCLineNumberWidth       "LineNumberWidth"
#define XtNsourceEditing         "sourceEditing"
#define XtCSourceEditing         "SourceEditing"
#define XtNlineBufferedConsole   "lineBufferredConsole"
#define XtCLineBuffered          "LineBuffered"
#define XtNconsoleHasFocus       "consoleHasFocus"
#define XtCConsoleHasFocus       "ConsoleHasFocus"
#define XtNgdbDisplayShortcuts   "gdbDisplayShortcuts"
#define XtNdbxDisplayShortcuts   "dbxDisplayShortcuts"
#define XtNxdbDisplayShortcuts   "xdbDisplayShortcuts"
#define XtNjdbDisplayShortcuts   "jdbDisplayShortcuts"
#define XtNpydbDisplayShortcuts  "pydbDisplayShortcuts"
#define XtNperlDisplayShortcuts  "perlDisplayShortcuts"
#define XtNbashDisplayShortcuts  "bashDisplayShortcuts"
#define XtCDisplayShortcuts      "DisplayShortcuts"
#define XtNconsoleButtons        "consoleButtons"
#define XtNsourceButtons         "sourceButtons"
#define XtNdataButtons           "dataButtons"
#define XtNtoolButtons           "toolButtons"
#define XtCButtons               "Buttons"
#define XtNlabelDelimiter        "labelDelimiter"
#define XtCLabelDelimiter        "LabelDelimiter"
#define XtNcommandToolBar        "commandToolBar"
#define XtNcommonToolBar         "commonToolBar"
#define XtCToolBar               "ToolBar"
#define XtNseparateDataWindow    "separateDataWindow"
#define XtNseparateSourceWindow  "separateSourceWindow"
#define XtNseparateExecWindow    "separateExecWindow"
#define XtCSeparate              "Separate"
#define XtNopenDataWindow        "openDataWindow"
#define XtNopenSourceWindow      "openSourceWindow"
#define XtNopenDebuggerConsole   "openDebuggerConsole"
#ifndef XtCWindow
#define XtCWindow                "Window"
#endif
#define XtNautoCloseDataWindow   "autoCloseDataWindow"
#define XtCAutoClose             "AutoClose"
#define XtNplotCommand           "plotCommand"
#define XtCPlotCommand           "PlotCommand"
#define XtNplotTermType          "plotTermType"
#define XtCPlotTermType          "PlotTermType"
#define XtNplotInitCommands      "plotInitCommands"
#define XtCPlotInitCommands      "PlotInitCommands"
#define XtNplot2dSettings        "plot2dSettings"
#define XtNplot3dSettings        "plot3dSettings"
#define XtCPlotSettings          "PlotSettings"
#define XtNplotWindowClass       "plotWindowClass"
#define XtCPlotWindowClass       "PlotWindowClass"
#define XtNplotWindowDelay       "plotWindowDelay"
#define XtCWindowDelay           "WindowDelay"
#define XtNtermCommand           "termCommand"
#define XtCTermCommand           "TermCommand"
#define XtNtermType              "termType"
#define XtCTermType              "TermType"
#define XtNuseTTYCommand         "useTTYCommand"
#define XtCUseTTYCommand         "UseTTYCommand"
#define XtNquestionTimeout       "questionTimeout"
#define XtCQuestionTimeout       "QuestionTimeout"
#define XtNpositionTimeout       "positionTimeout"
#define XtCPositionTimeout       "PositionTimeout"
#define XtNdisplayTimeout        "displayTimeout"
#define XtCDisplayTimeout        "DisplayTimeout"
#define XtNsynchronousDebugger   "synchronousDebugger"
#define XtCSynchronousDebugger   "SynchronousDebugger"
#define XtNterminateOnEOF        "terminateOnEOF"
#define XtCTerminateOnEOF        "TerminateOnEOF"
#define XtNstopAndContinue       "stopAndContinue"
#define XtCStopAndContinue       "StopAndContinue"
#define XtNrunInterruptDelay     "runInterruptDelay"
#define XtNcontInterruptDelay    "contInterruptDelay"
#define XtCInterruptDelay        "InterruptDelay"
#define XtNpollChildStatus       "pollChildStatus"
#define XtCPollChildStatus       "PollChildStatus"
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
#define XtNfilterFiles           "filterFiles"
#define XtCFilterFiles           "FilterFiles"
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
#define XtNshowFonts             "showFonts"
#define XtCShowFonts             "ShowFonts"
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
#define XtNtransientDialogs      "transientDialogs"
#define XtCTransientDialogs      "TransientDialogs"
#define XtNglobalTabCompletion   "globalTabCompletion"
#define XtCGlobalTabCompletion   "GlobalTabCompletion"
#define XtNsaveHistoryOnExit     "saveHistoryOnExit"
#define XtNsaveOptionsOnExit     "saveOptionsOnExit"
#define XtCSaveOnExit            "SaveOnExit"
#define XtNcacheSourceFiles      "cacheSourceFiles"
#define XtCCacheSourceFiles      "CacheSourceFiles"
#define XtNcacheMachineCode      "cacheMachineCode"
#define XtCCacheMachineCode      "CacheMachineCode"
#define XtNsuppressWarnings      "suppressWarnings"
#define XtCSuppressWarnings      "SuppressWarnings"
#define XtNwarnIfLocked          "warnIfLocked"
#define XtCWarnIfLocked          "WarnIfLocked"
#define XtNcheckOptions          "checkOptions"
#define XtCCheckOptions          "CheckOptions"
#define XtNttyMode               "ttyMode"
#define XtCTTYMode               "TTYMode"
#define XtNannotate              "annotate"
#define XtCAnnotate              "Annotate"
#define XtNblockTTYInput         "blockTTYInput"
#define XtCBlockTTYInput         "BlockTTYInput"
#define XtNbufferGDBOutput       "bufferGDBOutput"
#define XtCBufferGDBOutput       "BufferGDBOutput"
#define XtNdisplayGlyphs         "displayGlyphs"
#define XtCDisplayGlyphs         "DisplayGlyphs"
#define XtNmaxGlyphs             "maxGlyphs"
#define XtCMaxGlyphs             "MaxGlyphs"
#define XtNcacheGlyphImages      "cacheGlyphImages"
#define XtCCacheGlyphImages      "CacheGlyphImages"
#define XtNglyphUpdateDelay      "glyphUpdateDelay"
#define XtCGlyphUpdateDelay      "GlyphUpdateDelay"
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
#define XtNtoolbarsAtBottom      "toolbarsAtBottom"
#define XtCToolbarsAtBottom      "ToolbarsAtBottom"
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
#define XtNcheckGrabs            "checkGrabs"
#define XtCCheckGrabs            "CheckGrabs"
#define XtNcheckGrabDelay        "checkGrabDelay"
#define XtCCheckGrabDelay        "CheckGrabDelay"
#define XtNgrabActionDelay       "grabActionDelay"
#define XtCGrabActionDelay       "GrabActionDelay"
#define XtNgrabAction            "grabAction"
#define XtCGrabAction            "GrabAction"
#define XtNdetectAliases         "detectAliases"
#define XtCDetectAliases         "DetectAliases"
#define XtNtypedAliases          "typedAliases"
#define XtCTypedAliases          "TypedAliases"
#define XtNdeleteAliasDisplays   "deleteAliasDisplays"
#define XtCDeleteAliasDisplays   "DeleteAliasDisplays"
#define XtNalign2dArrays         "align2dArrays"
#define XtCAlign2dArrays         "Align2dArrays"
#define XtNarrayOrientation      "arrayOrientation"
#define XtNstructOrientation     "structOrientation"
#define XtNshowMemberNames       "showMemberNames"
#define XtCShowMemberNames       "ShowMemberNames"
#define XtNexpandRepeatedValues  "expandRepeatedValues"
#define XtCExpandRepeatedValues  "ExpandRepeatedValues"
#define XtNbumpDisplays          "bumpDisplays"
#define XtCBumpDisplays          "BumpDisplays"
#define XtNhideInactiveDisplays  "hideInactiveDisplays"
#define XtCHideInactiveDisplays  "HideInactiveDisplays"
#define XtNshowBaseDisplayTitles "showBaseDisplayTitles"
#define XtNshowDependentDisplayTitles "showDependentDisplayTitles"
#define XtCShowDisplayTitles     "ShowDisplayTitles"
#define XtNclusterDisplays       "clusterDisplays"
#define XtCClusterDisplays       "ClusterDisplays"
#define XtNdisplayPlacement      "displayPlacement"
#define XtNstatusHistorySize     "statusHistorySize"
#define XtCHistorySize           "HistorySize"
#define XtNpopdownHistorySize    "popsownHistorySize"
#define XtNsortPopdownHistory    "sortPopdownHistory"
#define XtCSortPopdownHistory    "SortPopdownHistory"
#define XtNverifyButtons         "verifyButtons"
#define XtCVerifyButtons         "VerifyButtons"
#define XtNautoRaiseTool         "autoRaiseTool"
#define XtCAutoRaiseTool         "AutoRaiseTool"
#define XtNdecorateTool          "decorateTool"
#define XtCDecorate              "Decorate"
#define XtNstickyTool            "stickyTool"
#define XtCStickyTool            "StickyTool"
#define XtNtoolRightOffset       "toolRightOffset"
#define XtNtoolTopOffset         "toolTopOffset"
#define XtCToolOffset            "ToolOffset"
#define XtNbuttonImages          "buttonImages"
#define XtCButtonImages          "ButtonImages"
#define XtNbuttonCaptions        "buttonCaptions"
#define XtCButtonCaptions        "ButtonCaptions"
#define XtNbuttonCaptionGeometry "buttonCaptionGeometry"
#define XtCButtonCaptionGeometry "ButtonCaptionGeometry"
#define XtNbuttonImageGeometry   "buttonImageGeometry"
#define XtCButtonImageGeometry   "ButtonImageGeometry"
#define XtNflatToolbarButtons    "flatToolbarButtons"
#define XtNflatDialogButtons     "flatDialogButtons"
#define XtCFlatButtons           "FlatButtons"
#define XtNbuttonColorKey        "buttonColorKey"
#define XtNactiveButtonColorKey  "activeButtonColorKey"
#define XtNsplashScreenColorKey  "splashScreenColorKey"
#define XtCColorKey              "ColorKey"
#define XtNautoCommands          "autoCommands"
#define XtCAutoCommands          "AutoCommands"
#define XtNautoCommandPrefix     "autoCommandPrefix"
#define XtCAutoCommandPrefix     "AutoCommandPrefix"
#define XtNmaxBreakpointNumber   "maxBreakpointNumber"
#define XtNmaxDisplayNumber      "maxDisplayNumber"
#define XtCMaxNumber             "MaxNumber"
#define XtNsplashScreen          "splashScreen"
#define XtCSplashScreen          "SplashScreen"
#define XtNcolorWMIcons          "colorWMIcons"
#define XtCColorWMIcons          "ColorWMIcons"
#define XtNcutCopyPasteBindings  "cutCopyPasteBindings"
#define XtNselectAllBindings     "selectAllBindings"
#define XtCBindingStyle          "BindingStyle"
#define XtNmaxUndoDepth          "maxUndoDepth"
#define XtCMaxUndoDepth          "MaxUndoDepth"
#define XtNmaxUndoSize           "maxUndoSize"
#define XtCMaxUndoSize           "MaxUndoSize"
#define XtNthemes                "themes"
#define XtCThemes                "Themes"
#define XtNsuppressTheme         "suppressTheme"
#define XtCTheme                 "Theme"
#define XtNdumpCore              "dumpCore"
#define XtCDumpCore              "DumpCore"
#define XtNdebugCoreDumps        "debugCoreDumps"
#define XtCDebugCoreDumps        "DebugCoreDumps"
#define XtNmaintenance       	 "maintenance"
#define XtCMaintenance       	 "Maintenance"
#define XtNlessTifVersion        "lessTifVersion"
#define XtCLessTifVersion        "LessTifVersion"
#define XtNdddinitVersion        "dddinitVersion"
#define XtNappDefaultsVersion    "appDefaultsVersion"
#define XtCVersion               "Version"

struct AppData {
    const _XtString    session;
    const _XtString    initial_session;
    const _XtString    debugger;
    const _XtString    debugger_command;
    Boolean   auto_debugger;
    Boolean   open_selection;
    Boolean   trace;
    const _XtString    play_log;
    const _XtString    gdb_init_commands;
    const _XtString    gdb_settings;
    const _XtString    dbx_init_commands;
    const _XtString    dbx_settings;
    const _XtString    xdb_init_commands;
    const _XtString    xdb_settings;
    const _XtString    jdb_init_commands;
    const _XtString    jdb_settings;
    const _XtString    pydb_init_commands;
    const _XtString    pydb_settings;
    const _XtString    perl_init_commands;
    const _XtString    bash_init_commands;
    const _XtString    bash_settings;
    const _XtString    perl_settings;
    const _XtString    restart_commands;
    Boolean   source_init_commands;
    const _XtString    init_symbols;
    const _XtString    vsl_path;
    const _XtString    vsl_library;
    const _XtString    vsl_defs;
    const _XtString    vsl_base_defs;
    const _XtString    default_font;
    const _XtString    variable_width_font;
    const _XtString    fixed_width_font;
    const _XtString    data_font;
    Cardinal  default_font_size;
    Cardinal  variable_width_font_size;
    Cardinal  fixed_width_font_size;
    Cardinal  data_font_size;
    const _XtString    font_select_command;
    Cardinal  tab_width;
    Cardinal  lines_above_cursor;
    Cardinal  lines_below_cursor;
    Cardinal  indent_source;
    Cardinal  indent_script;
    Cardinal  indent_code;
    Cardinal  line_number_width;
    Boolean   source_editing;
    Boolean   line_buffered_console;
    OnOff     console_has_focus;
    const _XtString    gdb_display_shortcuts;
    const _XtString    dbx_display_shortcuts;
    const _XtString    xdb_display_shortcuts;
    const _XtString    jdb_display_shortcuts;
    const _XtString    pydb_display_shortcuts;
    const _XtString    perl_display_shortcuts;
    const _XtString    bash_display_shortcuts;
    const _XtString    console_buttons;
    const _XtString    source_buttons;
    const _XtString    data_buttons;
    const _XtString    tool_buttons;
    const _XtString    label_delimiter;
    Boolean   command_toolbar;
    Boolean   common_toolbar;
    Boolean   separate_data_window;
    Boolean   separate_source_window;
    Boolean   separate_exec_window;
    Boolean   data_window;
    Boolean   source_window;
    Boolean   debugger_console;
    Boolean   auto_close_data_window;
    const _XtString    plot_command;
    const _XtString    plot_term_type;
    const _XtString    plot_window_class;
    Cardinal  plot_window_delay;
    const _XtString    plot_init_commands;
    const _XtString    plot_2d_settings;
    const _XtString    plot_3d_settings;
    const _XtString    term_command;
    const _XtString    term_type;
    Boolean   use_tty_command;
    int       question_timeout;
    int       position_timeout;
    int       display_timeout;
    Boolean   synchronous_gdb;
    Boolean   terminate_on_eof;
    Boolean   stop_and_continue;
    int       run_interrupt_delay;
    int       cont_interrupt_delay;
    Boolean   poll_child_status;
    const _XtString    debugger_host;
    const _XtString    debugger_rhost;
    const _XtString    debugger_host_login;
    const _XtString    rhost_init_commands;
    const _XtString    rsh_command;
    Boolean   filter_files;
    const _XtString    list_exec_command;
    const _XtString    list_source_command;
    const _XtString    list_core_command;
    const _XtString    list_dir_command;
    const _XtString    uncompress_command;
    const _XtString    ps_command;
    const _XtString    www_page;
    const _XtString    www_command;
    Boolean   show_invocation;
    Boolean   show_version;
    Boolean   show_configuration;
    Boolean   show_manual;
    Boolean   show_license;
    Boolean   show_news;
    Boolean   show_fonts;
    Boolean   check_configuration;
    const _XtString    print_command;
    const _XtString    paper_size;
    const _XtString    edit_command;
    const _XtString    get_core_command;
    Boolean   panned_graph_editor;
    Boolean   find_words_only;
    Boolean   find_case_sensitive;
    Boolean   group_iconify;
    Boolean   uniconify_when_ready;
    Boolean   transient_dialogs;
    Boolean   global_tab_completion;
    Boolean   save_history_on_exit;
    Boolean   save_options_on_exit;
    Boolean   cache_source_files;
    Boolean   cache_machine_code;
    Boolean   suppress_warnings;
    Boolean   warn_if_locked;
    Cardinal  check_options;
    Boolean   tty_mode;
    OnOff     block_tty_input;
    OnOff     buffer_gdb_output;
    Cardinal  annotate;
    Boolean   display_glyphs;
    Cardinal  max_glyphs;
    Boolean   cache_glyph_images;
    Cardinal  glyph_update_delay;
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
    Boolean   toolbars_at_bottom;
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
    Boolean   check_grabs;
    Cardinal  check_grab_delay;
    Cardinal  grab_action_delay;
    const _XtString    grab_action;
    Boolean   detect_aliases;
    Boolean   typed_aliases;
    Boolean   delete_alias_displays;
    Boolean   align_2d_arrays;
    unsigned char array_orientation;
    unsigned char struct_orientation;
    Boolean   show_member_names;
    Boolean   expand_repeated_values;
    Boolean   bump_displays;
    Boolean   hide_inactive_displays;
    Boolean   show_base_display_titles;
    Boolean   show_dependent_display_titles;
    Boolean   cluster_displays;
    unsigned char display_placement;
    Cardinal  status_history_size;
    Cardinal  popdown_history_size;
    Boolean   sort_popdown_history;
    Boolean   verify_buttons;
    Boolean   auto_raise_tool;
    OnOff     decorate_tool;
    Boolean   sticky_tool;
    Position  tool_right_offset;
    Position  tool_top_offset;
    Boolean   button_captions;
    Boolean   button_images;
    const _XtString    button_caption_geometry;
    const _XtString    button_image_geometry;
    Boolean   flat_toolbar_buttons;
    Boolean   flat_dialog_buttons;
    const _XtString    button_color_key;
    const _XtString    active_button_color_key;
    const _XtString    splash_screen_color_key;
    Boolean   auto_commands;
    const _XtString    auto_command_prefix;
    int       max_breakpoint_number;
    int       max_display_number;
    Boolean   splash_screen;
    Boolean   color_wm_icons;
    BindingStyle cut_copy_paste_bindings;
    BindingStyle select_all_bindings;
    int       max_undo_depth;
    int       max_undo_size;
    const _XtString    themes;
    const _XtString    suppress_theme;
    Boolean   maintenance;
    Boolean   dump_core;
    Boolean   debug_core_dumps;
    int       lesstif_version;
    const _XtString    dddinit_version;
    const _XtString    app_defaults_version;
};

extern AppData app_data;

#endif // _DDD_AppData_h
// DON'T ADD ANYTHING BEHIND THIS #endif
