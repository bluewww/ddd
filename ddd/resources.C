// $Id$ -*- C++ -*-
// DDD application resources

// Copyright (C) 1996-1998 Technische Universitaet Braunschweig, Germany.
// Copyright (C) 1999-2000 Universitaet Passau, Germany.
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

char resources_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

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
	CONST_CAST(char *,XtNsession),
	CONST_CAST(char *,XtCSessionID),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, session),
	XtRString, 
	XtPointer(0)
    },

    {
	CONST_CAST(char *,XtNinitialSession),
	CONST_CAST(char *,XtCSessionID),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, initial_session),
	XtRString, 
	XtPointer(0)
    },

    { 
	CONST_CAST(char *,XtNdebugger),
	CONST_CAST(char *,XtCDebugger),
	XtRString, 
	sizeof(String),
	XtOffsetOf(AppData, debugger), 
	XtRString, 
	XtPointer("")
    },

    { 
	CONST_CAST(char *,XtNdebuggerCommand),
	CONST_CAST(char *,XtCDebuggerCommand),
	XtRString, 
	sizeof(String),
	XtOffsetOf(AppData, debugger_command), 
	XtRString, 
	XtPointer("")
    },

    { 
	CONST_CAST(char *,XtNautoDebugger), 
	CONST_CAST(char *,XtCAutoDebugger), 
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, auto_debugger),
	XmRImmediate,
	XtPointer(True)
    },

    { 
	CONST_CAST(char *,XtNopenSelection), 
	CONST_CAST(char *,XtCOpenSelection), 
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, open_selection),
	XmRImmediate,
	XtPointer(False)
    },

    { 
	CONST_CAST(char *,XtNtrace), 
	CONST_CAST(char *,XtCTrace), 
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, trace),
	XmRImmediate, 
	XtPointer(False)
    },

    { 
	CONST_CAST(char *,XtNplayLog),
	CONST_CAST(char *,XtCPlayLog),
	XtRString, 
	sizeof(String),
	XtOffsetOf(AppData, play_log), 
	XtRString, 
	XtPointer(0)
    },

    { 
	CONST_CAST(char *,XtNgdbInitCommands), 
	CONST_CAST(char *,XtCInitCommands), 
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
	CONST_CAST(char *,XtNgdbSettings), 
	CONST_CAST(char *,XtCSettings), 
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, gdb_settings), 
	XtRString,
	XtPointer("")
    },

    { 
	CONST_CAST(char *,XtNdbxInitCommands), 
	CONST_CAST(char *,XtCInitCommands), 
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, dbx_init_commands), 
	XtRString,
	XtPointer("")
    },

    { 
	CONST_CAST(char *,XtNdbxSettings), 
	CONST_CAST(char *,XtCSettings), 
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, dbx_settings), 
	XtRString,
	XtPointer("")
    },

    { 
	CONST_CAST(char *,XtNxdbInitCommands), 
	CONST_CAST(char *,XtCInitCommands), 
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, xdb_init_commands), 
	XtRString,
	XtPointer("")
    },

    { 
	CONST_CAST(char *,XtNxdbSettings), 
	CONST_CAST(char *,XtCSettings), 
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, xdb_settings), 
	XtRString,
	XtPointer("")
    },

    { 
	CONST_CAST(char *,XtNjdbInitCommands), 
	CONST_CAST(char *,XtCInitCommands), 
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, jdb_init_commands), 
	XtRString,
	XtPointer("")
    },

    { 
	CONST_CAST(char *,XtNjdbSettings), 
	CONST_CAST(char *,XtCSettings), 
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, jdb_settings), 
	XtRString,
	XtPointer("")
    },

    { 
	CONST_CAST(char *,XtNpydbInitCommands), 
	CONST_CAST(char *,XtCInitCommands), 
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, pydb_init_commands), 
	XtRString,
	XtPointer("import os\n")
    },

    { 
	CONST_CAST(char *,XtNpydbSettings), 
	CONST_CAST(char *,XtCSettings), 
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, pydb_settings), 
	XtRString,
	XtPointer("")
    },

    { 
	CONST_CAST(char *,XtNperlInitCommands), 
	CONST_CAST(char *,XtCInitCommands), 
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, perl_init_commands), 
	XtRString,
	XtPointer("")
    },

    { 
	CONST_CAST(char *,XtNperlSettings), 
	CONST_CAST(char *,XtCSettings), 
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, perl_settings),
	XtRString,
	XtPointer("")
    },

    { 
	CONST_CAST(char *,XtNbashInitCommands), 
	CONST_CAST(char *,XtCInitCommands), 
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, bash_init_commands), 
	XtRString,
	XtPointer("")
    },

    { 
	CONST_CAST(char *,XtNbashSettings), 
	CONST_CAST(char *,XtCSettings), 
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, bash_settings),
	XtRString,
	XtPointer("")
    },

    {
	CONST_CAST(char *,XtNrestartCommands),
	CONST_CAST(char *,XtCInitCommands),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, restart_commands), 
	XtRString,
	XtPointer("")
    },

    {
	CONST_CAST(char *,XtNsourceInitCommands),
	CONST_CAST(char *,XtCSourceInitCommands),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, source_init_commands),
	XtRImmediate,
	XtPointer(False)
    },

    { 
	CONST_CAST(char *,XtNinitSymbols),
	CONST_CAST(char *,XtCInitSymbols),
	XtRString, 
	sizeof(String),
	XtOffsetOf(AppData, init_symbols),
	XtRString, 
	XtPointer("main:MAIN__")
    },

    { 
	CONST_CAST(char *,XtNvslPath), 
	CONST_CAST(char *,XtCVSLPath), 
	XtRString, 
	sizeof(String),
	XtOffsetOf(AppData, vsl_path),
	XtRString, 
	XtPointer("user_themes:ddd_themes:.")
    },

    { 
	CONST_CAST(char *,XtNvslLibrary), 
	CONST_CAST(char *,XtCVSLLibrary), 
	XtRString, 
	sizeof(String),
	XtOffsetOf(AppData, vsl_library), 
	XtRString, 
	XtPointer("builtin")
    },

    { 
	CONST_CAST(char *,XtNdefaultFont), 
	CONST_CAST(char *,XtCFont), 
	XtRString, 
	sizeof(String),
	XtOffsetOf(AppData, default_font),
	XtRString, 
	XtPointer("-*-helvetica-bold-r-*")
    },

    { 
	CONST_CAST(char *,XtNvariableWidthFont), 
	CONST_CAST(char *,XtCFont),
	XtRString, 
	sizeof(String),
	XtOffsetOf(AppData, variable_width_font),
	XtRString,
	XtPointer("-*-helvetica-medium-r-*")
    },

    { 
	CONST_CAST(char *,XtNfixedWidthFont), 
	XtCFont,
	XtRString, 
	sizeof(String),
	XtOffsetOf(AppData, fixed_width_font),
	XtRString,
	XtPointer("-*-courier-medium-r-*")
    },

    { 
	CONST_CAST(char *,XtNdataFont), 
	XtCFont,
	XtRString, 
	sizeof(String),
	XtOffsetOf(AppData, data_font),
	XtRString,
	XtPointer("-*-courier-medium-r-*")
    },

    {
	CONST_CAST(char *,XtNdefaultFontSize),
	CONST_CAST(char *,XtCFontSize),
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, default_font_size),
	XmRImmediate,
	XtPointer(120)
    },

    {
	CONST_CAST(char *,XtNvariableWidthFontSize),
	CONST_CAST(char *,XtCFontSize),
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, variable_width_font_size),
	XmRImmediate,
	XtPointer(120)
    },

    {
	CONST_CAST(char *,XtNfixedWidthFontSize),
	CONST_CAST(char *,XtCFontSize),
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, fixed_width_font_size),
	XmRImmediate,
	XtPointer(120)
    },

    {
	CONST_CAST(char *,XtNdataFontSize),
	CONST_CAST(char *,XtCFontSize),
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, data_font_size),
	XmRImmediate,
	XtPointer(120)
    },

    { 
	CONST_CAST(char *,XtNfontSelectCommand), 
	CONST_CAST(char *,XtCFontSelectCommand),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, font_select_command),
	XtRString,
	XtPointer("xfontsel -print")
    },

    { 
	CONST_CAST(char *,XtNvslDefs), 
	CONST_CAST(char *,XtCVSLDefs), 
	XtRString, 
	sizeof(String),
	XtOffsetOf(AppData, vsl_defs),
	XtRString, 
	XtPointer("")
    },

    { 
	CONST_CAST(char *,XtNvslBaseDefs), 
	CONST_CAST(char *,XtCVSLDefs), 
	XtRString, 
	sizeof(String),
	XtOffsetOf(AppData, vsl_base_defs),
	XtRString, 
	XtPointer("")
    },

    {
	CONST_CAST(char *,XtNtabWidth),
	CONST_CAST(char *,XtCTabWidth),
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, tab_width),
	XmRImmediate,
	XtPointer(DEFAULT_TAB_WIDTH)
    },

    {
	CONST_CAST(char *,XtNlinesAboveCursor),
	CONST_CAST(char *,XtCLinesAboveCursor),
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, lines_above_cursor),
	XmRImmediate,
	XtPointer(2)
    },

    {
	CONST_CAST(char *,XtNlinesBelowCursor),
	CONST_CAST(char *,XtCLinesBelowCursor),
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, lines_below_cursor),
	XmRImmediate,
	XtPointer(3)
    },

    {
	CONST_CAST(char *,XtNindentSource),
	CONST_CAST(char *,XtCIndent),
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, indent_source),
	XmRImmediate,
	XtPointer(4)
    },

    {
	CONST_CAST(char *,XtNindentScript),
	CONST_CAST(char *,XtCIndent),
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, indent_script),
	XmRImmediate,
	XtPointer(4)
    },

    {
	CONST_CAST(char *,XtNindentCode),
	CONST_CAST(char *,XtCIndent),
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, indent_code),
	XmRImmediate,
	XtPointer(4)
    },

    {
	CONST_CAST(char *,XtNlineNumberWidth),
	CONST_CAST(char *,XtCLineNumberWidth),
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, line_number_width),
	XmRImmediate,
	XtPointer(4)
    },

    {
	CONST_CAST(char *,XtNsourceEditing),
	CONST_CAST(char *,XtCSourceEditing),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, source_editing),
	XtRImmediate,
	XtPointer(False)
    },

    {
	CONST_CAST(char *,XtNlineBufferedConsole),
	CONST_CAST(char *,XtCLineBuffered),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, line_buffered_console),
	XtRImmediate,
	XtPointer(True)
    },

    {
	CONST_CAST(char *,XtNconsoleHasFocus),
	CONST_CAST(char *,XtCConsoleHasFocus),
	CONST_CAST(char *,XtROnOff),
	sizeof(OnOff),
	XtOffsetOf(AppData, console_has_focus),
	XmRImmediate,
	XtPointer(On)
    },

    {
	CONST_CAST(char *,XtNgdbDisplayShortcuts),
	CONST_CAST(char *,XtCDisplayShortcuts),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, gdb_display_shortcuts),
	XtRString,
	XtPointer("")
    },

    {
	CONST_CAST(char *,XtNdbxDisplayShortcuts),
	CONST_CAST(char *,XtCDisplayShortcuts),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, dbx_display_shortcuts),
	XtRString,
	XtPointer("")
    },

    {
	CONST_CAST(char *,XtNxdbDisplayShortcuts),
	CONST_CAST(char *,XtCDisplayShortcuts),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, xdb_display_shortcuts),
	XtRString,
	XtPointer("")
    },

    {
	CONST_CAST(char *,XtNjdbDisplayShortcuts),
	CONST_CAST(char *,XtCDisplayShortcuts),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, jdb_display_shortcuts),
	XtRString,
	XtPointer("")
    },

    {
	CONST_CAST(char *,XtNpydbDisplayShortcuts),
	CONST_CAST(char *,XtCDisplayShortcuts),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, pydb_display_shortcuts),
	XtRString,
	XtPointer("")
    },

    {
	CONST_CAST(char *,XtNperlDisplayShortcuts),
	CONST_CAST(char *,XtCDisplayShortcuts),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, perl_display_shortcuts),
	XtRString,
	XtPointer("")
    },

    {
	CONST_CAST(char *,XtNbashDisplayShortcuts),
	CONST_CAST(char *,XtCDisplayShortcuts),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, bash_display_shortcuts),
	XtRString,
	XtPointer("")
    },

    {
	CONST_CAST(char *,XtNconsoleButtons),
	CONST_CAST(char *,XtCButtons),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, console_buttons),
	XtRString,
	XtPointer("")
    },

    {
	CONST_CAST(char *,XtNsourceButtons),
	CONST_CAST(char *,XtCButtons),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, source_buttons),
	XtRString,
	XtPointer("")
    },

    {
	CONST_CAST(char *,XtNdataButtons),
	CONST_CAST(char *,XtCButtons),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, data_buttons),
	XtRString,
	XtPointer("")
    },

    {
	CONST_CAST(char *,XtNtoolButtons),
	CONST_CAST(char *,XtCButtons),
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
	CONST_CAST(char *,XtNlabelDelimiter),
	CONST_CAST(char *,XtCLabelDelimiter),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, label_delimiter),
	XtRString,
	XtPointer("//")
    },

    {
	CONST_CAST(char *,XtNcommandToolBar),
	CONST_CAST(char *,XtCToolBar),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, command_toolbar),
	XtRImmediate,
	XtPointer(False)
    },

    {
	CONST_CAST(char *,XtNcommonToolBar),
	CONST_CAST(char *,XtCToolBar),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, common_toolbar),
	XtRImmediate,
	XtPointer(True)
    },

    {
	CONST_CAST(char *,XtNseparateDataWindow),
	CONST_CAST(char *,XtCSeparate),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, separate_data_window),
	XtRImmediate,
	XtPointer(False)
    },

    {
	CONST_CAST(char *,XtNseparateSourceWindow),
	CONST_CAST(char *,XtCSeparate),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, separate_source_window),
	XtRImmediate,
	XtPointer(False)
    },

    {
	CONST_CAST(char *,XtNseparateExecWindow),
	CONST_CAST(char *,XtCSeparate),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, separate_exec_window),
	XtRImmediate,
	XtPointer(False)
    },

    {
	CONST_CAST(char *,XtNopenDataWindow),
	CONST_CAST(char *,XtCWindow),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, data_window),
	XtRImmediate,
	XtPointer(True)
    },

    {
	CONST_CAST(char *,XtNopenSourceWindow),
	CONST_CAST(char *,XtCWindow),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, source_window),
	XtRImmediate,
	XtPointer(True)
    },

    {
	CONST_CAST(char *,XtNopenDebuggerConsole),
	CONST_CAST(char *,XtCWindow),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, debugger_console),
	XtRImmediate,
	XtPointer(True)
    },

    {
	CONST_CAST(char *,XtNautoCloseDataWindow),
	CONST_CAST(char *,XtCAutoClose),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, auto_close_data_window),
	XtRImmediate,
	XtPointer(True)
    },

    {
	CONST_CAST(char *,XtNplotCommand),
	CONST_CAST(char *,XtCPlotCommand),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, plot_command),
	XtRString,
	XtPointer("gnuplot -name @NAME@")
    },

    {
	CONST_CAST(char *,XtNplotTermType),
	CONST_CAST(char *,XtCPlotTermType),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, plot_term_type),
	XtRString,
	XtPointer("xlib")
    },

    {
	CONST_CAST(char *,XtNplotWindowClass),
	CONST_CAST(char *,XtCPlotWindowClass),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, plot_window_class),
	XtRString,
	XtPointer("Gnuplot")
    },

    {
	CONST_CAST(char *,XtNplotWindowDelay),
	CONST_CAST(char *,XtCWindowDelay),
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, plot_window_delay),
	XtRImmediate,
	XtPointer(2000)
    },

    {
	CONST_CAST(char *,XtNplotInitCommands),
	CONST_CAST(char *,XtCPlotInitCommands),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, plot_init_commands),
	XtRString,
	XtPointer("set parametric")
    },

    {
	CONST_CAST(char *,XtNplot2dSettings),
	CONST_CAST(char *,XtCPlotSettings),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, plot_2d_settings),
	XtRString,
	XtPointer("")
    },

    {
	CONST_CAST(char *,XtNplot3dSettings),
	CONST_CAST(char *,XtCPlotSettings),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, plot_3d_settings),
	XtRString,
	XtPointer("")
    },

    {
	CONST_CAST(char *,XtNtermCommand),
	CONST_CAST(char *,XtCTermCommand),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, term_command),
	XtRString,
	XtPointer("xterm -e /bin/sh -c")
    },

    {
	CONST_CAST(char *,XtNtermType),
	CONST_CAST(char *,XtCTermType),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, term_type),
	XtRString,
	XtPointer("xterm")
   },

   {
	CONST_CAST(char *,XtNuseTTYCommand),
	CONST_CAST(char *,XtCUseTTYCommand),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, use_tty_command),
	XtRImmediate,
	XtPointer(False)
    },

    {
	CONST_CAST(char *,XtNquestionTimeout),
	CONST_CAST(char *,XtCQuestionTimeout),
	XmRInt,
	sizeof(int),
	XtOffsetOf(AppData, question_timeout),
	XtRImmediate,
	XtPointer(5)
    },

    {
	CONST_CAST(char *,XtNpositionTimeout),
	CONST_CAST(char *,XtCPositionTimeout),
	XmRInt,
	sizeof(int),
	XtOffsetOf(AppData, position_timeout),
	XtRImmediate,
	XtPointer(500)
    },

    {
	CONST_CAST(char *,XtNdisplayTimeout),
	CONST_CAST(char *,XtCDisplayTimeout),
	XmRInt,
	sizeof(int),
	XtOffsetOf(AppData, display_timeout),
	XtRImmediate,
	XtPointer(2000)
    },

    {
	CONST_CAST(char *,XtNsynchronousDebugger),
	CONST_CAST(char *,XtCSynchronousDebugger),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, synchronous_gdb),
	XtRImmediate,
	XtPointer(False)
    },

    {
	CONST_CAST(char *,XtNterminateOnEOF),
	CONST_CAST(char *,XtCTerminateOnEOF),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, terminate_on_eof),
	XtRImmediate,
	XtPointer(False)
    },

    {
	CONST_CAST(char *,XtNstopAndContinue),
	CONST_CAST(char *,XtCStopAndContinue),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, stop_and_continue),
	XtRImmediate,
	XtPointer(True)
    },

    {
	CONST_CAST(char *,XtNrunInterruptDelay),
	CONST_CAST(char *,XtCInterruptDelay),
	XmRInt,
	sizeof(int),
	XtOffsetOf(AppData, run_interrupt_delay),
	XtRImmediate,
	XtPointer(2000)
    },

    {
	CONST_CAST(char *,XtNcontInterruptDelay),
	CONST_CAST(char *,XtCInterruptDelay),
	XmRInt,
	sizeof(int),
	XtOffsetOf(AppData, cont_interrupt_delay),
	XtRImmediate,
	XtPointer(200)
    },

    {
	CONST_CAST(char *,XtNpollChildStatus),
	CONST_CAST(char *,XtCPollChildStatus),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, poll_child_status),
	XtRImmediate,
	XtPointer(True)
    },

    {
        CONST_CAST(char *,XtNdebuggerHost),
	CONST_CAST(char *,XtCDebuggerHost),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, debugger_host),
	XtRString,
	XtPointer("")
    },

    {
        CONST_CAST(char *,XtNdebuggerHostLogin),
	CONST_CAST(char *,XtCDebuggerHostLogin),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, debugger_host_login),
	XtRString,
	XtPointer("")
    },

    {
        CONST_CAST(char *,XtNrHostInitCommands),
	CONST_CAST(char *,XtCRHostInitCommands),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, rhost_init_commands),
	XtRString,
	XtPointer(STTY_COMMAND)
    },

    {
        CONST_CAST(char *,XtNdebuggerRHost),
	CONST_CAST(char *,XtCDebuggerRHost),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, debugger_rhost),
	XtRString,
	XtPointer("")
    },

    {
        CONST_CAST(char *,XtNrshCommand),
	CONST_CAST(char *,XtCRshCommand),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, rsh_command),
	XtRString,
	XtPointer("rsh")
    },

    {
	CONST_CAST(char *,XtNfilterFiles),
	CONST_CAST(char *,XtCFilterFiles),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, filter_files),
	XtRImmediate,
	XtPointer(True)
    },

    {
        CONST_CAST(char *,XtNlistExecCommand),
	CONST_CAST(char *,XtCListExecCommand),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, list_exec_command),
	XtRString,
	XtPointer("file @MASK@ | grep  '.*:.*exec.*' | cut -d: -f1")
    },

    {
        CONST_CAST(char *,XtNlistSourceCommand),
	CONST_CAST(char *,XtCListSourceCommand),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, list_source_command),
	XtRString,
	XtPointer("file @MASK@ | grep '.*:.*text.*' | cut -d: -f1")
    },

    {
        CONST_CAST(char *,XtNlistCoreCommand),
	CONST_CAST(char *,XtCListCoreCommand),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, list_core_command),
	XtRString,
	XtPointer("file @MASK@ | grep '.*:.*core.*' | cut -d: -f1")
    },

    {
        CONST_CAST(char *,XtNlistDirCommand),
	CONST_CAST(char *,XtCListDirCommand),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, list_dir_command),
	XtRString,
	XtPointer("file @MASK@ | grep '.*:.*directory.*' | cut -d: -f1")
    },

    {
        CONST_CAST(char *,XtNuncompressCommand),
	CONST_CAST(char *,XtCUncompressCommand),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, uncompress_command),
	XtRString,
	XtPointer("gzip -d -c")
    },

    {
        CONST_CAST(char *,XtNpsCommand),
	CONST_CAST(char *,XtCPsCommand),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, ps_command),
	XtRString,
	XtPointer("ps")
    },

    {
        CONST_CAST(char *,XtNwwwPage),
	CONST_CAST(char *,XtCWWWPage),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, www_page),
	XtRString,
	XtPointer("http://www.gnu.org/software/ddd/")
    },

    {
        CONST_CAST(char *,XtNwwwCommand),
	CONST_CAST(char *,XtCWWWCommand),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, www_command),
	XtRString,
	XtPointer("netscape @URL@")
    },

    {
        CONST_CAST(char *,XtNshowInvocation),
	CONST_CAST(char *,XtCShowInvocation),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, show_invocation),
	XtRImmediate,
	XtPointer(False)
    },

    {
        CONST_CAST(char *,XtNshowVersion),
	CONST_CAST(char *,XtCShowVersion),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, show_version),
	XtRImmediate,
	XtPointer(False)
    },

    {
        CONST_CAST(char *,XtNshowConfiguration),
	CONST_CAST(char *,XtCShowConfiguration),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, show_configuration),
	XtRImmediate,
	XtPointer(False)
    },

    {
        CONST_CAST(char *,XtNshowManual),
	CONST_CAST(char *,XtCShowManual),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, show_manual),
	XtRImmediate,
	XtPointer(False)
    },

    {
        CONST_CAST(char *,XtNshowLicense),
	CONST_CAST(char *,XtCShowLicense),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, show_license),
	XtRImmediate,
	XtPointer(False)
    },

    {
        CONST_CAST(char *,XtNshowNews),
	CONST_CAST(char *,XtCShowNews),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, show_news),
	XtRImmediate,
	XtPointer(False)
    },

    {
        CONST_CAST(char *,XtNshowFonts),
	CONST_CAST(char *,XtCShowFonts),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, show_fonts),
	XtRImmediate,
	XtPointer(False)
    },

    {
        CONST_CAST(char *,XtNcheckConfiguration),
	CONST_CAST(char *,XtCCheckConfiguration),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, check_configuration),
	XtRImmediate,
	XtPointer(False)
    },

    {
        CONST_CAST(char *,XtNprintCommand),
	CONST_CAST(char *,XtCPrintCommand),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, print_command),
	XtRString,
	XtPointer("lp")
    },

    {
        CONST_CAST(char *,XtNpaperSize),
	CONST_CAST(char *,XtCPaperSize),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, paper_size),
	XtRString,
	XtPointer("210mm x 297mm")
    },

    {
        CONST_CAST(char *,XtNgetCoreCommand),
	CONST_CAST(char *,XtCGetCoreCommand),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, get_core_command),
	XtRString,
	XtPointer("")
    },

    {
        CONST_CAST(char *,XtNeditCommand),
	CONST_CAST(char *,XtCEditCommand),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, edit_command),
	XtRString,
	XtPointer("xterm -e vi +@LINE@ @FILE@")
    },

    {
        CONST_CAST(char *,XtNpannedGraphEditor),
	CONST_CAST(char *,XtCPannedGraphEditor),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, panned_graph_editor),
	XtRImmediate,
	XtPointer(False)
    },

    {
        CONST_CAST(char *,XtNfindWordsOnly),
	CONST_CAST(char *,XtCFindWordsOnly),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, find_words_only),
	XtRImmediate,
	XtPointer(True)
    },

    {
        CONST_CAST(char *,XtNfindCaseSensitive),
	CONST_CAST(char *,XtCFindCaseSensitive),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, find_case_sensitive),
	XtRImmediate,
	XtPointer(True)
    },

    {
	CONST_CAST(char *,XtNgroupIconify),
	CONST_CAST(char *,XtCGroupIconify),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, group_iconify),
	XtRImmediate,
	XtPointer(False)
    },

    {
	CONST_CAST(char *,XtNuniconifyWhenReady),
	CONST_CAST(char *,XtCUniconifyWhenReady),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, uniconify_when_ready),
	XtRImmediate,
	XtPointer(True)
    },

    {
	CONST_CAST(char *,XtNtransientDialogs),
	CONST_CAST(char *,XtCTransientDialogs),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, transient_dialogs),
	XtRImmediate,
	XtPointer(True)
    },

    {
	CONST_CAST(char *,XtNglobalTabCompletion),
	CONST_CAST(char *,XtCGlobalTabCompletion),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, global_tab_completion),
	XtRImmediate,
	XtPointer(False)
    },

    {
	CONST_CAST(char *,XtNsaveHistoryOnExit),
	CONST_CAST(char *,XtCSaveOnExit),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, save_history_on_exit),
	XtRImmediate,
	XtPointer(True)
    },

    {
	CONST_CAST(char *,XtNsaveOptionsOnExit),
	CONST_CAST(char *,XtCSaveOnExit),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, save_options_on_exit),
	XtRImmediate,
	XtPointer(True)
    },

    {
	CONST_CAST(char *,XtNcacheSourceFiles),
	CONST_CAST(char *,XtCCacheSourceFiles),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, cache_source_files),
	XtRImmediate,
	XtPointer(True)
    },

    {
	CONST_CAST(char *,XtNcacheMachineCode),
	CONST_CAST(char *,XtCCacheMachineCode),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, cache_machine_code),
	XtRImmediate,
	XtPointer(True)
    },

    {
	CONST_CAST(char *,XtNsuppressWarnings),
	CONST_CAST(char *,XtCSuppressWarnings),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, suppress_warnings),
	XtRImmediate,
	XtPointer(False)
    },

    {
	CONST_CAST(char *,XtNwarnIfLocked),
	CONST_CAST(char *,XtCWarnIfLocked),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, warn_if_locked),
	XtRImmediate,
	XtPointer(False)
    },

    {
	CONST_CAST(char *,XtNcheckOptions),
	CONST_CAST(char *,XtCCheckOptions),
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, check_options),
	XtRImmediate,
	XtPointer(30)
    },

    {
	CONST_CAST(char *,XtNttyMode),
	CONST_CAST(char *,XtCTTYMode),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, tty_mode),
	XtRImmediate,
	XtPointer(False)
    },

    {
	CONST_CAST(char *,XtNblockTTYInput),
	CONST_CAST(char *,XtCBlockTTYInput),
	CONST_CAST(char *,XtROnOff),
	sizeof(OnOff),
	XtOffsetOf(AppData, block_tty_input),
	XmRImmediate,
	XtPointer(Auto)
    },

    {
	CONST_CAST(char *,XtNbufferGDBOutput),
	CONST_CAST(char *,XtCBufferGDBOutput),
	CONST_CAST(char *,XtROnOff),
	sizeof(OnOff),
	XtOffsetOf(AppData, buffer_gdb_output),
	XmRImmediate,
	XtPointer(Auto)
    },

    {
	CONST_CAST(char *,XtNannotate),
	CONST_CAST(char *,XtCAnnotate),
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, annotate),
	XtRImmediate,
	XtPointer(0)
    },

    {
	CONST_CAST(char *,XtNdisplayGlyphs),
	CONST_CAST(char *,XtCDisplayGlyphs),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, display_glyphs),
	XtRImmediate,
	XtPointer(True)
    },

    {
	CONST_CAST(char *,XtNmaxGlyphs),
	CONST_CAST(char *,XtCMaxGlyphs),
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, max_glyphs),
	XtRImmediate,
	XtPointer(10)
    },

    {
	CONST_CAST(char *,XtNcacheGlyphImages),
	CONST_CAST(char *,XtCCacheGlyphImages),
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
	CONST_CAST(char *,XtNglyphUpdateDelay),
	CONST_CAST(char *,XtCGlyphUpdateDelay),
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, glyph_update_delay),
	XtRImmediate,
	XtPointer(10)
    },

    {
	CONST_CAST(char *,XtNdisplayLineNumbers),
	CONST_CAST(char *,XtCDisplayLineNumbers),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, display_line_numbers),
	XtRImmediate,
	XtPointer(False)
    },

    {
	CONST_CAST(char *,XtNdisassemble),
	CONST_CAST(char *,XtCDisassemble),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, disassemble),
	XtRImmediate,
	XtPointer(False)
    },

    {
	CONST_CAST(char *,XtNmaxDisassemble),
	CONST_CAST(char *,XtCMaxDisassemble),
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, max_disassemble),
	XmRImmediate,
	XtPointer(1024)
    },

    {
	CONST_CAST(char *,XtNuseSourcePath),
	CONST_CAST(char *,XtCUseSourcePath),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, use_source_path),
	XtRImmediate,
	XtPointer(False)
    },

    {
	CONST_CAST(char *,XtNallRegisters),
	CONST_CAST(char *,XtCAllRegisters),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, all_registers),
	XtRImmediate,
	XtPointer(False)
    },

    {
	CONST_CAST(char *,XtNbuttonTips),
	CONST_CAST(char *,XtCTips),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, button_tips),
	XtRImmediate,
	XtPointer(True)
    },

    {
	CONST_CAST(char *,XtNvalueTips),
	CONST_CAST(char *,XtCTips),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, value_tips),
	XtRImmediate,
	XtPointer(True)
    },

    {
	CONST_CAST(char *,XtNbuttonDocs),
	CONST_CAST(char *,XtCDocs),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, button_docs),
	XtRImmediate,
	XtPointer(True)
    },

    {
	CONST_CAST(char *,XtNvalueDocs),
	CONST_CAST(char *,XtCDocs),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, value_docs),
	XtRImmediate,
	XtPointer(True)
    },

    {
	CONST_CAST(char *,XtNstartupTips),
	CONST_CAST(char *,XtCStartupTips),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, startup_tips),
	XtRImmediate,
	XtPointer(True)
    },

    {
	CONST_CAST(char *,XtNstartupTipCount),
	CONST_CAST(char *,XtCStartupTipCount),
	XmRInt,
	sizeof(int),
	XtOffsetOf(AppData, startup_tip_count),
	XtRImmediate,
	XtPointer(1)
    },

    {
	CONST_CAST(char *,XtNstatusAtBottom),
	CONST_CAST(char *,XtCStatusAtBottom),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, status_at_bottom),
	XtRImmediate,
	XtPointer(True)
    },

    {
	CONST_CAST(char *,XtNtoolbarsAtBottom),
	CONST_CAST(char *,XtCToolbarsAtBottom),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, toolbars_at_bottom),
	XtRImmediate,
	XtPointer(True)
    },

    {
	CONST_CAST(char *,XtNmaxDisplayTitleLength),
	CONST_CAST(char *,XtCMaxLength),
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, max_display_title_length),
	XmRImmediate,
	XtPointer(20)
    },

    {
	CONST_CAST(char *,XtNmaxPopupExprLength),
	CONST_CAST(char *,XtCMaxLength),
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, max_popup_expr_length),
	XmRImmediate,
	XtPointer(20)
    },

    {
	CONST_CAST(char *,XtNmaxValueTipLength),
	CONST_CAST(char *,XtCMaxLength),
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, max_value_tip_length),
	XmRImmediate,
	XtPointer(32)
    },

    {
	CONST_CAST(char *,XtNmaxValueDocLength),
	CONST_CAST(char *,XtCMaxLength),
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, max_value_doc_length),
	XmRImmediate,
	XtPointer(128)
    },

    {
	CONST_CAST(char *,XtNbuttonTipDelay),
	CONST_CAST(char *,XtCTipDelay),
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, button_tip_delay),
	XmRImmediate,
	XtPointer(750)
    },

    {
	CONST_CAST(char *,XtNvalueTipDelay),
	CONST_CAST(char *,XtCTipDelay),
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, value_tip_delay),
	XmRImmediate,
	XtPointer(750)
    },

    {
	CONST_CAST(char *,XtNbuttonDocDelay),
	CONST_CAST(char *,XtCDocDelay),
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, button_doc_delay),
	XmRImmediate,
	XtPointer(0)
    },

    {
	CONST_CAST(char *,XtNvalueDocDelay),
	CONST_CAST(char *,XtCDocDelay),
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, value_doc_delay),
	XmRImmediate,
	XtPointer(0)
    },

    {
	CONST_CAST(char *,XtNclearDocDelay),
	CONST_CAST(char *,XtCClearDelay),
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, clear_doc_delay),
	XmRImmediate,
	XtPointer(1000)
    },

    {
	CONST_CAST(char *,XtNclearTipDelay),
	CONST_CAST(char *,XtCClearDelay),
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, clear_tip_delay),
	XmRImmediate,
	XtPointer(50)
    },

    {
	CONST_CAST(char *,XtNblinkWhileBusy),
	CONST_CAST(char *,XtCBlinkWhileBusy),
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, blink_while_busy),
	XmRImmediate,
	XtPointer(True)
    },

    {
	CONST_CAST(char *,XtNbusyBlinkRate),
	CONST_CAST(char *,XmCBlinkRate),
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, busy_blink_rate),
	XmRImmediate,
	XtPointer(500)
    },

    {
	CONST_CAST(char *,XtNcheckGrabs),
	CONST_CAST(char *,XtCCheckGrabs),
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, check_grabs),
	XmRImmediate,
	XtPointer(True)
    },

    {
	CONST_CAST(char *,XtNcheckGrabDelay),
	CONST_CAST(char *,XtCCheckGrabDelay),
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, check_grab_delay),
	XmRImmediate,
	XtPointer(5000)
    },

    {
	CONST_CAST(char *,XtNgrabActionDelay),
	CONST_CAST(char *,XtCGrabActionDelay),
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, grab_action_delay),
	XmRImmediate,
	XtPointer(10000)
    },

    {
	CONST_CAST(char *,XtNgrabAction),
	CONST_CAST(char *,XtCGrabAction),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, grab_action),
	XmRImmediate,
	XtPointer("cont")
    },

    {
	CONST_CAST(char *,XtNdetectAliases),
	CONST_CAST(char *,XtCDetectAliases),
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, detect_aliases),
	XmRImmediate,
	XtPointer(False)
    },

    {
	CONST_CAST(char *,XtNtypedAliases),
	CONST_CAST(char *,XtCTypedAliases),
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, typed_aliases),
	XmRImmediate,
	XtPointer(True)
    },

    {
	CONST_CAST(char *,XtNdeleteAliasDisplays),
	CONST_CAST(char *,XtCDeleteAliasDisplays),
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, delete_alias_displays),
	XmRImmediate,
	XtPointer(True)
    },

    {
	CONST_CAST(char *,XtNalign2dArrays),
	CONST_CAST(char *,XtCAlign2dArrays),
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, align_2d_arrays),
	XmRImmediate,
	XtPointer(True)
    },

    {
	CONST_CAST(char *,XtNarrayOrientation),
	CONST_CAST(char *,XtCOrientation),
	XtROrientation,
	sizeof(unsigned char),
	XtOffsetOf(AppData, array_orientation),
	XmRImmediate,
	XtPointer(XmVERTICAL)
    },

    {
	CONST_CAST(char *,XtNstructOrientation),
	CONST_CAST(char *,XtCOrientation),
	XtROrientation,
	sizeof(unsigned char),
	XtOffsetOf(AppData, struct_orientation),
	XmRImmediate,
	XtPointer(XmVERTICAL)
    },

    {
	CONST_CAST(char *,XtNshowMemberNames),
	CONST_CAST(char *,XtCShowMemberNames),
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, show_member_names),
	XmRImmediate,
	XtPointer(True)
    },

    {
	CONST_CAST(char *,XtNexpandRepeatedValues),
	CONST_CAST(char *,XtCExpandRepeatedValues),
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, expand_repeated_values),
	XmRImmediate,
	XtPointer(False)
    },

    {
	CONST_CAST(char *,XtNbumpDisplays),
	CONST_CAST(char *,XtCBumpDisplays),
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, bump_displays),
	XmRImmediate,
	XtPointer(True)
    },

    {
	CONST_CAST(char *,XtNhideInactiveDisplays),
	CONST_CAST(char *,XtCHideInactiveDisplays),
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, hide_inactive_displays),
	XmRImmediate,
	XtPointer(True)
    },

    {
	CONST_CAST(char *,XtNshowBaseDisplayTitles),
	CONST_CAST(char *,XtCShowDisplayTitles),
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, show_base_display_titles),
	XmRImmediate,
	XtPointer(True)
    },

    {
	CONST_CAST(char *,XtNshowDependentDisplayTitles),
	CONST_CAST(char *,XtCShowDisplayTitles),
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, show_dependent_display_titles),
	XmRImmediate,
	XtPointer(True)
    },

    {
	CONST_CAST(char *,XtNclusterDisplays),
	CONST_CAST(char *,XtCClusterDisplays),
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, cluster_displays),
	XmRImmediate,
	XtPointer(False)
    },

    {
	CONST_CAST(char *,XtNdisplayPlacement),
	CONST_CAST(char *,XtCOrientation),
	XtROrientation,
	sizeof(unsigned char),
	XtOffsetOf(AppData, display_placement),
	XmRImmediate,
	XtPointer(XmVERTICAL)
    },

    {
	CONST_CAST(char *,XtNstatusHistorySize),
	CONST_CAST(char *,XtCHistorySize),
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, status_history_size),
	XmRImmediate,
	XtPointer(10)
    },

    {
	CONST_CAST(char *,XtNpopdownHistorySize),
	CONST_CAST(char *,XtCHistorySize),
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, popdown_history_size),
	XmRImmediate,
	XtPointer(0)
    },

    {
	CONST_CAST(char *,XtNsortPopdownHistory),
	CONST_CAST(char *,XtCSortPopdownHistory),
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, sort_popdown_history),
	XmRImmediate,
	XtPointer(True)
    },

    {
	CONST_CAST(char *,XtNverifyButtons),
	CONST_CAST(char *,XtCVerifyButtons),
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, verify_buttons),
	XmRImmediate,
	XtPointer(True)
    },

    {
	CONST_CAST(char *,XtNautoRaiseTool),
	CONST_CAST(char *,XtCAutoRaiseTool),
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, auto_raise_tool),
	XmRImmediate,
	XtPointer(True)
    },

    {
	CONST_CAST(char *,XtNdecorateTool),
	CONST_CAST(char *,XtCDecorate),
	CONST_CAST(char *,XtROnOff),
	sizeof(OnOff),
	XtOffsetOf(AppData, decorate_tool),
	XmRImmediate,
	XtPointer(Auto)
    },

    {
	CONST_CAST(char *,XtNstickyTool),
	CONST_CAST(char *,XtCStickyTool),
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, sticky_tool),
	XmRImmediate,
	XtPointer(True)
    },

    {
	CONST_CAST(char *,XtNtoolRightOffset),
	CONST_CAST(char *,XtCToolOffset),
	XmRPosition,
	sizeof(Position),
	XtOffsetOf(AppData, tool_right_offset),
	XmRImmediate,
	XtPointer(8)
    },

    {
	CONST_CAST(char *,XtNtoolTopOffset),
	CONST_CAST(char *,XtCToolOffset),
	XmRPosition,
	sizeof(Position),
	XtOffsetOf(AppData, tool_top_offset),
	XmRImmediate,
	XtPointer(8)
    },

    {
	CONST_CAST(char *,XtNbuttonCaptions),
	CONST_CAST(char *,XtCButtonCaptions),
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, button_captions),
	XmRImmediate,
	XtPointer(True)
    },

    {
	CONST_CAST(char *,XtNbuttonImages),
	CONST_CAST(char *,XtCButtonImages),
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, button_images),
	XmRImmediate,
	XtPointer(True)
    },

    {
	CONST_CAST(char *,XtNbuttonCaptionGeometry),
	CONST_CAST(char *,XtCButtonCaptionGeometry),
	XmRString,
	sizeof(String),
	XtOffsetOf(AppData, button_caption_geometry),
	XmRImmediate,
	XtPointer("29x7+0-0")
    },

    {
	CONST_CAST(char *,XtNbuttonImageGeometry),
	CONST_CAST(char *,XtCButtonImageGeometry),
	XmRString,
	sizeof(String),
	XtOffsetOf(AppData, button_image_geometry),
	XmRImmediate,
	XtPointer("21x17+4+0")
    },

    {
	CONST_CAST(char *,XtNflatToolbarButtons),
	CONST_CAST(char *,XtCFlatButtons),
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, flat_toolbar_buttons),
	XmRImmediate,
	XtPointer(True)
    },

    {
	CONST_CAST(char *,XtNflatDialogButtons),
	CONST_CAST(char *,XtCFlatButtons),
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, flat_dialog_buttons),
	XmRImmediate,
	XtPointer(False)
    },

    {
	CONST_CAST(char *,XtNbuttonColorKey),
	CONST_CAST(char *,XtCColorKey),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, button_color_key),
	XtRString,
	XtPointer("g")
    },

    {
	CONST_CAST(char *,XtNactiveButtonColorKey),
	CONST_CAST(char *,XtCColorKey),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, active_button_color_key),
	XtRString,
	XtPointer("c")
    },

    {
	CONST_CAST(char *,XtNsplashScreenColorKey),
	CONST_CAST(char *,XtCColorKey),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, splash_screen_color_key),
	XtRString,
	XtPointer("c")
    },

    {
	CONST_CAST(char *,XtNautoCommands),
	CONST_CAST(char *,XtCAutoCommands),
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, auto_commands),
	XmRImmediate,
	XtPointer(False)
    },

    {
	CONST_CAST(char *,XtNautoCommandPrefix),
	CONST_CAST(char *,XtCAutoCommandPrefix),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, auto_command_prefix),
	XtRString,
	XtPointer(ddd_NAME ": ")
    },

    {
	CONST_CAST(char *,XtNmaxBreakpointNumber),
	CONST_CAST(char *,XtCMaxNumber),
	XmRInt,
	sizeof(int),
	XtOffsetOf(AppData, max_breakpoint_number),
	XmRImmediate,
	XtPointer(99)
    },

    {
	CONST_CAST(char *,XtNmaxDisplayNumber),
	CONST_CAST(char *,XtCMaxNumber),
	XmRInt,
	sizeof(int),
	XtOffsetOf(AppData, max_display_number),
	XmRImmediate,
	XtPointer(99)
    },

    {
	CONST_CAST(char *,XtNsplashScreen),
	CONST_CAST(char *,XtCSplashScreen),
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, splash_screen),
	XmRImmediate,
	XtPointer(True)
    },

    {
	CONST_CAST(char *,XtNcolorWMIcons),
	CONST_CAST(char *,XtCColorWMIcons),
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, color_wm_icons),
	XmRImmediate,
	XtPointer(True)
    },

    {
	CONST_CAST(char *,XtNcutCopyPasteBindings),
	CONST_CAST(char *,XtCBindingStyle),
	CONST_CAST(char *,XtRBindingStyle),
	sizeof(BindingStyle),
	XtOffsetOf(AppData, cut_copy_paste_bindings),
	XmRImmediate,
	XtPointer(KDEBindings)
    },

    {
	CONST_CAST(char *,XtNselectAllBindings),
	CONST_CAST(char *,XtCBindingStyle),
	CONST_CAST(char *,XtRBindingStyle),
	sizeof(BindingStyle),
	XtOffsetOf(AppData, select_all_bindings),
	XmRImmediate,
	XtPointer(KDEBindings)
    },

    {
	CONST_CAST(char *,XtNmaxUndoDepth),
	CONST_CAST(char *,XtCMaxUndoDepth),
	XmRInt,
	sizeof(int),
	XtOffsetOf(AppData, max_undo_depth),
	XmRImmediate,
	XtPointer(-1)
    },

    {
	CONST_CAST(char *,XtNmaxUndoSize),
	CONST_CAST(char *,XtCMaxUndoSize),
	XmRInt,
	sizeof(int),
	XtOffsetOf(AppData, max_undo_size),
	XmRImmediate,
	XtPointer(2000000)
    },

    {
	CONST_CAST(char *,XtNthemes),
	CONST_CAST(char *,XtCThemes),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, themes),
	XtRString,
	XtPointer("")
    },

    {
	CONST_CAST(char *,XtNsuppressTheme),
	CONST_CAST(char *,XtCTheme),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, suppress_theme),
	XtRString,
	XtPointer("suppress.vsl")
    },

    {
	CONST_CAST(char *,XtNmaintenance),
	CONST_CAST(char *,XtCMaintenance),
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, maintenance),
	XmRImmediate,
	XtPointer(False)
    },

    {
	CONST_CAST(char *,XtNdumpCore),
	CONST_CAST(char *,XtCDumpCore),
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, dump_core),
	XmRImmediate,
	XtPointer(True)
    },

    {
	CONST_CAST(char *,XtNdebugCoreDumps),
	CONST_CAST(char *,XtCDebugCoreDumps),
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, debug_core_dumps),
	XmRImmediate,
	XtPointer(False)
    },

    {
	CONST_CAST(char *,XtNlessTifVersion),
	CONST_CAST(char *,XtCLessTifVersion),
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
	CONST_CAST(char *,XtNdddinitVersion),
	CONST_CAST(char *,XtCVersion),
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, dddinit_version),
	XtRString,
	XtPointer(0)
    },

    {
	CONST_CAST(char *,XtNappDefaultsVersion),
	CONST_CAST(char *,XtCVersion),
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
String ddd_fallback_resources[] = {
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

    if (own_app_defaults_file != "")
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
