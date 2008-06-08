// $Id$ -*- C++ -*-
// Elementary DDD dialogs

// Copyright (C) 1996 Technische Universitaet Braunschweig, Germany.
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

char post_rcsid[] = 
    "$Id$";

#include "post.h"

#include "AppData.h"
#include "Command.h"
#include "Delay.h"
#include "DeleteWCB.h"
#include "DestroyCB.h"
#include "editing.h"
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

#if defined(IF_XM)
#include <Xm/Xm.h>
#include <Xm/AtomMgr.h>
#include <Xm/MessageB.h>
#else
#include <GUI/Dialog.h>
#include <GUI/Button.h>
#include <GUI/Label.h>
#endif

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

#if defined(IF_XM)
static Widget yn_dialog;
#else
static GUI::Dialog *yn_dialog;
static GUI::Label *yn_dialog_label = NULL;
#endif

#if defined(IF_XM)
// Issue CLIENT_DATA as command and unmanage YN_DIALOG.
void YnCB(Widget dialog, 
	  XtPointer client_data, 
	  XtPointer call_data)
{
    gdbCommandCB(dialog, client_data, call_data);
    unpost_gdb_yn();
}
#else
// Issue CLIENT_DATA as command and unmanage YN_DIALOG.
void YnCB(GUI::Widget *dialog, const char *command)
{
    gdbCommandCB(dialog, command);
    unpost_gdb_yn();
}
#endif

#if defined(IF_XM)
Widget post_gdb_yn(string question, Widget w)
{
    strip_trailing_space(question);
    if (question.empty())
	return 0;

    Arg args[10];
    int arg;

    arg = 0;
    MString mquestion = rm(question);
    XtSetArg(args[arg], XmNmessageString, mquestion.xmstring()); arg++;

    if (yn_dialog == 0)
    {
	XtSetArg(args[arg], XmNdeleteResponse, XmDO_NOTHING); arg++;
	yn_dialog = verify(XmCreateQuestionDialog(find_shell(w),
						  XMST("yn_dialog"), 
						  args, arg));
	Delay::register_shell(yn_dialog);
	XtAddCallback (yn_dialog, XmNokCallback,     YnCB, XtPointer("yes"));
	XtAddCallback (yn_dialog, XmNcancelCallback, YnCB, XtPointer("no"));
	XtAddCallback (yn_dialog, XmNhelpCallback,   ImmediateHelpCB, 0);

	// If the dialog is closed, assume `no'.
	AddDeleteWindowCallback(XtParent(yn_dialog), YnCB, XtPointer("no"));
    }
    else
    {
	XtSetValues(yn_dialog, args, arg);
    }

    manage_and_raise(yn_dialog);
    return yn_dialog;
}
#else
GUI::Dialog *post_gdb_yn(string question, GUI::Widget *w)
{
    strip_trailing_space(question);
    if (question.empty())
	return 0;

    if (yn_dialog == 0)
    {
	yn_dialog = new GUI::Dialog(*find_shell(w), "yn_dialog");
	yn_dialog_label = new GUI::Label(*yn_dialog, GUI::PACK_SHRINK, question.chars());
	yn_dialog_label->show();
	Delay::register_shell(yn_dialog);
	GUI::Button *button;
	button = yn_dialog->add_button("ok", _("Yes"));
	button->signal_clicked().connect(sigc::bind(sigc::ptr_fun(YnCB), yn_dialog, "yes"));
	button->show();
	button = yn_dialog->add_button("cancel", _("No"));
	button->signal_clicked().connect(sigc::bind(sigc::ptr_fun(YnCB), yn_dialog, "no"));
	button->show();
	// Hide the signal argument (the event structure).
	// Return false to allow the event to propagate further.
	yn_dialog->signal_delete_event().connect(
	    sigc::bind_return(sigc::hide(sigc::bind(sigc::ptr_fun(YnCB),
						    yn_dialog, "no")),
			      false));
    }
    else
    {
	yn_dialog_label->set_text(question.chars());
    }

    manage_and_raise(yn_dialog);
    return yn_dialog;
}
#endif

void unpost_gdb_yn()
{
#if defined(IF_XM)
    if (yn_dialog)
	XtUnmanageChild(yn_dialog);
#else
    if (yn_dialog)
	yn_dialog->hide();
#endif
}


//-----------------------------------------------------------------------------
// GDB is busy
//-----------------------------------------------------------------------------

#if defined(IF_XM)
static Widget busy_dialog = 0;
#else
static GUI::Dialog *busy_dialog = 0;
static GUI::Label *busy_dialog_label = 0;
#endif

