// $Id$ -*- C++ -*-
// DDD application resources

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

char resources_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "resources.h"
#include "version.h"
#include "AppData.h"
#include "stty.h"

#include <Xm/Xm.h>

// Application resource definitions
XtResource ddd_resources[] = {
    {
	XtNsession,
	XtCSessionID,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, session),
	XtRString, 
	XtPointer(NULL)
    },

    {
	XtNinitialSession,
	XtCSessionID,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, initial_session),
	XtRString, 
	XtPointer(NULL)
    },

    { 
	XtNdebugger,
	XtCDebugger,
	XtRString, 
	sizeof(String),
	XtOffsetOf(AppData, debugger), 
	XtRString, 
	XtPointer("")
    },

    { 
	XtNdebuggerCommand,
	XtCDebuggerCommand,
	XtRString, 
	sizeof(String),
	XtOffsetOf(AppData, debugger_command), 
	XtRString, 
	XtPointer("")
    },

    { 
	XtNtrace, 
	XtCTrace, 
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, trace),
	XmRImmediate, 
	XtPointer(False)
    },

    { 
	XtNgdbInitCommands, 
	XtCInitCommands, 
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, gdb_init_commands), 
	XtRString,
	XtPointer(
	    "set height 0\n"
	    "set width 0\n"
	    " set verbose off\n"
	    "set prompt (gdb) \n")
    },

    { 
	XtNgdbSettings, 
	XtCSettings, 
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, gdb_settings), 
	XtRString,
	XtPointer("")
    },

    { 
	XtNdbxInitCommands, 
	XtCInitCommands, 
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, dbx_init_commands), 
	XtRString,
	XtPointer("")
    },

    { 
	XtNdbxSettings, 
	XtCSettings, 
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, dbx_settings), 
	XtRString,
	XtPointer("")
    },

    { 
	XtNxdbInitCommands, 
	XtCInitCommands, 
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, xdb_init_commands), 
	XtRString,
	XtPointer("")
    },

    { 
	XtNxdbSettings, 
	XtCSettings, 
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, xdb_settings), 
	XtRString,
	XtPointer("")
    },

    { 
	XtNjdbInitCommands, 
	XtCInitCommands, 
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, jdb_init_commands), 
	XtRString,
	XtPointer("")
    },

    { 
	XtNjdbSettings, 
	XtCSettings, 
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, jdb_settings), 
	XtRString,
	XtPointer("")
    },

    {
	XtNrestartCommands,
	XtCInitCommands,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, restart_commands), 
	XtRString,
	XtPointer("")
    },

    { 
	XtNvslPath, 
	XtCVSLPath, 
	XtRString, 
	sizeof(String),
	XtOffsetOf(AppData, vsl_path),
	XtRString, 
	XtPointer(".")
    },

    { 
	XtNvslLibrary, 
	XtCVSLLibrary, 
	XtRString, 
	sizeof(String),
	XtOffsetOf(AppData, vsl_library), 
	XtRString, 
	XtPointer("builtin")
    },

    { 
	XtNdefaultFont, 
	XtCFont, 
	XtRString, 
	sizeof(String),
	XtOffsetOf(AppData, default_font),
	XtRString, 
	XtPointer("-*-helvetica-bold-r-*")
    },

    { 
	XtNvariableWidthFont, 
	XtCFont,
	XtRString, 
	sizeof(String),
	XtOffsetOf(AppData, variable_width_font),
	XtRString,
	XtPointer("-*-helvetica-medium-r-*")
    },

    { 
	XtNfixedWidthFont, 
	XtCFont,
	XtRString, 
	sizeof(String),
	XtOffsetOf(AppData, fixed_width_font),
	XtRString,
	XtPointer("-*-courier-medium-r-*")
    },

    {
	XtNdefaultFontSize,
	XtCFontSize,
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, default_font_size),
	XmRImmediate,
	XtPointer(120)
    },

    {
	XtNvariableWidthFontSize,
	XtCFontSize,
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, variable_width_font_size),
	XmRImmediate,
	XtPointer(120)
    },

    {
	XtNfixedWidthFontSize,
	XtCFontSize,
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, fixed_width_font_size),
	XmRImmediate,
	XtPointer(120)
    },

    { 
	XtNfontSelectCommand, 
	XtCFontSelectCommand,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, font_select_command),
	XtRString,
	XtPointer("xfontsel -print")
    },

    { 
	XtNvslDefs, 
	XtCVSLDefs, 
	XtRString, 
	sizeof(String),
	XtOffsetOf(AppData, vsl_defs),
	XtRString, 
	XtPointer("")
    },

    { 
	XtNvslBaseDefs, 
	XtCVSLDefs, 
	XtRString, 
	sizeof(String),
	XtOffsetOf(AppData, vsl_base_defs),
	XtRString, 
	XtPointer("")
    },

    {
	XtNtabWidth,
	XtCTabWidth,
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, tab_width),
	XmRImmediate,
	XtPointer(8)
    },

    {
	XtNlinesAboveCursor,
	XtCLinesAboveCursor,
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, lines_above_cursor),
	XmRImmediate,
	XtPointer(2)
    },

    {
	XtNlinesBelowCursor,
	XtCLinesBelowCursor,
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, lines_below_cursor),
	XmRImmediate,
	XtPointer(3)
    },

    {
	XtNindentSource,
	XtCIndent,
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, indent_source),
	XmRImmediate,
	XtPointer(4)
    },

    {
	XtNindentCode,
	XtCIndent,
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, indent_code),
	XmRImmediate,
	XtPointer(4)
    },

    {
	XtNlineNumberWidth,
	XtCLineNumberWidth,
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, line_number_width),
	XmRImmediate,
	XtPointer(4)
    },

    {
	XtNsourceEditing,
	XtCSourceEditing,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, source_editing),
	XtRImmediate,
	XtPointer(False)
    },

    {
	XtNlineBufferedConsole,
	XtCLineBuffered,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, line_buffered_console),
	XtRImmediate,
	XtPointer(True)
    },

    {
	XtNdisplayShortcuts,
	XtCDisplayShortcuts,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, display_shortcuts),
	XtRString,
	XtPointer("")
    },

    {
	XtNconsoleButtons,
	XtCButtons,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, console_buttons),
	XtRString,
	XtPointer("")
    },

    {
	XtNsourceButtons,
	XtCButtons,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, source_buttons),
	XtRString,
	XtPointer("")
    },

    {
	XtNdataButtons,
	XtCButtons,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, data_buttons),
	XtRString,
	XtPointer("")
    },

    {
	XtNtoolButtons,
	XtCButtons,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, tool_buttons),
	XtRString,
	XtPointer("run\n"
		  "interrupt^C\n"
		  "next\n"
		  "step\n"
		  "continue\n"
		  "finish\n"
		  "up\n"
		  "down")
    },

    {
	XtNlabelDelimiter,
	XtCLabelDelimiter,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, label_delimiter),
	XtRString,
	XtPointer("//")
    },

    {
	XtNcommandToolBar,
	XtCToolBar,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, command_toolbar),
	XtRImmediate,
	XtPointer(False)
    },

    {
	XtNcommonToolBar,
	XtCToolBar,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, common_toolbar),
	XtRImmediate,
	XtPointer(True)
    },

    {
	XtNseparateDataWindow,
	XtCSeparate,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, separate_data_window),
	XtRImmediate,
	XtPointer(False)
    },

    {
	XtNseparateSourceWindow,
	XtCSeparate,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, separate_source_window),
	XtRImmediate,
	XtPointer(False)
    },

    {
	XtNseparateExecWindow,
	XtCSeparate,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, separate_exec_window),
	XtRImmediate,
	XtPointer(False)
    },

    {
	XtNopenDataWindow,
	XtCWindow,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, data_window),
	XtRImmediate,
	XtPointer(True)
    },

    {
	XtNopenSourceWindow,
	XtCWindow,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, source_window),
	XtRImmediate,
	XtPointer(True)
    },

    {
	XtNopenDebuggerConsole,
	XtCWindow,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, debugger_console),
	XtRImmediate,
	XtPointer(True)
    },

    {
	XtNtermCommand,
	XtCTermCommand,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, term_command),
	XtRString,
	XtPointer("xterm -e /bin/sh -c")
    },

    {
	XtNtermType,
	XtCTermType,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, term_type),
	XtRString,
	XtPointer("xterm")
   },

   {
	XtNuseTTYCommand,
	XtCUseTTYCommand,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, use_tty_command),
	XtRImmediate,
	XtPointer(False)
    },

    {
	XtNquestionTimeout,
	XtCQuestionTimeout,
	XtRInt,
	sizeof(int),
	XtOffsetOf(AppData, question_timeout),
	XtRImmediate,
	XtPointer(5)
    },

    {
	XtNsynchronousDebugger,
	XtCSynchronousDebugger,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, synchronous_gdb),
	XtRImmediate,
	XtPointer(False)
    },

    {
	XtNterminateOnEOF,
	XtCTerminateOnEOF,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, terminate_on_eof),
	XtRImmediate,
	XtPointer(False)
    },

    {
        XtNdebuggerHost,
	XtCDebuggerHost,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, debugger_host),
	XtRString,
	XtPointer("")
    },

    {
        XtNdebuggerHostLogin,
	XtCDebuggerHostLogin,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, debugger_host_login),
	XtRString,
	XtPointer("")
    },

    {
        XtNrHostInitCommands,
	XtCRHostInitCommands,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, rhost_init_commands),
	XtRString,
	XtPointer(STTY_COMMAND)
    },

    {
        XtNdebuggerRHost,
	XtCDebuggerRHost,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, debugger_rhost),
	XtRString,
	XtPointer("")
    },

    {
        XtNrshCommand,
	XtCRshCommand,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, rsh_command),
	XtRString,
	XtPointer("rsh")
    },

    {
        XtNlistExecCommand,
	XtCListExecCommand,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, list_exec_command),
	XtRString,
	XtPointer("file @MASK@ | grep  '.*:.*exec.*' | cut -d: -f1")
    },

    {
        XtNlistSourceCommand,
	XtCListSourceCommand,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, list_source_command),
	XtRString,
	XtPointer("file @MASK@ | grep '.*:.*text.*' | cut -d: -f1")
    },

    {
        XtNlistCoreCommand,
	XtCListCoreCommand,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, list_core_command),
	XtRString,
	XtPointer("file @MASK@ | grep '.*:.*core.*' | cut -d: -f1")
    },

    {
        XtNlistDirCommand,
	XtCListDirCommand,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, list_dir_command),
	XtRString,
	XtPointer("file @MASK@ | grep '.*:.*directory.*' | cut -d: -f1")
    },

    {
        XtNuncompressCommand,
	XtCUncompressCommand,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, uncompress_command),
	XtRString,
	XtPointer("gzip -d -c")
    },

    {
        XtNpsCommand,
	XtCPsCommand,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, ps_command),
	XtRString,
	XtPointer("ps")
    },

    {
        XtNwwwPage,
	XtCWWWPage,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, www_page),
	XtRString,
	XtPointer("http://www.cs.tu-bs.de/softech/ddd/")
    },

    {
        XtNwwwCommand,
	XtCWWWCommand,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, www_command),
	XtRString,
	XtPointer("netscape @URL@")
    },

    {
        XtNshowInvocation,
	XtCShowInvocation,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, show_invocation),
	XtRImmediate,
	XtPointer(False)
    },

    {
        XtNshowVersion,
	XtCShowVersion,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, show_version),
	XtRImmediate,
	XtPointer(False)
    },

    {
        XtNshowConfiguration,
	XtCShowConfiguration,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, show_configuration),
	XtRImmediate,
	XtPointer(False)
    },

    {
        XtNshowManual,
	XtCShowManual,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, show_manual),
	XtRImmediate,
	XtPointer(False)
    },

    {
        XtNshowLicense,
	XtCShowLicense,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, show_license),
	XtRImmediate,
	XtPointer(False)
    },

    {
        XtNshowNews,
	XtCShowNews,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, show_news),
	XtRImmediate,
	XtPointer(False)
    },

    {
        XtNshowFonts,
	XtCShowFonts,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, show_fonts),
	XtRImmediate,
	XtPointer(False)
    },

    {
        XtNcheckConfiguration,
	XtCCheckConfiguration,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, check_configuration),
	XtRImmediate,
	XtPointer(False)
    },

    {
        XtNprintCommand,
	XtCPrintCommand,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, print_command),
	XtRString,
	XtPointer("lp")
    },

    {
        XtNpaperSize,
	XtCPaperSize,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, paper_size),
	XtRString,
	XtPointer("210mm x 297mm")
    },

    {
        XtNgetCoreCommand,
	XtCGetCoreCommand,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, get_core_command),
	XtRString,
	XtPointer("")
    },

    {
        XtNeditCommand,
	XtCEditCommand,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, edit_command),
	XtRString,
	XtPointer("xterm -e vi +@LINE@ @FILE@")
    },

    {
        XtNpannedGraphEditor,
	XtCPannedGraphEditor,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, panned_graph_editor),
	XtRImmediate,
	XtPointer(False)
    },

    {
        XtNfindWordsOnly,
	XtCFindWordsOnly,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, find_words_only),
	XtRImmediate,
	XtPointer(True)
    },

    {
        XtNfindCaseSensitive,
	XtCFindCaseSensitive,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, find_case_sensitive),
	XtRImmediate,
	XtPointer(True)
    },

    {
	XtNgroupIconify,
	XtCGroupIconify,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, group_iconify),
	XtRImmediate,
	XtPointer(False)
    },

    {
	XtNuniconifyWhenReady,
	XtCUniconifyWhenReady,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, uniconify_when_ready),
	XtRImmediate,
	XtPointer(True)
    },

    {
	XtNtransientDialogs,
	XtCTransientDialogs,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, transient_dialogs),
	XtRImmediate,
	XtPointer(True)
    },

    {
	XtNglobalTabCompletion,
	XtCGlobalTabCompletion,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, global_tab_completion),
	XtRImmediate,
	XtPointer(False)
    },

    {
	XtNsaveHistoryOnExit,
	XtCSaveHistoryOnExit,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, save_history_on_exit),
	XtRImmediate,
	XtPointer(True)
    },

    {
	XtNcacheSourceFiles,
	XtCCacheSourceFiles,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, cache_source_files),
	XtRImmediate,
	XtPointer(True)
    },

    {
	XtNcacheMachineCode,
	XtCCacheMachineCode,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, cache_machine_code),
	XtRImmediate,
	XtPointer(True)
    },

    {
	XtNsuppressWarnings,
	XtCSuppressWarnings,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, suppress_warnings),
	XtRImmediate,
	XtPointer(False)
    },

    {
	XtNttyMode,
	XtCTTYMode,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, tty_mode),
	XtRImmediate,
	XtPointer(False)
    },

    {
	XtNblockTTYInput,
	XtCBlockTTYInput,
	XtROnOff,
	sizeof(OnOff),
	XtOffsetOf(AppData, block_tty_input),
	XmRImmediate,
	XtPointer(Auto)
    },

    {
	XtNfullNameMode,
	XtCTTYMode,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, full_name_mode),
	XtRImmediate,
	XtPointer(False)
    },

    {
	XtNdisplayGlyphs,
	XtCDisplayGlyphs,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, display_glyphs),
	XtRImmediate,
	XtPointer(True)
    },

    {
	XtNmaxGlyphs,
	XtCMaxGlyphs,
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, max_glyphs),
	XtRImmediate,
	XtPointer(10)
    },

    {
	XtNcacheGlyphImages,
	XtCCacheGlyphImages,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, cache_glyph_images),
	XtRImmediate,
