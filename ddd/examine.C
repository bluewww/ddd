// $Id$ -*- C++ -*-
// Examine range of memory

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

char examine_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "examine.h"

#include "bool.h"
#include "ddd.h"
#include "disp-read.h"
#include "history.h"
#include "strclass.h"
#include "string-fun.h"
#include "verify.h"
#include "wm.h"
#include "ArgField.h"
#include "Command.h"
#include "Delay.h"
#include "DestroyCB.h"
#include "HelpCB.h"
#include "HistoryD.h"
#include "MakeMenu.h"

#include <Xm/SelectioB.h>
#include <Xm/TextF.h>


static Widget repeat_w;		// Repeat count
static Widget address_w;	// Starting address

static string the_format = "";	// The format

static void SetFormatCB(Widget w, XtPointer, XtPointer)
{
    the_format = XtName(w);
}

static string the_size   = "";	// The size

static void SetSizeCB(Widget w, XtPointer, XtPointer)
{
    the_size = XtName(w);
}

static Widget octal_w;		// Initial items
static Widget byte_w;

static MMDesc format_menu[] = { 
    { "o", MMPush, { SetFormatCB }, NULL, &octal_w },
    { "x", MMPush, { SetFormatCB }},
    { "d", MMPush, { SetFormatCB }},
    { "u", MMPush, { SetFormatCB }},
    { "t", MMPush, { SetFormatCB }},
    { "f", MMPush, { SetFormatCB }},
    { "a", MMPush, { SetFormatCB }},
    { "i", MMPush, { SetFormatCB }},
    { "c", MMPush, { SetFormatCB }},
    { "s", MMPush, { SetFormatCB }},
    MMEnd
};

static MMDesc size_menu[] = { 
    { "b", MMPush, { SetSizeCB }, NULL, &byte_w },
    { "h", MMPush, { SetSizeCB }},
    { "w", MMPush, { SetSizeCB }},
    { "g", MMPush, { SetSizeCB }},
    MMEnd
};

static MMDesc examine_menu[] = { 
    { "examine", MMSpinBox,    MMNoCB, NULL, &repeat_w },
    { "format",  MMOptionMenu, MMNoCB, format_menu },
    { "size",    MMOptionMenu, MMNoCB, size_menu },
    { "address", MMComboBox,   MMNoCB, NULL, &address_w },
    MMEnd
};

static string examine_command()
{
    String s_repeat = XmTextFieldGetString(repeat_w);
    string repeat(s_repeat);
    XtFree(s_repeat);

    String s_address = XmTextFieldGetString(address_w);
    string address(s_address);
    XtFree(s_address);

    strip_space(repeat);
    strip_space(address);

    return string("x /") + repeat + the_format + the_size + " " + address;
}

static void DisplayExaminedCB(Widget w, XtPointer, XtPointer)
{
    gdb_command("graph display `" + examine_command() + "`", w);
}

static void PrintExaminedCB(Widget w, XtPointer, XtPointer)
{
    gdb_command(examine_command(), w);
}

void gdbExamineCB(Widget w, XtPointer, XtPointer)
{
    static Widget dialog = 0;
    if (dialog == 0)
    {
	Arg args[10];
	Cardinal arg = 0;
	XtSetArg(args[arg], XmNautoUnmanage, False); arg++;
	dialog = verify(XmCreatePromptDialog(find_shell(w),
					     "examine_dialog",
					     args, arg));
	Delay::register_shell(dialog);

	XtManageChild(XmSelectionBoxGetChild(dialog,
					     XmDIALOG_APPLY_BUTTON));
	XtUnmanageChild(XmSelectionBoxGetChild(dialog, 
					       XmDIALOG_SELECTION_LABEL));
	XtUnmanageChild(XmSelectionBoxGetChild(dialog, XmDIALOG_TEXT));

	arg = 0;
	XtSetArg(args[arg], XmNorientation, XmHORIZONTAL); arg++;
	XtSetArg(args[arg], XmNborderWidth,  0); arg++;
	XtSetArg(args[arg], XmNentryBorder,  0); arg++;
	XtSetArg(args[arg], XmNspacing,      0); arg++;
	XtSetArg(args[arg], XmNmarginWidth,  0); arg++;
	XtSetArg(args[arg], XmNmarginHeight, 0); arg++;
	Widget panel = MMcreateButtonPanel(dialog, "panel", examine_menu, 
					   args, arg);
	(void) panel;
	MMaddCallbacks(examine_menu);
	MMaddHelpCallback(examine_menu, ImmediateHelpCB);

	// Initialize: use `o' and `b' as default menu items
	XtCallActionProc(octal_w, "ArmAndActivate", 
			 (XEvent *)0, (String *)0, 0);
	XtCallActionProc(byte_w, "ArmAndActivate", 
			 (XEvent *)0, (String *)0, 0);

	tie_combo_box_to_history(address_w, arg_history_filter);

	XtAddCallback(dialog, XmNokCallback,
		      PrintExaminedCB, XtPointer(0));
	XtAddCallback(dialog, XmNapplyCallback, 
		      DisplayExaminedCB, XtPointer(0));
	XtAddCallback(dialog, XmNcancelCallback, 
		      UnmanageThisCB, XtPointer(dialog));
	XtAddCallback(dialog, XmNhelpCallback,
		      ImmediateHelpCB, XtPointer(0));
    }

    string arg = source_arg->get_string();
    if (!is_file_pos(arg) && arg != "")
	XmTextFieldSetString(address_w, arg);

    manage_and_raise(dialog);
}