#if defined(IF_XM)
Widget post_gdb_busy(Widget w)
{
    if (ddd_is_exiting)
	return 0;

    if (busy_dialog == 0)
    {
	busy_dialog = 
	    verify(XmCreateWorkingDialog(find_shell(w), 
					 XMST("busy_dialog"), 
					 ArgList(0), 0));
	Delay::register_shell(busy_dialog);
	XtUnmanageChild(XmMessageBoxGetChild(busy_dialog, 
					     XmDIALOG_CANCEL_BUTTON));
	XtAddCallback(busy_dialog, XmNhelpCallback, 
		      ImmediateHelpCB, XtPointer(0));
    }

    manage_and_raise(busy_dialog);
    return busy_dialog;
}
#else
GUI::Dialog *post_gdb_busy(GUI::Widget *w)
{
    if (ddd_is_exiting)
	return 0;

    if (busy_dialog == 0)
    {
	busy_dialog = new GUI::Dialog(*find_shell(w), "busy_dialog");
	Delay::register_shell(busy_dialog);
	GUI::Button *button;
	button = busy_dialog->add_button("ok", _("OK"));
	button->signal_clicked().connect(sigc::bind(sigc::ptr_fun(UnmanageThisCB), busy_dialog));
    }

    manage_and_raise(busy_dialog);
    return busy_dialog;
}
#endif

void unpost_gdb_busy()
{
#if defined(IF_XM)
    if (busy_dialog)
	XtUnmanageChild(busy_dialog);
#else
    if (busy_dialog)
	busy_dialog->hide();
#endif
}


//-----------------------------------------------------------------------------
// GDB terminated abnormally
//-----------------------------------------------------------------------------

#if defined(IF_XM)
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
	_DDDExitCB(find_shell(w), XtPointer(EXIT_SUCCESS), 0);
	return 0;
    }

    if (ddd_is_exiting)
    {
	// GDB would not die
	std::cerr << reason << "\n";
	return 0;
    }

    Arg args[10];
    int arg;

    Widget dialog = 0;
    Widget shell = find_shell(w);
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
    }
    else
    {
	arg = 0;
	MString msg = rm(gdb->title() + " could not be started.");
	XtSetArg(args[arg], XmNmessageString, msg.xmstring()); arg++;
	dialog = verify(XmCreateErrorDialog(shell, 
					    XMST("no_debugger_dialog"), 
					    args, arg));
	XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_CANCEL_BUTTON));
	XtAddCallback(dialog, XmNhelpCallback, ImmediateHelpCB, XtPointer(0));
	XtAddCallback(dialog, XmNokCallback, DDDExitCB, XtPointer(exit_state));
    }

    Delay::register_shell(dialog);
    manage_and_raise(dialog);
    return dialog;
}
#else
GUI::Dialog *post_gdb_died(string reason, int state, GUI::Widget *w)
{
    strip_trailing_space(reason);

    int exit_state = EXIT_FAILURE;
    if (state >= 0 && WIFEXITED(((state))))
	exit_state = WEXITSTATUS(((state)));

    bool exited = (exit_state == EXIT_SUCCESS || reason.contains("Exit 0"));

    if (gdb_initialized && gdb_is_exiting && exited)
    {
	// Exit was expected
#if defined(IF_XM)
	_DDDExitCB(find_shell(w), XtPointer(EXIT_SUCCESS), 0);
#else
	_DDDExitCB(EXIT_SUCCESS);
#endif
	return 0;
    }

    if (ddd_is_exiting)
    {
	// GDB would not die
	std::cerr << reason << "\n";
	return 0;
    }


    GUI::Dialog *dialog = 0;
    GUI::Shell *shell = find_shell(w);
    GUI::Button *button;
    GUI::Label *label;
    if (gdb_initialized)
    {
	const char *name;
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

	dialog = new GUI::Dialog(*shell, name);
#ifdef NAG_ME
#warning XmCreateWarningDialog vs XmCreateErrorDialog
#endif
	label = new GUI::Label(*dialog, GUI::PACK_SHRINK, msg.xmstring());
	label->show();
	button = dialog->add_button("ok", _("Restart @GDB@"));
	button->signal_clicked().connect(sigc::ptr_fun(RestartDebuggerCB));
	button = dialog->add_button("cancel", _("Exit"));
	button->signal_clicked().connect(sigc::bind(sigc::ptr_fun(DDDExitCB), dialog, exit_state));
    }
    else
    {
	MString msg = rm(gdb->title() + " could not be started.");
	dialog = new GUI::Dialog(*shell, "no_debugger_dialog");
	label = new GUI::Label(*dialog, GUI::PACK_SHRINK, msg.xmstring());
	label->show();
	button = dialog->add_button("ok", _("Exit"));
	button->signal_clicked().connect(sigc::bind(sigc::ptr_fun(ExitCB), exit_state));
    }

    Delay::register_shell(dialog);
    manage_and_raise(dialog);
    return dialog;
}
#endif


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

#if defined(IF_XM)
static void GDBOutCB(XtPointer client_data, XtIntervalId *)
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
#else
static bool GDBOutCB(PostInfo *info)
{
    if (!info->text.empty())
    {
	gdb_out("\r" + info->text + "\n");
	if (info->prompt)
	    prompt();
    }

    delete info;
    return false;
}
#endif

