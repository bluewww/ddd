// $Id$ -*- C++ -*-
// Argument Dialog

// Copyright (C) 1996-1998 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@gnu.org>.
// Cross-platform interface by Peter Wainwright <prw@ceiriog.eclipse.co.uk>
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

char args_rcsid[] = 
    "$Id$";

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#include "args.h"

#include "Delay.h"
#include "HelpCB.h"
#include "StringA.h"
#include "Command.h"
#include "SourceView.h"
#include "ddd.h"
#include "disp-read.h"
#include "file.h"
#include "mydialogs.h"
#include "options.h"
#include "cook.h"
#include "regexps.h"
#include "session.h"
#include "string-fun.h"
#include "verify.h"
#include "wm.h"

#if defined(IF_XM)
#include <Xm/Xm.h>
#include <Xm/List.h>
#include <Xm/MessageB.h>
#include <Xm/SelectioB.h>
#include <Xm/Text.h>
#else
#include <GUI/Dialog.h>
#include <GUI/ComboBox.h>
#include "gtk_wrapper.h"
#endif

#include <ctype.h>


//-----------------------------------------------------------------------------
// Run, Make, and CD Dialogs
//-----------------------------------------------------------------------------

// Argument storage
#if defined(IF_XM)
static Widget run_dialog;
static Widget run_arguments_w;
#else
static GUI::Dialog *run_dialog;
static GUI::ComboBoxEntryText *run_arguments_w;
#endif
static StringArray run_arguments;
static string last_run_argument;
static bool run_arguments_updated = false;

#if defined(IF_XM)
static Widget make_dialog;
#else
static GUI::Dialog *make_dialog;
#endif
static StringArray make_arguments;
#if defined(IF_XM)
static Widget make_arguments_w;
#else
static GUI::ComboBoxEntryText *make_arguments_w;
#endif
static bool make_arguments_updated = false;
static string last_make_argument;

#if defined(IF_XM)
static Widget cd_dialog;
static StringArray cd_arguments;
static Widget cd_arguments_w;
#else
static GUI::Dialog *cd_dialog;
static StringArray cd_arguments;
static GUI::ComboBoxEntryText *cd_arguments_w;
#endif
static bool cd_arguments_updated = false;
static string last_cd_argument;


// Add ARG to the list of arguments
static void add_argument(string arg, StringArray& arguments, 
			 string& last, bool& updated)
{
    strip_trailing_space(arg);
    while (arg.length() > 0 && isspace(arg[0]))
	arg = arg.after(0);

    last = arg;

    // Insertion sort
    int i;
    for (i = 0; i < arguments.size(); i++)
    {
	int cmp = compare(arguments[i], arg);
	if (cmp == 0)
	    return;		// Already present
	if (cmp > 0)
	    break;
    }

    arguments += "<dummy>";

    for (int j = arguments.size() - 1; j > i; j--)
	arguments[j] = arguments[j - 1];
    arguments[i] = arg;

    updated = false;
}

