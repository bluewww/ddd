// $Id$ -*- C++ -*-
//

// Copyright (C) 1997 Technische Universitaet Braunschweig, Germany.
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

char attach_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "attach.h"

#include "AppData.h"
#include "HelpCB.h"
#include "StringA.h"
#include "commandQ.h"
#include "ddd.h"
#include "mydialogs.h"
#include "shell.h"
#include "status.h"
#include "string-fun.h"
#include "verify.h"
#include "wm.h"

#include <Xm/Xm.h>
#include <Xm/List.h>
#include <Xm/SelectioB.h>
#include <Xm/Text.h>

#include <sys/types.h>
#include <signal.h>

#include <ctype.h>

static Widget attach_dialog;
static Widget processes_w;

// Create list of processes
static void update_processes()
{
    StatusDelay delay("Getting list of processes");

    FILE *fp = popen(sh_command(app_data.ps_command) + " 2>&1", "r");

    StringArray processes;
    int c;
    string line = "";

    while ((c = getc(fp)) != EOF)
    {
	if (c == '\n')
	{
	    // Validate line: if we can send a signal to a process, we
	    // can also attach GDB to it.
	    int pid = atoi(line);
	    if (pid == 0 || remote_gdb() || kill(pid, 0) == 0)
		processes += line;

	    line = "";
	}
	else
	{
	    line += c;
	}
    }

    pclose(fp);

    bool *selected = new bool[processes.size()];
    for (int i = 0; i < processes.size(); i++)
	selected[i] = false;

    setLabelList(processes_w, processes.values(),
		 selected, processes.size(), true, false);

    delete[] selected;
}

static void gdbUpdateProcessesCB(Widget, XtPointer, XtPointer)
{
    update_processes();
}

// Attach process
static void gdbAttachDCB(Widget, XtPointer, XtPointer)
{
    IntArray pids;
    
    getDisplayNumbers(processes_w, pids);

    if (pids.size() == 1)
	gdb_command("attach " + itostring(pids[0]), attach_dialog);
}

// Create `Attach' dialog
void gdbAttachCB(Widget w, XtPointer, XtPointer)
{
    if (attach_dialog == 0)
    {
	Arg args[10];
	int arg = 0;

	attach_dialog = 
	    verify(XmCreateSelectionDialog(find_shell(w), 
					   "attach_dialog", args, arg));

	Delay::register_shell(attach_dialog);
	XtAddCallback(attach_dialog, XmNokCallback,    
		      gdbAttachDCB, 0);
	XtAddCallback(attach_dialog, XmNapplyCallback, 
		      gdbUpdateProcessesCB, 0);
	XtAddCallback(attach_dialog, XmNhelpCallback,
		      ImmediateHelpCB, 0);

	processes_w = XmSelectionBoxGetChild(attach_dialog, XmDIALOG_LIST);

	XtUnmanageChild(XmSelectionBoxGetChild(attach_dialog, 
					       XmDIALOG_SELECTION_LABEL));
	XtUnmanageChild(XmSelectionBoxGetChild(attach_dialog, 
					       XmDIALOG_TEXT));
    }

    update_processes();
    manage_and_raise(attach_dialog);
}
