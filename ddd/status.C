// $Id$ -*- C++ -*-
// Interpret GDB output

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
// or send a mail to the DDD developers <ddd@ips.cs.tu-bs.de>.

char status_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "status.h"

#include "Delay.h"
#include "GDBAgent.h"
#include "HelpCB.h"
#include "charsets.h"
#include "Command.h"
#include "ddd.h"
#include "mydialogs.h"
#include "post.h"
#include "string-fun.h"
#include "verify.h"
#include "DestroyCB.h"
#include "findParent.h"

#include <ctype.h>
#include <Xm/Xm.h>
#include <Xm/Text.h>
#include <Xm/SelectioB.h>
#include <Xm/RowColumn.h>
#include <Xm/Label.h>
#include <Xm/MenuShell.h>

#include <X11/IntrinsicP.h>	// LessTif hacks
#include "LessTifH.h"

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

// True if last cmd came from GDB window
bool gdb_keyboard_command = false;

// True if the next line is to be displayed in the status line
bool show_next_line_in_status = false;

// True if GDB is asking `yes or no' right now
bool gdb_asks_yn;

// Current contents of status window
static MString current_status_text;

// Non-zero if status is locked (i.e. unchangeable)
static int status_locked = 0;


//-----------------------------------------------------------------------------
// Status lock
//-----------------------------------------------------------------------------

void lock_status()       { status_locked++;   }
void unlock_status()     { status_locked--;   }
void reset_status_lock() { status_locked = 0; }


//-----------------------------------------------------------------------------
// Prompt recognition
//-----------------------------------------------------------------------------

void set_buttons_from_gdb(Widget buttons, string& text)
{
    bool yn = text.contains("(y or n) ", -1) 
	|| text.contains("(yes or no) ", -1)
	|| (gdb->type() == XDB && text.contains("? ", -1));

    if (yn)
    {
	gdb_asks_yn = true;
    }
    else if (gdb->isReadyWithPrompt())
    {
	gdb_asks_yn = false;
	unpost_gdb_yn();
    }

    if (yn && !gdb_keyboard_command)
    {
	// Fetch previous output lines, in case this is a multi-line message.
	String s = XmTextGetString(gdb_w);
	string prompt(s);
	XtFree(s);

	char prompt_start = (gdb->type() == XDB ? '>' : '(');

	int pos = prompt.index(prompt_start, -1);
	if (pos >= 0)
	    pos = prompt.index('\n', pos) + 1;
	if (pos == 0)
	    pos = messagePosition;

	XmTextReplace(gdb_w, pos, XmTextGetLastPosition(gdb_w), "");

	prompt = prompt.from(pos);
	if (text.contains('('))
	    prompt += text.before('(', -1); // Don't repeat `(y or n)'
	else
	    prompt += text;

	post_gdb_yn(prompt);
	text = "";
	return;
    }

    if (buttons == 0)
	return;

    static bool last_yn = false;
    if (yn == last_yn)
	return;

    last_yn = yn;

    if (XtIsComposite(buttons))
    {
	XtSetSensitive(buttons, false);

	WidgetList children   = 0;
	Cardinal num_children = 0;

	XtVaGetValues(buttons,
		      XmNchildren, &children,
		      XmNnumChildren, &num_children,
		      NULL);

	int i;
	for (i = 0; i < int(num_children); i++)
	    XtManageChild(children[i]);
	for (i = 0; i < int(num_children); i++)
	{
	
	    Widget w = children[i];
	    string name = XtName(w);

	    if (yn == (name == "Yes" || name == "No"))
		XtManageChild(w);
	    else
		XtUnmanageChild(w);
	}

	XtSetSensitive(buttons, true);
    }
}


//-----------------------------------------------------------------------------
// Status history
//-----------------------------------------------------------------------------

int status_history_size = 20;
static MString *history = 0;
static int current_history = 0;

static Widget history_label = 0;
static Widget history_row   = 0;

static Widget create_status_history(Widget parent)
{
    static Widget history_shell = 0;

    if (history_shell != 0)
	return history_shell;

    Arg args[10];
    int arg;

    arg = 0;
    XtSetArg(args[arg], XmNallowShellResize, True); arg++;
    XtSetArg(args[arg], XmNwidth,            10);   arg++;
    XtSetArg(args[arg], XmNheight,           10);   arg++;
    history_shell = verify(XmCreateMenuShell(parent, "status_history", 
					     args, arg));

    arg = 0;
    XtSetArg(args[arg], XmNmarginWidth, 0);     arg++;
    XtSetArg(args[arg], XmNmarginHeight, 0);    arg++;
    XtSetArg(args[arg], XmNresizeWidth, True);  arg++;
    XtSetArg(args[arg], XmNresizeHeight, True); arg++;
    XtSetArg(args[arg], XmNborderWidth, 0);     arg++;
    XtSetArg(args[arg], XmNshadowThickness, 0); arg++;
    history_row = verify(XmCreateRowColumn(history_shell, "row", args, arg));
    XtManageChild(history_row);

    arg = 0;
    XtSetArg(args[arg], XmNresizable, True); arg++;
    XtSetArg(args[arg], XmNalignment, XmALIGNMENT_BEGINNING); arg++;
    history_label = verify(XmCreateLabel(history_row, "label", args, arg));
    XtManageChild(history_label);
					   
    return history_shell;
}

