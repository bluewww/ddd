// $Id$ -*- C++ -*-
// Examine range of memory

// Copyright (C) 1998 Technische Universitaet Braunschweig, Germany.
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

char examine_rcsid[] = 
    "$Id$";

#include "config.h"

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

#ifdef IF_MOTIF
#include <Xm/SelectioB.h>
#include <Xm/TextF.h>
#endif

#ifndef IF_XM
#include <GUI/Dialog.h>
#endif


static SPINBUTTON_P repeat_w;	// Repeat count
static COMBOBOXENTRYTEXT_P address_w;	// Starting address

static string the_format = "";	// The format

static void SetFormatCB(CB_ALIST_1(Widget w))
{
    the_format = XtName(w);
}

static string the_size   = "";	// The size

static void SetSizeCB(CB_ALIST_1(Widget w))
{
    the_size = XtName(w);
}

static Widget octal_w;		// Initial items
static Widget byte_w;
static Widget long_w;

static Widget unsigned_char_w;
static Widget binary_w;
static Widget address_format_w;
static Widget wide_char_w;
static Widget wide_string_w;

static MMDesc format_menu[] = { 
    MENTRYL("o", "o", MMPush, BIND_0(PTR_FUN(SetFormatCB)), 0, &octal_w),
    MENTRYL("x", "x", MMPush, BIND_0(PTR_FUN(SetFormatCB)), 0, 0),
    MENTRYL("d", "d", MMPush, BIND_0(PTR_FUN(SetFormatCB)), 0, 0),
    MENTRYL("u", "u", MMPush, BIND_0(PTR_FUN(SetFormatCB)), 0, &unsigned_char_w),
    MENTRYL("t", "t", MMPush, BIND_0(PTR_FUN(SetFormatCB)), 0, &binary_w),
    MENTRYL("f", "f", MMPush, BIND_0(PTR_FUN(SetFormatCB)), 0, 0),
    MENTRYL("a", "a", MMPush, BIND_0(PTR_FUN(SetFormatCB)), 0, &address_format_w),
    MENTRYL("i", "i", MMPush, BIND_0(PTR_FUN(SetFormatCB)), 0, 0),
    MENTRYL("c", "c", MMPush, BIND_0(PTR_FUN(SetFormatCB)), 0, 0),
    MENTRYL("C", "C", MMPush, BIND_0(PTR_FUN(SetFormatCB)), 0, &wide_char_w),
    MENTRYL("s", "s", MMPush, BIND_0(PTR_FUN(SetFormatCB)), 0, 0),
    MENTRYL("W", "W", MMPush, BIND_0(PTR_FUN(SetFormatCB)), 0, &wide_string_w),
    MMEnd
};

static MMDesc size_menu[] = { 
    MENTRYL("b", "b", MMPush, BIND_0(PTR_FUN(SetSizeCB)), 0, &byte_w),
    MENTRYL("h", "h", MMPush, BIND_0(PTR_FUN(SetSizeCB)), 0, 0),
    MENTRYL("w", "w", MMPush, BIND_0(PTR_FUN(SetSizeCB)), 0, 0),
    MENTRYL("g", "g", MMPush, BIND_0(PTR_FUN(SetSizeCB)), 0, 0),
    MENTRYL("G", "G", MMPush, BIND_0(PTR_FUN(SetSizeCB)), 0, &long_w),
    MMEnd
};

static MMDesc examine_menu[] = { 
    MENTRYL("examine", "examine", MMSpinBox,    MMNoCB, 0, (Widget *)&repeat_w),
    MENTRYL("format", "format",   MMOptionMenu, MMNoCB, format_menu, 0),
    MENTRYL("size", "size",       MMOptionMenu, MMNoCB, size_menu, 0),
    MENTRYL("address", "address", MMComboBox,   MMNoCB, 0, (Widget *)&address_w),
    MMEnd
};

static string format(const string& format, const string& size)
{
    switch (gdb->type())
    {
    case GDB:
	return format + size;

    case DBX:
	// Translate GDB spec to DBX spec.
	//
	// Legal values for FORMAT are:
	//
	// i     instruction (disassembly)
	// d,D   decimal (2 or 4 bytes)
	// o,O   octal (2 or 4 bytes)
	// x,X   hexadecimal (2 or 4 bytes)
	// b     octal (1 byte)
	// c     character
	// w     wide character
	// s     string
	// W     wide character string
	// f     hex and float (4 bytes, 6 digit prec.)
	// F     hex and float (8 bytes, 14 digit prec.)
	// g     same as `F'
	// E     hex and float (16 bytes, 14 digit prec.)
	// ld,lD decimal (4 bytes, same as D)
	// lo,lO octal (4 bytes, same as O)
	// lx,LX hexadecimal (4 bytes, same as X)
	// Ld,LD decimal (8 bytes)

	// Handle bytes
	if (format == 'o' && size == 'b')
	    return "b";

	// Handle wide characters
	if (format == "C")
	    return "w";

	// Handle float lengths
	if (format == 'f' && size == 'w')
	    return "f";
	if (format == 'f' && size == 'g')
	    return "g";
	if (format == 'f' && size == 'G')
	    return "E";

	// Handle size
	if (format == 'd' || format == 'o' || format == 'x')
	{
	    if (size == 'w')
		return "l" + format;
	    if (size == 'g')
		return "L" + format;
	}

	// Ignore size in all other cases
	return format;

    case BASH:
    case DBG:
    case JDB:
    case PYDB:
    case PERL:
    case XDB:
	// No way
	break;
    }

    return "";
}

