// $Id$ -*- C++ -*-
// Elementary DDD dialogs

// Copyright (C) 1996 Technische Universitaet Braunschweig, Germany.
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

char post_rcsid[] = 
    "$Id$";

#include "post.h"

#include "AppData.h"
#include "Command.h"
#include "Delay.h"
#include "DeleteWCB.h"
#include "DestroyCB.h"
#include "GDBAgent.h"
#include "HelpCB.h"
#include "TimeOut.h"
#include "assert.h"
#include "charsets.h"
#include "cmdtty.h"
#include "ddd.h"
#include "exit.h"
#include "findParent.h"
#include "session.h"
#include "string-fun.h"
#include "verify.h"
#include "wm.h"
#include "editing.h"

#ifdef IF_MOTIF
#include <Xm/Xm.h>
#include <Xm/AtomMgr.h>
#include <Xm/MessageB.h>
#endif // IF_MOTIF

#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

extern "C" {
#include <sys/types.h>
#include <sys/wait.h>
}



//=============================================================================
// Dialogs
//=============================================================================

//-----------------------------------------------------------------------------
// Yes/No dialogs
//-----------------------------------------------------------------------------

static DIALOG_P yn_dialog;
#ifndef IF_MOTIF
static LABEL_P yn_dialog_label = NULL;
#endif // IF_MOTIF

// Issue CLIENT_DATA as command and unmanage YN_DIALOG.
#ifdef IF_MOTIF
void YnCB(Widget dialog, XtPointer client_data, XtPointer call_data)
#else // NOT IF_MOTIF
void YnCB(CB_ALIST_12(Widget dialog, XtP(const char *) client_data))
#endif // IF_MOTIF
{
#if defined(IF_XM)
    gdbCommandCB1(dialog, client_data, call_data);
#else
    gdbCommandCB2(dialog, (const char *)client_data);
#endif

    unpost_gdb_yn();
}

Widget post_gdb_yn(string question, Widget w)
{
    strip_trailing_space(question);
    if (question.empty())
	return 0;

#ifdef IF_MOTIF
    Arg args[10];
    int arg;

    arg = 0;
#endif // IF_MOTIF
    MString mquestion = rm(question);
#ifdef IF_MOTIF
    XtSetArg(args[arg], XmNmessageString, mquestion.xmstring()); arg++;
#endif // IF_MOTIF

    if (yn_dialog == 0)
    {
#ifdef IF_MOTIF
	XtSetArg(args[arg], XmNdeleteResponse, XmDO_NOTHING); arg++;
	yn_dialog = verify(XmCreateQuestionDialog(find_shell(w),
						  XMST("yn_dialog"), 
						  args, arg));
#else // NOT IF_MOTIF
	yn_dialog = new Gtk::Dialog(XMST("yn_dialog"), *find_shell(w));
	yn_dialog_label = new Gtk::Label(mquestion.xmstring());
	yn_dialog->get_vbox()->pack_start(*yn_dialog_label, Gtk::PACK_SHRINK);
#endif // IF_MOTIF
	Delay::register_shell(yn_dialog);
#ifdef IF_MOTIF
	XtAddCallback (yn_dialog, XmNokCallback,     YnCB, XtPointer("yes"));
	XtAddCallback (yn_dialog, XmNcancelCallback, YnCB, XtPointer("no"));
	XtAddCallback (yn_dialog, XmNhelpCallback,   ImmediateHelpCB, 0);

	// If the dialog is closed, assume `no'.
	AddDeleteWindowCallback(XtParent(yn_dialog), YnCB, XtPointer("no"));
#else // NOT IF_MOTIF
	Gtk::Button *button;
	button = yn_dialog->add_button(XMST("OK"), 0);
	button->signal_clicked().connect(sigc::bind(PTR_FUN(YnCB), yn_dialog, "yes"));
	button = yn_dialog->add_button(XMST("Cancel"), 0);
	button->signal_clicked().connect(sigc::bind(PTR_FUN(YnCB), yn_dialog, "no"));
#ifdef NAG_ME
#warning What is the return value?
#endif
	yn_dialog->signal_delete_event().connect(sigc::bind_return(sigc::hide<0>(sigc::bind(PTR_FUN(YnCB), yn_dialog, "no")), false));
#endif // IF_MOTIF
    }
    else
    {
#ifdef IF_MOTIF
	XtSetValues(yn_dialog, args, arg);
#else // NOT IF_MOTIF
	yn_dialog_label->set_text(mquestion.xmstring());
#endif // IF_MOTIF
    }

    manage_and_raise(yn_dialog);
    return yn_dialog;
}

void unpost_gdb_yn()
{
    if (yn_dialog)
	XtUnmanageChild(yn_dialog);
}


