// $Id$ -*- C++ -*-
// DDD buttons

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
// 675 Mass Ave, Cambridge, MA 02139, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

const char buttons_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "buttons.h"

#include "HelpCB.h"
#include "MString.h"
#include "bool.h"
#include "ctrl.h"
#include "ddd.h"
#include "editing.h"
#include "question.h"
#include "source.h"
#include "status.h"
#include "string-fun.h"
#include "verify.h"

#include <Xm/Xm.h>
#include <Xm/RowColumn.h>
#include <Xm/PushB.h>
#include <ctype.h>

//-----------------------------------------------------------------------------
// Buttons
//-----------------------------------------------------------------------------

static void YnButtonCB(Widget dialog, 
		       XtPointer client_data, 
		       XtPointer call_data)
{
    _gdb_out(string((char *)client_data) + '\n');
    gdbCommandCB(dialog, client_data, call_data);
    gdb_keyboard_command = true;
}


//-----------------------------------------------------------------------------
// Default help texts (especially buttons)
//-----------------------------------------------------------------------------

MString gdbDefaultHelp(Widget widget)
{
    string name = XtName(widget);

    string help = gdb_question("help " + name);
    if (help == NO_GDB_ANSWER)
    {
	help = "No help available now.\n"
	    "Please try again when the debugger is ready.";
    }
    strip_final_blanks(help);

    return MString(name + "\n", "bf") +	MString(help, "rm");
}

Widget make_buttons(Widget parent, const string& name, 
		    const string& button_list)
{
    if (button_list == "")
	return 0;

    Widget buttons = verify(XmCreateWorkArea(parent, name, 0, 0));
    if (buttons == 0)
    {
	// Not available in LessTif 0.1
	buttons = verify(XmCreateRowColumn(parent, name, 0, 0));
    }

    add_buttons(buttons, button_list);

    XtManageChild(buttons);

    XtWidgetGeometry size;
    size.request_mode = CWHeight;
    XtQueryGeometry(buttons, NULL, &size);
    unsigned char unit_type;
    XtVaGetValues(buttons, XmNunitType, &unit_type, NULL);
    int new_height = XmConvertUnits(buttons, XmVERTICAL, XmPIXELS, 
				    size.height, unit_type);
    XtVaSetValues(buttons,
		  XmNpaneMaximum, new_height,
		  XmNpaneMinimum, new_height,
		  NULL);

    return buttons;
}

void add_buttons(Widget buttons, const string& button_list)
{
    int colons = button_list.freq(':') + 1;
    string *commands = new string[colons];
    split(button_list, commands, colons, ':');

    for (int i = 0; i < colons; i++)
    {
	XtCallbackProc callback = gdbCommandCB;

	string name = commands[i];
	string command = name;
	if (name.contains("..."))
	{
	    name = name.before("...");
	    command = name + ' ';
	}
	else if (name.contains('^'))
	{
	    command = ctrl(name.from('^'));
	    name = name.before('^');
	}
	else if (name != "" && iscntrl(name[name.length() - 1]))
	{
	    command = string(name[name.length() - 1]);
	    name = name.before(-1);
	}

	Widget button = verify(XmCreatePushButton(buttons, name, 0, 0));
	XtManageChild(button);

	if (name == "Yes")
	{
	    command = "yes";
	    XtUnmanageChild(button);
	    callback = YnButtonCB;
	}
	else if (name == "No")
	{
	    command = "no";
	    XtUnmanageChild(button);
	    callback = YnButtonCB;
	}
	else if (name == "Prev")
	    callback = gdbPrevCB;
	else if (name == "Next")
	    callback = gdbNextCB;
	else if (name == "Clear")
	    callback = gdbClearCB;
	else if (name == "Complete")
	    callback = gdbCompleteCB;
	else if (name == "Apply")
	    callback = gdbApplyCB;
	else if (name == "Back")
	    callback = gdbGoBackCB;
	else if (name == "Forward")
	    callback = gdbGoForwardCB;
	else if (name == "Edit")
	    callback = gdbEditSourceCB;
	else if (name == "Reload")
	    callback = gdbReloadSourceCB;

	// We remove all callbacks to avoid popping down DialogShells
	XtRemoveAllCallbacks(button, XmNactivateCallback);
	XtAddCallback(button, XmNactivateCallback, callback,
		      (XtPointer)XtNewString((String)command));
    }
    delete[] commands;

    // Register default help command
    DefaultHelpText = gdbDefaultHelp;
}
