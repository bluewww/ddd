// $Id$ -*- C++ -*-
// Return current clipboard selection

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

char selection_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "selection.h"
#include "bool.h"

#include <Xm/CutPaste.h>

// Return the current clipboard selection
string current_clipboard(Widget w)
{
    Display *dpy = XtDisplay(w);
    Window win = XtWindow(w);

    if (!XtIsRealized(w))
	win = RootWindowOfScreen(XtScreen(w));

    int result;
    do {
	Time time = XtLastTimestampProcessed(dpy);
	result = XmClipboardStartRetrieve(dpy, win, time);
    } while (result == XmClipboardLocked);

    if (result != XmClipboardSuccess)
	return "";

    // Figure out how much we need to paste
    unsigned long len;
    do {
	result = XmClipboardInquireLength(dpy, win, "STRING", &len);
    } while (result == XmClipboardLocked);

    if (result != XmClipboardSuccess)
	return "";

    char *buf = XtMalloc(len + 1);
    buf[0] = '\0';

    unsigned long nbytes;
#if XmVersion >= 1002
    long private_id;
#else
    int private_id;
#endif
    do {
	result = XmClipboardRetrieve(dpy, win, "STRING",
				     buf, len, &nbytes, &private_id);
    } while (result == XmClipboardLocked);

    string selection(buf, len);
    XtFree(buf);

    if (result != XmClipboardSuccess)
    	return "";

    do {
	result = XmClipboardEndRetrieve(dpy, win);
    } while (result == XmClipboardLocked);

    if (result != XmClipboardSuccess)
	return "";

    return selection;
}

static void GotSelection(Widget, XtPointer client_data, Atom *,
			 Atom *type, XtPointer value, unsigned long *length,
			 int *format)
{
    string& selection = *((string *)client_data);
    selection = "";

    if (value == 0)
	return;

    if (*type != XA_STRING || *format != 8)
	return;

    selection = string((char *)value, *length);
}

// Return current primary selection.
string current_primary(Widget w)
{
    if (!XtIsRealized(w))
    {
	Arg args[10];
	int arg = 0;
	XtSetArg(args[arg], XmNwidth,  1); arg++;
	XtSetArg(args[arg], XmNheight, 1); arg++;
	Widget shell = XtCreatePopupShell("shell", overrideShellWidgetClass, 
					  w, args, arg);
	XtRealizeWidget(shell);
	XtUnmapWidget(shell);

	string ret = current_primary(shell);

	XtDestroyWidget(shell);

	return ret;
    }

    Time time = XtLastTimestampProcessed(XtDisplay(w));
    string selection = char(-1);
    XtGetSelectionValue(w, XA_PRIMARY, XA_STRING, GotSelection, 
			XtPointer(&selection), time);

    while (selection == char(-1))
	XtAppProcessEvent(XtWidgetToApplicationContext(w), XtIMAll);

    return selection;
}

// Return current cut buffer selection.
string current_cut_buffer(Widget w)
{
    int nbytes;
    char *buf = XFetchBytes(XtDisplay(w), &nbytes);
    string selection = buf;
    XFree(buf);

    return selection;
}

string current_selection(Widget w)
{
    string selection = "";

    if (selection == "")
	selection = current_primary(w);
    if (selection == "")
	selection = current_cut_buffer(w);
    if (selection == "")
	selection = current_clipboard(w);

    return selection;
}
