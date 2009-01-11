// $Id$ -*- C++ -*-
// Examine range of memory

// Copyright (C) 1998 Technische Universitaet Braunschweig, Germany.
// Copyright (C) 2001-2006 Free Software Foundation, Inc.
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

char examine_rcsid[] = 
    "$Id$";

#if defined(HAVE_CONFIG_H)
#include "config.h"
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

#if defined(IF_XM)
#include <Xm/SelectioB.h>
#include <Xm/TextF.h>
#else
#include <GUI/Dialog.h>
#include <GUI/ComboBox.h>
#include <GUI/SpinButton.h>
#endif


#if defined(IF_XM)
static Widget repeat_w;		// Repeat count
static Widget address_w;	// Starting address
#else
static GUI::SpinButton *repeat_w;	// Repeat count
static GUI::ComboBoxEntryText *address_w;	// Starting address
#endif

static string the_format = "";	// The format

#if defined(IF_XM)
static void SetFormatCB(Widget w, XtPointer, XtPointer)
{
    the_format = XtName(w);
}
#else
static void SetFormatCB(GUI::Widget *w)
{
    the_format = w->get_name().c_str();
}
#endif

static string the_size   = "";	// The size

#if defined(IF_XM)
static void SetSizeCB(Widget w, XtPointer, XtPointer)
{
    the_size = XtName(w);
}
#else
static void SetSizeCB(GUI::Widget *w)
{
    the_size = w->get_name().c_str();
}
#endif

#if defined(IF_XM)
static Widget octal_w;		// Initial items
static Widget byte_w;
static Widget long_w;

static Widget unsigned_char_w;
static Widget binary_w;
static Widget address_format_w;
static Widget wide_char_w;
static Widget wide_string_w;
#else
static GUI::Widget *octal_w;		// Initial items
static GUI::Widget *byte_w;
static GUI::Widget *long_w;

static GUI::Widget *unsigned_char_w;
static GUI::Widget *binary_w;
static GUI::Widget *address_format_w;
static GUI::Widget *wide_char_w;
static GUI::Widget *wide_string_w;
#endif

static MMDesc format_menu[] = { 
    GENTRYL("o", "o", MMPush|MMNoLocale,
	    BIND(SetFormatCB, 0),
	    sigc::ptr_fun(SetFormatCB),
	    0, &octal_w),
    GENTRYL("x", "x", MMPush|MMNoLocale,
	    BIND(SetFormatCB, 0),
	    sigc::ptr_fun(SetFormatCB),
	    0, 0),
    GENTRYL("d", "d", MMPush|MMNoLocale,
	    BIND(SetFormatCB, 0),
	    sigc::ptr_fun(SetFormatCB),
	    0, 0),
    GENTRYL("u", "u", MMPush|MMNoLocale,
	    BIND(SetFormatCB, 0),
	    sigc::ptr_fun(SetFormatCB),
	    0, &unsigned_char_w),
    GENTRYL("t", "t", MMPush|MMNoLocale,
	    BIND(SetFormatCB, 0),
	    sigc::ptr_fun(SetFormatCB),
	    0, &binary_w),
    GENTRYL("f", "f", MMPush|MMNoLocale,
	    BIND(SetFormatCB, 0),
	    sigc::ptr_fun(SetFormatCB),
	    0, 0),
    GENTRYL("a", "a", MMPush|MMNoLocale,
	    BIND(SetFormatCB, 0),
	    sigc::ptr_fun(SetFormatCB),
	    0, &address_format_w),
    GENTRYL("i", "i", MMPush|MMNoLocale,
	    BIND(SetFormatCB, 0),
	    sigc::ptr_fun(SetFormatCB),
	    0, 0),
    GENTRYL("c", "c", MMPush|MMNoLocale,
	    BIND(SetFormatCB, 0),
	    sigc::ptr_fun(SetFormatCB),
	    0, 0),
    GENTRYL("C", "C", MMPush|MMNoLocale,
	    BIND(SetFormatCB, 0),
	    sigc::ptr_fun(SetFormatCB),
	    0, &wide_char_w),
    GENTRYL("s", "s", MMPush|MMNoLocale,
	    BIND(SetFormatCB, 0),
	    sigc::ptr_fun(SetFormatCB),
	    0, 0),
    GENTRYL("W", "W", MMPush|MMNoLocale,
	    BIND(SetFormatCB, 0),
	    sigc::ptr_fun(SetFormatCB),
	    0, &wide_string_w),
    MMEnd
};

static MMDesc size_menu[] = { 
    GENTRYL("b", "b", MMPush|MMNoLocale,
	    BIND(SetSizeCB, 0),
	    sigc::ptr_fun(SetSizeCB),
	    0, &byte_w),
    GENTRYL("h", "h", MMPush|MMNoLocale,
	    BIND(SetSizeCB, 0),
	    sigc::ptr_fun(SetSizeCB),
	    0, 0),
    GENTRYL("w", "w", MMPush|MMNoLocale,
	    BIND(SetSizeCB, 0),
	    sigc::ptr_fun(SetSizeCB),
	    0, 0),
    GENTRYL("g", "g", MMPush|MMNoLocale,
	    BIND(SetSizeCB, 0),
	    sigc::ptr_fun(SetSizeCB),
	    0, 0),
    GENTRYL("G", "G", MMPush|MMNoLocale,
	    BIND(SetSizeCB, 0),
	    sigc::ptr_fun(SetSizeCB),
	    0, &long_w),
    MMEnd
};