//-----------------------------------------------------------------------------
// GDB is busy
//-----------------------------------------------------------------------------

static DIALOG_P busy_dialog = 0;
#ifndef IF_MOTIF
static LABEL_P *busy_dialog_label = 0;
#endif // IF_MOTIF

Widget post_gdb_busy(Widget w)
{
    if (ddd_is_exiting)
	return 0;

    if (busy_dialog == 0)
    {
#ifdef IF_MOTIF
	busy_dialog = 
	    verify(XmCreateWorkingDialog(find_shell(w), 
					 XMST("busy_dialog"), 
					 ArgList(0), 0));
#else // NOT IF_MOTIF
	busy_dialog = new Gtk::Dialog(XMST("busy_dialog"), *find_shell(w));
#endif // IF_MOTIF
	Delay::register_shell(busy_dialog);
#ifdef IF_MOTIF
	XtUnmanageChild(XmMessageBoxGetChild(busy_dialog, 
					     XmDIALOG_CANCEL_BUTTON));
	XtAddCallback(busy_dialog, XmNhelpCallback, 
		      ImmediateHelpCB, XtPointer(0));
#else // NOT IF_MOTIF
	Gtk::Button *button;
	button = busy_dialog->add_button(XMST("OK"), 0);
	button->signal_clicked().connect(sigc::bind(PTR_FUN(UnmanageThisCB2), busy_dialog));
#endif // IF_MOTIF
    }

    manage_and_raise(busy_dialog);
    return busy_dialog;
}

void unpost_gdb_busy()
{
    if (busy_dialog)
	XtUnmanageChild(busy_dialog);
}


//-----------------------------------------------------------------------------
// GDB terminated abnormally
//-----------------------------------------------------------------------------

Widget post_gdb_died(string reason, int state, Widget w)
{
    strip_trailing_space(reason);

    int exit_state = EXIT_FAILURE;
    if (state >= 0 && WIFEXITED(((state))))
	exit_state = WEXITSTATUS(((state)));

    bool exited = (exit_state == EXIT_SUCCESS || reason.contains("Exit 0"));

    if (gdb_initialized && gdb_is_exiting && exited)
    {
	// Exit was expected
	_DDDExitCB(CB_ARGS_2(EXIT_SUCCESS));
	return 0;
    }

    if (ddd_is_exiting)
    {
	// GDB would not die
	std::cerr << reason << "\n";
	return 0;
    }

#ifdef IF_MOTIF
    Arg args[10];
    int arg;
#endif // IF_MOTIF

    DIALOG_P dialog = 0;
    WINDOW_P shell = find_shell(w);
#ifndef IF_MOTIF
    Gtk::Button *button;
    Gtk::Label *label;
#endif // IF_MOTIF
    if (gdb_initialized)
    {
	const _XtString name;
	MString msg;

	if (gdb->has_exec_files() && exited)
	{
	    // In GDB, DBX, and XDB, a sudden exit is probably an error.
	    msg = rm(gdb->title() + " suddenly exited.");
	    name = "exited_dialog";
	}
	else if (exited)
	{
	    // In Perl, PYDB, and JDB, the debugger dies with the program.
	    _gdb_out("\n" + reason + "\n");

	    msg = rm("The program terminated (" + reason + ")");
	    name = "done_dialog";
	}
	else if (state < 0)
	{
	    // In JDB, the debugger may hang after raising an exception
	    _gdb_out("\n" + reason + "\n");

	    msg = rm(gdb->title() + " raised an internal exception");
	    name = "exception_dialog";
	}
	else
	{
	    _gdb_out("\n" + reason + "\n");

	    // Abnormal exits are always errors.
	    msg = rm(gdb->title() + " terminated abnormally (" + reason + ")");
	    name = "terminated_dialog";
	}

#ifdef IF_MOTIF
	arg = 0;
	XtSetArg(args[arg], XmNmessageString, msg.xmstring()); arg++;
	if (exited)
	    dialog = verify(XmCreateWarningDialog(shell, XMST(name), args, arg));
	else
	    dialog = verify(XmCreateErrorDialog(shell, XMST(name), args, arg));

	XtAddCallback(dialog, XmNhelpCallback,
		      ImmediateHelpCB, XtPointer(0));
	XtAddCallback(dialog, XmNokCallback,     
		      RestartDebuggerCB, XtPointer(0));
	XtAddCallback(dialog, XmNcancelCallback, 
		      DDDExitCB, XtPointer(exit_state));
#else // NOT IF_MOTIF
	dialog = new Gtk::Dialog(XMST(name), *shell);
#ifdef NAG_ME
#warning XmCreateWarningDialog vs XmCreateErrorDialog
#endif
	label = new Gtk::Label(msg.xmstring());
	label->show();
	dialog->get_vbox()->pack_start(*label, Gtk::PACK_SHRINK);
	button = dialog->add_button(XMST("OK"), 0);
	button->signal_clicked().connect(PTR_FUN(RestartDebuggerCB));
	button = dialog->add_button(XMST("Cancel"), 0);
	button->signal_clicked().connect(sigc::bind(PTR_FUN(DDDExitCB), dialog, exit_state));
#endif // IF_MOTIF
    }
    else
    {
	MString msg = rm(gdb->title() + " could not be started.");
#ifdef IF_MOTIF
	arg = 0;
	XtSetArg(args[arg], XmNmessageString, msg.xmstring()); arg++;
	dialog = verify(XmCreateErrorDialog(shell, 
					    XMST("no_debugger_dialog"), 
					    args, arg));
	XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_CANCEL_BUTTON));
	XtAddCallback(dialog, XmNhelpCallback, ImmediateHelpCB, XtPointer(0));
	XtAddCallback(dialog, XmNokCallback, DDDExitCB, XtPointer(exit_state));
