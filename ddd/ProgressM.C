// $Id$ -*- C++ -*-
// Data Display progress meter

// Copyright (C) 1998 Technische Universitaet Braunschweig, Germany.
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

char ProgressMeter_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "ProgressM.h"

#include "Command.h"
#include "DispValue.h"
#include "charsets.h"
#include "ddd.h"
#include "string-fun.h"
#include "verify.h"
#include "version.h"
#include "wm.h"

#include <Xm/Scale.h>
#include <Xm/MessageB.h>

// Update progress meter every UPDATE_THRESHOLD characters
const int ProgressMeter::UPDATE_THRESHOLD = 256;

// Popup working dialog when updating from at least DIALOG_THRESHOLD chars.
const int ProgressMeter::DIALOG_THRESHOLD = 4096;

Widget ProgressMeter::dialog = 0;
Widget ProgressMeter::scale  = 0;

ProgressMeter *ProgressMeter::active = 0;

ProgressMeter::ProgressMeter(const string& _msg)
    : current(0), base(0), total(0), msg(_msg),
      delay(_msg), last_shown(0),
      old_background(DispValue::background),
      aborted(false)
{
    DispValue::background = _process;
    active = (ProgressMeter *)this;

    if (dialog == 0)
    {
	Arg args[10];
	Cardinal arg = 0;
	XtSetArg(args[arg], XmNdialogStyle, 
		 XmDIALOG_FULL_APPLICATION_MODAL); arg++;
	dialog = verify(XmCreateWorkingDialog(find_shell(), 
					      "update_displays_dialog", 
					      args, arg));
	XtUnmanageChild(XmMessageBoxGetChild(dialog, 
					     XmDIALOG_OK_BUTTON));
	XtUnmanageChild(XmMessageBoxGetChild(dialog, 
					     XmDIALOG_HELP_BUTTON));

	arg = 0;
	XtSetArg(args[arg], XmNorientation, XmHORIZONTAL); arg++;
	XtSetArg(args[arg], XmNeditable, False);           arg++;
#if XmVersion >= 2000
	XtSetArg(args[arg], XmNslidingMode, XmTHERMOMETER); arg++;
#endif
	scale = verify(XmCreateScale(dialog, "scale", args, arg));
	XtManageChild(scale);
    }
}

ProgressMeter::~ProgressMeter()
{
    DispValue::background = old_background;
    if (active == this)
	active = 0;

    if (aborted)
	delay.outcome = "aborted";
    XtRemoveCallback(dialog, XmNcancelCallback, CancelCB, 
		     XtPointer(&aborted));
    XtUnmanageChild(dialog);
}

void ProgressMeter::CancelCB(Widget, XtPointer client_data, XtPointer)
{
    bool *flag = (bool *)client_data;
    *flag = true;
}

bool ProgressMeter::_process(int remaining_length)
{
    return active->process(remaining_length);
}

bool ProgressMeter::process(int remaining_length)
{
    int processed = base + current - remaining_length;

#if 0
    clog << "Processed " << processed << "/" <<  total << " characters\n";
#endif

    if (!aborted && total >= DIALOG_THRESHOLD && !XtIsManaged(dialog))
    {
	MString mmsg = rm(msg + "...");
	XtVaSetValues(dialog, XmNmessageString, mmsg.xmstring(), NULL);
	string title = DDD_NAME ": " + capitalize(msg);
	XtVaSetValues(XtParent(dialog), XmNtitle, (char *)title, NULL);
	XtAddCallback(dialog, XmNcancelCallback, CancelCB, 
		      XtPointer(&aborted));
	manage_and_raise(dialog);
    }

    if (processed - last_shown >= UPDATE_THRESHOLD)
    {
	// Another bunch of characters processed.  Wow!
	int percent = (processed * 100) / total;

	if (XtIsManaged(dialog))
	    XmScaleSetValue(scale, percent);
	else
	    set_status(msg + "... (" + itostring(percent) + "% processed)", 
		       true);

	last_shown = processed;
    }

    // Interrupt if emergency
    if (!aborted && process_emergencies())
	aborted = true;

    // If we have a dialog, process its events
    // (The dialog is modal, such that we cannot be called recursively)
    if (!aborted && XtIsManaged(dialog))
	process_pending_events();

    return aborted;
}