#if defined(IF_XM)
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
	XtAppAddTimeOut(XtWidgetToApplicationContext(gdb_w), 0,
			GDBOutCB, XtPointer(info));
	return 0;
    }

    Arg args[10];
    int arg = 0;

    MString mtext = rm(text);
    XtSetArg(args[arg], XmNmessageString, mtext.xmstring()); arg++;

    static Widget gdb_message_dialog = 0;
    if (gdb_message_dialog == 0)
    {
	gdb_message_dialog = 
	    verify(XmCreateWarningDialog(find_shell(w),
					 XMST("gdb_message_dialog"),
					 args, arg));
	Delay::register_shell(gdb_message_dialog);
	XtUnmanageChild(XmMessageBoxGetChild(gdb_message_dialog, 
					     XmDIALOG_CANCEL_BUTTON));
	XtAddCallback(gdb_message_dialog, XmNhelpCallback, 
		      ImmediateHelpCB, XtPointer(0));
    }
    else
    {
	XtSetValues(gdb_message_dialog, args, arg);
    }

    manage_and_raise(gdb_message_dialog);
    return gdb_message_dialog;
}
#else
GUI::Dialog *post_gdb_message(string text, bool prompt, GUI::Widget *w)
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
	GUI::signal_idle().connect(sigc::bind(sigc::ptr_fun(GDBOutCB), info));
	return 0;
    }

    MString mtext = rm(text);

    static GUI::Dialog *gdb_message_dialog = 0;
    static GUI::Label *gdb_message_dialog_label = 0;
    if (gdb_message_dialog == 0)
    {
	gdb_message_dialog = new GUI::Dialog(*find_shell(w), "gdb_message_dialog");
	gdb_message_dialog_label = new GUI::Label(*gdb_message_dialog, GUI::PACK_SHRINK, mtext.xmstring());
	gdb_message_dialog_label->show();
	Delay::register_shell(gdb_message_dialog);
	GUI::Button *button;
	button = gdb_message_dialog->add_button("ok", _("OK"));
	button->signal_clicked().connect(sigc::bind(sigc::ptr_fun(UnmanageThisCB), gdb_message_dialog));
    }
    else
    {
	gdb_message_dialog_label->set_text(mtext.xmstring());
    }

    manage_and_raise(gdb_message_dialog);
    return gdb_message_dialog;
}
#endif


//-----------------------------------------------------------------------------
// DDD errors
//-----------------------------------------------------------------------------

#if defined(IF_XM)
Widget post_error(string text, const _XtString name, Widget w)
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

    Arg args[10];
    int arg = 0;

    MString mtext = rm(text);
    XtSetArg(args[arg], XmNmessageString, mtext.xmstring()); arg++;

    Widget ddd_error = 
	verify(XmCreateErrorDialog(find_shell(w), XMST(name), args, arg));
    Delay::register_shell(ddd_error);
    XtUnmanageChild(XmMessageBoxGetChild(ddd_error, XmDIALOG_CANCEL_BUTTON));
    XtAddCallback(ddd_error, XmNhelpCallback, ImmediateHelpCB, XtPointer(0));

    manage_and_raise(ddd_error);
    return ddd_error;
}
#else
GUI::Dialog *post_error(string text, const char *name, GUI::Widget *w)
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

    GUI::Dialog *ddd_error = 
	new GUI::Dialog(*find_shell(w), name);
    GUI::Label *ddd_error_label = new GUI::Label(*ddd_error, GUI::PACK_SHRINK, mtext.xmstring());
    ddd_error_label->show();
    Delay::register_shell(ddd_error);
    GUI::Button *button;
    button = ddd_error->add_button("ok", _("OK"));
    button->signal_clicked().connect(sigc::bind(sigc::ptr_fun(UnmanageThisCB), ddd_error));

    manage_and_raise(ddd_error);
    return ddd_error;
}
#endif


//-----------------------------------------------------------------------------
// DDD warnings
//-----------------------------------------------------------------------------

#if defined(IF_XM)
Widget post_warning(string text, const _XtString name, Widget w)
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

    Arg args[10];
    int arg = 0;

    MString mtext = rm(text);
    XtSetArg(args[arg], XmNmessageString, mtext.xmstring()); arg++;

    Widget ddd_warning = 
	verify(XmCreateWarningDialog(find_shell(w), XMST(name), args, arg));
    Delay::register_shell(ddd_warning);
    XtUnmanageChild(XmMessageBoxGetChild(ddd_warning, XmDIALOG_CANCEL_BUTTON));
    XtAddCallback(ddd_warning, XmNhelpCallback, ImmediateHelpCB, XtPointer(0));

    manage_and_raise(ddd_warning);
    return ddd_warning;
}
#else
GUI::Dialog *post_warning(string text, const char *name, GUI::Widget *w)
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

#ifdef NAG_ME
#warning Should these dialogs be destroyed on close?
#endif
    GUI::Dialog *ddd_warning = 
	new GUI::Dialog(*find_shell(w), name);
    GUI::Label *ddd_warning_label = new GUI::Label(*ddd_warning, GUI::PACK_SHRINK, mtext.xmstring());
    ddd_warning_label->show();

    Delay::register_shell(ddd_warning);
    GUI::Button *button;
    button = ddd_warning->add_button("ok", _("OK"));
    button->signal_clicked().connect(sigc::bind(sigc::ptr_fun(UnmanageThisCB), ddd_warning));

    manage_and_raise(ddd_warning);
    return ddd_warning;
}
#endif