// If LINE is an argument-setting command, add it to the list of arguments
void add_to_arguments(const string& line)
{
    if (is_set_args_cmd(line))
    {
	string args = line.after("args");
	args = args.after(rxwhite);
	add_argument(args, run_arguments, last_run_argument, 
		     run_arguments_updated);
    }
    else if (gdb->type() == PERL && line.contains("@ARGV = ", 0))
    {
	// @ARGV = ('arg1', 'arg2', )
	string args = line.after("('");
	args.gsub("', '", " ");
	args = args.before("', )");

	add_argument(args, run_arguments, last_run_argument, 
		     run_arguments_updated);
    }
    else if (is_run_cmd(line))
    {
	string args;
	if (gdb->type() == JDB)
	{
	    // `run CLASS ARGS...'
	    args = line.after(rxwhite);
	    args = args.after(rxwhite);
	}
	else if (gdb->type() == PERL && line.contains("exec ", 0))
	{
	    // `exec "perl -d PROGRAM ARGS..."'
	    args = line.after("exec ");
	    strip_leading_space(args);
	    if (args.contains('\"', 0) || args.contains('\'', 0))
		args = unquote(args);

	    args = args.after("-d ");
	    strip_leading_space(args);
	    args = args.after(rxwhite);
	}
	else
	{
	    // `run ARGS...'
	    args = line.after(rxwhite);
	}

	add_argument(args, run_arguments, last_run_argument, 
		     run_arguments_updated);
    }
    else if (is_make_cmd(line))
    {
	string args = line.after("make");
	args = args.after(rxwhite);
	add_argument(args, make_arguments, last_make_argument, 
		     make_arguments_updated);
    }
    else if (gdb->type() == PERL && line.contains("system 'make", 0))
    {
	string args = line.after("make");
	args = args.after(rxwhite);
	args = args.before("'");
	add_argument(args, make_arguments, last_make_argument, 
		     make_arguments_updated);
    }
    else if (is_cd_cmd(line))
    {
	string dir = line.after("cd");
	dir = dir.after(rxwhite);
	dir = source_view->full_path(dir);
	if (dir.contains('/', 0))
	    add_argument(dir, cd_arguments, last_cd_argument, 
			 cd_arguments_updated);
    }
    else if (gdb->type() == PERL && line.contains("chdir '", 0))
    {
	string dir = line.after("'");
	dir = dir.before("'");
	add_argument(dir, cd_arguments, last_cd_argument, 
		     cd_arguments_updated);
    }
    else if (gdb->type() == PERL && is_file_cmd(line, gdb))
    {
	string args = line.after(" -d ");
	args = args.after(rxwhite); // Skip file name
	args = args.before('\"');
	add_argument(args, run_arguments, last_run_argument, 
		     run_arguments_updated);
    }
}


#if defined(IF_XM)
// Update list of arguments
static void update_arguments(Widget dialog, Widget arguments_w,
			     StringArray& arguments, const string& last,
			     bool& updated)
{
    if (updated || dialog == 0)
	return;

    bool *selected = new bool[arguments.size()];
    int pos = -1;
    for (int i = 0; i < arguments.size(); i++)
    {
	if (arguments[i] == last)
	    pos = i;
	selected[i] = false;
    }
    if (pos >= 0)
	selected[pos] = true;

    setLabelList(arguments_w, arguments.values(),
		 selected, arguments.size(), false, false);

    if (pos >= 0)
	XmListSelectPos(arguments_w, pos + 1, False);

    delete[] selected;

    Widget text_w = XmSelectionBoxGetChild(dialog, XmDIALOG_TEXT);
    XmTextSetString(text_w, XMST(last.chars()));

    updated = true;
}
#endif

static void update_run_arguments()
{
#if defined(IF_XM)
    update_arguments(run_dialog, run_arguments_w, run_arguments,
		     last_run_argument, run_arguments_updated);
#else
    std::cerr << "update_run_arguments not implemented yet.\n";
#endif
}

static void update_make_arguments()
{
#if defined(IF_XM)
    update_arguments(make_dialog, make_arguments_w, make_arguments,
		     last_make_argument, make_arguments_updated);
#else
    std::cerr << "update_make_arguments not implemented yet.\n";
#endif
}

static void update_cd_arguments()
{
#if defined(IF_XM)
    update_arguments(cd_dialog, cd_arguments_w, cd_arguments,
		     last_cd_argument, cd_arguments_updated);
#else
    std::cerr << "update_cd_arguments not implemented yet.\n";
#endif
}

void update_arguments()
{
#if defined(IF_XM)
    update_run_arguments();
    update_make_arguments();
    update_cd_arguments();
#else
    std::cerr << "update_arguments not implemented yet.\n";
#endif
}

//-----------------------------------------------------------------------------
// Run Dialog
//-----------------------------------------------------------------------------