static MMDesc examine_menu[] = { 
    GENTRYL("examine", N_("examine"), MMSpinBox,
	    MMNoCB, MDUMMY, 0, &repeat_w),
    GENTRYL("format", N_("format"), MMOptionMenu,
	    MMNoCB, MDUMMY, format_menu, 0),
    GENTRYL("size", N_("size"), MMOptionMenu,
	    MMNoCB, MDUMMY, size_menu, 0),
    GENTRYL("address", N_("address"), MMComboBox,
	    MMNoCB, MDUMMY, 0, &address_w),
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
    case MAKE:
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
#if defined(IF_XM)
    String s_repeat = XmTextFieldGetString(repeat_w);
    string repeat(s_repeat);
    XtFree(s_repeat);
#else
    const char *s_repeat = repeat_w->get_text().c_str();
    string repeat(s_repeat);
#endif

#if defined(IF_XM)
    String s_address = XmTextFieldGetString(address_w);
    string address(s_address);
    XtFree(s_address);
#else
    string address = address_w->get_text().c_str();
#endif

    strip_space(repeat);
    strip_space(address);

    if (GDB == gdb->type() && gdb->cpu == cpu_intel) {
      
      /* Intel x86 8 and 16-bit handling: 
	 When you have an assembly line like this:
         movw array(,%ecx,2), %ax

	 there is no easy way to show memory addressed by
	 "array(,%ecx,2)", which is very useful for debugging. This was
	 solved via Data/Memory dialog, which was changed to handle
	 these expressions.
	 
	 This way, by putting "array(,%ecx,2)" in the argument field,
	 and calling Data/Memory dialog, correct value(s) will be
	 shown. But, there is a catch: all data displays shown this way
	 will have "X" as their name. I didn't figure out how to name
	 them correctly (or, in fact, now to name them at all).
      */

      //ZARKO - prikaz sadrzaja memorije kod opsteg formata adresiranja
      int left_par, right_par, comma1, comma2;
      string addr,addr_addr,addr_base,addr_index,addr_mul;
      addr = address;
      char added = 0;
      addr.gsub(" ","");	//izbaci sve razmake
      addr.gsub("\t","");	//izbaci sve tabove

      //zameni % sa $, pa ako je bilo ikakvih zamena, radi dalje
      if (addr.gsub("%","$") > 0) {
	left_par = addr.index("(");
	right_par = addr.index(")", left_par+1);

	//samo ako ima zagrada, treba dalje analizirati
	if ((left_par != -1) && (right_par != -1)) {
	  addr_addr = "";		//adresa
	  addr_base = "";		//baza
	  addr_mul = "";		//mnozilac
	  addr_index = "";	//indeks
	  if ((comma1 = addr.index(",",left_par+1)) != -1) {
	    if ((comma2 = addr.index(",", comma1+1)) == -1) 
	      comma2 = right_par;
	  } else comma1 = comma2 = right_par;
	  if (left_par > 0) addr_addr = addr.at(0,left_par);
	  if (comma1 > left_par+1) 
	    addr_base = addr.at(left_par+1, comma1-left_par-1);
	  if (comma2 > comma1+1) 
	    addr_index = addr.at(comma1+1,comma2-comma1-1);
	  if (right_par > comma2+1) 
	    addr_mul = addr.at(comma2+1,right_par-comma2-1);
	  address = "(";
	  if (addr_addr.length()) {
	    if (((addr_addr.at(0,1) >= '0') && (addr_addr.at(0,1) <= '9')) 
		|| (addr_addr.at(0,1) == '-'))
	      address += addr_addr;
	    else	
	      address += "(char*)&"+addr_addr;
	    added = 1;
	  }
	  if (addr_base.length()) {
	    if (added) address += "+";
	    address += addr_base;
	    added = 1;
	  }
	  if (addr_index.length()) {
	    if (added) address += "+";
	    address += addr_index;
	  }
	  if (addr_mul.length()) address += "*"+addr_mul;
	  address += ")";
	}
      }
      //ZARKO - prikaz sadrzaja memorije kod opsteg formata adresiranja
    }
    
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
    case MAKE:
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
    gdb_command("graph display `" + examine_command() + "`", w);
}

static void PrintExaminedCB(GUI::Widget *w)
{
    gdb_command(examine_command(), w);
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
		      UnmanageThisCB, XtPointer(dialog));
	XtAddCallback(dialog, XmNhelpCallback,
		      ImmediateHelpCB, XtPointer(0));
    }

    string arg = source_arg->get_string();
    if (!is_file_pos(arg) && !arg.empty())
	XmTextFieldSetString(address_w, XMST(arg.chars()));

    manage_and_raise(dialog);
}
#else
void gdbExamineCB(GUI::Widget *w)
{
    static GUI::Dialog *dialog = 0;
    if (dialog == 0)
    {
	dialog = new GUI::Dialog(*find_shell(w), "examine_dialog");
	Delay::register_shell(dialog);

#ifdef NAG_ME
#warning Dialog buttons?
#endif

	GUI::Container *panel = MMcreateButtonPanel(dialog, "panel", examine_menu);

	(void) panel;
	MMaddCallbacks(examine_menu);
	MMaddHelpCallback(examine_menu, sigc::ptr_fun(ImmediateHelpCB));

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
	button = dialog->add_button("ok", _("OK"));
	button->signal_clicked().connect(sigc::bind(sigc::ptr_fun(PrintExaminedCB), dialog));
	button->show();
	button = dialog->add_button("apply", _("Apply"));
	button->signal_clicked().connect(sigc::bind(sigc::ptr_fun(DisplayExaminedCB), dialog));
	button->show();
	button = dialog->add_button("cancel", _("Cancel"));
	button->signal_clicked().connect(sigc::bind(sigc::ptr_fun(UnmanageThisCB), dialog));
	button->show();
    }

    string arg = source_arg->get_string();
    if (!is_file_pos(arg) && !arg.empty()) {
	address_w->set_text(arg.chars());
    }

    manage_and_raise(dialog);
}
#endif