#else // NOT IF_MOTIF
	dialog = new Gtk::Dialog(XMST("no_debugger_dialog"), *shell);
	label = new Gtk::Label(msg.xmstring());
	label->show();
	dialog->get_vbox()->pack_start(*label, Gtk::PACK_SHRINK);
	button = dialog->add_button(XMST("OK"), 0);
	button->signal_clicked().connect(sigc::bind(PTR_FUN(ExitCB), exit_state));
#endif // IF_MOTIF
    }

    Delay::register_shell(dialog);
    manage_and_raise(dialog);
    return dialog;
}


//-----------------------------------------------------------------------------
// GDB issued a message
//-----------------------------------------------------------------------------

struct PostInfo {
    string text;
    bool prompt;

    PostInfo(const string& t, bool p)
	: text(t), prompt(p)
    {}
};

#ifdef IF_MOTIF
static void GDBOutCB(XtPointer client_data, XtIntervalId *)
#else // NOT IF_MOTIF
static void GDBOutCB(PostInfo *client_data)
#endif // IF_MOTIF
{
    PostInfo *info = (PostInfo *)client_data;
    if (!info->text.empty())
    {
	gdb_out("\r" + info->text + "\n");
	if (info->prompt)
	    prompt();
    }

    delete info;
}

Widget post_gdb_message(string text, bool prompt, Widget w)
{
    strip_trailing_space(text);
    if (text == NO_GDB_ANSWER || text.empty())
	return 0;

    dddlog << "!  " << text << '\n';
    dddlog.flush();

    if (ddd_is_exiting)
    {
	if (!text.empty())
	    std::cerr << ddd_NAME << ": " << text << "\n";
	return 0;
    }

    if (gdb->isReadyWithPrompt())
    {
	// We don't output this immediately, because we might be in a
	// private input state (private_gdb_input or tty_gdb_input
	// might be set)
	PostInfo *info = new PostInfo(text, prompt);
#ifdef IF_MOTIF
	XtAppAddTimeOut(XtWidgetToApplicationContext(gdb_w), 0,
			GDBOutCB, XtPointer(info));
#else // NOT IF_MOTIF
	Glib::signal_idle().connect(sigc::bind_return(sigc::bind(PTR_FUN(GDBOutCB), info), false));
#endif // IF_MOTIF
	return 0;
    }

    MString mtext = rm(text);
#ifdef IF_MOTIF
    Arg args[10];
    int arg = 0;

    XtSetArg(args[arg], XmNmessageString, mtext.xmstring()); arg++;
#endif // IF_MOTIF

    static DIALOG_P gdb_message_dialog = 0;
#ifndef IF_MOTIF
    static LABEL_P gdb_message_dialog_label = 0;
#endif // IF_MOTIF
    if (gdb_message_dialog == 0)
    {
#ifdef IF_MOTIF
	gdb_message_dialog = 
	    verify(XmCreateWarningDialog(find_shell(w),
					 XMST("gdb_message_dialog"),
					 args, arg));
#else // NOT IF_MOTIF
	gdb_message_dialog = new Gtk::Dialog(XMST("gdb_message_dialog"), *find_shell(w));
	gdb_message_dialog_label = new Gtk::Label(mtext.xmstring());
	gdb_message_dialog_label->show();
	gdb_message_dialog->get_vbox()->pack_start(*gdb_message_dialog_label, Gtk::PACK_SHRINK);
#endif // IF_MOTIF
	Delay::register_shell(gdb_message_dialog);
#ifdef IF_MOTIF
	XtUnmanageChild(XmMessageBoxGetChild(gdb_message_dialog, 
					     XmDIALOG_CANCEL_BUTTON));
	XtAddCallback(gdb_message_dialog, XmNhelpCallback, 
		      ImmediateHelpCB, XtPointer(0));
#else // NOT IF_MOTIF
	Gtk::Button *button;
	button = gdb_message_dialog->add_button(XMST("OK"), 0);
	button->signal_clicked().connect(sigc::bind(PTR_FUN(UnmanageThisCB2), gdb_message_dialog));
#endif // IF_MOTIF
    }
    else
    {
#ifdef IF_MOTIF
	XtSetValues(gdb_message_dialog, args, arg);
#else // NOT IF_MOTIF
	gdb_message_dialog_label->set_text(mtext.xmstring());
#endif // IF_MOTIF
    }

    manage_and_raise(gdb_message_dialog);
    return gdb_message_dialog;
}