static string examine_command()
{
#ifdef IF_MOTIF
    String s_repeat = XmTextFieldGetString(repeat_w);
    string repeat(s_repeat);
    XtFree(s_repeat);
#else // NOT IF_MOTIF
    const char *s_repeat = repeat_w->get_text().c_str();
    string repeat(s_repeat);
#endif // IF_MOTIF

#ifdef IF_MOTIF
    String s_address = XmTextFieldGetString(address_w);
    string address(s_address);
    XtFree(s_address);
#else // NOT IF_MOTIF
    Gtk::Entry *entry = dynamic_cast<Gtk::Entry *>(address_w->get_child());
    const char *s_address = entry->get_text().c_str();
    string address(s_address);
#endif // IF_MOTIF

    strip_space(repeat);
    strip_space(address);

    string fmt = "/" + repeat + format(the_format, the_size);
    switch (gdb->type())
    {
    case DBX:
	// x ADDRESS /FMT
	return "x " + address + " " + fmt;

    case GDB:
	// x /FMT ADDRESS
	return "x " + fmt + " " + address;

    case BASH:
    case DBG:
    case JDB:
    case PYDB:
    case PERL:
    case XDB:
	break;			// No way
    }

    return "";
}

#if defined(IF_XM)

static void DisplayExaminedCB(Widget w, XtPointer, XtPointer)
{
    gdb_command("graph display `" + examine_command() + "`", w);
}

static void PrintExaminedCB(Widget w, XtPointer, XtPointer)
{
    gdb_command(examine_command(), w);
}

#else

static void DisplayExaminedCB(GUI::Widget *w)
{
    gdb_command("graph display `" + examine_command() + "`", w->internal());
}

static void PrintExaminedCB(GUI::Widget *w)
{
    gdb_command(examine_command(), w->internal());
}


#endif

#if defined(IF_XM)

void gdbExamineCB(Widget w, XtPointer, XtPointer)
{
    static Widget dialog = 0;
    if (dialog == 0)
    {
	Arg args[10];
	Cardinal arg = 0;
	XtSetArg(args[arg], XmNautoUnmanage, False); arg++;
	dialog = verify(XmCreatePromptDialog(find_shell(w),
					     XMST("examine_dialog"),
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

	manage_child(unsigned_char_w,  gdb->type() == GDB);
	manage_child(binary_w,         gdb->type() == GDB);
	manage_child(address_format_w, gdb->type() == GDB);
	manage_child(wide_char_w,      gdb->type() == DBX);
	manage_child(wide_string_w,    gdb->type() == DBX);
	manage_child(long_w,           gdb->type() == DBX);

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
		      UnmanageThisCB1, XtPointer(dialog));
	XtAddCallback(dialog, XmNhelpCallback,
		      ImmediateHelpCB, XtPointer(0));
    }

    string arg = source_arg->get_string();
    if (!is_file_pos(arg) && !arg.empty()) {
	XmTextFieldSetString(address_w, XMST(arg.chars()));
    }

    manage_and_raise(dialog);
}

#else

void gdbExamineCB(GUI::Widget *w)
{
    static GUI::Dialog *dialog = 0;
    if (dialog == 0)
    {
	dialog = new GUI::Dialog(*find_shell1(w), "examine_dialog");
	Delay::register_shell(dialog);

#ifdef NAG_ME
#warning Dialog buttons?
#endif

	GUI::Container *panel = MMcreateButtonPanel(dialog, "panel", examine_menu);

	(void) panel;
	MMaddCallbacks(examine_menu);
	MMaddHelpCallback(examine_menu, sigc::ptr_fun(ImmediateHelpCB1));

	manage_child(unsigned_char_w,  gdb->type() == GDB);
	manage_child(binary_w,         gdb->type() == GDB);
	manage_child(address_format_w, gdb->type() == GDB);
	manage_child(wide_char_w,      gdb->type() == DBX);
	manage_child(wide_string_w,    gdb->type() == DBX);
	manage_child(long_w,           gdb->type() == DBX);

#ifdef NAG_ME
#warning ArmAndActivate action undefined
#endif

	tie_combo_box_to_history(address_w, arg_history_filter);

	GUI::Button *button;
	button = dialog->add_button("ok", "OK");
	button->signal_clicked().connect(sigc::bind(sigc::ptr_fun(PrintExaminedCB), dialog));
	button = dialog->add_button("apply", "Apply");
	button->signal_clicked().connect(sigc::bind(sigc::ptr_fun(DisplayExaminedCB), dialog));
	button = dialog->add_button("cancel", "Cancel");
	button->signal_clicked().connect(sigc::bind(sigc::ptr_fun(UnmanageThisCB), dialog));
    }

    string arg = source_arg->get_string();
    if (!is_file_pos(arg) && !arg.empty()) {
	Gtk::Entry *entry = dynamic_cast<Gtk::Entry *>(address_w->get_child());
	entry->set_text(XMST(arg.chars()));
    }

    manage_and_raise1(dialog);
}

#endif