#if defined(__linux__) && XmVersion >= 2001
	// Motif 2.1 on XFree86 servers has trouble with cached images.
	XtPointer(False)
#else
	XtPointer(True)
#endif
    },

    {
	XtNglyphUpdateDelay,
	XtCGlyphUpdateDelay,
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, glyph_update_delay),
	XtRImmediate,
	XtPointer(10)
    },

    {
	XtNdisplayLineNumbers,
	XtCDisplayLineNumbers,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, display_line_numbers),
	XtRImmediate,
	XtPointer(False)
    },

    {
	XtNdisassemble,
	XtCDisassemble,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, disassemble),
	XtRImmediate,
	XtPointer(False)
    },

    {
	XtNmaxDisassemble,
	XtCMaxDisassemble,
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, max_disassemble),
	XmRImmediate,
	XtPointer(1024)
    },

    {
	XtNuseSourcePath,
	XtCUseSourcePath,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, use_source_path),
	XtRImmediate,
	XtPointer(False)
    },

    {
	XtNallRegisters,
	XtCAllRegisters,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, all_registers),
	XtRImmediate,
	XtPointer(False)
    },

    {
	XtNbuttonTips,
	XtCTips,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, button_tips),
	XtRImmediate,
	XtPointer(True)
    },

    {
	XtNvalueTips,
	XtCTips,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, value_tips),
	XtRImmediate,
	XtPointer(True)
    },

    {
	XtNbuttonDocs,
	XtCDocs,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, button_docs),
	XtRImmediate,
	XtPointer(True)
    },

    {
	XtNvalueDocs,
	XtCDocs,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, value_docs),
	XtRImmediate,
	XtPointer(True)
    },

    {
	XtNstartupTips,
	XtCStartupTips,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, startup_tips),
	XtRImmediate,
	XtPointer(True)
    },

    {
	XtNstartupTipCount,
	XtCStartupTipCount,
	XmRInt,
	sizeof(int),
	XtOffsetOf(AppData, startup_tip_count),
	XtRImmediate,
	XtPointer(1)
    },

    {
	XtNstatusAtBottom,
	XtCStatusAtBottom,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, status_at_bottom),
	XtRImmediate,
	XtPointer(True)
    },

    {
	XtNtoolbarsAtBottom,
	XtCToolbarsAtBottom,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, toolbars_at_bottom),
	XtRImmediate,
	XtPointer(True)
    },

    {
	XtNmaxDisplayTitleLength,
	XtCMaxLength,
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, max_display_title_length),
	XmRImmediate,
	XtPointer(20)
    },

    {
	XtNmaxPopupExprLength,
	XtCMaxLength,
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, max_popup_expr_length),
	XmRImmediate,
	XtPointer(20)
    },

    {
	XtNmaxValueTipLength,
	XtCMaxLength,
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, max_value_tip_length),
	XmRImmediate,
	XtPointer(32)
    },

    {
	XtNmaxValueDocLength,
	XtCMaxLength,
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, max_value_doc_length),
	XmRImmediate,
	XtPointer(128)
    },

    {
	XtNbuttonTipDelay,
	XtCTipDelay,
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, button_tip_delay),
	XmRImmediate,
	XtPointer(750)
    },

    {
	XtNvalueTipDelay,
	XtCTipDelay,
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, value_tip_delay),
	XmRImmediate,
	XtPointer(750)
    },

    {
	XtNbuttonDocDelay,
	XtCDocDelay,
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, button_doc_delay),
	XmRImmediate,
	XtPointer(0)
    },

    {
	XtNvalueDocDelay,
	XtCDocDelay,
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, value_doc_delay),
	XmRImmediate,
	XtPointer(0)
    },

    {
	XtNclearDocDelay,
	XtCClearDelay,
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, clear_doc_delay),
	XmRImmediate,
	XtPointer(1000)
    },

    {
	XtNclearTipDelay,
	XtCClearDelay,
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, clear_tip_delay),
	XmRImmediate,
	XtPointer(50)
    },

    {
	XtNblinkWhileBusy,
	XtCBlinkWhileBusy,
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, blink_while_busy),
	XmRImmediate,
	XtPointer(True)
    },

    {
	XtNbusyBlinkRate,
	XmCBlinkRate,
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, busy_blink_rate),
	XmRImmediate,
	XtPointer(500)
    },

    {
	XtNcheckGrabs,
	XtCCheckGrabs,
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, check_grabs),
	XmRImmediate,
	XtPointer(True)
    },

    {
	XtNcheckGrabDelay,
	XtCCheckGrabDelay,
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, check_grab_delay),
	XmRImmediate,
	XtPointer(5000)
    },

    {
	XtNgrabActionDelay,
	XtCGrabActionDelay,
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, grab_action_delay),
	XmRImmediate,
	XtPointer(10000)
    },

    {
	XtNgrabAction,
	XtCGrabAction,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, grab_action),
	XmRImmediate,
	XtPointer("cont")
    },

    {
	XtNdetectAliases,
	XtCDetectAliases,
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, detect_aliases),
	XmRImmediate,
	XtPointer(False)
    },

    {
	XtNalign2dArrays,
	XtCAlign2dArrays,
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, align_2d_arrays),
	XmRImmediate,
	XtPointer(True)
    },

    {
	XtNexpandRepeatedValues,
	XtCExpandRepeatedValues,
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, expand_repeated_values),
	XmRImmediate,
	XtPointer(False)
    },

    {
	XtNbumpDisplays,
	XtCBumpDisplays,
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, bump_displays),
	XmRImmediate,
	XtPointer(True)
    },

    {
	XtNhideInactiveDisplays,
	XtCHideInactiveDisplays,
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, hide_inactive_displays),
	XmRImmediate,
	XtPointer(True)
    },

    {
	XtNstatusHistorySize,
	XtCStatusHistorySize,
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, status_history_size),
	XmRImmediate,
	XtPointer(10)
    },

    {
	XtNverifyButtons,
	XtCVerifyButtons,
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, verify_buttons),
	XmRImmediate,
	XtPointer(True)
    },

    {
	XtNautoRaiseTool,
	XtCAutoRaiseTool,
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, auto_raise_tool),
	XmRImmediate,
	XtPointer(True)
    },

    {
	XtNdecorateTool,
	XtCDecorateTool,
	XtROnOff,
	sizeof(OnOff),
	XtOffsetOf(AppData, decorate_tool),
	XmRImmediate,
	XtPointer(Auto)
    },

    {
	XtNstickyTool,
	XtCStickyTool,
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, sticky_tool),
	XmRImmediate,
	XtPointer(True)
    },

    {
	XtNtoolRightOffset,
	XtCToolOffset,
	XmRPosition,
	sizeof(Position),
	XtOffsetOf(AppData, tool_right_offset),
	XmRImmediate,
	XtPointer(8)
    },

    {
	XtNtoolTopOffset,
	XtCToolOffset,
	XmRPosition,
	sizeof(Position),
	XtOffsetOf(AppData, tool_top_offset),
	XmRImmediate,
	XtPointer(8)
    },

    {
	XtNbuttonCaptions,
	XtCButtonCaptions,
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, button_captions),
	XmRImmediate,
	XtPointer(True)
    },

    {
	XtNbuttonImages,
	XtCButtonImages,
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, button_images),
	XmRImmediate,
	XtPointer(True)
    },

    {
	XtNbuttonCaptionGeometry,
	XtCButtonCaptionGeometry,
	XmRString,
	sizeof(String),
	XtOffsetOf(AppData, button_caption_geometry),
	XmRImmediate,
	XtPointer("29x7+0-0")
    },

    {
	XtNbuttonImageGeometry,
	XtCButtonImageGeometry,
	XmRString,
	sizeof(String),
	XtOffsetOf(AppData, button_image_geometry),
	XmRImmediate,
	XtPointer("21x17+4+0")
    },

    {
	XtNflatToolbarButtons,
	XtCFlatButtons,
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, flat_toolbar_buttons),
	XmRImmediate,
	XtPointer(True)
    },

    {
	XtNflatDialogButtons,
	XtCFlatButtons,
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, flat_dialog_buttons),
	XmRImmediate,
	XtPointer(False)
    },

    {
	XtNbuttonColorKey,
	XtCColorKey,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, button_color_key),
	XtRString,
	XtPointer("g")
    },

    {
	XtNsplashScreenColorKey,
	XtCColorKey,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, splash_screen_color_key),
	XtRString,
	XtPointer("c")
    },

    {
	XtNautoCommands,
	XtCAutoCommands,
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, auto_commands),
	XmRImmediate,
	XtPointer(False)
    },

    {
	XtNautoCommandPrefix,
	XtCAutoCommandPrefix,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, auto_command_prefix),
	XtRString,
	XtPointer(ddd_NAME ": ")
    },

    {
	XtNmaxBreakpointNumber,
	XtCMaxNumber,
	XmRInt,
	sizeof(int),
	XtOffsetOf(AppData, max_breakpoint_number),
	XmRImmediate,
	XtPointer(99)
    },

    {
	XtNmaxDisplayNumber,
	XtCMaxNumber,
	XmRInt,
	sizeof(int),
	XtOffsetOf(AppData, max_display_number),
	XmRImmediate,
	XtPointer(99)
    },

    {
	XtNsplashScreen,
	XtCSplashScreen,
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, splash_screen),
	XmRImmediate,
	XtPointer(True)
    },

    {
	XtNcolorWMIcons,
	XtCColorWMIcons,
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, color_wm_icons),
	XmRImmediate,
	XtPointer(True)
    },

    {
	XtNcutCopyPasteBindings,
	XtCBindingStyle,
	XtRBindingStyle,
	sizeof(BindingStyle),
	XtOffsetOf(AppData, cut_copy_paste_bindings),
	XmRImmediate,
	XtPointer(KDEBindings)
    },

    {
	XtNlessTifVersion,
	XtCLessTifVersion,
	XmRInt,
	sizeof(int),
	XtOffsetOf(AppData, lesstif_version),
	XmRImmediate,
#if !defined(LesstifVersion)
	XtPointer(1000)		// Treat OSF/Motif like LessTif 1.0
#else
	XtPointer(LesstifVersion)
#endif
    },

    {
	XtNdddinitVersion,
	XtCVersion,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, dddinit_version),
	XtRString,
	XtPointer(0)
    },

    {
	XtNappDefaultsVersion,
	XtCVersion,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, app_defaults_version),
	XtRString,
	XtPointer(0)
    }
};

const int ddd_resources_size = XtNumber(ddd_resources);

// Resources
String ddd_fallback_resources[] = {
#include "Ddd.ad.h"
0
};
