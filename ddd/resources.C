// $Id$ -*- C++ -*-
// DDD application resources

// Copyright (C) 1996-1998 Technische Universitaet Braunschweig, Germany.
// Copyright (C) 1999-2000 Universitaet Passau, Germany.
// Copyright (C) 2001-2006 Free Software Foundation, Inc.
// Written by Andreas Zeller <zeller@gnu.org>.
// 
// This file is part of DDD.
// 
// DDD is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
// 
// DDD is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with DDD -- see the file COPYING.
// If not, see <http://www.gnu.org/licenses/>.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.gnu.org/software/ddd/',
// or send a mail to the DDD developers <ddd@gnu.org>.

char resources_rcsid[] = 
    "$Id$";

#include "resources.h"
#include "version.h"
#include "AppData.h"
#include "stty.h"
#include "config.h"
#include "resolveP.h"
#include "tabs.h"

#include <Xm/Xm.h>

// Application resource definitions
XtResource ddd_resources[] = {
    {
	XTRESSTR(XtNsession),
	XTRESSTR(XtCSessionID),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, session),
	XtRString, 
	XtPointer(0)
    },

    {
	XTRESSTR(XtNinitialSession),
	XTRESSTR(XtCSessionID),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, initial_session),
	XtRString, 
	XtPointer(0)
    },

    { 
	XTRESSTR(XtNdebugger),
	XTRESSTR(XtCDebugger),
	XtRString, 
	sizeof(String),
	XtOffsetOf(AppData, debugger), 
	XtRString, 
	XtPointer("")
    },

    { 
	XTRESSTR(XtNdebuggerCommand),
	XTRESSTR(XtCDebuggerCommand),
	XtRString, 
	sizeof(String),
	XtOffsetOf(AppData, debugger_command), 
	XtRString, 
	XtPointer("")
    },

    { 
	XTRESSTR(XtNautoDebugger), 
	XTRESSTR(XtCAutoDebugger), 
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, auto_debugger),
	XmRImmediate,
	XtPointer(True)
    },

    { 
	XTRESSTR(XtNopenSelection), 
	XTRESSTR(XtCOpenSelection), 
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, open_selection),
	XmRImmediate,
	XtPointer(False)
    },

    { 
	XTRESSTR(XtNtrace), 
	XTRESSTR(XtCTrace), 
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, trace),
	XmRImmediate, 
	XtPointer(False)
    },

    { 
	XTRESSTR(XtNplayLog),
	XTRESSTR(XtCPlayLog),
	XtRString, 
	sizeof(String),
	XtOffsetOf(AppData, play_log), 
	XtRString, 
	XtPointer(0)
    },

    { 
	XTRESSTR(XtNgdbInitCommands), 
	XTRESSTR(XtCInitCommands), 
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
	XTRESSTR(XtNgdbSettings), 
	XTRESSTR(XtCSettings), 
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, gdb_settings), 
	XtRString,
	XtPointer("")
    },

    { 
	XTRESSTR(XtNdbxInitCommands), 
	XTRESSTR(XtCInitCommands), 
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, dbx_init_commands), 
	XtRString,
	XtPointer("")
    },

    { 
	XTRESSTR(XtNdbxSettings), 
	XTRESSTR(XtCSettings), 
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, dbx_settings), 
	XtRString,
	XtPointer("")
    },

    { 
	XTRESSTR(XtNxdbInitCommands), 
	XTRESSTR(XtCInitCommands), 
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, xdb_init_commands), 
	XtRString,
	XtPointer("")
    },

    { 
	XTRESSTR(XtNxdbSettings), 
	XTRESSTR(XtCSettings), 
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, xdb_settings), 
	XtRString,
	XtPointer("")
    },

    { 
	XTRESSTR(XtNjdbInitCommands), 
	XTRESSTR(XtCInitCommands), 
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, jdb_init_commands), 
	XtRString,
	XtPointer("")
    },

    { 
	XTRESSTR(XtNjdbSettings), 
	XTRESSTR(XtCSettings), 
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, jdb_settings), 
	XtRString,
	XtPointer("")
    },

    { 
	XTRESSTR(XtNpydbInitCommands), 
	XTRESSTR(XtCInitCommands), 
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, pydb_init_commands), 
	XtRString,
	XtPointer("import os\n")
    },

    { 
	XTRESSTR(XtNpydbSettings), 
	XTRESSTR(XtCSettings), 
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, pydb_settings), 
	XtRString,
	XtPointer("")
    },

    { 
	XTRESSTR(XtNperlInitCommands), 
	XTRESSTR(XtCInitCommands), 
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, perl_init_commands), 
	XtRString,
	XtPointer("")
    },

    { 
	XTRESSTR(XtNperlSettings), 
	XTRESSTR(XtCSettings), 
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, perl_settings),
	XtRString,
	XtPointer("")
    },

    { 
	XTRESSTR(XtNbashInitCommands), 
	XTRESSTR(XtCInitCommands), 
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, bash_init_commands), 
	XtRString,
	XtPointer("")
    },

    { 
	XTRESSTR(XtNbashSettings), 
	XTRESSTR(XtCSettings), 
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, bash_settings),
	XtRString,
	XtPointer("")
    },

    { 
	XTRESSTR(XtNmakeInitCommands), 
	XTRESSTR(XtCInitCommands), 
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, make_init_commands), 
	XtRString,
	XtPointer("")
    },

    { 
	XTRESSTR(XtNmakeSettings), 
	XTRESSTR(XtCSettings), 
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, make_settings),
	XtRString,
	XtPointer("")
    },

    {
	XTRESSTR(XtNrestartCommands),
	XTRESSTR(XtCInitCommands),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, restart_commands), 
	XtRString,
	XtPointer("")
    },

    {
	XTRESSTR(XtNsourceInitCommands),
	XTRESSTR(XtCSourceInitCommands),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, source_init_commands),
	XtRImmediate,
	XtPointer(False)
    },

    { 
	XTRESSTR(XtNinitSymbols),
	XTRESSTR(XtCInitSymbols),
	XtRString, 
	sizeof(String),
	XtOffsetOf(AppData, init_symbols),
	XtRString, 
	XtPointer("main:MAIN__")
    },

    { 
	XTRESSTR(XtNvslPath), 
	XTRESSTR(XtCVSLPath), 
	XtRString, 
	sizeof(String),
	XtOffsetOf(AppData, vsl_path),
	XtRString, 
	XtPointer("user_themes:ddd_themes:.")
    },

    { 
	XTRESSTR(XtNvslLibrary), 
	XTRESSTR(XtCVSLLibrary), 
	XtRString, 
	sizeof(String),
	XtOffsetOf(AppData, vsl_library), 
	XtRString, 
	XtPointer("builtin")
    },

    { 
	XTRESSTR(XtNdefaultFont), 
	XTRESSTR(XtCFont), 
	XtRString, 
	sizeof(String),
	XtOffsetOf(AppData, default_font),
	XtRString, 
	XtPointer("-*-helvetica-bold-r-*")
    },

    { 
	XTRESSTR(XtNvariableWidthFont), 
	XTRESSTR(XtCFont),
	XtRString, 
	sizeof(String),
	XtOffsetOf(AppData, variable_width_font),
	XtRString,
	XtPointer("-*-helvetica-medium-r-*")
    },

    { 
	XTRESSTR(XtNfixedWidthFont), 
	XtCFont,
	XtRString, 
	sizeof(String),
	XtOffsetOf(AppData, fixed_width_font),
	XtRString,
	XtPointer("-*-courier-medium-r-*")
    },

    { 
	XTRESSTR(XtNdataFont), 
	XtCFont,
	XtRString, 
	sizeof(String),
	XtOffsetOf(AppData, data_font),
	XtRString,
	XtPointer("-*-courier-medium-r-*")
    },

    {
	XTRESSTR(XtNdefaultFontSize),
	XTRESSTR(XtCFontSize),
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, default_font_size),
	XmRImmediate,
	XtPointer(120)
    },

    {
	XTRESSTR(XtNvariableWidthFontSize),
	XTRESSTR(XtCFontSize),
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, variable_width_font_size),
	XmRImmediate,
	XtPointer(120)
    },

    {
	XTRESSTR(XtNfixedWidthFontSize),
	XTRESSTR(XtCFontSize),
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, fixed_width_font_size),
	XmRImmediate,
	XtPointer(120)
    },

    {
	XTRESSTR(XtNdataFontSize),
	XTRESSTR(XtCFontSize),
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, data_font_size),
	XmRImmediate,
	XtPointer(120)
    },

    { 
	XTRESSTR(XtNfontSelectCommand), 
	XTRESSTR(XtCFontSelectCommand),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, font_select_command),
	XtRString,
	XtPointer("xfontsel -print")
    },

    { 
	XTRESSTR(XtNvslDefs), 
	XTRESSTR(XtCVSLDefs), 
	XtRString, 
	sizeof(String),
	XtOffsetOf(AppData, vsl_defs),
	XtRString, 
	XtPointer("")
    },

    { 
	XTRESSTR(XtNvslBaseDefs), 
	XTRESSTR(XtCVSLDefs), 
	XtRString, 
	sizeof(String),
	XtOffsetOf(AppData, vsl_base_defs),
	XtRString, 
	XtPointer("")
    },

    {
	XTRESSTR(XtNtabWidth),
	XTRESSTR(XtCTabWidth),
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, tab_width),
	XmRImmediate,
	XtPointer(DEFAULT_TAB_WIDTH)
    },

    {
	XTRESSTR(XtNlinesAboveCursor),
	XTRESSTR(XtCLinesAboveCursor),
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, lines_above_cursor),
	XmRImmediate,
	XtPointer(2)
    },

    {
	XTRESSTR(XtNlinesBelowCursor),
	XTRESSTR(XtCLinesBelowCursor),
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, lines_below_cursor),
	XmRImmediate,
	XtPointer(3)
    },

    {
	XTRESSTR(XtNindentSource),
	XTRESSTR(XtCIndent),
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, indent_source),
	XmRImmediate,
	XtPointer(4)
    },

    {
	XTRESSTR(XtNindentScript),
	XTRESSTR(XtCIndent),
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, indent_script),
	XmRImmediate,
	XtPointer(4)
    },

    {
	XTRESSTR(XtNindentCode),
	XTRESSTR(XtCIndent),
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, indent_code),
	XmRImmediate,
	XtPointer(4)
    },

    {
	XTRESSTR(XtNlineNumberWidth),
	XTRESSTR(XtCLineNumberWidth),
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, line_number_width),
	XmRImmediate,
	XtPointer(4)
    },

    {
	XTRESSTR(XtNsourceEditing),
	XTRESSTR(XtCSourceEditing),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, source_editing),
	XtRImmediate,
	XtPointer(False)
    },

    {
	XTRESSTR(XtNlineBufferedConsole),
	XTRESSTR(XtCLineBuffered),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, line_buffered_console),
	XtRImmediate,
	XtPointer(True)
    },

    {
	XTRESSTR(XtNconsoleHasFocus),
	XTRESSTR(XtCConsoleHasFocus),
	XTRESSTR(XtROnOff),
	sizeof(OnOff),
	XtOffsetOf(AppData, console_has_focus),
	XmRImmediate,
	XtPointer(On)
    },

    {
	XTRESSTR(XtNbashDisplayShortcuts),
	XTRESSTR(XtCDisplayShortcuts),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, bash_display_shortcuts),
	XtRString,
	XtPointer("")
    },

    {
	XTRESSTR(XtNdbgDisplayShortcuts),
	XTRESSTR(XtCDisplayShortcuts),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, dbg_display_shortcuts),
	XtRString,
	XtPointer("")
    },

    {
	XTRESSTR(XtNdbxDisplayShortcuts),
	XTRESSTR(XtCDisplayShortcuts),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, dbx_display_shortcuts),
	XtRString,
	XtPointer("")
    },

    {
	XTRESSTR(XtNgdbDisplayShortcuts),
	XTRESSTR(XtCDisplayShortcuts),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, gdb_display_shortcuts),
	XtRString,
	XtPointer("")
    },

    {
	XTRESSTR(XtNjdbDisplayShortcuts),
	XTRESSTR(XtCDisplayShortcuts),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, jdb_display_shortcuts),
	XtRString,
	XtPointer("")
    },

    {
	XTRESSTR(XtNperlDisplayShortcuts),
	XTRESSTR(XtCDisplayShortcuts),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, perl_display_shortcuts),
	XtRString,
	XtPointer("")
    },

    {
	XTRESSTR(XtNpydbDisplayShortcuts),
	XTRESSTR(XtCDisplayShortcuts),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, pydb_display_shortcuts),
	XtRString,
	XtPointer("")
    },

    {
	XTRESSTR(XtNxdbDisplayShortcuts),
	XTRESSTR(XtCDisplayShortcuts),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, xdb_display_shortcuts),
	XtRString,
	XtPointer("")
    },

    {
	XTRESSTR(XtNconsoleButtons),
	XTRESSTR(XtCButtons),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, console_buttons),
	XtRString,
	XtPointer("")
    },

    {
	XTRESSTR(XtNsourceButtons),
	XTRESSTR(XtCButtons),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, source_buttons),
	XtRString,
	XtPointer("")
    },

    {
	XTRESSTR(XtNdataButtons),
	XTRESSTR(XtCButtons),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, data_buttons),
	XtRString,
	XtPointer("")
    },

    {
	XTRESSTR(XtNtoolButtons),
	XTRESSTR(XtCButtons),
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
	XTRESSTR(XtNlabelDelimiter),
	XTRESSTR(XtCLabelDelimiter),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, label_delimiter),
	XtRString,
	XtPointer("//")
    },

    {
	XTRESSTR(XtNcommandToolBar),
	XTRESSTR(XtCToolBar),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, command_toolbar),
	XtRImmediate,
	XtPointer(False)
    },

    {
	XTRESSTR(XtNcommonToolBar),
	XTRESSTR(XtCToolBar),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, common_toolbar),
	XtRImmediate,
	XtPointer(True)
    },

    {
	XTRESSTR(XtNseparateDataWindow),
	XTRESSTR(XtCSeparate),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, separate_data_window),
	XtRImmediate,
	XtPointer(False)
    },

    {
	XTRESSTR(XtNseparateSourceWindow),
	XTRESSTR(XtCSeparate),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, separate_source_window),
	XtRImmediate,
	XtPointer(False)
    },

    {
	XTRESSTR(XtNseparateExecWindow),
	XTRESSTR(XtCSeparate),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, separate_exec_window),
	XtRImmediate,
	XtPointer(False)
    },

    {
	XTRESSTR(XtNopenDataWindow),
	XTRESSTR(XtCWindow),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, data_window),
	XtRImmediate,
	XtPointer(True)
    },

    {
	XTRESSTR(XtNopenSourceWindow),
	XTRESSTR(XtCWindow),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, source_window),
	XtRImmediate,
	XtPointer(True)
    },

    {
	XTRESSTR(XtNopenDebuggerConsole),
	XTRESSTR(XtCWindow),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, debugger_console),
	XtRImmediate,
	XtPointer(True)
    },

    {
	XTRESSTR(XtNautoCloseDataWindow),
	XTRESSTR(XtCAutoClose),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, auto_close_data_window),
	XtRImmediate,
	XtPointer(True)
    },

    {
	XTRESSTR(XtNplotCommand),
	XTRESSTR(XtCPlotCommand),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, plot_command),
	XtRString,
	XtPointer("gnuplot -name @NAME@")
    },

    {
	XTRESSTR(XtNplotTermType),
	XTRESSTR(XtCPlotTermType),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, plot_term_type),
	XtRString,
	XtPointer("xlib")
    },

    {
	XTRESSTR(XtNplotWindowClass),
	XTRESSTR(XtCPlotWindowClass),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, plot_window_class),
	XtRString,
	XtPointer("Gnuplot")
    },

    {
	XTRESSTR(XtNplotWindowDelay),
	XTRESSTR(XtCWindowDelay),
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, plot_window_delay),
	XtRImmediate,
	XtPointer(2000)
    },

    {
	XTRESSTR(XtNplotInitCommands),
	XTRESSTR(XtCPlotInitCommands),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, plot_init_commands),
	XtRString,
	XtPointer("set parametric")
    },

    {
	XTRESSTR(XtNplot2dSettings),
	XTRESSTR(XtCPlotSettings),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, plot_2d_settings),
	XtRString,
	XtPointer("")
    },

    {
	XTRESSTR(XtNplot3dSettings),
	XTRESSTR(XtCPlotSettings),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, plot_3d_settings),
	XtRString,
	XtPointer("")
    },

    {
	XTRESSTR(XtNtermCommand),
	XTRESSTR(XtCTermCommand),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, term_command),
	XtRString,
	XtPointer("xterm -e /bin/sh -c")
    },

    {
	XTRESSTR(XtNtermType),
	XTRESSTR(XtCTermType),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, term_type),
	XtRString,
	XtPointer("xterm")
   },

   {
	XTRESSTR(XtNuseTTYCommand),
	XTRESSTR(XtCUseTTYCommand),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, use_tty_command),
	XtRImmediate,
	XtPointer(False)
    },

    {
	XTRESSTR(XtNquestionTimeout),
	XTRESSTR(XtCQuestionTimeout),
	XmRInt,
	sizeof(int),
	XtOffsetOf(AppData, question_timeout),
	XtRImmediate,
	XtPointer(5)
    },

    {
	XTRESSTR(XtNpositionTimeout),
	XTRESSTR(XtCPositionTimeout),
	XmRInt,
	sizeof(int),
	XtOffsetOf(AppData, position_timeout),
	XtRImmediate,
	XtPointer(500)
    },

    {
	XTRESSTR(XtNdisplayTimeout),
	XTRESSTR(XtCDisplayTimeout),
	XmRInt,
	sizeof(int),
	XtOffsetOf(AppData, display_timeout),
	XtRImmediate,
	XtPointer(2000)
    },

    {
	XTRESSTR(XtNsynchronousDebugger),
	XTRESSTR(XtCSynchronousDebugger),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, synchronous_gdb),
	XtRImmediate,
	XtPointer(False)
    },

    {
	XTRESSTR(XtNterminateOnEOF),
	XTRESSTR(XtCTerminateOnEOF),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, terminate_on_eof),
	XtRImmediate,
	XtPointer(False)
    },

    {
	XTRESSTR(XtNstopAndContinue),
	XTRESSTR(XtCStopAndContinue),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, stop_and_continue),
	XtRImmediate,
	XtPointer(True)
    },

    {
	XTRESSTR(XtNrunInterruptDelay),
	XTRESSTR(XtCInterruptDelay),
	XmRInt,
	sizeof(int),
	XtOffsetOf(AppData, run_interrupt_delay),
	XtRImmediate,
	XtPointer(2000)
    },

    {
	XTRESSTR(XtNcontInterruptDelay),
	XTRESSTR(XtCInterruptDelay),
	XmRInt,
	sizeof(int),
	XtOffsetOf(AppData, cont_interrupt_delay),
	XtRImmediate,
	XtPointer(200)
    },

    {
	XTRESSTR(XtNpollChildStatus),
	XTRESSTR(XtCPollChildStatus),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, poll_child_status),
	XtRImmediate,
	XtPointer(True)
    },

    {
        XTRESSTR(XtNdebuggerHost),
	XTRESSTR(XtCDebuggerHost),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, debugger_host),
	XtRString,
	XtPointer("")
    },

    {
        XTRESSTR(XtNdebuggerHostLogin),
	XTRESSTR(XtCDebuggerHostLogin),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, debugger_host_login),
	XtRString,
	XtPointer("")
    },

    {
        XTRESSTR(XtNrHostInitCommands),
	XTRESSTR(XtCRHostInitCommands),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, rhost_init_commands),
	XtRString,
	XtPointer(STTY_COMMAND)
    },

    {
        XTRESSTR(XtNdebuggerRHost),
	XTRESSTR(XtCDebuggerRHost),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, debugger_rhost),
	XtRString,
	XtPointer("")
    },

    {
        XTRESSTR(XtNrshCommand),
	XTRESSTR(XtCRshCommand),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, rsh_command),
	XtRString,
	XtPointer("rsh")
    },

    {
	XTRESSTR(XtNfilterFiles),
	XTRESSTR(XtCFilterFiles),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, filter_files),
	XtRImmediate,
	XtPointer(True)
    },

    {
        XTRESSTR(XtNlistExecCommand),
	XTRESSTR(XtCListExecCommand),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, list_exec_command),
	XtRString,
	XtPointer("file @MASK@ | grep  '.*:.*exec.*' | cut -d: -f1")
    },

    {
        XTRESSTR(XtNlistSourceCommand),
	XTRESSTR(XtCListSourceCommand),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, list_source_command),
	XtRString,
	XtPointer("file @MASK@ | grep '.*:.*text.*' | cut -d: -f1")
    },

    {
        XTRESSTR(XtNlistCoreCommand),
	XTRESSTR(XtCListCoreCommand),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, list_core_command),
	XtRString,
	XtPointer("file @MASK@ | grep '.*:.*core.*' | cut -d: -f1")
    },

    {
        XTRESSTR(XtNlistDirCommand),
	XTRESSTR(XtCListDirCommand),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, list_dir_command),
	XtRString,
	XtPointer("file @MASK@ | grep '.*:.*directory.*' | cut -d: -f1")
    },

    {
        XTRESSTR(XtNuncompressCommand),
	XTRESSTR(XtCUncompressCommand),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, uncompress_command),
	XtRString,
	XtPointer("gzip -d -c")
    },

    {
        XTRESSTR(XtNpsCommand),
	XTRESSTR(XtCPsCommand),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, ps_command),
	XtRString,
	XtPointer("ps")
    },

    {
        XTRESSTR(XtNwwwPage),
	XTRESSTR(XtCWWWPage),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, www_page),
	XtRString,
	XtPointer("http://www.gnu.org/software/ddd/")
    },

    {
        XTRESSTR(XtNwwwCommand),
	XTRESSTR(XtCWWWCommand),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, www_command),
	XtRString,
	XtPointer("netscape @URL@")
    },

    {
        XTRESSTR(XtNshowInvocation),
	XTRESSTR(XtCShowInvocation),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, show_invocation),
	XtRImmediate,
	XtPointer(False)
    },

    {
        XTRESSTR(XtNshowVersion),
	XTRESSTR(XtCShowVersion),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, show_version),
	XtRImmediate,
	XtPointer(False)
    },

    {
        XTRESSTR(XtNshowConfiguration),
	XTRESSTR(XtCShowConfiguration),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, show_configuration),
	XtRImmediate,
	XtPointer(False)
    },

    {
        XTRESSTR(XtNshowManual),
	XTRESSTR(XtCShowManual),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, show_manual),
	XtRImmediate,
	XtPointer(False)
    },

    {
        XTRESSTR(XtNshowLicense),
	XTRESSTR(XtCShowLicense),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, show_license),
	XtRImmediate,
	XtPointer(False)
    },

    {
        XTRESSTR(XtNshowNews),
	XTRESSTR(XtCShowNews),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, show_news),
	XtRImmediate,
	XtPointer(False)
    },

    {
        XTRESSTR(XtNshowFonts),
	XTRESSTR(XtCShowFonts),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, show_fonts),
	XtRImmediate,
	XtPointer(False)
    },

    {
        XTRESSTR(XtNcheckConfiguration),
	XTRESSTR(XtCCheckConfiguration),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, check_configuration),
	XtRImmediate,
	XtPointer(False)
    },

    {
        XTRESSTR(XtNprintCommand),
	XTRESSTR(XtCPrintCommand),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, print_command),
	XtRString,
	XtPointer("lp")
    },

    {
        XTRESSTR(XtNpaperSize),
	XTRESSTR(XtCPaperSize),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, paper_size),
	XtRString,
	XtPointer("210mm x 297mm")
    },

    {
        XTRESSTR(XtNgetCoreCommand),
	XTRESSTR(XtCGetCoreCommand),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, get_core_command),
	XtRString,
	XtPointer("")
    },

    {
        XTRESSTR(XtNeditCommand),
	XTRESSTR(XtCEditCommand),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, edit_command),
	XtRString,
	XtPointer("xterm -e vi +@LINE@ @FILE@")
    },

    {
        XTRESSTR(XtNpannedGraphEditor),
	XTRESSTR(XtCPannedGraphEditor),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, panned_graph_editor),
	XtRImmediate,
	XtPointer(False)
    },

    {
        XTRESSTR(XtNfindWordsOnly),
	XTRESSTR(XtCFindWordsOnly),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, find_words_only),
	XtRImmediate,
	XtPointer(True)
    },

    {
        XTRESSTR(XtNfindCaseSensitive),
	XTRESSTR(XtCFindCaseSensitive),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, find_case_sensitive),
	XtRImmediate,
	XtPointer(True)
    },

    {
	XTRESSTR(XtNgroupIconify),
	XTRESSTR(XtCGroupIconify),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, group_iconify),
	XtRImmediate,
	XtPointer(False)
    },

    {
	XTRESSTR(XtNuniconifyWhenReady),
	XTRESSTR(XtCUniconifyWhenReady),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, uniconify_when_ready),
	XtRImmediate,
	XtPointer(True)
    },

    {
	XTRESSTR(XtNtransientDialogs),
	XTRESSTR(XtCTransientDialogs),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, transient_dialogs),
	XtRImmediate,
	XtPointer(True)
    },

    {
	XTRESSTR(XtNglobalTabCompletion),
	XTRESSTR(XtCGlobalTabCompletion),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, global_tab_completion),
	XtRImmediate,
	XtPointer(False)
    },

    {
	XTRESSTR(XtNsaveHistoryOnExit),
	XTRESSTR(XtCSaveOnExit),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, save_history_on_exit),
	XtRImmediate,
	XtPointer(True)
    },

    {
	XTRESSTR(XtNsaveOptionsOnExit),
	XTRESSTR(XtCSaveOnExit),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, save_options_on_exit),
	XtRImmediate,
	XtPointer(True)
    },

    {
	XTRESSTR(XtNcacheSourceFiles),
	XTRESSTR(XtCCacheSourceFiles),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, cache_source_files),
	XtRImmediate,
	XtPointer(True)
    },

    {
	XTRESSTR(XtNcacheMachineCode),
	XTRESSTR(XtCCacheMachineCode),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, cache_machine_code),
	XtRImmediate,
	XtPointer(True)
    },

    {
	XTRESSTR(XtNsuppressWarnings),
	XTRESSTR(XtCSuppressWarnings),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, suppress_warnings),
	XtRImmediate,
	XtPointer(False)
    },

    {
	XTRESSTR(XtNwarnIfLocked),
	XTRESSTR(XtCWarnIfLocked),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, warn_if_locked),
	XtRImmediate,
	XtPointer(False)
    },

    {
	XTRESSTR(XtNcheckOptions),
	XTRESSTR(XtCCheckOptions),
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, check_options),
	XtRImmediate,
	XtPointer(30)
    },

    {
	XTRESSTR(XtNttyMode),
	XTRESSTR(XtCTTYMode),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, tty_mode),
	XtRImmediate,
	XtPointer(False)
    },

    {
	XTRESSTR(XtNblockTTYInput),
	XTRESSTR(XtCBlockTTYInput),
	XTRESSTR(XtROnOff),
	sizeof(OnOff),
	XtOffsetOf(AppData, block_tty_input),
	XmRImmediate,
	XtPointer(Auto)
    },

    {
	XTRESSTR(XtNbufferGDBOutput),
	XTRESSTR(XtCBufferGDBOutput),
	XTRESSTR(XtROnOff),
	sizeof(OnOff),
	XtOffsetOf(AppData, buffer_gdb_output),
	XmRImmediate,
	XtPointer(Auto)
    },

    {
	XTRESSTR(XtNannotate),
	XTRESSTR(XtCAnnotate),
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, annotate),
	XtRImmediate,
	XtPointer(0)
    },

    {
	XTRESSTR(XtNdisplayGlyphs),
	XTRESSTR(XtCDisplayGlyphs),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, display_glyphs),
	XtRImmediate,
	XtPointer(True)
    },

    {
	XTRESSTR(XtNmaxGlyphs),
	XTRESSTR(XtCMaxGlyphs),
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, max_glyphs),
	XtRImmediate,
	XtPointer(10)
    },

    {
	XTRESSTR(XtNcacheGlyphImages),
	XTRESSTR(XtCCacheGlyphImages),
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
	XTRESSTR(XtNglyphUpdateDelay),
	XTRESSTR(XtCGlyphUpdateDelay),
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, glyph_update_delay),
	XtRImmediate,
	XtPointer(10)
    },

    {
	XTRESSTR(XtNdisplayLineNumbers),
	XTRESSTR(XtCDisplayLineNumbers),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, display_line_numbers),
	XtRImmediate,
	XtPointer(False)
    },

    {
	XTRESSTR(XtNdisassemble),
	XTRESSTR(XtCDisassemble),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, disassemble),
	XtRImmediate,
	XtPointer(False)
    },

    {
	XTRESSTR(XtNmaxDisassemble),
	XTRESSTR(XtCMaxDisassemble),
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, max_disassemble),
	XmRImmediate,
	XtPointer(1024)
    },

    {
	XTRESSTR(XtNuseSourcePath),
	XTRESSTR(XtCUseSourcePath),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, use_source_path),
	XtRImmediate,
	XtPointer(False)
    },

    {
	XTRESSTR(XtNallRegisters),
	XTRESSTR(XtCAllRegisters),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, all_registers),
	XtRImmediate,
	XtPointer(False)
    },

    {
	XTRESSTR(XtNbuttonTips),
	XTRESSTR(XtCTips),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, button_tips),
	XtRImmediate,
	XtPointer(True)
    },

    {
	XTRESSTR(XtNvalueTips),
	XTRESSTR(XtCTips),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, value_tips),
	XtRImmediate,
	XtPointer(True)
    },

    {
	XTRESSTR(XtNbuttonDocs),
	XTRESSTR(XtCDocs),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, button_docs),
	XtRImmediate,
	XtPointer(True)
    },

    {
	XTRESSTR(XtNvalueDocs),
	XTRESSTR(XtCDocs),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, value_docs),
	XtRImmediate,
	XtPointer(True)
    },

    {
	XTRESSTR(XtNstartupTips),
	XTRESSTR(XtCStartupTips),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, startup_tips),
	XtRImmediate,
	XtPointer(True)
    },

    {
	XTRESSTR(XtNstartupTipCount),
	XTRESSTR(XtCStartupTipCount),
	XmRInt,
	sizeof(int),
	XtOffsetOf(AppData, startup_tip_count),
	XtRImmediate,
	XtPointer(1)
    },

    {
	XTRESSTR(XtNstatusAtBottom),
	XTRESSTR(XtCStatusAtBottom),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, status_at_bottom),
	XtRImmediate,
	XtPointer(True)
    },

    {
	XTRESSTR(XtNtoolbarsAtBottom),
	XTRESSTR(XtCToolbarsAtBottom),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, toolbars_at_bottom),
	XtRImmediate,
	XtPointer(True)
    },

    {
	XTRESSTR(XtNmaxDisplayTitleLength),
	XTRESSTR(XtCMaxLength),
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, max_display_title_length),
	XmRImmediate,
	XtPointer(20)
    },

    {
	XTRESSTR(XtNmaxPopupExprLength),
	XTRESSTR(XtCMaxLength),
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, max_popup_expr_length),
	XmRImmediate,
	XtPointer(20)
    },

    {
	XTRESSTR(XtNmaxValueTipLength),
	XTRESSTR(XtCMaxLength),
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, max_value_tip_length),
	XmRImmediate,
	XtPointer(32)
    },

    {
	XTRESSTR(XtNmaxValueDocLength),
	XTRESSTR(XtCMaxLength),
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, max_value_doc_length),
	XmRImmediate,
	XtPointer(128)
    },

    {
	XTRESSTR(XtNbuttonTipDelay),
	XTRESSTR(XtCTipDelay),
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, button_tip_delay),
	XmRImmediate,
	XtPointer(750)
    },

    {
	XTRESSTR(XtNvalueTipDelay),
	XTRESSTR(XtCTipDelay),
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, value_tip_delay),
	XmRImmediate,
	XtPointer(750)
    },

    {
	XTRESSTR(XtNbuttonDocDelay),
	XTRESSTR(XtCDocDelay),
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, button_doc_delay),
	XmRImmediate,
	XtPointer(0)
    },

    {
	XTRESSTR(XtNvalueDocDelay),
	XTRESSTR(XtCDocDelay),
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, value_doc_delay),
	XmRImmediate,
	XtPointer(0)
    },

    {
	XTRESSTR(XtNclearDocDelay),
	XTRESSTR(XtCClearDelay),
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, clear_doc_delay),
	XmRImmediate,
	XtPointer(1000)
    },

    {
	XTRESSTR(XtNclearTipDelay),
	XTRESSTR(XtCClearDelay),
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, clear_tip_delay),
	XmRImmediate,
	XtPointer(50)
    },

    {
	XTRESSTR(XtNblinkWhileBusy),
	XTRESSTR(XtCBlinkWhileBusy),
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, blink_while_busy),
	XmRImmediate,
	XtPointer(True)
    },

    {
	XTRESSTR(XtNbusyBlinkRate),
	XTRESSTR(XmCBlinkRate),
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, busy_blink_rate),
	XmRImmediate,
	XtPointer(500)
    },

    {
	XTRESSTR(XtNcheckGrabs),
	XTRESSTR(XtCCheckGrabs),
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, check_grabs),
	XmRImmediate,
	XtPointer(True)
    },

    {
	XTRESSTR(XtNcheckGrabDelay),
	XTRESSTR(XtCCheckGrabDelay),
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, check_grab_delay),
	XmRImmediate,
	XtPointer(5000)
    },

    {
	XTRESSTR(XtNgrabActionDelay),
	XTRESSTR(XtCGrabActionDelay),
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, grab_action_delay),
	XmRImmediate,
	XtPointer(10000)
    },

    {
	XTRESSTR(XtNgrabAction),
	XTRESSTR(XtCGrabAction),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, grab_action),
	XmRImmediate,
	XtPointer("cont")
    },

    {
	XTRESSTR(XtNdetectAliases),
	XTRESSTR(XtCDetectAliases),
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, detect_aliases),
	XmRImmediate,
	XtPointer(False)
    },

    {
	XTRESSTR(XtNtypedAliases),
	XTRESSTR(XtCTypedAliases),
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, typed_aliases),
	XmRImmediate,
	XtPointer(True)
    },

    {
	XTRESSTR(XtNdeleteAliasDisplays),
	XTRESSTR(XtCDeleteAliasDisplays),
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, delete_alias_displays),
	XmRImmediate,
	XtPointer(True)
    },

    {
	XTRESSTR(XtNalign2dArrays),
	XTRESSTR(XtCAlign2dArrays),
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, align_2d_arrays),
	XmRImmediate,
	XtPointer(True)
    },

    {
	XTRESSTR(XtNarrayOrientation),
	XTRESSTR(XtCOrientation),
	XtROrientation,
	sizeof(unsigned char),
	XtOffsetOf(AppData, array_orientation),
	XmRImmediate,
	XtPointer(XmVERTICAL)
    },

    {
	XTRESSTR(XtNstructOrientation),
	XTRESSTR(XtCOrientation),
	XtROrientation,
	sizeof(unsigned char),
	XtOffsetOf(AppData, struct_orientation),
	XmRImmediate,
	XtPointer(XmVERTICAL)
    },

    {
	XTRESSTR(XtNshowMemberNames),
	XTRESSTR(XtCShowMemberNames),
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, show_member_names),
	XmRImmediate,
	XtPointer(True)
    },

    {
	XTRESSTR(XtNexpandRepeatedValues),
	XTRESSTR(XtCExpandRepeatedValues),
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, expand_repeated_values),
	XmRImmediate,
	XtPointer(False)
    },

    {
	XTRESSTR(XtNbumpDisplays),
	XTRESSTR(XtCBumpDisplays),
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, bump_displays),
	XmRImmediate,
	XtPointer(True)
    },

    {
	XTRESSTR(XtNhideInactiveDisplays),
	XTRESSTR(XtCHideInactiveDisplays),
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, hide_inactive_displays),
	XmRImmediate,
	XtPointer(True)
    },

    {
	XTRESSTR(XtNshowBaseDisplayTitles),
	XTRESSTR(XtCShowDisplayTitles),
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, show_base_display_titles),
	XmRImmediate,
	XtPointer(True)
    },

    {
	XTRESSTR(XtNshowDependentDisplayTitles),
	XTRESSTR(XtCShowDisplayTitles),
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, show_dependent_display_titles),
	XmRImmediate,
	XtPointer(True)
    },

    {
	XTRESSTR(XtNclusterDisplays),
	XTRESSTR(XtCClusterDisplays),
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, cluster_displays),
	XmRImmediate,
	XtPointer(False)
    },

    {
	XTRESSTR(XtNdisplayPlacement),
	XTRESSTR(XtCOrientation),
	XtROrientation,
	sizeof(unsigned char),
	XtOffsetOf(AppData, display_placement),
	XmRImmediate,
	XtPointer(XmVERTICAL)
    },

    {
	XTRESSTR(XtNstatusHistorySize),
	XTRESSTR(XtCHistorySize),
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, status_history_size),
	XmRImmediate,
	XtPointer(10)
    },

    {
	XTRESSTR(XtNpopdownHistorySize),
	XTRESSTR(XtCHistorySize),
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, popdown_history_size),
	XmRImmediate,
	XtPointer(0)
    },

    {
	XTRESSTR(XtNsortPopdownHistory),
	XTRESSTR(XtCSortPopdownHistory),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, sort_popdown_history),
	XmRImmediate,
	XtPointer(True)
    },

    {
	XTRESSTR(XtNverifyButtons),
	XTRESSTR(XtCVerifyButtons),
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, verify_buttons),
	XmRImmediate,
	XtPointer(True)
    },

    {
	XTRESSTR(XtNautoRaiseTool),
	XTRESSTR(XtCAutoRaiseTool),
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, auto_raise_tool),
	XmRImmediate,
	XtPointer(True)
    },

    {
	XTRESSTR(XtNdecorateTool),
	XTRESSTR(XtCDecorate),
	XTRESSTR(XtROnOff),
	sizeof(OnOff),
	XtOffsetOf(AppData, decorate_tool),
	XmRImmediate,
	XtPointer(Auto)
    },

    {
	XTRESSTR(XtNstickyTool),
	XTRESSTR(XtCStickyTool),
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, sticky_tool),
	XmRImmediate,
	XtPointer(True)
    },

    {
	XTRESSTR(XtNtoolRightOffset),
	XTRESSTR(XtCToolOffset),
	XmRPosition,
	sizeof(Position),
	XtOffsetOf(AppData, tool_right_offset),
	XmRImmediate,
	XtPointer(8)
    },

    {
	XTRESSTR(XtNtoolTopOffset),
	XTRESSTR(XtCToolOffset),
	XmRPosition,
	sizeof(Position),
	XtOffsetOf(AppData, tool_top_offset),
	XmRImmediate,
	XtPointer(8)
    },

    {
	XTRESSTR(XtNbuttonCaptions),
	XTRESSTR(XtCButtonCaptions),
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, button_captions),
	XmRImmediate,
	XtPointer(True)
    },

    {
	XTRESSTR(XtNbuttonImages),
	XTRESSTR(XtCButtonImages),
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, button_images),
	XmRImmediate,
	XtPointer(True)
    },

    {
	XTRESSTR(XtNbuttonCaptionGeometry),
	XTRESSTR(XtCButtonCaptionGeometry),
	XmRString,
	sizeof(String),
	XtOffsetOf(AppData, button_caption_geometry),
	XmRImmediate,
	XtPointer("29x7+0-0")
    },

    {
	XTRESSTR(XtNbuttonImageGeometry),
	XTRESSTR(XtCButtonImageGeometry),
	XmRString,
	sizeof(String),
	XtOffsetOf(AppData, button_image_geometry),
	XmRImmediate,
	XtPointer("21x17+4+0")
    },

    {
	XTRESSTR(XtNflatToolbarButtons),
	XTRESSTR(XtCFlatButtons),
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, flat_toolbar_buttons),
	XmRImmediate,
	XtPointer(True)
    },

    {
	XTRESSTR(XtNflatDialogButtons),
	XTRESSTR(XtCFlatButtons),
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, flat_dialog_buttons),
	XmRImmediate,
	XtPointer(False)
    },

    {
	XTRESSTR(XtNbuttonColorKey),
	XTRESSTR(XtCColorKey),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, button_color_key),
	XtRString,
	XtPointer("g")
    },

    {
	XTRESSTR(XtNactiveButtonColorKey),
	XTRESSTR(XtCColorKey),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, active_button_color_key),
	XtRString,
	XtPointer("c")
    },

    {
	XTRESSTR(XtNsplashScreenColorKey),
	XTRESSTR(XtCColorKey),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, splash_screen_color_key),
	XtRString,
	XtPointer("c")
    },

    {
	XTRESSTR(XtNautoCommands),
	XTRESSTR(XtCAutoCommands),
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, auto_commands),
	XmRImmediate,
	XtPointer(False)
    },

    {
	XTRESSTR(XtNautoCommandPrefix),
	XTRESSTR(XtCAutoCommandPrefix),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, auto_command_prefix),
	XtRString,
	XtPointer(ddd_NAME ": ")
    },

    {
	XTRESSTR(XtNmaxBreakpointNumber),
	XTRESSTR(XtCMaxNumber),
	XmRInt,
	sizeof(int),
	XtOffsetOf(AppData, max_breakpoint_number),
	XmRImmediate,
	XtPointer(99)
    },

    {
	XTRESSTR(XtNmaxDisplayNumber),
	XTRESSTR(XtCMaxNumber),
	XmRInt,
	sizeof(int),
	XtOffsetOf(AppData, max_display_number),
	XmRImmediate,
	XtPointer(99)
    },

    {
	XTRESSTR(XtNsplashScreen),
	XTRESSTR(XtCSplashScreen),
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, splash_screen),
	XmRImmediate,
	XtPointer(True)
    },

    {
	XTRESSTR(XtNcolorWMIcons),
	XTRESSTR(XtCColorWMIcons),
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, color_wm_icons),
	XmRImmediate,
	XtPointer(True)
    },

    {
	XTRESSTR(XtNcutCopyPasteBindings),
	XTRESSTR(XtCBindingStyle),
	XTRESSTR(XtRBindingStyle),
	sizeof(BindingStyle),
	XtOffsetOf(AppData, cut_copy_paste_bindings),
	XmRImmediate,
	XtPointer(KDEBindings)
    },

    {
	XTRESSTR(XtNselectAllBindings),
	XTRESSTR(XtCBindingStyle),
	XTRESSTR(XtRBindingStyle),
	sizeof(BindingStyle),
	XtOffsetOf(AppData, select_all_bindings),
	XmRImmediate,
	XtPointer(KDEBindings)
    },

    {
	XTRESSTR(XtNmaxUndoDepth),
	XTRESSTR(XtCMaxUndoDepth),
	XmRInt,
	sizeof(int),
	XtOffsetOf(AppData, max_undo_depth),
	XmRImmediate,
	XtPointer(-1)
    },

    {
	XTRESSTR(XtNmaxUndoSize),
	XTRESSTR(XtCMaxUndoSize),
	XmRInt,
	sizeof(int),
	XtOffsetOf(AppData, max_undo_size),
	XmRImmediate,
	XtPointer(2000000)
    },

    {
	XTRESSTR(XtNthemes),
	XTRESSTR(XtCThemes),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, themes),
	XtRString,
	XtPointer("")
    },

    {
	XTRESSTR(XtNsuppressTheme),
	XTRESSTR(XtCTheme),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, suppress_theme),
	XtRString,
	XtPointer("suppress.vsl")
    },

    {
	XTRESSTR(XtNmaintenance),
	XTRESSTR(XtCMaintenance),
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, maintenance),
	XmRImmediate,
	XtPointer(False)
    },

    {
	XTRESSTR(XtNdumpCore),
	XTRESSTR(XtCDumpCore),
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, dump_core),
	XmRImmediate,
	XtPointer(True)
    },

    {
	XTRESSTR(XtNdebugCoreDumps),
	XTRESSTR(XtCDebugCoreDumps),
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, debug_core_dumps),
	XmRImmediate,
	XtPointer(False)
    },

    {
	XTRESSTR(XtNlessTifVersion),
	XTRESSTR(XtCLessTifVersion),
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
	XTRESSTR(XtNdddinitVersion),
	XTRESSTR(XtCVersion),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, dddinit_version),
	XtRString,
	XtPointer(0)
    },

    {
	XTRESSTR(XtNappDefaultsVersion),
	XTRESSTR(XtCVersion),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, app_defaults_version),
	XtRString,
	XtPointer(0)
    }
};