Widget status_history(Widget parent)
{
    Widget history_shell = create_status_history(parent);

    MString history_msg;

    if (history == 0 || status_history_size == 0)
    {
	history_msg = rm("No history.");
    }
    else
    {
	history_msg = bf("Recent messages");
	history_msg += rm(" (oldest first)");
	history_msg += cr();

	int i = current_history;
	do {
	    if (!history[i].isEmpty())
	    {
		if (!history_msg.isEmpty())
		    history_msg += cr();
		history_msg += history[i];
	    }
	    i = (i + 1) % status_history_size;
	} while (i != current_history);
    }

    if (lesstif_version < 1000)
    {
	// LessTif 0.79 fails to resize the shell properly.  Use this
	// hack instead.
	XmFontList font_list;
	XtVaGetValues(history_label, XmNfontList, &font_list, NULL);
    
	Dimension history_width  = history_msg.width(font_list)  + 6;
	Dimension history_height = history_msg.height(font_list) + 6;

	XtResizeWidget(history_label, history_width, history_height, 0);
	XtResizeWidget(history_row,   history_width, history_height, 0);

	XtResizeWidget(history_shell, history_width, history_height, 1);
    }

    XtVaSetValues(history_label, XmNlabelString, history_msg.xmstring(), 
		  XtPointer(0));

    return history_shell;
}

static void add_to_status_history(const MString& message)
{
    static MString empty = rm(" ");

    if (history == 0)
	history = new MString[status_history_size];

    int last_history = 
	(status_history_size + current_history - 1) % status_history_size;

    if (message.isNull() || message.isEmpty() || message == empty)
	return;
    if (message == history[last_history])
	return;

    history[current_history] = message;
    current_history = (current_history + 1) % status_history_size;
}

//-----------------------------------------------------------------------------
// Status recognition
//-----------------------------------------------------------------------------

void set_status_from_gdb(const string& text)
{
    if (private_gdb_input)
	return;

    if (!show_next_line_in_status 
	&& (gdb->type() == XDB || !text.contains(") ", -1))
	&& (gdb->type() != XDB || !text.contains(">", -1)))
	return;

    // Fetch line before prompt in GDB window
    String s = XmTextGetString(gdb_w);
    string message = s + messagePosition;
    XtFree(s);

    if (message == "" && text.contains('\n'))
	message = text;

    if (show_next_line_in_status && 
	(message == "" || message[message.length() - 1] != '\n'))
	return;

    // Skip prompt and uncomplete lines
    int idx = message.index('\n', -1);
    if (idx >= 0)
	message = message.before(idx);

    strip_final_newlines(message);
    if (message == "" && text.contains('\n'))
	message = text;

    if (show_next_line_in_status)
    {
	messagePosition = XmTextGetLastPosition(gdb_w) + text.length();
	show_next_line_in_status = false;
	message.gsub('\n', ' ');
    }
    else
    {
	// Show first line only
	while (message != "" && message[0] == '\n')
	    message = message.after('\n');
	if (message.contains('\n'))
	    message = message.before('\n');
    }

    strip_final_newlines(message);
    if (message == "")
	return;

    set_status(message);
}

// Show MESSAGE in status window.
// If TEMPORARY is set, override locks and do not add to status history.
void set_status(string message, bool temporary)
{
    if (status_w == 0)
	return;

    if (message.length() > 0
	&& !message.contains("=") 
	&& isascii(message[0])
	&& islower(message[0]))
	message[0] = toupper(message[0]);

    set_status_mstring(rm(message), temporary);
}

// Same, but use an MString.
void set_status_mstring(MString message, bool temporary)
{
    if (status_w == 0)
	return;

    if (!temporary)
    {
	add_to_status_history(message);
	if (status_locked)
	    return;
    }

    current_status_text = message;

    XtVaSetValues(status_w,
		  XmNlabelString, message.xmstring(),
		  NULL);
    XFlush(XtDisplay(status_w));
    XmUpdateDisplay(status_w);
}

const MString& current_status()
{
    return current_status_text;
}