#if defined(IF_XM)
// Run program with given arguments
static void gdbRunDCB(Widget, XtPointer, XtPointer)
{

    Widget text  = XmSelectionBoxGetChild(run_dialog, XmDIALOG_TEXT);
    String _args = XmTextGetString(text);
    string args(_args);
    XtFree(_args);

    string cmd = gdb->run_command(args);
    while (!cmd.empty())
    {
	string c;
	if (cmd.contains('\n'))
	    c = cmd.before('\n');
	else
	    c = cmd;
	cmd = cmd.after('\n');
	gdb_command(c, run_dialog);
    }
}
#else
// Run program with given arguments
static void gdbRunDCB(void)
{
    GUI::String _args = run_arguments_w->get_text();
    string args(_args.c_str());

    string cmd = gdb->run_command(args);
    while (!cmd.empty())
    {
	string c;
	if (cmd.contains('\n'))
	    c = cmd.before('\n');
	else
	    c = cmd;
	cmd = cmd.after('\n');
	gdb_command(c, (GUI::Widget *)run_dialog);
    }
}
#endif

#if defined(IF_XM)
// Set program arguments from list
static void SelectRunArgsCB(Widget, XtPointer, XtPointer call_data)
{
    XmListCallbackStruct *cbs = (XmListCallbackStruct *)call_data;
    int pos = cbs->item_position - 1;
    const string& args = run_arguments[pos];
    
    Widget text_w = XmSelectionBoxGetChild(run_dialog, XmDIALOG_TEXT);
    XmTextSetString(text_w, XMST(args.chars()));
}
#endif

#if defined(IF_XM)
// Create `Run' dialog
void gdbRunCB(Widget w, XtPointer, XtPointer)
{
    if (run_dialog == 0)
    {
	Arg args[10];
	int arg = 0;

	run_dialog = 
	    verify(XmCreateSelectionDialog(find_shell(w), 
					   XMST("run_dialog"), 
					   args, arg));
	XtUnmanageChild(XmSelectionBoxGetChild(run_dialog, 
					       XmDIALOG_APPLY_BUTTON));

	Delay::register_shell(run_dialog);
	XtAddCallback(run_dialog, XmNokCallback,     gdbRunDCB, 0);
	XtAddCallback(run_dialog, XmNapplyCallback,  gdbRunDCB, 0);
	XtAddCallback(run_dialog, XmNhelpCallback,   ImmediateHelpCB, 0);

	run_arguments_w = XmSelectionBoxGetChild(run_dialog, XmDIALOG_LIST);
	XtAddCallback(run_arguments_w, XmNsingleSelectionCallback,
		      SelectRunArgsCB, 0);
	XtAddCallback(run_arguments_w, XmNmultipleSelectionCallback,
		      SelectRunArgsCB, 0);
	XtAddCallback(run_arguments_w, XmNextendedSelectionCallback,
		      SelectRunArgsCB, 0);
	XtAddCallback(run_arguments_w, XmNbrowseSelectionCallback,
		      SelectRunArgsCB, 0);
    }

    update_run_arguments();
    manage_and_raise(run_dialog);
}
#else
// Create `Run' dialog
void gdbRunCB(GUI::Widget *w)
{
    if (run_dialog == (GUI::Widget *)0)
    {
	run_dialog = new GUI::Dialog(*find_shell(w), "run_dialog");

	Delay::register_shell(run_dialog);
	GUI::Button *button = run_dialog->add_button("run", _("Run"));
	button->signal_clicked().connect(sigc::ptr_fun(gdbRunDCB));
	button->show();

	run_arguments_w = new GUI::ComboBoxEntryText(*run_dialog);
	run_arguments_w->show();
    }

    update_run_arguments();
    manage_and_raise(run_dialog);
}
#endif


//-----------------------------------------------------------------------------
// Make Dialog
//-----------------------------------------------------------------------------

#if defined(IF_XM)
// Set program arguments from list
static void SelectMakeArgsCB(Widget, XtPointer, XtPointer call_data)
{
    XmListCallbackStruct *cbs = (XmListCallbackStruct *)call_data;
    int pos = cbs->item_position - 1;
    const string& args = make_arguments[pos];
    
    Widget text_w = XmSelectionBoxGetChild(make_dialog, XmDIALOG_TEXT);
    XmTextSetString(text_w, XMST(args.chars()));
}
#endif