const int ddd_resources_size = XtNumber(ddd_resources);

// Application resources
AppData app_data;

// Make sure APP_DATA is initialized with above default values
static struct AppDataInitializer {
    AppDataInitializer();
} app_data_initializer;


// Copy the SIZE bytes at FROM into the SIZE bytes at DEST, even if
// those two blocks of space overlap.  In the case of overlap, be
// careful to copy the original values of the bytes in the block at
// SRC, including those bytes which also belong to the block at DEST.
static void copy(char *dest, const char *src, Cardinal size)
{
    // This is taken from libiberty's bcopy().
    if (dest < src)
	while (size--)
	    *dest++ = *src++;
    else
    {
	const char *lasts = src + (size - 1);
	char *lastd = dest + (size - 1);
	while (size--)
	    *(char *)lastd-- = *lasts--;
    }
}

// Copy the value at SRC of size SIZE to the value at DEST.
static void CopyArg(XtPointer src, XtPointer dest, Cardinal size)
{
    // This stuff is taken from Xt11R6.3 _XtCopyArg().  I hope it
    // gets all possible conversions right...

    if (size > sizeof(XtArgVal))
    {
	copy(STATIC_CAST(char *,dest), STATIC_CAST(char *,src), size);
    }
    else 
    {
	union {
	    long longval;
	    int intval;
	    short shortval;
	    char charval;
	    char * charptr;
	    XtPointer ptr;
	} u;
	char *p = (char *)&u;
	if (size == sizeof(long))
	    u.longval = (long)src;
	else if (size == sizeof(int))
	    u.intval = (int)(long)src;
	else if (size == sizeof(short))
	    u.shortval = (short)(long)src;
	else if (size == sizeof(char))
	    u.charval = (char)(long)src;
	else if (size == sizeof(XtPointer))
	    u.ptr = (XtPointer)src;
	else if (size == sizeof(char *))
	    u.charptr = (char *)src;
	else
	    p = (char *)&src;

	copy(STATIC_CAST(char *,dest), STATIC_CAST(char *,p), size);
    }
}


