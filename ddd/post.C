// $Id$ -*- C++ -*-
// Elementary DDD dialogs

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

const char post_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "post.h"
#include "verify.h"
#include "ddd.h"
#include "Delay.h"
#include "HelpCB.h"
#include "DestroyCB.h"
#include "commandQ.h"
#include "exit.h"
#include "AppData.h"
#include "string-fun.h"
#include "GDBAgent.h"

#include <Xm/Xm.h>
#include <Xm/MessageB.h>


//-----------------------------------------------------------------------------
// Dialogs
//-----------------------------------------------------------------------------

Widget yn_dialog;

// Issue CLIENT_DATA as command and unmanage YN_DIALOG.
void YnCB(Widget dialog, 
	  XtPointer client_data, 
	  XtPointer call_data)
{
    gdbCommandCB(dialog, client_data, call_data);
    if (yn_dialog)
	XtUnmanageChild(yn_dialog);
}

void post_gdb_yn(string question, Widget w)
{
    strip_final_blanks(question);
    if (question == "")
	return;

    if (yn_dialog)
	DestroyWhenIdle(yn_dialog);
    yn_dialog = verify(XmCreateQuestionDialog(find_shell(w),
					      "yn_dialog", NULL, 0));
    Delay::register_shell(yn_dialog);
    XtAddCallback (yn_dialog, XmNokCallback,     YnCB, (void *)"yes");
    XtAddCallback (yn_dialog, XmNcancelCallback, YnCB, (void *)"no");
    XtAddCallback (yn_dialog, XmNhelpCallback,   ImmediateHelpCB, 0);

    XmString xmtext = XmStringCreateLtoR (question, "rm");
    XtVaSetValues (yn_dialog, XmNmessageString, xmtext, NULL);
    XmStringFree (xmtext);

    XtManageChild (yn_dialog);
}

void post_gdb_busy(Widget w)
{
    if (ddd_is_exiting)
	return;

    static Widget busy_dialog = 0;
    if (busy_dialog)
	DestroyWhenIdle(busy_dialog);

    busy_dialog = 
	verify(XmCreateWorkingDialog (find_shell(w), 
				      "busy_dialog", NULL, 0));
    Delay::register_shell(busy_dialog);
    XtUnmanageChild(XmMessageBoxGetChild 
		    (busy_dialog, XmDIALOG_CANCEL_BUTTON));
    XtAddCallback(busy_dialog, XmNhelpCallback, ImmediateHelpCB, NULL);
    XtManageChild(busy_dialog);
}

void post_gdb_died(string reason, Widget w)
{
    strip_final_blanks(reason);

    if (gdb_initialized && reason.contains("Exit 0"))
    {
	_DDDExitCB(find_shell(w), 0, 0);
	return;
    }

    if (ddd_is_exiting)
    {
	cerr << reason << "\n";
	return;
    }

    string gdb_path = app_data.debugger_command;
    if (gdb_path.contains(" "))
	gdb_path = gdb_path.before(" ");
    _gdb_out("\n" + gdb_path + ": " + reason + "\n");

    static Widget died_dialog = 0;
    if (died_dialog)
	DestroyWhenIdle(died_dialog);
    died_dialog = 
	verify(XmCreateErrorDialog (find_shell(w), 
				    "terminated_dialog", NULL, 0));
    Delay::register_shell(died_dialog);

    XtAddCallback (died_dialog, XmNhelpCallback,   ImmediateHelpCB, NULL);
    XtAddCallback (died_dialog, XmNokCallback,     DDDRestartCB,    NULL);
    XtAddCallback (died_dialog, XmNcancelCallback, DDDExitCB,       NULL);

    XtManageChild (died_dialog);
}

void post_gdb_message(string text, Widget w)
{
    strip_final_blanks(text);
    if (text == "")
	return;

    if (ddd_is_exiting)
    {
	cerr << ddd_NAME << ": " << text << "\n";
	return;
    }

    if (gdb->isReadyWithPrompt())
    {
	gdb_out("\r" + text + "\n" + gdb->default_prompt());
	return;
    }

    static Widget gdb_message_dialog = 0;
    if (gdb_message_dialog)
	DestroyWhenIdle(gdb_message_dialog);
    gdb_message_dialog = 
	verify(XmCreateWarningDialog (find_shell(w), 
				      "gdb_message_dialog", NULL, 0));
    Delay::register_shell(gdb_message_dialog);
    XtUnmanageChild (XmMessageBoxGetChild 
		     (gdb_message_dialog, XmDIALOG_CANCEL_BUTTON));
    XtAddCallback (gdb_message_dialog,
		   XmNhelpCallback,
		   ImmediateHelpCB,
		   NULL);

    MString mtext(text, "rm");
    XtVaSetValues (gdb_message_dialog,
		   XmNmessageString, mtext.xmstring(),
		   NULL);

    XtManageChild (gdb_message_dialog);
}

void post_error (string text, String name, Widget w)
{
    strip_final_blanks(text);
    if (ddd_is_exiting)
    {
	cerr << ddd_NAME << ": " << text << "\n";
	return;
    }

#if 0
    if (status_w && !text.contains('\n'))
    {
	set_status(text);
	return;
    }
#endif

    if (name == 0)
	name = "ddd_error";

    static Widget ddd_error = 0;
    if (ddd_error)
	DestroyWhenIdle(ddd_error);
    ddd_error = 
	verify(XmCreateErrorDialog (find_shell(w), name, NULL, 0));
    Delay::register_shell(ddd_error);
    XtUnmanageChild (XmMessageBoxGetChild 
		     (ddd_error, XmDIALOG_CANCEL_BUTTON));
    XtAddCallback (ddd_error, XmNhelpCallback, ImmediateHelpCB, NULL);

    MString mtext(text, "rm");
    XtVaSetValues (ddd_error,
		   XmNmessageString, mtext.xmstring(),
		   NULL);

    XtManageChild (ddd_error);
}

void post_warning (string text, String name, Widget w)
{
    strip_final_blanks(text);
    if (ddd_is_exiting)
    {
	cerr << ddd_NAME << ": warning: " << text << "\n";
	return;
    }

#if 0
    if (status_w && !text.contains('\n'))
    {
	set_status(text);
	return;
    }
#endif

    if (name == 0)
	name = "ddd_warning";

    static Widget ddd_warning = 0;
    if (ddd_warning)
	DestroyWhenIdle(ddd_warning);
    ddd_warning = 
	verify(XmCreateWarningDialog (find_shell(w), name, NULL, 0));
    Delay::register_shell(ddd_warning);
    XtUnmanageChild (XmMessageBoxGetChild 
		     (ddd_warning, XmDIALOG_CANCEL_BUTTON));
    XtAddCallback (ddd_warning, XmNhelpCallback, ImmediateHelpCB, NULL);

    MString mtext(text, "rm");
    XtVaSetValues (ddd_warning,
		   XmNmessageString, mtext.xmstring(),
		   NULL);

    XtManageChild (ddd_warning);
}
