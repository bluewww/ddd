// $Id$ -*- C++ -*-
// Elementary DDD dialogs

// Copyright (C) 1996 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@ips.cs.tu-bs.de>.
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

char post_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "post.h"

#include "AppData.h"
#include "Delay.h"
#include "DestroyCB.h"
#include "GDBAgent.h"
#include "HelpCB.h"
#include "TimeOut.h"
#include "charsets.h"
#include "Command.h"
#include "ddd.h"
#include "exit.h"
#include "string-fun.h"
#include "verify.h"
#include "wm.h"

#include <Xm/Xm.h>
#include <Xm/MessageB.h>
#include <Xm/Protocols.h>

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

static Widget yn_dialog;

// Issue CLIENT_DATA as command and unmanage YN_DIALOG.
void YnCB(Widget dialog, 
	  XtPointer client_data, 
	  XtPointer call_data)
{
    gdbCommandCB(dialog, client_data, call_data);
    unpost_gdb_yn();
}

Widget post_gdb_yn(string question, Widget w)
{
    strip_final_blanks(question);
    if (question == "")
	return 0;

    if (yn_dialog)
	DestroyWhenIdle(yn_dialog);

    Arg args[10];
    int arg;

    arg = 0;
    XtSetArg(args[arg], XmNdeleteResponse, XmDO_NOTHING); arg++;
    MString mquestion = rm(question);
    XtSetArg(args[arg], XmNmessageString, mquestion.xmstring()); arg++;

    yn_dialog = verify(XmCreateQuestionDialog(find_shell(w),
					      "yn_dialog", args, arg));
    Delay::register_shell(yn_dialog);
    XtAddCallback (yn_dialog, XmNokCallback,     YnCB, (void *)"yes");
    XtAddCallback (yn_dialog, XmNcancelCallback, YnCB, (void *)"no");
    XtAddCallback (yn_dialog, XmNhelpCallback,   ImmediateHelpCB, 0);

    // If the dialog is closed, assume `no'.
    Atom WM_DELETE_WINDOW =
	XmInternAtom(XtDisplay(yn_dialog), "WM_DELETE_WINDOW", False);
    XmAddWMProtocolCallback(XtParent(yn_dialog), WM_DELETE_WINDOW, 
			    YnCB, (caddr_t)"no");

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

static Widget busy_dialog = 0;

Widget post_gdb_busy(Widget w)
{
    if (ddd_is_exiting)
	return 0;

    if (busy_dialog)
	DestroyWhenIdle(busy_dialog);

    busy_dialog = verify(XmCreateWorkingDialog(find_shell(w), "busy_dialog", 
					       NULL, 0));
    Delay::register_shell(busy_dialog);
    XtUnmanageChild(XmMessageBoxGetChild(busy_dialog, XmDIALOG_CANCEL_BUTTON));
    XtAddCallback(busy_dialog, XmNhelpCallback, ImmediateHelpCB, NULL);

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

Widget post_gdb_died(string reason, int gdb_status, Widget w)
{
    strip_final_blanks(reason);

    int exit_status = EXIT_FAILURE;
    if (WIFEXITED(gdb_status))
	exit_status = WEXITSTATUS(gdb_status);

    if (gdb_initialized 
	&& (exit_status == EXIT_SUCCESS || reason.contains("Exit 0")))
    {
	_DDDExitCB(find_shell(w), XtPointer(EXIT_SUCCESS), 0);
	return 0;
    }

    if (ddd_is_exiting)
    {
	cerr << reason << "\n";
	return 0;
    }

    Arg args[10];
    int arg;

    static Widget died_dialog = 0;
    if (died_dialog)
	DestroyWhenIdle(died_dialog);

    if (gdb_initialized)
    {
	_gdb_out(reason + "\n");

	arg = 0;
	MString msg = rm(gdb->title() + " terminated abnormally"
			 + " (" + reason + ")");
	XtSetArg(args[arg], XmNmessageString, msg.xmstring()); arg++;
	died_dialog = 
	    verify(XmCreateErrorDialog (find_shell(w), 
					"terminated_dialog", args, arg));
	XtAddCallback(died_dialog, XmNhelpCallback,   ImmediateHelpCB, NULL);
	XtAddCallback(died_dialog, XmNokCallback,
		      DDDExitCB, XtPointer(exit_status));
	XtAddCallback(died_dialog, XmNcancelCallback,
		      DDDRestartCB, XtPointer(exit_status));
    }
    else
    {
	arg = 0;
	MString msg = rm(gdb->title() + " could not be started.");
	XtSetArg(args[arg], XmNmessageString, msg.xmstring()); arg++;
	died_dialog = 
	    verify(XmCreateErrorDialog (find_shell(w), 
					"no_debugger_dialog", args, arg));
	XtUnmanageChild(XmMessageBoxGetChild
			(died_dialog, XmDIALOG_CANCEL_BUTTON));
	XtAddCallback(died_dialog, XmNhelpCallback,   ImmediateHelpCB, NULL);
	XtAddCallback(died_dialog, XmNokCallback,
		      DDDExitCB, XtPointer(exit_status));
    }

    Delay::register_shell(died_dialog);
    manage_and_raise(died_dialog);
    return died_dialog;
}


//-----------------------------------------------------------------------------
// GDB issued a message
//-----------------------------------------------------------------------------

static void GDBOutCB(XtPointer client_data, XtIntervalId *)
{
    string *text_ptr = (string *)client_data;
    gdb_out("\r" + *text_ptr + "\n" + gdb->prompt());
    delete text_ptr;
}

Widget post_gdb_message(string text, Widget w)
{
    strip_final_blanks(text);
    if (text == "" || text == NO_GDB_ANSWER)
	return 0;

    if (ddd_is_exiting)
    {
	cerr << ddd_NAME << ": " << text << "\n";
	return 0;
    }

    if (gdb->isReadyWithPrompt())
    {
	// We don't output this immediately, because we might be in a
	// private input state (private_gdb_input or tty_gdb_input
	// might be set)
	string *text_ptr = new string(text);
	XtAppAddTimeOut(XtWidgetToApplicationContext(gdb_w), 0,
			GDBOutCB, XtPointer(text_ptr));
	return 0;
    }

    static Widget gdb_message_dialog = 0;
    if (gdb_message_dialog)
	DestroyWhenIdle(gdb_message_dialog);


    Arg args[10];
    int arg = 0;

    MString mtext = rm(text);
    XtSetArg(args[arg], XmNmessageString, mtext.xmstring()); arg++;

    gdb_message_dialog = 
	verify(XmCreateWarningDialog(find_shell(w), "gdb_message_dialog",
				     args, arg));
    Delay::register_shell(gdb_message_dialog);
    XtUnmanageChild(XmMessageBoxGetChild(gdb_message_dialog, 
					 XmDIALOG_CANCEL_BUTTON));
    XtAddCallback(gdb_message_dialog, XmNhelpCallback, ImmediateHelpCB, NULL);

    manage_and_raise(gdb_message_dialog);
    return gdb_message_dialog;
}


//-----------------------------------------------------------------------------
// DDD errors
//-----------------------------------------------------------------------------

Widget post_error(string text, String name, Widget w)
{
    strip_final_blanks(text);
    if (ddd_is_exiting)
    {
	cerr << ddd_NAME << ": " << text << "\n";
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

    static Widget ddd_error = 0;
    if (ddd_error)
	DestroyWhenIdle(ddd_error);

    Arg args[10];
    int arg = 0;

    MString mtext = rm(text);
    XtSetArg(args[arg], XmNmessageString, mtext.xmstring()); arg++;

    ddd_error = verify(XmCreateErrorDialog(find_shell(w), name, args, arg));
    Delay::register_shell(ddd_error);
    XtUnmanageChild(XmMessageBoxGetChild(ddd_error, XmDIALOG_CANCEL_BUTTON));
    XtAddCallback(ddd_error, XmNhelpCallback, ImmediateHelpCB, NULL);

    manage_and_raise(ddd_error);
    return ddd_error;
}


//-----------------------------------------------------------------------------
// DDD warnings
//-----------------------------------------------------------------------------

Widget post_warning(string text, String name, Widget w)
{
    strip_final_blanks(text);
    if (ddd_is_exiting)
    {
	cerr << ddd_NAME << ": warning: " << text << "\n";
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

    static Widget ddd_warning = 0;
    if (ddd_warning)
	DestroyWhenIdle(ddd_warning);

    Arg args[10];
    int arg = 0;

    MString mtext = rm(text);
    XtSetArg(args[arg], XmNmessageString, mtext.xmstring()); arg++;

    ddd_warning = 
	verify(XmCreateWarningDialog(find_shell(w), name, args, arg));
    Delay::register_shell(ddd_warning);
    XtUnmanageChild(XmMessageBoxGetChild(ddd_warning, XmDIALOG_CANCEL_BUTTON));
    XtAddCallback(ddd_warning, XmNhelpCallback, ImmediateHelpCB, NULL);

    manage_and_raise(ddd_warning);
    return ddd_warning;
}
