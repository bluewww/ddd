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

#include <Xm/Xm.h>

// Application resource definitions
XtResource ddd_resources[] = {
    {
	(char *)XtNsession,
	(char *)XtCSessionID,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, session),
	XtRString, 
	XtPointer(NULL)
    },

    {
	(char *)XtNinitialSession,
	(char *)XtCSessionID,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, initial_session),
	XtRString, 
	XtPointer(NULL)
    },

    { 
	(char *)XtNdebugger,
	(char *)XtCDebugger,
	XtRString, 
	sizeof(String),
	XtOffsetOf(AppData, debugger), 
	XtRString, 
	XtPointer("")
    },

    { 
	(char *)XtNdebuggerCommand,
	(char *)XtCDebuggerCommand,
	XtRString, 
	sizeof(String),
	XtOffsetOf(AppData, debugger_command), 
	XtRString, 
	XtPointer("")
    },

    { 
	(char *)XtNautoDebugger, 
	(char *)XtCAutoDebugger, 
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, auto_debugger),
	XmRImmediate,
	XtPointer(True)
    },

    { 
	(char *)XtNopenSelection, 
	(char *)XtCOpenSelection, 
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, open_selection),
	XmRImmediate,
	XtPointer(False)
    },

    { 
	(char *)XtNtrace, 
	(char *)XtCTrace, 
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, trace),
	XmRImmediate, 
	XtPointer(False)
    },

    { 
	(char *)XtNplayLog,
	(char *)XtCPlayLog,
	XtRString, 
	sizeof(String),
	XtOffsetOf(AppData, play_log), 
	XtRString, 
	XtPointer(NULL)
    },

    { 
	(char *)XtNgdbInitCommands, 
	(char *)XtCInitCommands, 
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
	(char *)XtNgdbSettings, 
	(char *)XtCSettings, 
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, gdb_settings), 
	XtRString,
	XtPointer("")
    },

    { 
	(char *)XtNdbxInitCommands, 
	(char *)XtCInitCommands, 
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, dbx_init_commands), 
	XtRString,
	XtPointer("")
    },

    { 
	(char *)XtNdbxSettings, 
	(char *)XtCSettings, 
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, dbx_settings), 
	XtRString,
	XtPointer("")
    },

    { 
	(char *)XtNxdbInitCommands, 
	(char *)XtCInitCommands, 
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, xdb_init_commands), 
	XtRString,
	XtPointer("")
    },

    { 
	(char *)XtNxdbSettings, 
	(char *)XtCSettings, 
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, xdb_settings), 
	XtRString,
	XtPointer("")
    },

    { 
	(char *)XtNjdbInitCommands, 
	(char *)XtCInitCommands, 
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, jdb_init_commands), 
	XtRString,
	XtPointer("")
    },

    { 
	(char *)XtNjdbSettings, 
	(char *)XtCSettings, 
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, jdb_settings), 
	XtRString,
	XtPointer("")
    },

    { 
	(char *)XtNpydbInitCommands, 
	(char *)XtCInitCommands, 
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, pydb_init_commands), 
	XtRString,
	XtPointer("import os\n")
    },

    { 
	(char *)XtNpydbSettings, 
	(char *)XtCSettings, 
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, pydb_settings), 
	XtRString,
	XtPointer("")
    },

    { 
	(char *)XtNperlInitCommands, 
	(char *)XtCInitCommands, 
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, perl_init_commands), 
	XtRString,
	XtPointer("")
    },

    { 
	(char *)XtNperlSettings, 
	(char *)XtCSettings, 
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, perl_settings),
	XtRString,
	XtPointer("")
    },

    {
	(char *)XtNrestartCommands,
	(char *)XtCInitCommands,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, restart_commands), 
	XtRString,
	XtPointer("")
    },

    {
	(char *)XtNsourceInitCommands,
	(char *)XtCSourceInitCommands,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, source_init_commands),
	XtRImmediate,
	XtPointer(False)
    },

    { 
	(char *)XtNinitSymbols,
	(char *)XtCInitSymbols,
	XtRString, 
	sizeof(String),
	XtOffsetOf(AppData, init_symbols),
	XtRString, 
	XtPointer("main:MAIN__")
    },

    { 
	(char *)XtNvslPath, 
	(char *)XtCVSLPath, 
	XtRString, 
	sizeof(String),
	XtOffsetOf(AppData, vsl_path),
	XtRString, 
	XtPointer("user_themes:ddd_themes:.")
    },

    { 
	(char *)XtNvslLibrary, 
	(char *)XtCVSLLibrary, 
	XtRString, 
	sizeof(String),
	XtOffsetOf(AppData, vsl_library), 
	XtRString, 
	XtPointer("builtin")
    },

    { 
	(char *)XtNdefaultFont, 
	(char *)XtCFont, 
	XtRString, 
	sizeof(String),
	XtOffsetOf(AppData, default_font),
	XtRString, 
	XtPointer("-*-helvetica-bold-r-*")
    },

    { 
	(char *)XtNvariableWidthFont, 
	(char *)XtCFont,
	XtRString, 
	sizeof(String),
	XtOffsetOf(AppData, variable_width_font),
	XtRString,
	XtPointer("-*-helvetica-medium-r-*")
    },

    { 
	(char *)XtNfixedWidthFont, 
	XtCFont,
	XtRString, 
	sizeof(String),
	XtOffsetOf(AppData, fixed_width_font),
	XtRString,
	XtPointer("-*-courier-medium-r-*")
    },

    { 
	(char *)XtNdataFont, 
	XtCFont,
	XtRString, 
	sizeof(String),
	XtOffsetOf(AppData, data_font),
	XtRString,
	XtPointer("-*-courier-medium-r-*")
    },

    {
	(char *)XtNdefaultFontSize,
	(char *)XtCFontSize,
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, default_font_size),
	XmRImmediate,
	XtPointer(120)
    },

    {
	(char *)XtNvariableWidthFontSize,
	(char *)XtCFontSize,
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, variable_width_font_size),
	XmRImmediate,
	XtPointer(120)
    },

    {
	(char *)XtNfixedWidthFontSize,
	(char *)XtCFontSize,
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, fixed_width_font_size),
	XmRImmediate,
	XtPointer(120)
    },

    {
	(char *)XtNdataFontSize,
	(char *)XtCFontSize,
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, data_font_size),
	XmRImmediate,
	XtPointer(120)
    },

    { 
	(char *)XtNfontSelectCommand, 
	(char *)XtCFontSelectCommand,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, font_select_command),
	XtRString,
	XtPointer("xfontsel -print")
    },

    { 
	(char *)XtNvslDefs, 
	(char *)XtCVSLDefs, 
	XtRString, 
	sizeof(String),
	XtOffsetOf(AppData, vsl_defs),
	XtRString, 
	XtPointer("")
    },

    { 
	(char *)XtNvslBaseDefs, 
	(char *)XtCVSLDefs, 
	XtRString, 
	sizeof(String),
	XtOffsetOf(AppData, vsl_base_defs),
	XtRString, 
	XtPointer("")
    },

    {
	(char *)XtNtabWidth,
	(char *)XtCTabWidth,
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, tab_width),
	XmRImmediate,
	XtPointer(8)
    },

    {
	(char *)XtNlinesAboveCursor,
	(char *)XtCLinesAboveCursor,
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, lines_above_cursor),
	XmRImmediate,
	XtPointer(2)
    },

    {
	(char *)XtNlinesBelowCursor,
	(char *)XtCLinesBelowCursor,
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, lines_below_cursor),
	XmRImmediate,
	XtPointer(3)
    },

    {
	(char *)XtNindentSource,
	(char *)XtCIndent,
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, indent_source),
	XmRImmediate,
	XtPointer(4)
    },

    {
	(char *)XtNindentScript,
	(char *)XtCIndent,
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, indent_script),
	XmRImmediate,
	XtPointer(4)
    },

    {
	(char *)XtNindentCode,
	(char *)XtCIndent,
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, indent_code),
	XmRImmediate,
	XtPointer(4)
    },

    {
	(char *)XtNlineNumberWidth,
	(char *)XtCLineNumberWidth,
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, line_number_width),
	XmRImmediate,
	XtPointer(4)
    },

    {
	(char *)XtNsourceEditing,
	(char *)XtCSourceEditing,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, source_editing),
	XtRImmediate,
	XtPointer(False)
    },

    {
	(char *)XtNlineBufferedConsole,
	(char *)XtCLineBuffered,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, line_buffered_console),
	XtRImmediate,
	XtPointer(True)
    },

    {
	(char *)XtNconsoleHasFocus,
	(char *)XtCConsoleHasFocus,
	(char *)XtROnOff,
	sizeof(OnOff),
	XtOffsetOf(AppData, console_has_focus),
	XmRImmediate,
	XtPointer(On)
    },

    {
	(char *)XtNgdbDisplayShortcuts,
	(char *)XtCDisplayShortcuts,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, gdb_display_shortcuts),
	XtRString,
	XtPointer("")
    },

    {
	(char *)XtNdbxDisplayShortcuts,
	(char *)XtCDisplayShortcuts,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, dbx_display_shortcuts),
	XtRString,
	XtPointer("")
    },

    {
	(char *)XtNxdbDisplayShortcuts,
	(char *)XtCDisplayShortcuts,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, xdb_display_shortcuts),
	XtRString,
	XtPointer("")
    },

    {
	(char *)XtNjdbDisplayShortcuts,
	(char *)XtCDisplayShortcuts,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, jdb_display_shortcuts),
	XtRString,
	XtPointer("")
    },

    {
	(char *)XtNpydbDisplayShortcuts,
	(char *)XtCDisplayShortcuts,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, pydb_display_shortcuts),
	XtRString,
	XtPointer("")
    },

    {
	(char *)XtNperlDisplayShortcuts,
	(char *)XtCDisplayShortcuts,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, perl_display_shortcuts),
	XtRString,
	XtPointer("")
    },

    {
	(char *)XtNconsoleButtons,
	(char *)XtCButtons,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, console_buttons),
	XtRString,
	XtPointer("")
    },

    {
	(char *)XtNsourceButtons,
	(char *)XtCButtons,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, source_buttons),
	XtRString,
	XtPointer("")
    },

    {
	(char *)XtNdataButtons,
	(char *)XtCButtons,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, data_buttons),
	XtRString,
	XtPointer("")
    },

    {
	(char *)XtNtoolButtons,
	(char *)XtCButtons,
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
	(char *)XtNlabelDelimiter,
	(char *)XtCLabelDelimiter,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, label_delimiter),
	XtRString,
	XtPointer("//")
    },

    {
	(char *)XtNcommandToolBar,
	(char *)XtCToolBar,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, command_toolbar),
	XtRImmediate,
	XtPointer(False)
    },

    {
	(char *)XtNcommonToolBar,
	(char *)XtCToolBar,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, common_toolbar),
	XtRImmediate,
	XtPointer(True)
    },

    {
	(char *)XtNseparateDataWindow,
	(char *)XtCSeparate,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, separate_data_window),
	XtRImmediate,
	XtPointer(False)
    },

    {
	(char *)XtNseparateSourceWindow,
	(char *)XtCSeparate,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, separate_source_window),
	XtRImmediate,
	XtPointer(False)
    },

    {
	(char *)XtNseparateExecWindow,
	(char *)XtCSeparate,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, separate_exec_window),
	XtRImmediate,
	XtPointer(False)
    },

    {
	(char *)XtNopenDataWindow,
	(char *)XtCWindow,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, data_window),
	XtRImmediate,
	XtPointer(True)
    },

    {
	(char *)XtNopenSourceWindow,
	(char *)XtCWindow,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, source_window),
	XtRImmediate,
	XtPointer(True)
    },

    {
	(char *)XtNopenDebuggerConsole,
	(char *)XtCWindow,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, debugger_console),
	XtRImmediate,
	XtPointer(True)
    },

    {
	(char *)XtNautoCloseDataWindow,
	(char *)XtCAutoClose,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, auto_close_data_window),
	XtRImmediate,
	XtPointer(True)
    },

    {
	(char *)XtNplotCommand,
	(char *)XtCPlotCommand,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, plot_command),
	XtRString,
	XtPointer("gnuplot -name @NAME@")
    },

    {
	(char *)XtNplotTermType,
	(char *)XtCPlotTermType,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, plot_term_type),
	XtRString,
	XtPointer("xlib")
    },

    {
	(char *)XtNplotWindowClass,
	(char *)XtCPlotWindowClass,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, plot_window_class),
	XtRString,
	XtPointer("Gnuplot")
    },

    {
	(char *)XtNplotWindowDelay,
	(char *)XtCWindowDelay,
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, plot_window_delay),
	XtRImmediate,
	XtPointer(2000)
    },

    {
	(char *)XtNplotInitCommands,
	(char *)XtCPlotInitCommands,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, plot_init_commands),
	XtRString,
	XtPointer("set parametric")
    },

    {
	(char *)XtNplot2dSettings,
	(char *)XtCPlotSettings,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, plot_2d_settings),
	XtRString,
	XtPointer("")
    },

    {
	(char *)XtNplot3dSettings,
	(char *)XtCPlotSettings,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, plot_3d_settings),
	XtRString,
	XtPointer("")
    },

    {
	(char *)XtNtermCommand,
	(char *)XtCTermCommand,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, term_command),
	XtRString,
	XtPointer("xterm -e /bin/sh -c")
    },

    {
	(char *)XtNtermType,
	(char *)XtCTermType,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, term_type),
	XtRString,
	XtPointer("xterm")
   },

   {
	(char *)XtNuseTTYCommand,
	(char *)XtCUseTTYCommand,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, use_tty_command),
	XtRImmediate,
	XtPointer(False)
    },

    {
	(char *)XtNquestionTimeout,
	(char *)XtCQuestionTimeout,
	XmRInt,
	sizeof(int),
	XtOffsetOf(AppData, question_timeout),
	XtRImmediate,
	XtPointer(5)
    },

    {
	(char *)XtNpositionTimeout,
	(char *)XtCPositionTimeout,
	XmRInt,
	sizeof(int),
	XtOffsetOf(AppData, position_timeout),
	XtRImmediate,
	XtPointer(500)
    },

    {
	(char *)XtNdisplayTimeout,
	(char *)XtCDisplayTimeout,
	XmRInt,
	sizeof(int),
	XtOffsetOf(AppData, display_timeout),
	XtRImmediate,
	XtPointer(2000)
    },

    {
	(char *)XtNsynchronousDebugger,
	(char *)XtCSynchronousDebugger,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, synchronous_gdb),
	XtRImmediate,
	XtPointer(False)
    },

    {
	(char *)XtNterminateOnEOF,
	(char *)XtCTerminateOnEOF,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, terminate_on_eof),
	XtRImmediate,
	XtPointer(False)
    },

    {
	(char *)XtNstopAndContinue,
	(char *)XtCStopAndContinue,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, stop_and_continue),
	XtRImmediate,
	XtPointer(True)
    },

    {
	(char *)XtNrunInterruptDelay,
	(char *)XtCInterruptDelay,
	XmRInt,
	sizeof(int),
	XtOffsetOf(AppData, run_interrupt_delay),
	XtRImmediate,
	XtPointer(2000)
    },

    {
	(char *)XtNcontInterruptDelay,
	(char *)XtCInterruptDelay,
	XmRInt,
	sizeof(int),
	XtOffsetOf(AppData, cont_interrupt_delay),
	XtRImmediate,
	XtPointer(200)
    },

    {
	(char *)XtNpollChildStatus,
	(char *)XtCPollChildStatus,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, poll_child_status),
	XtRImmediate,
	XtPointer(True)
    },

    {
        (char *)XtNdebuggerHost,
	(char *)XtCDebuggerHost,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, debugger_host),
	XtRString,
	XtPointer("")
    },

    {
        (char *)XtNdebuggerHostLogin,
	(char *)XtCDebuggerHostLogin,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, debugger_host_login),
	XtRString,
	XtPointer("")
    },

    {
        (char *)XtNrHostInitCommands,
	(char *)XtCRHostInitCommands,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, rhost_init_commands),
	XtRString,
	XtPointer(STTY_COMMAND)
    },

    {
        (char *)XtNdebuggerRHost,
	(char *)XtCDebuggerRHost,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, debugger_rhost),
	XtRString,
	XtPointer("")
    },

    {
        (char *)XtNrshCommand,
	(char *)XtCRshCommand,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, rsh_command),
	XtRString,
	XtPointer("rsh")
    },

    {
	(char *)XtNfilterFiles,
	(char *)XtCFilterFiles,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, filter_files),
	XtRImmediate,
	XtPointer(True)
    },

    {
        (char *)XtNlistExecCommand,
	(char *)XtCListExecCommand,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, list_exec_command),
	XtRString,
	XtPointer("file @MASK@ | grep  '.*:.*exec.*' | cut -d: -f1")
    },

    {
        (char *)XtNlistSourceCommand,
	(char *)XtCListSourceCommand,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, list_source_command),
	XtRString,
	XtPointer("file @MASK@ | grep '.*:.*text.*' | cut -d: -f1")
    },

    {
        (char *)XtNlistCoreCommand,
	(char *)XtCListCoreCommand,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, list_core_command),
	XtRString,
	XtPointer("file @MASK@ | grep '.*:.*core.*' | cut -d: -f1")
    },

    {
        (char *)XtNlistDirCommand,
	(char *)XtCListDirCommand,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, list_dir_command),
	XtRString,
	XtPointer("file @MASK@ | grep '.*:.*directory.*' | cut -d: -f1")
    },

    {
        (char *)XtNuncompressCommand,
	(char *)XtCUncompressCommand,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, uncompress_command),
	XtRString,
	XtPointer("gzip -d -c")
    },

    {
        (char *)XtNpsCommand,
	(char *)XtCPsCommand,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, ps_command),
	XtRString,
	XtPointer("ps")
    },

    {
        (char *)XtNwwwPage,
	(char *)XtCWWWPage,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, www_page),
	XtRString,
	XtPointer("http://www.gnu.org/software/ddd/")
    },

    {
        (char *)XtNwwwCommand,
	(char *)XtCWWWCommand,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, www_command),
	XtRString,
	XtPointer("netscape @URL@")
    },

    {
        (char *)XtNshowInvocation,
	(char *)XtCShowInvocation,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, show_invocation),
	XtRImmediate,
	XtPointer(False)
    },

    {
        (char *)XtNshowVersion,
	(char *)XtCShowVersion,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, show_version),
	XtRImmediate,
	XtPointer(False)
    },

    {
        (char *)XtNshowConfiguration,
	(char *)XtCShowConfiguration,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, show_configuration),
	XtRImmediate,
	XtPointer(False)
    },

    {
        (char *)XtNshowManual,
	(char *)XtCShowManual,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, show_manual),
	XtRImmediate,
	XtPointer(False)
    },

    {
        (char *)XtNshowLicense,
	(char *)XtCShowLicense,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, show_license),
	XtRImmediate,
	XtPointer(False)
    },

    {
        (char *)XtNshowNews,
	(char *)XtCShowNews,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, show_news),
	XtRImmediate,
	XtPointer(False)
    },

    {
        (char *)XtNshowFonts,
	(char *)XtCShowFonts,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, show_fonts),
	XtRImmediate,
	XtPointer(False)
    },

    {
        (char *)XtNcheckConfiguration,
	(char *)XtCCheckConfiguration,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, check_configuration),
	XtRImmediate,
	XtPointer(False)
    },

    {
        (char *)XtNprintCommand,
	(char *)XtCPrintCommand,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, print_command),
	XtRString,
	XtPointer("lp")
    },

    {
        (char *)XtNpaperSize,
	(char *)XtCPaperSize,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, paper_size),
	XtRString,
	XtPointer("210mm x 297mm")
    },

    {
        (char *)XtNgetCoreCommand,
	(char *)XtCGetCoreCommand,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, get_core_command),
	XtRString,
	XtPointer("")
    },

    {
        (char *)XtNeditCommand,
	(char *)XtCEditCommand,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, edit_command),
	XtRString,
	XtPointer("xterm -e vi +@LINE@ @FILE@")
    },

    {
        (char *)XtNpannedGraphEditor,
	(char *)XtCPannedGraphEditor,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, panned_graph_editor),
	XtRImmediate,
	XtPointer(False)
    },

    {
        (char *)XtNfindWordsOnly,
	(char *)XtCFindWordsOnly,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, find_words_only),
	XtRImmediate,
	XtPointer(True)
    },

    {
        (char *)XtNfindCaseSensitive,
	(char *)XtCFindCaseSensitive,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, find_case_sensitive),
	XtRImmediate,
	XtPointer(True)
    },

    {
	(char *)XtNgroupIconify,
	(char *)XtCGroupIconify,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, group_iconify),
	XtRImmediate,
	XtPointer(False)
    },

    {
	(char *)XtNuniconifyWhenReady,
	(char *)XtCUniconifyWhenReady,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, uniconify_when_ready),
	XtRImmediate,
	XtPointer(True)
    },

    {
	(char *)XtNtransientDialogs,
	(char *)XtCTransientDialogs,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, transient_dialogs),
	XtRImmediate,
	XtPointer(True)
    },

    {
	(char *)XtNglobalTabCompletion,
	(char *)XtCGlobalTabCompletion,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, global_tab_completion),
	XtRImmediate,
	XtPointer(False)
    },

    {
	(char *)XtNsaveHistoryOnExit,
	(char *)XtCSaveHistoryOnExit,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, save_history_on_exit),
	XtRImmediate,
	XtPointer(True)
    },

    {
	(char *)XtNcacheSourceFiles,
	(char *)XtCCacheSourceFiles,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, cache_source_files),
	XtRImmediate,
	XtPointer(True)
    },

    {
	(char *)XtNcacheMachineCode,
	(char *)XtCCacheMachineCode,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, cache_machine_code),
	XtRImmediate,
	XtPointer(True)
    },

    {
	(char *)XtNsuppressWarnings,
	(char *)XtCSuppressWarnings,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, suppress_warnings),
	XtRImmediate,
	XtPointer(False)
    },

    {
	(char *)XtNwarnIfLocked,
	(char *)XtCWarnIfLocked,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, warn_if_locked),
	XtRImmediate,
	XtPointer(False)
    },

    {
	(char *)XtNcheckOptions,
	(char *)XtCCheckOptions,
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, check_options),
	XtRImmediate,
	XtPointer(30)
    },

    {
	(char *)XtNttyMode,
	(char *)XtCTTYMode,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, tty_mode),
	XtRImmediate,
	XtPointer(False)
    },

    {
	(char *)XtNblockTTYInput,
	(char *)XtCBlockTTYInput,
	(char *)XtROnOff,
	sizeof(OnOff),
	XtOffsetOf(AppData, block_tty_input),
	XmRImmediate,
	XtPointer(Auto)
    },

    {
	(char *)XtNbufferGDBOutput,
	(char *)XtCBufferGDBOutput,
	(char *)XtROnOff,
	sizeof(OnOff),
	XtOffsetOf(AppData, buffer_gdb_output),
	XmRImmediate,
	XtPointer(Auto)
    },

    {
	(char *)XtNannotate,
	(char *)XtCAnnotate,
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, annotate),
	XtRImmediate,
	XtPointer(0)
    },

    {
	(char *)XtNdisplayGlyphs,
	(char *)XtCDisplayGlyphs,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, display_glyphs),
	XtRImmediate,
	XtPointer(True)
    },

    {
	(char *)XtNmaxGlyphs,
	(char *)XtCMaxGlyphs,
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, max_glyphs),
	XtRImmediate,
	XtPointer(10)
    },

    {
	(char *)XtNcacheGlyphImages,
	(char *)XtCCacheGlyphImages,
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
	(char *)XtNglyphUpdateDelay,
	(char *)XtCGlyphUpdateDelay,
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, glyph_update_delay),
	XtRImmediate,
	XtPointer(10)
    },

    {
	(char *)XtNdisplayLineNumbers,
	(char *)XtCDisplayLineNumbers,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, display_line_numbers),
	XtRImmediate,
	XtPointer(False)
    },

    {
	(char *)XtNdisassemble,
	(char *)XtCDisassemble,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, disassemble),
	XtRImmediate,
	XtPointer(False)
    },

    {
	(char *)XtNmaxDisassemble,
	(char *)XtCMaxDisassemble,
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, max_disassemble),
	XmRImmediate,
	XtPointer(1024)
    },

    {
	(char *)XtNuseSourcePath,
	(char *)XtCUseSourcePath,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, use_source_path),
	XtRImmediate,
	XtPointer(False)
    },

    {
	(char *)XtNallRegisters,
	(char *)XtCAllRegisters,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, all_registers),
	XtRImmediate,
	XtPointer(False)
    },

    {
	(char *)XtNbuttonTips,
	(char *)XtCTips,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, button_tips),
	XtRImmediate,
	XtPointer(True)
    },

    {
	(char *)XtNvalueTips,
	(char *)XtCTips,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, value_tips),
	XtRImmediate,
	XtPointer(True)
    },

    {
	(char *)XtNbuttonDocs,
	(char *)XtCDocs,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, button_docs),
	XtRImmediate,
	XtPointer(True)
    },

    {
	(char *)XtNvalueDocs,
	(char *)XtCDocs,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, value_docs),
	XtRImmediate,
	XtPointer(True)
    },

    {
	(char *)XtNstartupTips,
	(char *)XtCStartupTips,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, startup_tips),
	XtRImmediate,
	XtPointer(True)
    },

    {
	(char *)XtNstartupTipCount,
	(char *)XtCStartupTipCount,
	XmRInt,
	sizeof(int),
	XtOffsetOf(AppData, startup_tip_count),
	XtRImmediate,
	XtPointer(1)
    },

    {
	(char *)XtNstatusAtBottom,
	(char *)XtCStatusAtBottom,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, status_at_bottom),
	XtRImmediate,
	XtPointer(True)
    },

    {
	(char *)XtNtoolbarsAtBottom,
	(char *)XtCToolbarsAtBottom,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, toolbars_at_bottom),
	XtRImmediate,
	XtPointer(True)
    },

    {
	(char *)XtNmaxDisplayTitleLength,
	(char *)XtCMaxLength,
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, max_display_title_length),
	XmRImmediate,
	XtPointer(20)
    },

    {
	(char *)XtNmaxPopupExprLength,
	(char *)XtCMaxLength,
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, max_popup_expr_length),
	XmRImmediate,
	XtPointer(20)
    },

    {
	(char *)XtNmaxValueTipLength,
	(char *)XtCMaxLength,
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, max_value_tip_length),
	XmRImmediate,
	XtPointer(32)
    },

    {
	(char *)XtNmaxValueDocLength,
	(char *)XtCMaxLength,
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, max_value_doc_length),
	XmRImmediate,
	XtPointer(128)
    },

    {
	(char *)XtNbuttonTipDelay,
	(char *)XtCTipDelay,
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, button_tip_delay),
	XmRImmediate,
	XtPointer(750)
    },

    {
	(char *)XtNvalueTipDelay,
	(char *)XtCTipDelay,
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, value_tip_delay),
	XmRImmediate,
	XtPointer(750)
    },

    {
	(char *)XtNbuttonDocDelay,
	(char *)XtCDocDelay,
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, button_doc_delay),
	XmRImmediate,
	XtPointer(0)
    },

    {
	(char *)XtNvalueDocDelay,
	(char *)XtCDocDelay,
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, value_doc_delay),
	XmRImmediate,
	XtPointer(0)
    },

    {
	(char *)XtNclearDocDelay,
	(char *)XtCClearDelay,
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, clear_doc_delay),
	XmRImmediate,
	XtPointer(1000)
    },

    {
	(char *)XtNclearTipDelay,
	(char *)XtCClearDelay,
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, clear_tip_delay),
	XmRImmediate,
	XtPointer(50)
    },

    {
	(char *)XtNblinkWhileBusy,
	(char *)XtCBlinkWhileBusy,
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, blink_while_busy),
	XmRImmediate,
	XtPointer(True)
    },

    {
	(char *)XtNbusyBlinkRate,
	(char *)XmCBlinkRate,
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, busy_blink_rate),
	XmRImmediate,
	XtPointer(500)
    },

    {
	(char *)XtNcheckGrabs,
	(char *)XtCCheckGrabs,
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, check_grabs),
	XmRImmediate,
	XtPointer(True)
    },

    {
	(char *)XtNcheckGrabDelay,
	(char *)XtCCheckGrabDelay,
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, check_grab_delay),
	XmRImmediate,
	XtPointer(5000)
    },

    {
	(char *)XtNgrabActionDelay,
	(char *)XtCGrabActionDelay,
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, grab_action_delay),
	XmRImmediate,
	XtPointer(10000)
    },

    {
	(char *)XtNgrabAction,
	(char *)XtCGrabAction,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, grab_action),
	XmRImmediate,
	XtPointer("cont")
    },

    {
	(char *)XtNdetectAliases,
	(char *)XtCDetectAliases,
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, detect_aliases),
	XmRImmediate,
	XtPointer(False)
    },

    {
	(char *)XtNtypedAliases,
	(char *)XtCTypedAliases,
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, typed_aliases),
	XmRImmediate,
	XtPointer(True)
    },

    {
	(char *)XtNdeleteAliasDisplays,
	(char *)XtCDeleteAliasDisplays,
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, delete_alias_displays),
	XmRImmediate,
	XtPointer(True)
    },

    {
	(char *)XtNalign2dArrays,
	(char *)XtCAlign2dArrays,
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, align_2d_arrays),
	XmRImmediate,
	XtPointer(True)
    },

    {
	(char *)XtNarrayOrientation,
	(char *)XtCOrientation,
	XtROrientation,
	sizeof(unsigned char),
	XtOffsetOf(AppData, array_orientation),
	XmRImmediate,
	XtPointer(XmVERTICAL)
    },

    {
	(char *)XtNstructOrientation,
	(char *)XtCOrientation,
	XtROrientation,
	sizeof(unsigned char),
	XtOffsetOf(AppData, struct_orientation),
	XmRImmediate,
	XtPointer(XmVERTICAL)
    },

    {
	(char *)XtNshowMemberNames,
	(char *)XtCShowMemberNames,
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, show_member_names),
	XmRImmediate,
	XtPointer(True)
    },

    {
	(char *)XtNexpandRepeatedValues,
	(char *)XtCExpandRepeatedValues,
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, expand_repeated_values),
	XmRImmediate,
	XtPointer(False)
    },

    {
	(char *)XtNbumpDisplays,
	(char *)XtCBumpDisplays,
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, bump_displays),
	XmRImmediate,
	XtPointer(True)
    },

    {
	(char *)XtNhideInactiveDisplays,
	(char *)XtCHideInactiveDisplays,
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, hide_inactive_displays),
	XmRImmediate,
	XtPointer(True)
    },

    {
	(char *)XtNshowBaseDisplayTitles,
	(char *)XtCShowDisplayTitles,
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, show_base_display_titles),
	XmRImmediate,
	XtPointer(True)
    },

    {
	(char *)XtNshowDependentDisplayTitles,
	(char *)XtCShowDisplayTitles,
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, show_dependent_display_titles),
	XmRImmediate,
	XtPointer(True)
    },

    {
	(char *)XtNclusterDisplays,
	(char *)XtCClusterDisplays,
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, cluster_displays),
	XmRImmediate,
	XtPointer(False)
    },

    {
	(char *)XtNdisplayPlacement,
	(char *)XtCOrientation,
	XtROrientation,
	sizeof(unsigned char),
	XtOffsetOf(AppData, display_placement),
	XmRImmediate,
	XtPointer(XmVERTICAL)
    },

    {
	(char *)XtNstatusHistorySize,
	(char *)XtCHistorySize,
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, status_history_size),
	XmRImmediate,
	XtPointer(10)
    },

    {
	(char *)XtNpopdownHistorySize,
	(char *)XtCHistorySize,
	XtRCardinal,
	sizeof(Cardinal),
	XtOffsetOf(AppData, popdown_history_size),
	XmRImmediate,
	XtPointer(0)
    },

    {
	(char *)XtNsortPopdownHistory,
	(char *)XtCSortPopdownHistory,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, sort_popdown_history),
	XmRImmediate,
	XtPointer(True)
    },

    {
	(char *)XtNverifyButtons,
	(char *)XtCVerifyButtons,
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, verify_buttons),
	XmRImmediate,
	XtPointer(True)
    },

    {
	(char *)XtNautoRaiseTool,
	(char *)XtCAutoRaiseTool,
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, auto_raise_tool),
	XmRImmediate,
	XtPointer(True)
    },

    {
	(char *)XtNdecorateTool,
	(char *)XtCDecorate,
	(char *)XtROnOff,
	sizeof(OnOff),
	XtOffsetOf(AppData, decorate_tool),
	XmRImmediate,
	XtPointer(Auto)
    },

    {
	(char *)XtNstickyTool,
	(char *)XtCStickyTool,
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, sticky_tool),
	XmRImmediate,
	XtPointer(True)
    },

    {
	(char *)XtNtoolRightOffset,
	(char *)XtCToolOffset,
	XmRPosition,
	sizeof(Position),
	XtOffsetOf(AppData, tool_right_offset),
	XmRImmediate,
	XtPointer(8)
    },

    {
	(char *)XtNtoolTopOffset,
	(char *)XtCToolOffset,
	XmRPosition,
	sizeof(Position),
	XtOffsetOf(AppData, tool_top_offset),
	XmRImmediate,
	XtPointer(8)
    },

    {
	(char *)XtNbuttonCaptions,
	(char *)XtCButtonCaptions,
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, button_captions),
	XmRImmediate,
	XtPointer(True)
    },

    {
	(char *)XtNbuttonImages,
	(char *)XtCButtonImages,
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, button_images),
	XmRImmediate,
	XtPointer(True)
    },

    {
	(char *)XtNbuttonCaptionGeometry,
	(char *)XtCButtonCaptionGeometry,
	XmRString,
	sizeof(String),
	XtOffsetOf(AppData, button_caption_geometry),
	XmRImmediate,
	XtPointer("29x7+0-0")
    },

    {
	(char *)XtNbuttonImageGeometry,
	(char *)XtCButtonImageGeometry,
	XmRString,
	sizeof(String),
	XtOffsetOf(AppData, button_image_geometry),
	XmRImmediate,
	XtPointer("21x17+4+0")
    },

    {
	(char *)XtNflatToolbarButtons,
	(char *)XtCFlatButtons,
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, flat_toolbar_buttons),
	XmRImmediate,
	XtPointer(True)
    },

    {
	(char *)XtNflatDialogButtons,
	(char *)XtCFlatButtons,
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, flat_dialog_buttons),
	XmRImmediate,
	XtPointer(False)
    },

    {
	(char *)XtNbuttonColorKey,
	(char *)XtCColorKey,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, button_color_key),
	XtRString,
	XtPointer("g")
    },

    {
	(char *)XtNactiveButtonColorKey,
	(char *)XtCColorKey,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, active_button_color_key),
	XtRString,
	XtPointer("c")
    },

    {
	(char *)XtNsplashScreenColorKey,
	(char *)XtCColorKey,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, splash_screen_color_key),
	XtRString,
	XtPointer("c")
    },

    {
	(char *)XtNautoCommands,
	(char *)XtCAutoCommands,
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, auto_commands),
	XmRImmediate,
	XtPointer(False)
    },

    {
	(char *)XtNautoCommandPrefix,
	(char *)XtCAutoCommandPrefix,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, auto_command_prefix),
	XtRString,
	XtPointer(ddd_NAME ": ")
    },

    {
	(char *)XtNmaxBreakpointNumber,
	(char *)XtCMaxNumber,
	XmRInt,
	sizeof(int),
	XtOffsetOf(AppData, max_breakpoint_number),
	XmRImmediate,
	XtPointer(99)
    },

    {
	(char *)XtNmaxDisplayNumber,
	(char *)XtCMaxNumber,
	XmRInt,
	sizeof(int),
	XtOffsetOf(AppData, max_display_number),
	XmRImmediate,
	XtPointer(99)
    },

    {
	(char *)XtNsplashScreen,
	(char *)XtCSplashScreen,
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, splash_screen),
	XmRImmediate,
	XtPointer(True)
    },

    {
	(char *)XtNcolorWMIcons,
	(char *)XtCColorWMIcons,
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, color_wm_icons),
	XmRImmediate,
	XtPointer(True)
    },

    {
	(char *)XtNcutCopyPasteBindings,
	(char *)XtCBindingStyle,
	(char *)XtRBindingStyle,
	sizeof(BindingStyle),
	XtOffsetOf(AppData, cut_copy_paste_bindings),
	XmRImmediate,
	XtPointer(KDEBindings)
    },

    {
	(char *)XtNselectAllBindings,
	(char *)XtCBindingStyle,
	(char *)XtRBindingStyle,
	sizeof(BindingStyle),
	XtOffsetOf(AppData, select_all_bindings),
	XmRImmediate,
	XtPointer(KDEBindings)
    },

    {
	(char *)XtNmaxUndoDepth,
	(char *)XtCMaxUndoDepth,
	XmRInt,
	sizeof(int),
	XtOffsetOf(AppData, max_undo_depth),
	XmRImmediate,
	XtPointer(-1)
    },

    {
	(char *)XtNmaxUndoSize,
	(char *)XtCMaxUndoSize,
	XmRInt,
	sizeof(int),
	XtOffsetOf(AppData, max_undo_size),
	XmRImmediate,
	XtPointer(2000000)
    },

    {
	(char *)XtNthemes,
	(char *)XtCThemes,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, themes),
	XtRString,
	XtPointer("")
    },

    {
	(char *)XtNsuppressTheme,
	(char *)XtCTheme,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, suppress_theme),
	XtRString,
	XtPointer("suppress.vsl")
    },

    {
	(char *)XtNmaintenance,
	(char *)XtCMaintenance,
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, maintenance),
	XmRImmediate,
	XtPointer(False)
    },

    {
	(char *)XtNdumpCore,
	(char *)XtCDumpCore,
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, dump_core),
	XmRImmediate,
	XtPointer(True)
    },

    {
	(char *)XtNdebugCoreDumps,
	(char *)XtCDebugCoreDumps,
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, debug_core_dumps),
	XmRImmediate,
	XtPointer(False)
    },

    {
	(char *)XtNlessTifVersion,
	(char *)XtCLessTifVersion,
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
	(char *)XtNdddinitVersion,
	(char *)XtCVersion,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, dddinit_version),
	XtRString,
	XtPointer(0)
    },

    {
	(char *)XtNappDefaultsVersion,
	(char *)XtCVersion,
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
struct AppDataInitializer {
    AppDataInitializer();
} app_data_initializer;


// Copy the SIZE bytes at FROM into the SIZE bytes at DEST, even if
// those two blocks of space overlap.  In the case of overlap, be
// careful to copy the original values of the bytes in the block at
// SRC, including those bytes which also belong to the block at DEST.
static void copy(char *dest, char *src, Cardinal size)
{
    // This is taken from libiberty's bcopy().
    if (dest < src)
	while (size--)
	    *dest++ = *src++;
    else
    {
	char *lasts = src + (size - 1);
	char *lastd = dest + (size - 1);
	while (size--)
	    *(char *)lastd-- = *(char *)lasts--;
    }
}

// Copy the value at SRC of size SIZE to the value at DEST.
static void CopyArg(XtPointer src, XtPointer dest, Cardinal size)
{
    // This stuff is taken from Xt11R6.3 _XtCopyArg().  I hope it
    // gets all possible conversions right...

    if (size > sizeof(XtArgVal))
    {
	copy((char *)dest, (char *)src, size);
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

	copy((char *)dest, (char *)p, size);
    }
}


// This constructor is invoked before program start
AppDataInitializer::AppDataInitializer()
{
    // Copy resources to appropriate fields in APP_DATA
    for (int i = 0; i < int(ddd_resources_size); i++)
    {
	XtResource& res = ddd_resources[i];
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
0				// Terminating NULL
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
	XrmDatabase db2 = XrmGetFileDatabase(own_app_defaults_file);
	if (db2 != 0)
	    XrmMergeDatabases(db2, &db);
    }
    else
    {
	// No application defaults file in the standard place.
	// Search along the usual path.
	String official_app_defaults_file = 
	    XtResolvePathname(display, NULL, app_class, 
			      NULL, NULL, NULL, 0, NULL);
	if (official_app_defaults_file != NULL)
	{
	    XrmDatabase db2 = XrmGetFileDatabase(official_app_defaults_file);
	    if (db2 != 0)
		XrmMergeDatabases(db2, &db);
	}
    }

    return db;
}
