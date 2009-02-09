// $Id$ -*- C++ -*-
// Examine range of memory

// Copyright (C) 1998 Technische Universitaet Braunschweig, Germany.
// Copyright (C) 2001-2006, 2008 Free Software Foundation, Inc.
// Written by Andreas Zeller <zeller@gnu.org>.
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
static Widget long_w;

static Widget unsigned_char_w;
static Widget binary_w;
static Widget address_format_w;
static Widget wide_char_w;
static Widget wide_string_w;

static MMDesc format_menu[] = { 
    { "o", MMPush, { SetFormatCB, 0 }, 0, &octal_w, 0, 0 },
    { "x", MMPush, { SetFormatCB, 0 }, 0, 0, 0, 0},
    { "d", MMPush, { SetFormatCB, 0 }, 0, 0, 0, 0},
    { "u", MMPush, { SetFormatCB, 0 }, 0, &unsigned_char_w, 0, 0 },
    { "t", MMPush, { SetFormatCB, 0 }, 0, &binary_w, 0, 0 },
    { "f", MMPush, { SetFormatCB, 0 }, 0, 0, 0, 0},
    { "a", MMPush, { SetFormatCB, 0 }, 0, &address_format_w, 0, 0 },
    { "i", MMPush, { SetFormatCB, 0 }, 0, 0, 0, 0},
    { "c", MMPush, { SetFormatCB, 0 }, 0, 0, 0, 0},
    { "C", MMPush, { SetFormatCB, 0 }, 0, &wide_char_w, 0, 0 },
    { "s", MMPush, { SetFormatCB, 0 }, 0, 0, 0, 0},
    { "W", MMPush, { SetFormatCB, 0 }, 0, &wide_string_w, 0, 0 },
    MMEnd
};

static MMDesc size_menu[] = { 
    { "b", MMPush, { SetSizeCB, 0 }, 0, &byte_w, 0, 0 },
    { "h", MMPush, { SetSizeCB, 0 }, 0, 0, 0, 0},
    { "w", MMPush, { SetSizeCB, 0 }, 0, 0, 0, 0},
    { "g", MMPush, { SetSizeCB, 0 }, 0, 0, 0, 0},
    { "G", MMPush, { SetSizeCB, 0 }, 0, &long_w, 0, 0 },
    MMEnd
};

static MMDesc examine_menu[] = { 
    { "examine", MMSpinBox,    MMNoCB, 0, &repeat_w, 0, 0 },
    { "format",  MMOptionMenu, MMNoCB, format_menu, 0, 0, 0 },
    { "size",    MMOptionMenu, MMNoCB, size_menu, 0, 0, 0 },
    { "address", MMComboBox,   MMNoCB, 0, &address_w, 0, 0 },
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
    String s_repeat = XmTextFieldGetString(repeat_w);
    string repeat(s_repeat);
    XtFree(s_repeat);

    String s_address = XmTextFieldGetString(address_w);
    string address(s_address);
    XtFree(s_address);

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
