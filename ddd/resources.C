// $Id$ -*- C++ -*-
// DDD application resources

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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

char resources_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "resources.h"
#include "AppData.h"
#include "stty.h"

// Application resource definitions
XtResource ddd_resources[] = {
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
	XtNtraceDialog, 
	XtCTrace, 
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, trace_dialog),
	XmRImmediate, 
	XtPointer(False)
    },

    { 
	XtNtraceShellCommands,
	XtCTrace,
	XmRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, trace_shell_commands),
	XmRImmediate, 
	XtPointer(False)
    },

    { 
	XtNgdbInitCommands, 
	XtCGDBInitCommands, 
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, gdb_initial_cmds), 
	XtRString,
	XtPointer(
	    "set height 0\n"
	    "set width 0\n"
	    " set verbose off\n"
	    "set prompt (gdb) \n")
    },

    { 
	XtNgdbSettings, 
	XtCGDBSettings, 
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, gdb_settings), 
	XtRString,
	XtPointer("")
    },

    { 
	XtNdbxInitCommands, 
	XtCDBXInitCommands, 
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, dbx_initial_cmds), 
	XtRString,
	XtPointer("")
    },

    { 
	XtNdbxSettings, 
	XtCDBXSettings, 
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, dbx_settings), 
	XtRString,
	XtPointer("")
    },

    { 
	XtNxdbInitCommands, 
	XtCXDBInitCommands, 
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, xdb_initial_cmds), 
	XtRString,
	XtPointer("")
    },

    { 
	XtNxdbSettings, 
	XtCXDBSettings, 
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, xdb_settings), 
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
	XtNvslDefs, 
	XtCVSLDefs, 
	XtRString, 
	sizeof(String),
	XtOffsetOf(AppData, vsl_defs),
	XtRString, 
	XtPointer("")
    },

    {
	XtNmaxNameLength,
	XtCMaxNameLength,
	XmRInt,
	sizeof(int),
	XtOffsetOf(AppData, max_name_length),
	XmRImmediate,
	XtPointer(32)
    },

    {
	XtNindentAmount,
	XtCIndentAmount,
	XmRInt,
	sizeof(int),
	XtOffsetOf(AppData, indent_amount),
	XmRImmediate,
	XtPointer(8)
    },

    {
	XtNconsoleButtons,
	XtCConsoleButtons,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, console_buttons),
	XtRString,
	XtPointer("")
    },

    {
	XtNsourceButtons,
	XtCSourceButtons,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, source_buttons),
	XtRString,
	XtPointer("")
    },

    {
	XtNdataButtons,
	XtCDataButtons,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, data_buttons),
	XtRString,
	XtPointer("")
    },

    {
	XtNtoolButtons,
	XtCToolButtons,
	XtRString,
	sizeof(String),
	XtOffsetOf(AppData, tool_buttons),
	XtRString,
	XtPointer("run:interrupt^C:next:step:continue:finish:up:down")
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
        XtNshowResources,
	XtCShowResources,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, show_resources),
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
	XtNgroupIconify,
	XtCGroupIconify,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, group_iconify),
	XtRImmediate,
	XtPointer(False)
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
	XtNdisassemble,
	XtCDisassemble,
	XtRBoolean,
	sizeof(Boolean),
	XtOffsetOf(AppData, disassemble),
	XtRImmediate,
	XtPointer(False)
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