#if defined(IF_XM)
// Make program with given arguments
static void gdbMakeDCB(Widget, XtPointer, XtPointer)
{
    Widget text = XmSelectionBoxGetChild(make_dialog, XmDIALOG_TEXT);
    String _args = XmTextGetString(text);
    string args(_args);
    XtFree(_args);

    gdb_command(gdb->make_command(args));
}
#else
// Make program with given arguments
static void gdbMakeDCB(void)
{
    GUI::String _args = make_arguments_w->get_text();
    string args(_args.c_str());

    gdb_command(gdb->make_command(args));
}
#endif

#if defined(IF_XM)
void gdbMakeAgainCB(Widget, XtPointer, XtPointer)
{
    gdb_command(gdb->make_command(last_make_argument));
}
#else
void gdbMakeAgainCB(GUI::Widget *w)
{
    gdb_command(gdb->make_command(last_make_argument));
}
#endif

#if defined(IF_XM)
// Create `Make' dialog
void gdbMakeCB(Widget w, XtPointer, XtPointer)
{
    if (!gdb->has_make_command())
	return;

    if (make_dialog == 0)
    {
	Arg args[10];
	int arg = 0;

	make_dialog = 
	    verify(XmCreateSelectionDialog(find_shell(w), 
					   XMST("make_dialog"), 
					   args, arg));
	XtUnmanageChild(XmSelectionBoxGetChild(make_dialog, 
					       XmDIALOG_APPLY_BUTTON));

	Delay::register_shell(make_dialog);
	XtAddCallback(make_dialog, XmNokCallback,     gdbMakeDCB, 0);
	XtAddCallback(make_dialog, XmNapplyCallback,  gdbMakeDCB, 0);
	XtAddCallback(make_dialog, XmNhelpCallback,   ImmediateHelpCB, 0);

	make_arguments_w = XmSelectionBoxGetChild(make_dialog, XmDIALOG_LIST);
	XtAddCallback(make_arguments_w, XmNsingleSelectionCallback,
		      SelectMakeArgsCB, 0);
	XtAddCallback(make_arguments_w, XmNmultipleSelectionCallback,
		      SelectMakeArgsCB, 0);
	XtAddCallback(make_arguments_w, XmNextendedSelectionCallback,
		      SelectMakeArgsCB, 0);
	XtAddCallback(make_arguments_w, XmNbrowseSelectionCallback,
		      SelectMakeArgsCB, 0);
    }

    update_make_arguments();
    manage_and_raise(make_dialog);
}
#else
// Create `Make' dialog
void gdbMakeCB(GUI::Widget *w)
{
    if (!gdb->has_make_command())
	return;

    if (make_dialog == (GUI::Widget *)0)
    {
	make_dialog = new GUI::Dialog(*find_shell(w), "make_dialog");

	Delay::register_shell(make_dialog);
	GUI::Button *button = make_dialog->add_button("make", _("Make"));
	button->signal_clicked().connect(sigc::ptr_fun(gdbMakeDCB));
	button->show();

	make_arguments_w = new GUI::ComboBoxEntryText(*make_dialog);
	make_arguments_w->show();

    }

    update_make_arguments();
    manage_and_raise(make_dialog);
}
#endif



//-----------------------------------------------------------------------------
// CD Dialog
//-----------------------------------------------------------------------------

#if defined(IF_XM)
// Set program arguments from list
static void SelectChangeDirectoryArgsCB(Widget, XtPointer, XtPointer call_data)
{
    XmListCallbackStruct *cbs = (XmListCallbackStruct *)call_data;
    int pos = cbs->item_position - 1;
    string args = source_view->full_path(cd_arguments[pos]);
    
    Widget text_w = XmSelectionBoxGetChild(cd_dialog, XmDIALOG_TEXT);
    XmTextSetString(text_w, XMST(args.chars()));
}
#endif