//-----------------------------------------------------------------------------
// DDD errors
//-----------------------------------------------------------------------------

DIALOG_P post_error(string text, const _XtString name, Widget w)
{
    strip_trailing_space(text);

    dddlog << "!  " << text << '\n';
    dddlog.flush();

    if (ddd_is_exiting)
    {
	std::cerr << ddd_NAME << ": " << text << "\n";
	return 0;
    }

#if 0
    if (status_w && !text.contains('\n'))
    {
	set_status(text);
	return 0;
    }
#endif

    if (name == 0)
	name = "ddd_error";

    MString mtext = rm(text);

#ifdef IF_MOTIF
    Arg args[10];
    int arg = 0;

    XtSetArg(args[arg], XmNmessageString, mtext.xmstring()); arg++;

    DIALOG_P ddd_error = 
	verify(XmCreateErrorDialog(find_shell(w), XMST(name), args, arg));
#else // NOT IF_MOTIF
    DIALOG_P ddd_error = 
	new Gtk::Dialog(XMST(name), *find_shell(w));
    LABEL_P ddd_error_label = new Gtk::Label(mtext.xmstring());
    ddd_error_label->show();
    ddd_error->get_vbox()->pack_start(*ddd_error_label, Gtk::PACK_SHRINK);
#endif // IF_MOTIF
    Delay::register_shell(ddd_error);
#ifdef IF_MOTIF
    XtUnmanageChild(XmMessageBoxGetChild(ddd_error, XmDIALOG_CANCEL_BUTTON));
    XtAddCallback(ddd_error, XmNhelpCallback, ImmediateHelpCB, XtPointer(0));
#else // NOT IF_MOTIF
    Gtk::Button *button;
    button = ddd_error->add_button(XMST("OK"), 0);
    button->signal_clicked().connect(sigc::bind(PTR_FUN(UnmanageThisCB2), ddd_error));
#endif // IF_MOTIF

    manage_and_raise(ddd_error);
    return ddd_error;
}


//-----------------------------------------------------------------------------
// DDD warnings
//-----------------------------------------------------------------------------

DIALOG_P post_warning(string text, const _XtString name, Widget w)
{
    strip_trailing_space(text);

    dddlog << "!  " << text << '\n';
    dddlog.flush();

    if (ddd_is_exiting)
    {
	std::cerr << ddd_NAME << ": warning: " << text << "\n";
	return 0;
    }

#if 0
    if (status_w && !text.contains('\n'))
    {
	set_status(text);
	return 0;
    }
#endif

    if (name == 0)
	name = "ddd_warning";

    MString mtext = rm(text);
#ifdef IF_MOTIF
    Arg args[10];
    int arg = 0;

    XtSetArg(args[arg], XmNmessageString, mtext.xmstring()); arg++;
#endif // IF_MOTIF

#ifdef IF_MOTIF
    DIALOG_P ddd_warning = 
	verify(XmCreateWarningDialog(find_shell(w), XMST(name), args, arg));
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning Should these dialogs be destroyed on close?
#endif
    DIALOG_P ddd_warning = 
	new Gtk::Dialog(XMST(name), *find_shell(w));
    LABEL_P ddd_warning_label = new Gtk::Label(mtext.xmstring());
    ddd_warning_label->show();
    ddd_warning->get_vbox()->pack_start(*ddd_warning_label, Gtk::PACK_SHRINK);
#endif // IF_MOTIF
    Delay::register_shell(ddd_warning);
#ifdef IF_MOTIF
    XtUnmanageChild(XmMessageBoxGetChild(ddd_warning, XmDIALOG_CANCEL_BUTTON));
    XtAddCallback(ddd_warning, XmNhelpCallback, ImmediateHelpCB, XtPointer(0));
#else // NOT IF_MOTIF
    Gtk::Button *button;
    button = ddd_warning->add_button(XMST("OK"), 0);
    button->signal_clicked().connect(sigc::bind(PTR_FUN(UnmanageThisCB2), ddd_warning));
#endif // IF_MOTIF

    manage_and_raise(ddd_warning);
    return ddd_warning;
}