// This constructor is invoked before program start
AppDataInitializer::AppDataInitializer()
{
    // Copy resources to appropriate fields in APP_DATA
    for (int i = 0; i < int(ddd_resources_size); i++)
    {
	const XtResource& res = ddd_resources[i];
	XtPointer src = res.default_addr;
	XtPointer dest = ((char *)&app_data) + res.resource_offset;
	Cardinal size = res.resource_size;

	CopyArg(src, dest, size);
    }
}

// Fallback resources
// The resources should be loaded in read-only memory thanks to "const"
const _XtString const ddd_fallback_resources[] = {
#if WITH_BUILTIN_APP_DEFAULTS
#include "Ddd.ad.h"
#endif

// Terminating null pointer
0

};


// Return a database of default settings
XrmDatabase app_defaults(Display *display)
{
    XrmDatabase db = 0;

    // Add builtin fallback defaults.
    int i = 0;
    while (ddd_fallback_resources[i] != 0)
	XrmPutLineResource(&db, ddd_fallback_resources[i++]);

    // Add app-defaults file, overriding fallback defaults.
    static String app_name  = 0;
    static String app_class = 0;
    if (app_name == 0)
	XtGetApplicationNameAndClass(display, &app_name, &app_class);

    static string own_app_defaults_file = 
	resolvePath(string(ddd_NAME) + "/" + app_class);

    if (!own_app_defaults_file.empty())
    {
	// We have an application defaults file installed in the
	// standard place.  Use it.
	XrmDatabase db2 = XrmGetFileDatabase(own_app_defaults_file.chars());
	if (db2 != 0)
	    XrmMergeDatabases(db2, &db);
    }
    else
    {
	// No application defaults file in the standard place.
	// Search along the usual path.
	String official_app_defaults_file = 
	    XtResolvePathname(display, 
			      (String)0, // type
			      app_class, // filename
			      (String)0, // suffix
			      (String)0, // path
			      Substitution(0), 0, 
			      XtFilePredicate(0));

	if (official_app_defaults_file != 0)
	{
	    XrmDatabase db2 = XrmGetFileDatabase(official_app_defaults_file);
	    if (db2 != 0)
		XrmMergeDatabases(db2, &db);
	}
    }

    return db;
}