#if defined(IF_XM)
// ChangeDirectory program with given arguments
static void gdbChangeDirectoryDCB(Widget, XtPointer, XtPointer)
{
    Widget text = XmSelectionBoxGetChild(cd_dialog, XmDIALOG_TEXT);
    String _args = XmTextGetString(text);
    string args(_args);
    XtFree(_args);

    string path = source_view->full_path(args);
    switch (gdb->type()) {
    case PERL:
      gdb_command("chdir " + quote(path, '\''));
      break;
    case BASH:
      gdb_command("eval cd " + path);
      break;
    default:
      gdb_command("cd " + path);
    }
    
}
#else
// ChangeDirectory program with given arguments
static void gdbChangeDirectoryDCB(void)
{
#ifdef NAG_ME
#warning ChangeDirectory not supported yet
#endif
#if 0
    String _args = cd_dialog->get_text();
    string args(_args);

    string path = source_view->full_path(args);
    switch (gdb->type()) {
    case PERL:
      gdb_command("chdir " + quote(path, '\''));
      break;
    case BASH:
      gdb_command("eval cd " + path);
      break;
    default:
      gdb_command("cd " + path);
    }
#endif   
}
#endif

#if defined(IF_XM)
// Create `ChangeDirectory' dialog
void gdbChangeDirectoryCB(Widget w, XtPointer, XtPointer)
{
    if (cd_dialog == 0)
    {
	Arg args[10];
	int arg = 0;

	cd_dialog = 
	    verify(XmCreateSelectionDialog(find_shell(w), 
					   XMST("cd_dialog"), 
					   args, arg));
	XtUnmanageChild(XmSelectionBoxGetChild(cd_dialog, 
					       XmDIALOG_APPLY_BUTTON));

	Delay::register_shell(cd_dialog);
	XtAddCallback(cd_dialog, XmNokCallback,     gdbChangeDirectoryDCB, 0);
	XtAddCallback(cd_dialog, XmNapplyCallback,  gdbChangeDirectoryDCB, 0);
	XtAddCallback(cd_dialog, XmNhelpCallback,   ImmediateHelpCB, 0);

	cd_arguments_w = XmSelectionBoxGetChild(cd_dialog, XmDIALOG_LIST);
	XtAddCallback(cd_arguments_w, XmNsingleSelectionCallback,
		      SelectChangeDirectoryArgsCB, 0);
	XtAddCallback(cd_arguments_w, XmNmultipleSelectionCallback,
		      SelectChangeDirectoryArgsCB, 0);
	XtAddCallback(cd_arguments_w, XmNextendedSelectionCallback,
		      SelectChangeDirectoryArgsCB, 0);
	XtAddCallback(cd_arguments_w, XmNbrowseSelectionCallback,
		      SelectChangeDirectoryArgsCB, 0);

	add_argument("..", cd_arguments, last_cd_argument, 
		     cd_arguments_updated);
    }

    update_cd_arguments();
    manage_and_raise(cd_dialog);
}
#else
// Create `ChangeDirectory' dialog
void gdbChangeDirectoryCB(GUI::Widget *w)
{
    if (cd_dialog == (GUI::Widget *)0)
    {
	GUI::Shell *sh = find_shell(w);
	GUI::String str = GUI::String("cd_dialog");
	std::cerr << "OK, Window " << sh << "\n" << std::flush;
	cd_dialog = new GUI::Dialog(*sh, str);
	// cd_dialog = new GUI::Dialog(find_shell(w->internal()), "cd_dialog");

	GUI::Button *button = cd_dialog->add_button("change", _("Change Dir"));
	button->signal_clicked().connect(sigc::ptr_fun(gdbChangeDirectoryDCB));
	button->show();

#ifdef NAG_ME
#warning Unsupported list callbacks
#endif
#if 0
	cd_arguments_w = XmSelectionBoxGetChild(cd_dialog, XmDIALOG_LIST);
	XtAddCallback(cd_arguments_w, XmNsingleSelectionCallback,
		      SelectChangeDirectoryArgsCB, 0);
	XtAddCallback(cd_arguments_w, XmNmultipleSelectionCallback,
		      SelectChangeDirectoryArgsCB, 0);
	XtAddCallback(cd_arguments_w, XmNextendedSelectionCallback,
		      SelectChangeDirectoryArgsCB, 0);
	XtAddCallback(cd_arguments_w, XmNbrowseSelectionCallback,
		      SelectChangeDirectoryArgsCB, 0);
#endif

	add_argument("..", cd_arguments, last_cd_argument, 
		     cd_arguments_updated);
    }

    update_cd_arguments();
    manage_and_raise(cd_dialog);
}
#endif

//-----------------------------------------------------------------------------
// `run' arguments
//-----------------------------------------------------------------------------

#if defined(IF_XM)
static void RestartAndRunCB(Widget w, 
			    XtPointer client_data, XtPointer call_data)
{
    RestartDebuggerCB(w, client_data, call_data);

    const string& cmd = *((const string *)client_data);
    gdb_command(cmd, w);
}
#else
static void RestartAndRunCB(GUI::Widget *w, const string *client_data)
{
    RestartDebuggerCB();

    const string& cmd = *client_data;
    gdb_command(cmd, w);
}
#endif

#if defined(IF_XM)
bool add_running_arguments(string& cmd, Widget origin)
{
    if (cmd == "run")
	cmd = gdb->rerun_command();

    if (gdb->type() != JDB)
	return true;		// Ok, perform the command

    if (!is_run_cmd(cmd))
	return true;		// Ok, perform the command

    strip_leading_space(cmd);
    string args = cmd.after(rxwhite);

    ProgramInfo info;

    if (args.empty() && gdb->has_debug_command())
    {
	// JDB 1.1 requires at least a class name after the `run' command.
	cmd += " " + info.file;
    }

    if (info.running && !gdb->has_debug_command())
    {
	// JDB 1.2 cannot rerun a program after it has been started.
	// Offer to restart JDB instead.
	static Widget restart_jdb = 0;
	static string saved_run_command;

	if (restart_jdb == 0)
	{
#if defined(IF_XM)
	    restart_jdb = 
		verify(XmCreateQuestionDialog(find_shell(origin),
				 XMST("confirm_restart_gdb_dialog"), 
				 ArgList(0), 0));
	    Delay::register_shell(restart_jdb);
	    XtAddCallback(restart_jdb, XmNhelpCallback,   
			  ImmediateHelpCB, XtPointer(0));
	    XtAddCallback(restart_jdb, XmNokCallback,     
			  RestartAndRunCB, (XtPointer)&saved_run_command);
#else
	    std::cerr << "JDB: not supported yet\n";
#endif
	}

	saved_run_command = cmd;
#if defined(IF_XM)
	XtManageChild(restart_jdb);
#endif

	return false;		// Don't perform the command yet
    }

    return true;
}
#else
bool add_running_arguments(string& cmd, GUI::Widget *origin)
{
    if (cmd == "run")
	cmd = gdb->rerun_command();

    if (gdb->type() != JDB)
	return true;		// Ok, perform the command

    if (!is_run_cmd(cmd))
	return true;		// Ok, perform the command

    strip_leading_space(cmd);
    string args = cmd.after(rxwhite);

    ProgramInfo info;

    if (args.empty() && gdb->has_debug_command())
    {
	// JDB 1.1 requires at least a class name after the `run' command.
	cmd += " " + info.file;
    }

    if (info.running && !gdb->has_debug_command())
    {
	// JDB 1.2 cannot rerun a program after it has been started.
	// Offer to restart JDB instead.
	static string saved_run_command;

	std::cerr << "JDB: not supported yet\n";

	saved_run_command = cmd;

	return false;		// Don't perform the command yet
    }

    return true;
}

#endif
