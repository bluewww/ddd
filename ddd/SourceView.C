// $Id$
// Use the Source, Luke.

// Copyright (C) 1995-1997 Technische Universitaet Braunschweig, Germany.
// Written by Dorothea Luetkehaus <luetke@ips.cs.tu-bs.de> 
// and Andreas Zeller <zeller@ips.cs.tu-bs.de>
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

#ifdef __GNUG__
#pragma implementation
#endif

char SourceView_rcsid[] =
    "$Id$";


// Fixing Some Bugs on a Sunday Evening
// ------------------------------------
// 
// Whose bugs these are I think I know,
// But now he works at 3DO;
// He will not see me working here
// To fix his code and make it go.
// 
// The saner folk must think it queer
// To trace without the source code near
// After a launch and frozen mouse
// The weirdest stack crawl of the year.
// 
// I give my nodding head a shake
// To see if I can stay awake
// The only other thing to do
// Is find some more coffeine to take.
// 
// This bug is pretty hard to nip,
// But I have other ones to fix,
// And tons to go before we ship,
// And tons to go before we ship.
//
//
// Written by David A. Lyons <dlyons@apple.com>, January 1994
// (with apologies to Robert Frost)
// 
// Hey, it's fiction.  Close to reality in spirit,
// but does not refer to a specific project, bug, Sunday,
// or brand of soft drink.


//-----------------------------------------------------------------------------
#include "Delay.h"		// must be included first for GCC 2.5.8
#include "SourceView.h"
#include "IntArray.h"

// Misc stuff
#include "assert.h"
#include "HelpCB.h"
#include "DestroyCB.h"
#include "charsets.h"
#include "events.h"
#include "cook.h"
#include "misc.h"
#include "shorten.h"
#include "tabs.h"
#include "wm.h"
#include "TimeOut.h"

// Motif stuff
#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/MessageB.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <Xm/RowColumn.h>
#include <Xm/PushB.h>
#include <Xm/SelectioB.h>
#include <Xm/List.h>
#include <Xm/PanedW.h>
#include <Xm/ToggleB.h>
#include <X11/StringDefs.h>
#include <X11/cursorfont.h>

// LessTif hacks
#include <X11/IntrinsicP.h>
#include "LessTifH.h"

// System stuff
extern "C" {
#include <sys/types.h>
#include <sys/stat.h>
}
#include <stdio.h>
#include <fcntl.h>
#include <iomanip.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <errno.h>

// DDD stuff
#include "PosBuffer.h"
#include "string-fun.h"
#include "ddd.h"
#include "version.h"
#include "mydialogs.h"
#include "verify.h"
#include "Command.h"
#include "post.h"
#include "shell.h"
#include "windows.h"
#include "wm.h"
#include "dbx-lookup.h"
#include "question.h"
#include "status.h"
#include "file.h"
#include "AppData.h"
#include "options.h"
#include "charsets.h"
#include "regexps.h"
#include "index.h"




// Additional macros
inline int isid(char c)
{
    return isalnum(c) || c == '_' || c == '$';
}

// Test for regular file - see stat(3)
#ifndef S_ISREG
#define S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#endif


// ***************************************************************************
//-----------------------------------------------------------------------
// Xt stuff
//-----------------------------------------------------------------------
XtActionsRec SourceView::actions [] = {
    {"source-popup-menu",        SourceView::srcpopupAct        },
    {"source-start-select-word", SourceView::startSelectWordAct },
    {"source-end-select-word",   SourceView::endSelectWordAct   },
    {"source-update-glyphs",     SourceView::updateGlyphsAct    },
    {"source-drag-glyph",        SourceView::dragGlyphAct       },
    {"source-follow-glyph",      SourceView::followGlyphAct     },
    {"source-drop-glyph",        SourceView::dropGlyphAct       },
};

//-----------------------------------------------------------------------
// Menus
//-----------------------------------------------------------------------

// Popup menus - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
struct LineItms { enum Itms {SetBP, SetTempBP, Sep1, TempNContBP, 
			     Sep2, SetPC}; };
MMDesc SourceView::line_popup[] = 
{
    {"set",         MMPush, {SourceView::line_popup_setCB}},
    {"set_temp",    MMPush, {SourceView::line_popup_set_tempCB}},
    MMSep,
    {"temp_n_cont", MMPush, {SourceView::line_popup_temp_n_contCB}},
    MMSep,
    {"set_pc",      MMPush, {SourceView::line_popup_set_pcCB}},
    MMEnd
};

struct BPItms { enum Itms {Disable, Sep1, Condition, IgnoreCount, 
			   Sep2, Delete, Sep3, SetPC}; };
MMDesc SourceView::bp_popup[] =
{
    {"disable",      MMPush, {SourceView::bp_popup_disableCB}},
    MMSep,
    {"condition",    MMPush, {SourceView::EditBreakpointConditionCB}},
    {"ignore_count", MMPush, {SourceView::EditBreakpointIgnoreCountCB}},
    MMSep,
    {"delete",       MMPush, {SourceView::bp_popup_deleteCB}},
    MMSep,
    {"set_pc",       MMPush, {SourceView::bp_popup_set_pcCB}},
    MMEnd
};

struct BPButtons { enum Itms {New, Lookup, Enable, Disable, Condition, 
			      IgnoreCount, Delete}; };
MMDesc SourceView::bp_area[] =
{
    {"new",          MMPush,   {SourceView::NewBreakpointCB}},
    {"lookup",       MMPush,   {SourceView::LookupBreakpointCB}},
    {"enable",       MMPush,   {SourceView::BreakpointCmdCB, "enable"  }},
    {"disable",      MMPush,   {SourceView::BreakpointCmdCB, "disable" }},
    {"condition",    MMPush,   {SourceView::EditBreakpointConditionCB, 0}},
    {"ignore_count", MMPush,   {SourceView::EditBreakpointIgnoreCountCB, 0}},
    {"delete",       MMPush,   {SourceView::BreakpointCmdCB, "delete" }},
    MMEnd
};


struct TextItms { 
    enum Itms {
	Print, 
	Disp, 
	Dummy1, 
	PrintRef, 
	DispRef,
	Dummy2,
	Whatis,
	Dummy3,
	Lookup, 
	Break,
	Clear
    };
};

static String text_cmd_labels[] =
{
    "Print ", 
    "Display ", 
    "", 
    "Print ", 
    "Display ", 
    "",
    "What is ",
    "",
    "Lookup " , 
    "Break at ", 
    "Clear at "
};

MMDesc SourceView::text_popup[] =
{
    {"print",      MMPush, {SourceView::text_popup_printCB}},
    {"disp",       MMPush, {SourceView::text_popup_dispCB}},
    MMSep,
    {"printRef",   MMPush, {SourceView::text_popup_print_refCB}},
    {"dispRef",    MMPush, {SourceView::text_popup_disp_refCB}},
    MMSep,
    {"whatis",     MMPush, {SourceView::text_popup_whatisCB}},
    MMSep,
    {"lookup",     MMPush, {SourceView::text_popup_lookupCB}},
    {"breakAt",    MMPush, {SourceView::text_popup_breakCB}},
    {"clearAt",    MMPush, {SourceView::text_popup_clearCB}},
    MMEnd
};


//-----------------------------------------------------------------------
// Glyphs and images
//-----------------------------------------------------------------------

#include "arrow.xbm"
static XImage arrow_image = {
    arrow_width,		// width
    arrow_height,		// height
    0,				// xoffset
    XYBitmap,			// format
    arrow_bits,			// data
    MSBFirst,			// byte_order
    8,				// bitmap_unit
    LSBFirst,			// bitmap_bit_order
    8,				// bitmap_pad
    1,				// depth
    2				// bytes_per_line
};

#include "greyarrow.xbm"
static XImage grey_arrow_image = {
    grey_arrow_width,		// width
    grey_arrow_height,		// height
    0,				// xoffset
    XYBitmap,			// format
    grey_arrow_bits,		// data
    MSBFirst,			// byte_order
    8,				// bitmap_unit
    LSBFirst,			// bitmap_bit_order
    8,				// bitmap_pad
    1,				// depth
    2				// bytes_per_line
};

#include "signalarrow.xbm"
static XImage signal_arrow_image = {
    signal_arrow_width,		// width
    signal_arrow_height,	// height
    0,				// xoffset
    XYBitmap,			// format
    signal_arrow_bits,		// data
    MSBFirst,			// byte_order
    8,				// bitmap_unit
    LSBFirst,			// bitmap_bit_order
    8,				// bitmap_pad
    1,				// depth
    2				// bytes_per_line
};

#include "temparrow.xbm"
static XImage temp_arrow_image = {
    temp_arrow_width,		// width
    temp_arrow_height,		// height
    0,				// xoffset
    XYBitmap,			// format
    temp_arrow_bits,		// data
    MSBFirst,			// byte_order
    8,				// bitmap_unit
    LSBFirst,			// bitmap_bit_order
    8,				// bitmap_pad
    1,				// depth
    2				// bytes_per_line
};

#include "stop.xbm"
static XImage stop_image = {
    stop_width,			// width
    stop_height,		// height
    0,				// xoffset
    XYBitmap,			// format
    stop_bits,			// data
    MSBFirst,			// byte_order
    8,				// bitmap_unit
    LSBFirst,			// bitmap_bit_order
    8,				// bitmap_pad
    1,				// depth
    2				// bytes_per_line
};

#include "greystop.xbm"
static XImage grey_stop_image = {
    grey_stop_width,		// width
    grey_stop_height,		// height
    0,				// xoffset
    XYBitmap,			// format
    grey_stop_bits,		// data
    MSBFirst,			// byte_order
    8,				// bitmap_unit
    LSBFirst,			// bitmap_bit_order
    8,				// bitmap_pad
    1,				// depth
    2				// bytes_per_line
};

#include "tempstop.xbm"
static XImage temp_stop_image = {
    temp_stop_width,		// width
    temp_stop_height,		// height
    0,				// xoffset
    XYBitmap,			// format
    temp_stop_bits,		// data
    MSBFirst,			// byte_order
    8,				// bitmap_unit
    LSBFirst,			// bitmap_bit_order
    8,				// bitmap_pad
    1,				// depth
    2				// bytes_per_line
};



//-----------------------------------------------------------------------
// Data.  Lots of 'em.
//-----------------------------------------------------------------------

Widget SourceView::toplevel_w                = 0;
Widget SourceView::source_form_w             = 0;
Widget SourceView::source_text_w             = 0;
Widget SourceView::code_form_w               = 0;
Widget SourceView::code_text_w               = 0;
Widget SourceView::edit_breakpoints_dialog_w = 0;
Widget SourceView::breakpoint_list_w         = 0;
Widget SourceView::stack_dialog_w            = 0;
Widget SourceView::frame_list_w              = 0;
Widget SourceView::up_w                      = 0;
Widget SourceView::down_w                    = 0;
Widget SourceView::register_dialog_w         = 0;
Widget SourceView::register_list_w           = 0;
Widget SourceView::all_registers_w           = 0;
Widget SourceView::int_registers_w           = 0;
Widget SourceView::thread_dialog_w           = 0;
Widget SourceView::thread_list_w             = 0;

bool SourceView::stack_dialog_popped_up    = false;
bool SourceView::register_dialog_popped_up = false;
bool SourceView::thread_dialog_popped_up   = false;

bool SourceView::cache_source_files     = true;
bool SourceView::cache_machine_code     = true;
bool SourceView::display_glyphs         = true;
bool SourceView::display_line_numbers   = false;
bool SourceView::disassemble            = true;
bool SourceView::all_registers          = false;

int  SourceView::source_indent_amount = 4;
int  SourceView::code_indent_amount   = 4;
int  SourceView::line_indent_amount   = 4;
int  SourceView::tab_width            = 8;

int  SourceView::lines_above_cursor   = 2;
int  SourceView::lines_below_cursor   = 3;

SourceOrigin SourceView::current_origin = ORIGIN_NONE;

Map<int, BreakPoint> SourceView::bp_map;

string SourceView::current_file_name = "";
int    SourceView::line_count = 0;
IntIntArrayAssoc SourceView::bps_in_line;
TextPositionArray SourceView::_pos_of_line;
StringArray SourceView::bp_addresses;
StringStringAssoc SourceView::file_cache;
StringOriginAssoc SourceView::origin_cache;
StringStringAssoc SourceView::source_name_cache;
CodeCache SourceView::code_cache;

string SourceView::current_source;
string SourceView::current_code;
string SourceView::current_code_start;
string SourceView::current_code_end;

string SourceView::current_pwd = ".";

XmTextPosition SourceView::last_top                = 0;
XmTextPosition SourceView::last_pos                = 0;
XmTextPosition SourceView::last_start_highlight    = 0;
XmTextPosition SourceView::last_end_highlight      = 0;

XmTextPosition SourceView::last_top_pc             = 0;
XmTextPosition SourceView::last_pos_pc             = 0;
XmTextPosition SourceView::last_start_highlight_pc = 0;
XmTextPosition SourceView::last_end_highlight_pc   = 0;

XmTextPosition SourceView::last_start_secondary_highlight = 0;
XmTextPosition SourceView::last_end_secondary_highlight = 0;

XmTextPosition SourceView::last_start_secondary_highlight_pc = 0;
XmTextPosition SourceView::last_end_secondary_highlight_pc = 0;

string SourceView::last_execution_file = "";
int    SourceView::last_execution_line = 0;
string SourceView::last_execution_pc   = "";
string SourceView::last_shown_pc       = "";

int SourceView::last_frame_pos = 0;
bool SourceView::frame_pos_locked = false;
int SourceView::current_frame = -1;

StringArray SourceView::history;
int SourceView::history_position = 0;
bool SourceView::code_history_locked = false;
bool SourceView::source_history_locked = false;

bool SourceView::checking_scroll = false;

bool SourceView::at_lowest_frame = true;
bool SourceView::signal_received = false;

int SourceView::max_popup_expr_length = 20;

int SourceView::max_breakpoint_number_seen = 0;

//-----------------------------------------------------------------------
// Selection stuff
//-----------------------------------------------------------------------

static XmTextPosition selection_startpos;
static XmTextPosition selection_endpos;
static XmTextPosition selection_pos;
static Time           selection_time;
static XEvent         selection_event;


//-----------------------------------------------------------------------
// Helping functions.
//-----------------------------------------------------------------------

// Sort A
static void sort(IntArray& a)
{
    // Shell sort -- simple and fast
    int h = 1;
    do {
	h = h * 3 + 1;
    } while (h <= a.size());
    do {
	h /= 3;
	for (int i = h; i < a.size(); i++)
	{
	    int v = a[i];
	    int j;
	    for (j = i; j >= h && a[j - h] > v; j -= h)
		a[j] = a[j - h];
	    if (i != j)
		a[j] = v;
	}
    } while (h != 1);
}

// Return index of RXADDRESS in S, beginning from POS.  Stop search at newline.
static int address_index(const string& s, int pos)
{
    int eol = s.index('\n');
    if (eol < 0)
	eol = s.length();

    string first_line = ((string&)s).at(pos, eol - pos);
    int i = 0;
    while (i < int(first_line.length()) && isspace(first_line[i]))
	i++;
    i = first_line.index(rxaddress_start, i);
    if (i < 0)
	return -1;
    else
	return pos + i;
}

// Return true if W is a descendant of code_form_w
bool SourceView::is_code_widget(Widget w)
{
    while (w != 0)
    {
	if (w == code_form_w)
	    return true;
	else
	    w = XtParent(w);
    }
    return false;
}

// Return true if W is a descendant of source_form_w
bool SourceView::is_source_widget(Widget w)
{
    while (w != 0)
    {
	if (w == source_form_w)
	    return true;
	else
	    w = XtParent(w);
    }
    return false;
}

string& SourceView::current_text(Widget w)
{
    assert(is_source_widget(w) || is_code_widget(w));

    if (is_code_widget(w))
	return current_code;
    else
	return current_source;
}

int SourceView::indent_amount(Widget w)
{
    assert(is_source_widget(w) || is_code_widget(w));

    if (is_code_widget(w))
	return code_indent_amount;
    else if (display_line_numbers)
	return source_indent_amount + line_indent_amount;
    else
	return source_indent_amount;
}


//-----------------------------------------------------------------------
// Methods
//-----------------------------------------------------------------------


// ***************************************************************************
//
void SourceView::line_popup_setCB (Widget w,
				   XtPointer client_data,
				   XtPointer)
{
    string address = *((string *)client_data);
    create_bp(address, w);
}

void SourceView::line_popup_set_tempCB (Widget w,
					XtPointer client_data,
					XtPointer)
{
    string address = *((string *)client_data);
    create_temp_bp(address, w);
}

// Create or clear a breakpoint at position A.  If SET, create a
// breakpoint; if not SET, delete it.  If TEMP, make the breakpoint
// temporary.  If COND is given, break only iff COND evals to true. W
// is the origin.
void SourceView::set_bp(const string& a, bool set, bool temp, 
			const string& cond, Widget w)
{
    int new_bps = max_breakpoint_number_seen + 1;
    string address = a;

    if (address != "" && address[0] == '0')
	address = "*" + address; // Address given

    if (!set)
    {
	// Clear bp
	gdb_command(clear_command(address), w);
    }
    else
    {
	// Set bp
	switch (gdb->type())
	{
	case GDB:
	    if (temp)
		gdb_command("tbreak " + address, w);
	    else
		gdb_command("break " + address, w);

	    break;


	case DBX:
	{
	    string cond_suffix = "";
	    if (cond != "")
	    {
		if (gdb->has_handler_command())
		    cond_suffix = " -if " + cond;
		else
		    cond_suffix = " if " + cond;
	    }

	    if (address.contains('*', 0))
	    {
		// Address given
		address = address.after('*');
		gdb_command("stopi at " + address + cond_suffix, w);

		if (temp)
		{
		    syncCommandQueue();
		    gdb_command("when $pc == " + address + " "
				+ command_list(clear_command(address, true, 
							     new_bps)),
				w);
		}
	    }
	    else
	    {
		string line = "";
		if (address.matches(rxint))
		{
		    // Line number given
		    line = address;
		    gdb_command("stop at " + address + cond_suffix, w);
		}
		else if (address.contains(":") && !address.contains("::"))
		{
		    // File:line given
		    string file = address.before(':');
		    line = address.after(':');

		    gdb_command("file " + file, w);
		    gdb_command("stop at " + line + cond_suffix, w);
		}
		else
		{
		    // Function given
		    string pos = dbx_lookup(address);

		    if (pos.contains(':'))
		    {
			string file = pos.before(':');
			line = pos.after(':');

			gdb_command("file " + file, w);
			gdb_command("stop at " + line + cond_suffix, w);
		    }
		    else
		    {
			// Cannot determine function position - try this one
			gdb_command("stop in " + address + cond_suffix, w);
		    }
		}

		if (temp && line != "")
		{
		    syncCommandQueue();
		    string clear_cmd = clear_command(line, true, new_bps);
		    gdb_command("when at " + line + " " 
				+ command_list(clear_cmd), w);
		}
	    }
	    break;
	}

	case XDB:
	{
	    string command;
	    if (address.contains('*', 0))
		command = "ba " + address.after('*');
	    else
		command = "b " + address;

	    if (temp)
		command += " \\1t";

	    if (cond != "" && !gdb->has_condition_command())
		command += " {if " + cond + " {} {Q;c}}";

	    gdb_command(command, w);
	    break;
	}
	}

	if (cond != "" && gdb->has_condition_command())
	{
	    // Add condition
	    gdb_command(gdb->condition_command(itostring(new_bps), cond), w);
	}
    }
}

// ***************************************************************************
//
void SourceView::clearBP(XtPointer client_data, XtIntervalId *)
{
    int bp_nr = int(client_data);
    BreakPoint *bp = bp_map.get(bp_nr);
    if (bp != 0)
	bp_popup_deleteCB(source_text_w, XtPointer(&bp_nr), 0);
}

// Save last `jump' target for XDB
static string last_jump_address;

void SourceView::clearJumpBP(const string& msg, void *data)
{
    set_status(msg);

    if (gdb->type() == XDB && msg == "")
    {
	// Moving PC was successful.
	show_execution_position(last_jump_address, true);
    }

    int old_max_breakpoint_number_seen = int(data);

    for (int i = old_max_breakpoint_number_seen + 1;
	 i <= max_breakpoint_number_seen; i++)
    {
	// Delete all recently created breakpoints
	XtAppAddTimeOut(XtWidgetToApplicationContext(source_text_w),
			0, clearBP, XtPointer(i));
    }
}

void SourceView::line_popup_temp_n_contCB (Widget w,
					   XtPointer client_data,
					   XtPointer)
{
    string address = *((string *)client_data);
    temp_n_cont(address, w);
}

void SourceView::temp_n_cont(const string& a, Widget w)
{
    string address = a;

    switch (gdb->type())
    {
    case GDB:
#if 0				// GDB `until' only works in the current frame
	gdb_command("until " + address, w);
	break;
#endif
    
    case DBX:
    {
	int old_max_breakpoint_number_seen = max_breakpoint_number_seen;

	// Create a temporary breakpoint
	create_temp_bp(address, w);

	// Make sure the temporary breakpoint is deleted after `cont'
	Command c("cont", w);
	c.callback = clearJumpBP;
	c.data     = XtPointer(old_max_breakpoint_number_seen);
	gdb_command(c);
	break;
    }

    case XDB:
	if (address.contains('*', 0))
	    address = address.after('*');
	gdb_command("c " + address, w);
	break;
    }
}

// ***************************************************************************
//
void SourceView::line_popup_set_pcCB(Widget w, 
				     XtPointer client_data,
				     XtPointer)
{
    string address = *((string *)client_data);
    move_pc(address, w);
}

// ***************************************************************************
//
void SourceView::move_pc(const string& a, Widget w)
{
    string address = a;

    if (address.contains('*', 0))
    {
	if (compare_address(address.after('*'), last_execution_pc) == 0)
	    return;		// PC already at address
    }
    else
    {
	string file = address.before(':');
	int line    = get_positive_nr(address.after(':'));

	if (file_matches(file, last_execution_file)
	    && line == last_execution_line)
	    return;		// PC already at address
    }

    if (gdb->has_jump_command())
    {
	int old_max_breakpoint_number_seen = max_breakpoint_number_seen;

	// We prefer the GDB `jump' command to setting `$pc'
	// immediately since `jump' requires confirmation when jumping
	// out of the current function.

	if (gdb->type() != XDB)
	{
	    // GDB and DBX immediately resume execution, which we don't
	    // want.  Create a temporary breakpoint at ADDRESS.
	    create_temp_bp(address, w);
	}

	switch (gdb->type())
	{
	case DBX:
	    // DBX `cont at ' requires a line number.
	    gdb_command("file " + address.before(':'));
	    // FALL THROUGH

	case XDB:
	    // XDB 'g' wants only a line number
	    address = address.after(':');
	    break;

	case GDB:
	    break;
	}

	// Jump to the new address and clear the breakpoint again
	last_jump_address = a;
	Command c(gdb->jump_command(address), w);
	c.callback = clearJumpBP;
	c.data     = XtPointer(old_max_breakpoint_number_seen);
	gdb_command(c);
    }
    else
    {
	// Use the `set $pc = ADDR' alternative.
	if (address.contains('*', 0))
	{
	    address = address.after('*');
	}
	else
	{
	    lookup(address, true);
	    syncCommandQueue();
	    address = last_shown_pc;
	}

	if (address == "")
	    set_status("Cannot determine address of " + a);
	else
	    gdb_command(gdb->assign_command("$pc", address), w);
    }
}

void SourceView::move_bp(int bp_nr, const string& a, Widget w)
{
    string address = a;

    // clog << "Moving breakpoint " << bp_nr << " to " << address << '\n';

    BreakPoint *bp = bp_map.get(bp_nr);
    if (bp == 0)
	return;			// No such breakpoint

    if (address.contains('*', 0))
    {
	if (compare_address(address.after('*'), bp->address()) == 0)
	    return;		// Breakpoint already at address
    }
    else
    {
	string file = address.before(':');
	int line    = get_positive_nr(address.after(':'));

	if (file_matches(file, bp->file_name()) && line == bp->line_nr())
	    return;		// Breakpoint already at address
    }

    // Create a new breakpoint at ADDRESS, making it inherit the
    // current settings
    ostrstream os;
    bool ok = bp->get_state(os, 0, false, address);
    if (!ok)
	return;			// Command failed

    string commands(os);
    commands.gsub("@0@", itostring(next_breakpoint_number()));

    while (commands != "")
    {
	string command = commands.before('\n');
	gdb_command(command, w);
	commands = commands.after('\n');
    }

    // Delete old breakpoint
    bp_popup_deleteCB(w, XtPointer(&bp_nr), XtPointer(0));
}

void SourceView::set_bp_cond(int bp_nr, const string& cond, Widget w)
{
    if (gdb->has_condition_command())
    {
	gdb_command(gdb->condition_command(itostring(bp_nr), cond), w);
	return;
    }

    BreakPoint *bp = bp_map.get(bp_nr);
    if (bp == 0)
	return;			// No such breakpoint

    // Create a new breakpoint with a new condition COND, making it
    // inherit the current settings
    ostrstream os;
    bool ok = bp->get_state(os, 0, false, "", cond);
    if (!ok)
	return;			// Command failed

    string commands(os);
    commands.gsub("@0@", itostring(next_breakpoint_number()));

    while (commands != "")
    {
	string command = commands.before('\n');
	gdb_command(command, w);
	commands = commands.after('\n');
    }

    // Delete old breakpoint
    bp_popup_deleteCB(w, XtPointer(&bp_nr), XtPointer(0));
}


// ***************************************************************************
//
void SourceView::bp_popup_deleteCB (Widget w,
				    XtPointer client_data,
				    XtPointer)
{
    int bp_nr = *((int *)client_data);
    gdb_command(gdb->delete_command(itostring(bp_nr)), w);
}


// ***************************************************************************
//
void SourceView::bp_popup_disableCB (Widget w, 
				     XtPointer client_data,
				     XtPointer)
{
    int bp_nr = *((int *)client_data);
    BreakPoint *bp = bp_map.get(bp_nr);
    if (bp != 0)
    {
	if (bp->enabled())
	    disable_bp(bp_nr, w);
	else
	    enable_bp(bp_nr, w);
    }
}

void SourceView::enable_bp(int nr, Widget w)
{
    gdb_command(gdb->enable_command(itostring(nr)), w);
}

void SourceView::disable_bp(int nr, Widget w)
{
    gdb_command(gdb->disable_command(itostring(nr)), w);
}


// ***************************************************************************
//
void SourceView::bp_popup_set_pcCB(Widget w, XtPointer client_data, 
				   XtPointer call_data)
{
    int bp_nr = *((int *)client_data);
    BreakPoint *bp = bp_map.get(bp_nr);
    if (bp != 0 && bp->address() != "")
    {
	string address = string('*') + bp->address();
	line_popup_set_pcCB(w, XtPointer(&address), call_data);
    }
}

// ***************************************************************************
//
void SourceView::text_popup_breakCB (Widget w,
				     XtPointer client_data,
				     XtPointer)
{
    string* word_ptr = (string*)client_data;
    create_bp(*word_ptr, w);
}

void SourceView::text_popup_clearCB (Widget w, 
				     XtPointer client_data, 
				     XtPointer)
{
    string* word_ptr = (string*)client_data;
    clear_bp(*word_ptr, w);
}



// ***************************************************************************
//
void SourceView::text_popup_printCB (Widget w, 
				     XtPointer client_data, 
				     XtPointer)
{
    string* word_ptr = (string*)client_data;
    assert(word_ptr->length() > 0);

    gdb_command(gdb->print_command(*word_ptr, false), w);
}

void SourceView::text_popup_print_refCB (Widget w, 
					 XtPointer client_data, XtPointer)
{
    string* word_ptr = (string*)client_data;
    assert(word_ptr->length() > 0);

    gdb_command(gdb->print_command(gdb->dereferenced_expr(*word_ptr), false), 
		w);
}


// ***************************************************************************
//
void SourceView::text_popup_dispCB (Widget w, XtPointer client_data, XtPointer)
{
    string* word_ptr = (string*)client_data;
    assert(word_ptr->length() > 0);

    gdb_command("graph display " + *word_ptr, w);
}

void SourceView::text_popup_disp_refCB (Widget w, 
					XtPointer client_data, XtPointer)
{
    string* word_ptr = (string*)client_data;
    assert(word_ptr->length() > 0);

    gdb_command("graph display " + gdb->dereferenced_expr(*word_ptr), w);
}

// ***************************************************************************
//
void SourceView::text_popup_whatisCB (Widget w, XtPointer client_data, 
				      XtPointer)
{
    string* word_ptr = (string*)client_data;
    assert(word_ptr->length() > 0);

    gdb_command(gdb->whatis_command(*word_ptr), w);
}

// ***************************************************************************
//
void SourceView::text_popup_lookupCB (Widget, XtPointer client_data, XtPointer)
{
    string* word_ptr = (string*)client_data;
    lookup(*word_ptr);
}


// ***************************************************************************
//

// Return the normalized full path of FILE
string SourceView::full_path(string file)
{
    /* Chris van Engelen <engelen@lucent.com>, Jul 10, 1997
     *
     * The regular expression is used to remove parts from the full path
     * which look like "/aap/../". However, it should ***NOT*** remove
     * the sequence "/../../" from the path, obviously ! On the other
     * hand, sequences like "/.tmpdir.test/../" should be removed.
     * Therefore, the regular expression reads now like:
     *
     * - forward slash
     * - zero or more  characters other than forward slash (dot is allowed)
     * - any character other than forward slash or dot: this makes sure that
     *   a sequence like "/../../" is not modified.
     * - forward slash, two dots, and the final forward slash.
     *
     * The only valid patterns which are not normalized are patterns ending
     * in a dot: too bad, you can't win them all.
     */

#if RUNTIME_REGEX
    static regex rxdotdot("/[^/]*[^/.]/\\.\\./");
#endif
    unsigned int file_length, prev_file_length;

    if (file == "")
        return current_pwd;

    if (file[0] != '/')
        file = current_pwd + "/" + file;

    /* CvE, Jul 10, 1997
     *
     * Repeatedly remove patterns like /dir1/../ from the file name.
     * Note that a number of /../ patterns may follow each other, like
     * in "/dir1/dir1/dir3/../../../"
     */
    file_length = file.length();
    do {
        prev_file_length = file_length;
        file.gsub(rxdotdot, "/");
        file_length=file.length();
    } while (file_length != prev_file_length);

    /* CvE, Jul 10, 1997
     *
     *
     * Repeatedly remove pattern /./ from the file name.
     * Note that a number of /./ patterns may follow each other.
     * Note that if the first parameter of gsub is a C-string,
     * the pattern is not regarded to be a regular expression,
     * so the dot in the pattern does not need to be escaped!
     */
    file_length = file.length();
    do {
        prev_file_length = file_length;
        file.gsub("/./", "/");
        file_length=file.length();
    } while (file_length != prev_file_length);

    return file;
}

// Return the basename of FILE.  We don't use the default ::basename(),
// due to conflicts with the decl in <libiberty.h>.
const char *SourceView::basename(const char *name)
{
    const char *base = name;

    while (*name)
    {
	if (*name++ == '/')
	    base = name;
    }

    return base;
}

bool SourceView::file_matches(const string& file1, const string& file2)
{
    if (gdb->type() == GDB || app_data.use_source_path)
	return file1 == file2 || full_path(file1) == full_path(file2);
    else
	return base_matches(file1, file2);
}

bool SourceView::base_matches(const string& file1, const string& file2)
{
    return string(basename(file1.chars())) == string(basename(file2.chars()));
}

// Check if BP occurs in the current source text
bool SourceView::bp_matches(BreakPoint *bp)
{
    return bp->type() == BREAKPOINT
	    && (bp->file_name() == ""
		|| file_matches(bp->file_name(), current_source_name())
		|| file_matches(bp->file_name(), current_file_name));
}

// ***************************************************************************
//

// If this is true, no motion occurred while selecting
static bool selection_click = false;

static string last_info_output = "";

void SourceView::set_source_argCB(Widget text_w, 
				  XtPointer client_data, 
				  XtPointer call_data)
{
    string& text = current_text(text_w);
    if (text == "")
	return;

    if (bool(client_data))
	selection_click = false;

    XmTextPosition startPos, endPos;
    Boolean have_selection = 
	XmTextGetSelectionPosition(text_w, &startPos, &endPos);

    if (!have_selection)
    {
	// No selection?  If the current motion was caused by a mouse
	// click, fetch word at current cursor position instead.
	XmTextVerifyCallbackStruct *cbs = 
	    (XmTextVerifyCallbackStruct *)call_data;
	if (cbs != 0 && cbs->reason == XmCR_MOVING_INSERT_CURSOR)
	{
	    XEvent *event = cbs->event;
	    if (event == 0)
	    {
		// LessTif 0.79 may not pass a reasonable event here.
#if XtSpecificationRelease >= 6
		// Use the last processed event instead.
		event = XtLastEventProcessed(XtDisplay(text_w));
#else
		// Use the last recorded selection event instead.
		event = &selection_event;
#endif
		selection_click = true;
	    }

	    if (event != 0 && (event->type == ButtonPress ||
			       event->type == ButtonRelease))
	    {
		find_word_bounds(text_w, cbs->newInsert, 
				 startPos, endPos);
		have_selection = True;
	    }
	}
    }

    // Don't use this selection event again.
    selection_event.type = KeyPress;

    if (!have_selection)
    {
	// No selection - sorry
	return;
    }

    int startIndex = 0;
    if (startPos > 0)
	startIndex = text.index('\n', startPos - text.length()) + 1;

    int endIndex = 0;
    if (endPos > 0)
	endIndex = text.index('\n', endPos - text.length()) + 1;

    bool bp_selected = false;
    if (selection_click
	&& startIndex == endIndex
	&& startPos < XmTextPosition(text.length())
	&& endPos < XmTextPosition(text.length())
	&& text[startPos] != '\n'
	&& text[endPos] != '\n')
    {
	if (text_w == source_text_w)
	{
	    int line_nr = 0;
	    bool in_text;
	    int bp_nr;
	    string address;

	    if (get_line_of_pos(source_text_w, startPos, line_nr, address, 
				in_text, bp_nr) 
		&& !in_text)
	    {
		// Selection from line number area: prepend source file name
		source_arg->set_string(current_source_name() 
				       + ":" + itostring(line_nr));

		// If a breakpoint is here, select this one only
		MapRef ref;
		for (BreakPoint* bp = bp_map.first(ref);
		     bp != 0;
		     bp = bp_map.next(ref))
		{
		    bp->selected() = 
			bp_matches(bp) && bp->line_nr() == line_nr;
		}

		bp_selected = true;
	    }
	}
	else if (text_w == code_text_w
		 && startPos - startIndex <= indent_amount(text_w)
		 && endPos - endIndex <= indent_amount(text_w))
	{
	    // Selection from address area
	    int index = address_index(text, startPos);
	    if (index >= 0)
	    {
		string address = text.from(index);
		address = address.through(rxaddress);

		source_arg->set_string(address);

		// If a breakpoint is here, select this one only
		MapRef ref;
		for (BreakPoint* bp = bp_map.first(ref);
		     bp != 0;
		     bp = bp_map.next(ref))
		{
		    bp->selected() = 
			(bp->type() == BREAKPOINT
			 && (compare_address(address, bp->address()) == 0));
		}
		
		bp_selected = true;
	    }
	}
    }

    if (bp_selected)
    {
	// Update breakpoint selection
	process_breakpoints(last_info_output);
    }
    else
    {
	// Selection from source or code
	string s;
	if (startPos < XmTextPosition(text.length())
	    && endPos < XmTextPosition(text.length()))
	{
	    s = text(startPos, endPos - startPos);
	}

	while (s.contains('\n'))
	    s = s.after('\n');

	if (s != "")
	    source_arg->set_string(s);
    }
}

BreakPoint *SourceView::bp_at(string arg)
{
    MapRef ref;
    for (BreakPoint* bp = bp_map.first(ref); bp != 0; bp = bp_map.next(ref))
    {
	if (arg.matches(rxint))
	{
	    // Line number for current source given
	    if (bp_matches(bp) && bp->line_nr() == atoi(arg))
		return bp;
	}
	else
	{
	    string pos = arg;

	    if (!pos.contains(":") || pos.contains("::"))
	    {
		// Function given
		if (bp->arg() == pos)
		    return bp;

		if (gdb->type() == DBX)
		    pos = dbx_lookup(arg);
	    }
	    
	    if (pos.contains(":") && !pos.contains("::"))
	    {
		// File:line given
		string file = pos.before(':');
		string line = pos.after(':');

		if (file_matches(bp->file_name(), file) 
		    && bp->line_nr() == atoi(line))
		    return bp;
	    }
	}
    }

    return 0;
}


// ***************************************************************************

// Show position POS in TEXT_W, scrolling nicely
void SourceView::ShowPosition(Widget text_w, XmTextPosition pos, bool fromTop)
{
    string& text = current_text(text_w);
    if (text.length() == 0)
	return;			// No position to show

    short rows = 0;
    XmTextPosition current_top = 0;
    XtVaGetValues(text_w,
		  XmNrows, &rows,
		  XmNtopCharacter, &current_top,
		  NULL);

    // Find current relative row
    short relative_row = 1;
    for (XmTextPosition p = min(text.length() - 1, pos); p > current_top; p--)
	if (text[p] == '\n')
	    relative_row++;

    if (relative_row <= lines_above_cursor 
	|| relative_row >= rows - (lines_below_cursor + 1))
    {
	// Determine new TOP position
	short n = rows / 2;	// #Lines between new TOP and POS
	if (fromTop || relative_row <= lines_above_cursor)
	    n = lines_above_cursor;
	else if (relative_row >= rows - (lines_below_cursor + 1))
	    n = rows - (lines_below_cursor + 1);

	XmTextPosition new_top = pos;
	for (;;) {
	    while (new_top > 0 && text[new_top - 1] != '\n')
		new_top--;
	    if (new_top == 0 || n-- <= 0)
		break;
	    new_top--;
	}

	XmTextSetTopCharacter(text_w, new_top);
    }

    XmTextShowPosition(text_w, pos);	// just to make sure
}

void SourceView::SetInsertionPosition(Widget text_w, 
				      XmTextPosition pos, bool fromTop)
{
    ShowPosition(text_w, pos, fromTop);
    XmTextSetInsertionPosition(text_w, pos);
}


// ***************************************************************************

// Read local file from FILE_NAME
String SourceView::read_local(const string& file_name, long& length,
			      bool silent)
{
    StatusDelay delay("Reading file " + quote(file_name));
    length = 0;

    // Make sure the file is a regular text file and open it
    int fd;
    if ((fd = open(file_name, O_RDONLY)) < 0)
    {
	if (!silent)
	    post_error (file_name + ": " + strerror(errno), 
			"source_file_error", source_text_w);
        return 0;
    }

    struct stat statb;
    if (fstat(fd, &statb) < 0)
    {
	if (!silent)
	    post_error (file_name + ": " + strerror(errno), 
			"source_file_error", source_text_w);
	return 0;
    }

    // Avoid loading from directory, socket, device, or otherwise.
    if (!S_ISREG(statb.st_mode))
    {
	if (!silent)
	    post_error (file_name + ": not a regular file", 
			"source_file_error", source_text_w);
	return 0;
    }

    // Put the contents of the file in the Text widget by allocating
    // enough space for the entire file and reading the file into the
    // allocated space.
    char* text = XtMalloc(unsigned(statb.st_size + 1));
    if ((length = read(fd, text, statb.st_size)) != statb.st_size)
    {
	if (!silent)
	    post_error (file_name + ": " + strerror(errno),
			"source_trunc_error", source_text_w);
    }
    close(fd);

    text[statb.st_size] = '\0'; // be sure to null-terminate

    if (statb.st_size == 0)
    {
	if (!silent)
	    post_warning(file_name + ": empty file",
			 "source_empty_warning", source_text_w);
    }

    return text;
}


// Read (possibly remote) file FILE_NAME; a little slower
String SourceView::read_remote(const string& file_name, long& length, 
			       bool silent)
{
    StatusDelay delay("Reading file " + 
		      quote(file_name) + " from " + gdb_host);
    length = 0;

    string cat_command = sh_command("cat " + file_name);

    Agent cat(cat_command);
    cat.start();

    FILE *fp = cat.inputfp();
    if (fp == 0)
	return 0;

    String text = XtMalloc(1);

    do {
	text = XtRealloc(text, length + BUFSIZ + 1);
	length += fread(text + length, sizeof(char), BUFSIZ, fp);
    } while (!feof(fp));

    text[length] = '\0';  // be sure to null-terminate

    if (length == 0)
    {
	if (!silent)
	    post_error("Cannot access remote file " + quote(file_name), 
		       "remote_file_error", source_text_w);
    }

    return text;
}


// Read file FILE_NAME via the GDB `list' function
// Really slow, is guaranteed to work for source files.
String SourceView::read_from_gdb(const string& file_name, long& length, 
				 bool /* silent */)
{
    length = 0;
    if (!gdb->isReadyWithPrompt())
	return 0;

    StatusDelay delay("Reading file " + quote(file_name));

    string command;
    switch (gdb->type())
    {
    case GDB:
	command = "list " + file_name + ":1,1000000";
	break;

    case DBX:
	command = "list 1,1000000";
	break;

    case XDB:
	command = "w 1000000";
	break;
    }
    string listing = gdb_question(command, -1, true);

    // GDB listings have the format <NUMBER>\t<LINE>.
    // Copy LINE only; line numbers will be re-added later.
    // Note that tabs may be expanded to spaces due to a PTY interface.
    String text = XtMalloc(listing.length());

    int i = 0;
    length = 0;
    while (i < int(listing.length()))
    {
	int count = 0;

	// Skip leading spaces.  Some debuggers also issue `*', `=',
	// or `>' to indicate the current position.
	while (count < 8
	       && i < int(listing.length())
	       && (isspace(listing[i])
		   || listing[i] == '=' 
		   || listing[i] == '*'
		   || listing[i] == '>'))
	    i++, count++;

	if (isdigit(listing[i]))
	{
	    // Skip line number
	    while (i < int(listing.length()) && isdigit(listing[i]))
		i++, count++;

	    // Skip `:' (XDB output)
	    if (count < 8 && i < int(listing.length()) && listing[i] == ':')
		i++, count++;

	    // Break at first non-blank character or after 8 characters
	    while (count < 8 && i < int(listing.length()) && listing[i] == ' ')
		i++, count++;

	    // Skip tab character
	    if (count < 8 && i < int(listing.length()) && listing[i] == '\t')
		i++;

	    // Copy line
	    while (i < int(listing.length()) && listing[i] != '\n')
		text[length++] = listing[i++];

	    // Copy newline character
	    text[length++] = '\n';
	    i++;
	}
	else
	{
	    int start = i;

	    // Some other line -- the prompt, maybe?
	    while (i < int(listing.length()) && listing[i] != '\n')
		i++;
	    if (i < int(listing.length()))
		i++;

	    string msg = listing.from(start);
	    msg = msg.before('\n');
	    if (!msg.contains("end of file")) // XDB issues this
		post_gdb_message(msg, source_text_w);
	}
    }

    text[length] = '\0';  // be sure to null-terminate
    return text;
}


// Read file FILE_NAME and format it
String SourceView::read_indented(string& file_name, long& length, 
				 SourceOrigin& origin, bool silent)
{
    length = 0;
    Delay delay;
    long t;
	
    String text = 0;
    origin = ORIGIN_NONE;
    string full_file_name = file_name;

    for (int trial = 1; (text == 0 || length == 0) && trial <= 2; trial++)
    {
	switch (trial)
	{
	case 1:
	    // Loop #1: use full path of file
	    full_file_name = full_path(file_name);
	    break;

	case 2:
	    // Loop #2: ask debugger for full path, using `edit'
	    full_file_name = full_path(dbx_path(file_name));
	    if (full_file_name == full_path(file_name))
		continue;
	    break;
	}

	// Attempt #1.  Try to read file from remote source.
	if ((text == 0 || length == 0) && remote_gdb())
	{
	    text = read_remote(full_file_name, length, true);
	    if (text != 0)
		origin = ORIGIN_REMOTE;
	}

	// Attempt #2.  Read file from local source.
	if ((text == 0 || length == 0) && !remote_gdb())
	{
	    text = read_local(full_file_name, length, true);
	    if (text != 0)
		origin = ORIGIN_LOCAL;
	}

	// Attempt #3.  Read file from local source, even if we are remote.
	if ((text == 0 || length == 0) && remote_gdb())
	{
	    text = read_local(full_file_name, length, true);
	    if (text != 0)
		origin = ORIGIN_LOCAL;
	}
    }

    // Attempt #4.  Read file from GDB.
    if (text == 0 || length == 0)
    {
	string saved_current_file_name = current_file_name;
	current_file_name = full_file_name;
	string source_name = current_source_name();
	current_file_name = saved_current_file_name;

	text = read_from_gdb(source_name, length, silent);

	if (text != 0 && length != 0)
	{
	    // Use the source name as file name
	    full_file_name = source_name;
	    if (text != 0)
		origin = ORIGIN_GDB;
	}
    }

    if ((text == 0 || length == 0) && !silent)
    {
	// All failed - produce an appropriate error message.
	if (!remote_gdb())
	    text = read_local(full_file_name, length, silent);
	else
	    text = read_remote(full_file_name, length, silent);
    }

    if (text == 0 || length == 0)
    {
	origin = ORIGIN_NONE;
	return 0;
    }

    // At this point, we have a source text.
    file_name = full_file_name;

    // Determine text length and number of lines
    int lines = 0;
    for (t = 0; t < length; t++)
	if (text[t] == '\n')
	    lines++;

    int indented_text_length = length;
    if (length > 0 && text[length - 1] != '\n')
    {
	// Text does not end in '\n':
	// Make room for final '\n'
	indented_text_length += 1;

	// Make room for final line
	lines++;
    }

    // Make room for line numbers
    indented_text_length += indent_amount(source_text_w) * lines;

    String indented_text = XtMalloc(indented_text_length + 1);

    char *line_no_s = new char[indent_amount(source_text_w)];
    for (int i = 0; i < indent_amount(source_text_w); i++)
	line_no_s[i] = ' ';

    t = 0;
    char *pos_ptr = indented_text; // Writing position in indented_text
    while (t < length)
    {
	// Increase line number
	int i;
	for (i = indent_amount(source_text_w) - 2; i >= 0; i--)
	{
	    char& c = line_no_s[i];
	    if (c == ' ')
	    {
		c = '1';
		break;
	    }
	    else if (c < '9')
	    {
		c++;
		break;
	    }
	    else
		c = '0';
	}

	// Copy line number
	for (i = 0; i < indent_amount(source_text_w); i++)
	    *pos_ptr++ = display_line_numbers ? line_no_s[i] : ' ';

	// Copy line
	while (t < length && text[t] != '\n')
	    *pos_ptr++ = text[t++];

	// Copy '\n' or '\0'
	if (t == length)
	{
	    // Text doesn't end in '\n'
	    *pos_ptr++ = '\n';
	}
	else
	{
	    *pos_ptr++ = text[t++];
	}
    }

    delete[] line_no_s;

    assert (pos_ptr - indented_text == indented_text_length);
    *pos_ptr = '\0';

    XtFree(text);

    length = indented_text_length;
    return indented_text;
}


// Read file FILE_NAME into current_source; get it from the cache if possible
int SourceView::read_current(string& file_name, bool force_reload, bool silent)
{
    if (cache_source_files && !force_reload && file_cache.has(file_name))
    {
	current_source = file_cache[file_name];
	current_origin = origin_cache[file_name];
    }
    else
    {
	long length = 0;
	SourceOrigin orig;
	String indented_text = read_indented(file_name, length, orig, silent);
	if (indented_text)
	{
	    current_source = string(indented_text, length);
	    current_origin = orig;
	    XtFree(indented_text);
	}
	else
	{
	    current_source = "";
	    current_origin = ORIGIN_NONE;
	}

	if (current_source.length() > 0)
	{
	    file_cache[file_name]   = current_source;
	    origin_cache[file_name] = current_origin;
	}

	int null_count = current_source.freq('\0');
	if (null_count > 0 && !silent)
	    post_warning(file_name + ": binary file",
			 "source_binary_warning", source_text_w);
    }

    // Untabify current source, using the current tab width
    untabify(current_source, tab_width, indent_amount(source_text_w));

    // Setup global parameters

    // Number of lines
    line_count   = current_source.freq('\n');
    _pos_of_line = TextPositionArray(line_count + 2);
    _pos_of_line.operator += (XmTextPosition(0));
    _pos_of_line.operator += (XmTextPosition(0));

    for (int i = 0; i < int(current_source.length()); i++)
	if (current_source[i] == '\n')
	    _pos_of_line.operator += (XmTextPosition(i + 1));

    assert(_pos_of_line.size() == line_count + 2);

    if (current_source.length() == 0)
	return -1;
    else
	return 0;
}

// Return position of line LINE
XmTextPosition SourceView::pos_of_line(int line)
{
    if (line < 0 || line > line_count || line >= _pos_of_line.size())
	return 0;
    else
	return _pos_of_line[line];
}

// Clear the file cache
void SourceView::clear_file_cache()
{
    static StringStringAssoc string_empty;
    file_cache        = string_empty;
    source_name_cache = string_empty;

    static StringOriginAssoc origin_empty;
    origin_cache      = origin_empty;
}

void SourceView::reload()
{
    // Reload current file
    if (current_file_name == "")
	return;

    string file = file_of_cursor();
    string line = file.after(':');
    file        = file.before(':');

    StatusDelay delay("Reloading " + quote(file));

    read_file(file, atoi(line), true);
}

// Change tab width
void SourceView::set_tab_width (int width)
{
    if (width <= 0)
	return;

    if (tab_width != width)
    {
	tab_width = width;

	StatusDelay delay("Reformatting");
	reload();
    }
}

void SourceView::read_file (string file_name, 
			    int initial_line,
			    bool force_reload,
			    bool silent)
{
    if (file_name == "")
	return;

    /*
      Yves Arrouye <Yves.Arrouye@marin.fdn.fr> states:

      Paths to filename are not `normalized' before being passed to
      emacslient (or gnuclient). I mean that if one of your source
      files was compiled as

                /some/path//there/file.C

      (which happens fairly often due to empty paths components in
      Makefiles, for example), the file will not appear in Emacs
      because the Emacs visit-file function and friends do consider
      that when there is a // or a /~ in a path then everything
      before the first / is garbage. The file that will be looked
      for in the example is

                /there/file.C

      (the GUD Emacs mode exhibits the same bug ;-)). Thus // in
      file pathes must be changed to / (or Emacs changed so that
      only interactively-called versions of visit-file and friends
      do that, but I don't think this will be done...).
    */
    file_name.gsub("//", "/");

    // Read in current_source
    int error = read_current(file_name, force_reload, silent);
    if (error)
	return;

    add_to_history(file_name, initial_line);

    // The remainder may take some time...
    Delay delay;

    // Set source and initial line
    XmTextSetString(source_text_w, (String)current_source);

    XmTextPosition initial_pos = 0;
    if (initial_line > 0 && initial_line <= line_count)
	initial_pos = pos_of_line(initial_line) + indent_amount(source_text_w);

    SetInsertionPosition(source_text_w, initial_pos, true);

    current_file_name = file_name;

    // Refresh title
    update_title();

    // Refresh breakpoints
    static IntIntArrayAssoc empty_bps;
    bps_in_line = empty_bps;
    static StringArray empty_addresses;
    bp_addresses = empty_addresses;
    refresh_bp_disp();

    XtManageChild(source_text_w);

    ostrstream os;
    switch (current_origin)
    {
    case ORIGIN_LOCAL:
	os << "File " << quote(file_name);
	if (remote_gdb())
	    os << " (from local host)";
	break;

    case ORIGIN_REMOTE:
	os << "File " << quote(file_name) 
	   << " (from " << gdb_host << ")";
	break;

    case ORIGIN_GDB:
	os << "Source " << quote(file_name) 
	   << " (from " << gdb->title() << ")";
	break;

    case ORIGIN_NONE:
	os << quote(file_name);
	break;
    }
    os << ' ';

    if (line_count == 1)
	os << "1 line, ";
    else
	os << line_count << " lines, ";
    if (current_source.length() == 1)
	os << "1 character";
    else
	os << current_source.length() << " characters";

    string status(os);
    set_status(status);

    XmTextClearSelection(source_text_w, 
			 XtLastTimestampProcessed(XtDisplay(source_text_w)));
    XmTextSetHighlight(source_text_w,
		       0, XmTextGetLastPosition(source_text_w),
		       XmHIGHLIGHT_NORMAL);
    last_top = last_pos = last_start_highlight = last_end_highlight = 0;
    last_start_secondary_highlight = last_end_secondary_highlight = 0;
    update_glyphs(source_text_w);

    if ((source_view_shell != 0 || app_data.tty_mode)
	&& app_data.source_window)
    {
	// Make sure source is visible
	Widget shell = (source_view_shell != 0) ? 
	    source_view_shell : command_shell;
	initial_popup_shell(shell);

	if (!app_data.tool_bar)
	    initial_popup_shell(tool_shell);
    }
}

void SourceView::update_title()
{
    if (toplevel_w == 0)
	return;

    string title   = DDD_NAME ": " + current_file_name;
    String title_s = title;

    string icon   = 
	DDD_NAME ": " + string(basename(current_file_name.chars()));
    String icon_s = icon;

    XtVaSetValues(toplevel_w,
		  XmNtitle, title_s,
		  XmNiconName, icon_s,
		  NULL);
}



// ***************************************************************************
// Update breakpoint locations
void SourceView::refresh_bp_disp()
{
    refresh_source_bp_disp();
    refresh_code_bp_disp();
    update_glyphs();
}

void SourceView::refresh_source_bp_disp()
{
    // Overwrite old breakpoint displays - - - - - - - - - - - - -
    for (IntIntArrayAssocIter b_i_l_iter(bps_in_line);
	 b_i_l_iter.ok(); 
	 b_i_l_iter++)
    {
	int line_nr = b_i_l_iter.key();
	if (line_nr < 0 || line_nr > line_count)
	    continue;

	string s(current_source.at(int(pos_of_line(line_nr)), 
				   indent_amount(source_text_w) - 1));

	XmTextReplace (source_text_w,
		       pos_of_line(line_nr),
		       pos_of_line(line_nr) + indent_amount(source_text_w) - 1,
		       (String)s);
    }

    static IntIntArrayAssoc empty_bps;
    bps_in_line = empty_bps;

    // Find all breakpoints referring to this file
    MapRef ref;
    for (BreakPoint* bp = bp_map.first(ref); bp != 0; bp = bp_map.next(ref))
    {
	if (bp_matches(bp))
	    bps_in_line[bp->line_nr()] += bp->number();
    }

    if (!display_glyphs)
    {
	// Show breakpoints in text
	for (IntIntArrayAssocIter b_i_l_iter(bps_in_line);
	     b_i_l_iter.ok();
	     b_i_l_iter++)
	{
	    int line_nr = b_i_l_iter.key();
	    if (line_nr < 0 || line_nr > line_count)
		continue;

	    XmTextPosition pos = pos_of_line(line_nr);

	    string insert_string = "";

	    // Display all breakpoints in a line
	    VarIntArray& bps = bps_in_line[line_nr];

	    int i;
	    for (i = 0; i < bps.size(); i++)
	    {
		BreakPoint* bp = bp_map.get(bps[i]);
		assert (bp);
		insert_string += bp->enabled() ? '#' : '_';
		insert_string += bp->number_str();
		insert_string += bp->enabled() ? '#' : '_';
	    }
	    if (int(insert_string.length()) 
		>= indent_amount(source_text_w) - 1)
	    {
		insert_string =
		    insert_string.before(indent_amount(source_text_w) - 1);
	    }
	    else
	    {
		for (i = insert_string.length(); 
		     i < indent_amount(source_text_w) - 1; i++)
		{
		    insert_string += current_source[pos + i];
		}
	    }

	    assert(insert_string.length() 
		   == unsigned(indent_amount(source_text_w) - 1));

	    XmTextReplace (source_text_w, pos, 
			   pos + indent_amount(source_text_w) - 1,
			   (String)insert_string);
	}
    }
}

void SourceView::refresh_code_bp_disp()
{
    // Clear all addresses
    int i;
    for (i = 0; i < bp_addresses.size(); i++)
    {
	const string& address = bp_addresses[i];
	XmTextPosition pos = find_pc(address);
	if (pos == XmTextPosition(-1))
	    continue;

	// Process all breakpoints at ADDRESS
	string insert_string = replicate(' ', indent_amount(code_text_w));

	XmTextReplace (code_text_w, pos, pos + indent_amount(code_text_w),
		       (String)insert_string);
    }

    static StringArray empty;
    bp_addresses = empty;

    if (!display_glyphs)
    {
	// Collect all addresses
	MapRef ref;
	for (BreakPoint *bp = bp_map.first(ref);
	     bp != 0;
	     bp = bp_map.next(ref))
	{
	    if (bp->type() != BREAKPOINT)
		continue;

	    bp_addresses += bp->address();
	}

	// Process all bp_addresses
	for (i = 0; i < bp_addresses.size(); i++)
	{
	    const string& address = bp_addresses[i];
	    XmTextPosition pos = find_pc(address);
	    if (pos == XmTextPosition(-1))
		continue;

	    // Process all breakpoints at ADDRESS
	    string insert_string = "";
	    for (BreakPoint *bp = bp_map.first(ref);
		 bp != 0;
		 bp = bp_map.next(ref))
	    {
		if (bp->address() == address)
		{
		    insert_string += bp->enabled() ? '#' : '_';
		    insert_string += bp->number_str();
		    insert_string += bp->enabled() ? '#' : '_';
		}
	    }
	    insert_string += replicate(' ', indent_amount(code_text_w));
	    insert_string = insert_string.before(indent_amount(code_text_w));

	    XmTextReplace (code_text_w, pos, pos + indent_amount(code_text_w),
			   (String)insert_string);
	}
    }

    update_glyphs();
}



// ***************************************************************************
// Find the line number at POS
// LINE_NR becomes the line number at POS
// IN_TEXT becomes true iff POS is in the source area
// BP_NR is the number of the breakpoint at POS (none: 0)
// Return false iff failure
//
bool SourceView::get_line_of_pos (Widget   w,
				  XmTextPosition pos,
				  int&     line_nr,
				  string&  address,
				  bool&    in_text,
				  int&     bp_nr)
{
    bool found = false;

    line_nr = 0;
    address = "";
    in_text = true;
    bp_nr   = 0;

    Widget text_w;
    if (is_source_widget(w))
	text_w = source_text_w;
    else if (is_code_widget(w))
	text_w = code_text_w;
    else
	return false;

    if (w != text_w)
    {
	// Glyph selected

	MapRef ref;
	for (BreakPoint *bp = bp_map.first(ref);
	     bp != 0;
	     bp = bp_map.next(ref))
	{
	    if (w == bp->source_glyph() || w == bp->code_glyph())
	    {
		// Breakpoint glyph found
		line_nr = bp->line_nr();
		address = bp->address();
		in_text = false;
		bp_nr   = bp->number();
		return true;
	    }
	}
    }

    if (pos >= int(current_text(text_w).length()))
    {
	// Position is on the right of text
	in_text = false;
	line_nr = line_count;
	return true;
    }

    if (text_w == source_text_w)
    {
	// Search in source code
	XmTextPosition line_pos = 0;
	XmTextPosition next_line_pos = 0;

	while (!found && line_count >= line_nr)
	{
	    next_line_pos = (line_count >= line_nr + 1) ?
		pos_of_line(line_nr + 1) :
		XmTextGetLastPosition (text_w) + 1;

	    bool left_of_first_nonblank = false;
	    if (pos < next_line_pos)
	    {
		// Check if we're left of first non-blank source character
		int first_nonblank = line_pos + indent_amount(text_w);
		const string& text = current_text(text_w);
		while (first_nonblank < next_line_pos
		       && first_nonblank < int(text.length())
		       && isspace(text[first_nonblank]))
		    first_nonblank++;
		left_of_first_nonblank = (pos < first_nonblank);
	    }

	    if (left_of_first_nonblank 
		|| pos < (line_pos + indent_amount(text_w) - 1))
	    {
		// Position in breakpoint area
		found = true;
		in_text = false;
		line_nr = max(line_nr, 1);

		// Check for breakpoints...
		VarIntArray& bps = bps_in_line[line_nr];
		if (bps.size() == 1)
		{
		    // Return single breakpoint in this line
		    bp_nr = bps[0];
		}
		else if (bps.size() > 1)
		{
		    // Find which breakpoint was selected
		    XmTextPosition bp_disp_pos = line_pos;
		    int i;
		    for (i = 0; i < bps.size(); i++)
		    {
			BreakPoint* bp = bp_map.get(bps[i]);
			assert (bp);

			bp_disp_pos += 2; // respect '#' and '_';
			bp_disp_pos += bp->number_str().length();
			if (pos < bp_disp_pos)
			{
			    bp_nr = bps[i];
			    break; // exit for loop
			}
		    }
		}
	    }
	    else if (pos < next_line_pos)
	    {
		// Position is in text
		found   = true;
		in_text = true;
	    }
	    else
	    {
		// Position is in one of the following lines
		line_pos = next_line_pos;
		line_nr++;
	    }
	}
    }
    else if (text_w == code_text_w)
    {
	// Search in machine code
	XmTextPosition line_pos = pos;
	while (line_pos >= 0 && current_code[line_pos] != '\n')
	    line_pos--;
	line_pos++;

	if (pos - line_pos < indent_amount(text_w))
	{
	    // Breakpoint area
	    in_text = false;

	    // Check if we have a breakpoint around here
	    int index = address_index(current_code, pos);
	    if (index >= 0)
	    {
		address = current_code.from(index);
		address = address.through(rxaddress);

		VarIntArray bps;

		MapRef ref;
		for (BreakPoint *bp = bp_map.first(ref);
		     bp != 0;
		     bp = bp_map.next(ref))
		{
		    if (compare_address(address, bp->address()) == 0)
			bps += bp->number();
		}
		if (bps.size() == 1)
		{
		    // Return single breakpoint in this line
		    bp_nr = bps[0];
		}
		else if (bps.size() > 1)
		{
		    // Find which breakpoint was selected
		    int i;
		    XmTextPosition bp_disp_pos = line_pos;
		    for (i = 0; i < bps.size(); i++)
		    {
			BreakPoint* bp = bp_map.get(bps[i]);
			assert (bp);
			bp_disp_pos += 2; // respect '#' and '_';
			bp_disp_pos += bp->number_str().length();
			if (pos < bp_disp_pos)
			{
			    bp_nr = bps[i];
			    break; // exit for loop
			}
		    }
		}
	    }
	}

	found = true;
    }

    return found;
}

// ***************************************************************************
// Find word around POS.  STARTPOS is the first character, ENDPOS
// is the last character in the word.
void SourceView::find_word_bounds (Widget text_w,
				   const XmTextPosition pos,
				   XmTextPosition& startpos,
				   XmTextPosition& endpos)
{
    startpos = endpos = pos;

    string& text = current_text(text_w);

    XmTextPosition line_pos = pos;
    if (line_pos < XmTextPosition(text.length()))
	while (line_pos > 0 && text[line_pos - 1] != '\n')
	    line_pos--;

    int offset = pos - line_pos;
    if (offset < indent_amount(text_w))
    {
	// Do not select words in breakpoint area
	return;
    }

    while (startpos > 0 
	   && startpos < XmTextPosition(text.length())
	   && isid(text[startpos - 1]))
	startpos--;

    while (endpos < XmTextPosition(text.length())
	   && isid(text[endpos]))
	endpos++;
}

// Get the word at event position
string SourceView::get_word_at_event(Widget text_w,
				     XEvent *event,
				     XmTextPosition& startpos,
				     XmTextPosition& endpos)
{
    BoxPoint event_pos = point(event);
    XmTextPosition pos = XmTextXYToPos(text_w, event_pos[X], event_pos[Y]);

    return get_word_at_pos(text_w, pos, startpos, endpos);
}


// Get the word at POS
string SourceView::get_word_at_pos(Widget text_w,
				   XmTextPosition pos,
				   XmTextPosition& startpos,
				   XmTextPosition& endpos)
{
    string& text = current_text(text_w);
    if (text == "")
	return "";

    if (!XmTextGetSelectionPosition(text_w, &startpos, &endpos)
	|| pos < startpos
	|| pos > endpos)
    {
	find_word_bounds(text_w, pos, startpos, endpos);
    }

    string word = "";
    if (startpos < XmTextPosition(text.length())
	&& startpos < endpos)
	word = text.at(int(startpos), int(endpos - startpos));

    read_leading_blanks(word);
    strip_final_blanks(word);

    return word;
}



//----------------------------------------------------------------------------
// Create GDB requests and evaluate replies
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------------

SourceView::SourceView(XtAppContext app_context, Widget parent)
{
    // Find application shell
    toplevel_w = parent;
    while (toplevel_w != 0 && !XtIsWMShell(toplevel_w))
	toplevel_w = XtParent(toplevel_w);

    Arg args[10];
    Cardinal arg = 0;

    // Create source code window
    create_text(parent, "source", source_form_w, source_text_w);
    XtManageChild(source_form_w);

    // Create machine code window
    create_text(parent, "code", code_form_w, code_text_w);
    if (disassemble)
	XtManageChild(code_form_w);

    // Create breakpoint editor
    arg = 0;
    XtSetArg(args[arg], XmNvisibleItemCount, 0); arg++;
    edit_breakpoints_dialog_w =
	verify(XmCreatePromptDialog(parent, "edit_breakpoints_dialog",
				    args, arg));
    Delay::register_shell(edit_breakpoints_dialog_w);

    if (lesstif_version <= 79)
	XtUnmanageChild(XmSelectionBoxGetChild(edit_breakpoints_dialog_w,
					       XmDIALOG_APPLY_BUTTON));

    XtUnmanageChild(XmSelectionBoxGetChild(edit_breakpoints_dialog_w,
					   XmDIALOG_TEXT));
    XtUnmanageChild(XmSelectionBoxGetChild(edit_breakpoints_dialog_w,
					   XmDIALOG_CANCEL_BUTTON));
    XtUnmanageChild(XmSelectionBoxGetChild(edit_breakpoints_dialog_w,
					   XmDIALOG_SELECTION_LABEL));

    Widget form1 = 
	verify(XmCreateRowColumn(edit_breakpoints_dialog_w, "form1", NULL, 0));

    Widget label =
	verify(XmCreateLabel(form1, "breakpoints", NULL, 0));

    Widget form2 = 
	verify(XmCreateRowColumn(form1, "form2", NULL, 0));

    arg = 0;
    breakpoint_list_w = 
	verify(XmCreateScrolledList(form2, "breakpoints", args, arg));
    Widget buttons = 
	verify(MMcreateWorkArea(form2, "buttons", bp_area));
    MMaddCallbacks (bp_area);

    XtManageChild (buttons);
    XtManageChild (breakpoint_list_w);
    XtManageChild (form2);
    XtManageChild (label);
    XtManageChild (form1);

    if (breakpoint_list_w)
    {
	XtAddCallback(breakpoint_list_w,
		      XmNsingleSelectionCallback,
		      UpdateBreakpointButtonsCB,
		      0);
	XtAddCallback(breakpoint_list_w,
		      XmNmultipleSelectionCallback,
		      UpdateBreakpointButtonsCB,
		      0);
#if 0
	XtAddCallback(breakpoint_list_w,
		      XmNmultipleSelectionCallback,
		      LookupBreakpointCB,
		      0);
#endif
	XtAddCallback(breakpoint_list_w,
		      XmNextendedSelectionCallback,
		      UpdateBreakpointButtonsCB,
		      0);
	XtAddCallback(breakpoint_list_w,
		      XmNbrowseSelectionCallback,
		      UpdateBreakpointButtonsCB,
		      0);
    }

    if (edit_breakpoints_dialog_w)
    {
	XtAddCallback(edit_breakpoints_dialog_w,
		      XmNokCallback,
		      UnmanageThisCB,
		      edit_breakpoints_dialog_w);
	XtAddCallback(edit_breakpoints_dialog_w,
		      XmNhelpCallback,
		      ImmediateHelpCB,
		      0);
    }

    // Create stack view
    arg = 0;
    XtSetArg(args[arg], XmNautoUnmanage, False); arg++;
    stack_dialog_w =
	verify(XmCreateSelectionDialog(parent, 
				       "stack_dialog", args, arg));
    Delay::register_shell(stack_dialog_w);

    XtUnmanageChild(XmSelectionBoxGetChild(stack_dialog_w, 
					   XmDIALOG_TEXT));
    XtUnmanageChild(XmSelectionBoxGetChild(stack_dialog_w, 
					   XmDIALOG_SELECTION_LABEL));

    up_w   = XmSelectionBoxGetChild(stack_dialog_w, XmDIALOG_OK_BUTTON);
    down_w = XmSelectionBoxGetChild(stack_dialog_w, XmDIALOG_APPLY_BUTTON);

    XtSetSensitive(up_w,   False);
    XtSetSensitive(down_w, False);

    arg = 0;
    frame_list_w = XmSelectionBoxGetChild(stack_dialog_w, XmDIALOG_LIST);
    XtVaSetValues(frame_list_w,
		  XmNselectionPolicy, XmSINGLE_SELECT,
		  NULL);

    XtAddCallback(frame_list_w,
		  XmNsingleSelectionCallback, SelectFrameCB, 0);
    XtAddCallback(frame_list_w,
		  XmNmultipleSelectionCallback, SelectFrameCB, 0);
    XtAddCallback(frame_list_w,
		  XmNextendedSelectionCallback, SelectFrameCB, 0);
    XtAddCallback(frame_list_w,
		  XmNbrowseSelectionCallback, SelectFrameCB, 0);

    XtAddCallback(stack_dialog_w,
		  XmNokCallback, gdbCommandCB, "up");
    XtAddCallback(stack_dialog_w,
		  XmNapplyCallback, gdbCommandCB, "down");
    XtAddCallback(stack_dialog_w,
		  XmNcancelCallback, UnmanageThisCB, stack_dialog_w);
    XtAddCallback(stack_dialog_w,
		  XmNcancelCallback, StackDialogPoppedDownCB, 0);
    XtAddCallback(stack_dialog_w,
		  XmNhelpCallback, ImmediateHelpCB, 0);

    Widget cancel_w = XmSelectionBoxGetChild(stack_dialog_w, 
					     XmDIALOG_CANCEL_BUTTON);

    XtVaSetValues(stack_dialog_w, XmNdefaultButton, cancel_w, 0);

    // Create register view
    arg = 0;
    XtSetArg(args[arg], XmNautoUnmanage, False); arg++;
    register_dialog_w = 
	verify(XmCreateSelectionDialog(parent, 
				       "register_dialog", args, arg));
    Delay::register_shell(register_dialog_w);

    XtUnmanageChild(XmSelectionBoxGetChild(register_dialog_w, 
					   XmDIALOG_TEXT));
    XtUnmanageChild(XmSelectionBoxGetChild(register_dialog_w, 
					   XmDIALOG_SELECTION_LABEL));
    XtUnmanageChild(XmSelectionBoxGetChild(register_dialog_w, 
					   XmDIALOG_APPLY_BUTTON));
    XtUnmanageChild(XmSelectionBoxGetChild(register_dialog_w, 
					   XmDIALOG_CANCEL_BUTTON));

    arg = 0;
    Widget box = 
	XmCreateRadioBox(register_dialog_w, "box", args, arg);
    XtManageChild(box);

    arg = 0;
    XtSetArg(args[arg], XmNset, !all_registers); arg++;
    int_registers_w = 
	XmCreateToggleButton(box, "int_registers", args, arg);
    XtManageChild(int_registers_w);

    arg = 0;
    XtSetArg(args[arg], XmNset, all_registers); arg++;
    all_registers_w = 
	XmCreateToggleButton(box, "all_registers", args, arg);
    XtManageChild(all_registers_w);

    XtAddCallback(all_registers_w, XmNvalueChangedCallback, 
		  sourceToggleAllRegistersCB, XtPointer(0));

    arg = 0;
    register_list_w = XmSelectionBoxGetChild(register_dialog_w, XmDIALOG_LIST);
    XtVaSetValues(register_list_w,
		  XmNselectionPolicy, XmSINGLE_SELECT,
		  NULL);

    XtAddCallback(register_list_w,
		  XmNsingleSelectionCallback, SelectRegisterCB, 0);
    XtAddCallback(register_list_w,
		  XmNmultipleSelectionCallback, SelectRegisterCB, 0);
    XtAddCallback(register_list_w,
		  XmNextendedSelectionCallback, SelectRegisterCB, 0);
    XtAddCallback(register_list_w,
		  XmNbrowseSelectionCallback, SelectRegisterCB, 0);

    XtAddCallback(register_dialog_w,
		  XmNokCallback, UnmanageThisCB, register_dialog_w);
    XtAddCallback(register_dialog_w,
		  XmNokCallback, RegisterDialogPoppedDownCB, 0);
    XtAddCallback(register_dialog_w,
		  XmNhelpCallback, ImmediateHelpCB, 0);


    // Create thread view
    arg = 0;
    XtSetArg(args[arg], XmNautoUnmanage, False); arg++;
    thread_dialog_w = 
	verify(XmCreateSelectionDialog(parent, 
				       "thread_dialog", args, arg));
    Delay::register_shell(thread_dialog_w);

    XtUnmanageChild(XmSelectionBoxGetChild(thread_dialog_w, 
					   XmDIALOG_TEXT));
    XtUnmanageChild(XmSelectionBoxGetChild(thread_dialog_w, 
					   XmDIALOG_SELECTION_LABEL));
    XtUnmanageChild(XmSelectionBoxGetChild(thread_dialog_w, 
					   XmDIALOG_APPLY_BUTTON));
    XtUnmanageChild(XmSelectionBoxGetChild(thread_dialog_w, 
					   XmDIALOG_CANCEL_BUTTON));
    arg = 0;
    thread_list_w = XmSelectionBoxGetChild(thread_dialog_w, XmDIALOG_LIST);
    XtVaSetValues(thread_list_w,
		  XmNselectionPolicy, XmSINGLE_SELECT,
		  NULL);

    XtAddCallback(thread_list_w,
		  XmNsingleSelectionCallback, SelectThreadCB, 0);
    XtAddCallback(thread_list_w,
		  XmNmultipleSelectionCallback, SelectThreadCB, 0);
    XtAddCallback(thread_list_w,
		  XmNextendedSelectionCallback, SelectThreadCB, 0);
    XtAddCallback(thread_list_w,
		  XmNbrowseSelectionCallback, SelectThreadCB, 0);

    XtAddCallback(thread_dialog_w,
		  XmNokCallback, UnmanageThisCB, thread_dialog_w);
    XtAddCallback(thread_dialog_w,
		  XmNokCallback, ThreadDialogPoppedDownCB, 0);
    XtAddCallback(thread_dialog_w,
		  XmNhelpCallback, ImmediateHelpCB, 0);


    // Setup actions
    XtAppAddActions (app_context, actions, XtNumber (actions));

    // Install images
    XmInstallImage(&arrow_image,        "plain_arrow");
    XmInstallImage(&grey_arrow_image,   "grey_arrow");
    XmInstallImage(&signal_arrow_image, "signal_arrow");
    XmInstallImage(&temp_arrow_image,   "temp_arrow");
    XmInstallImage(&stop_image,         "plain_stop");
    XmInstallImage(&grey_stop_image,    "grey_stop");
    XmInstallImage(&temp_stop_image,    "temp_stop");

    // Create glyphs in the background
    XtAppAddWorkProc (app_context, CreateGlyphsWorkProc, XtPointer(0));
}

// LessTif 0.79 doesn't like setting `editable' to false, hence prohibit
// changes via this callback
void SourceView::InhibitModificationCB(Widget, XtPointer, XtPointer call_data)
{
    XmTextVerifyCallbackStruct *cbs = (XmTextVerifyCallbackStruct *)call_data;
    if (cbs->event != 0)
	cbs->doit = False;
}

// Create source or code window
void SourceView::create_text(Widget parent,
			     const string& base, Widget& form, Widget& text)
{
    Arg args[15];
    int arg = 0;

    // Create text window
    arg = 0;
    XtSetArg(args[arg], XmNmarginHeight, 0);    arg++;
    XtSetArg(args[arg], XmNmarginWidth,  0);    arg++;
    string form_name = base + "_form_w";
    form = verify(XmCreateForm(parent, form_name, args, arg));

    arg = 0;
    XtSetArg(args[arg], XmNselectionArrayCount, 1);               arg++;
    XtSetArg(args[arg], XmNtopAttachment,     XmATTACH_FORM);     arg++;
    XtSetArg(args[arg], XmNbottomAttachment,  XmATTACH_FORM);     arg++;
    XtSetArg(args[arg], XmNleftAttachment,    XmATTACH_FORM);     arg++;
    XtSetArg(args[arg], XmNrightAttachment,   XmATTACH_FORM);     arg++;
    XtSetArg(args[arg], XmNallowResize,       True);              arg++;
    XtSetArg(args[arg], XmNeditMode,          XmMULTI_LINE_EDIT); arg++;
    XtSetArg(args[arg], XmNautoShowCursorPosition, True);         arg++;
    XtSetArg(args[arg], XmNcursorPositionVisible, True);          arg++;

    if (lesstif_version <= 79)
    {
	// LessTif 0.79 has trouble with non-editable text windows
	XtSetArg(args[arg], XmNeditable, True); arg++;
    }
    else
    {
	XtSetArg(args[arg], XmNeditable, False); arg++;
    }

    string text_name = base + "_text_w";
    text = verify(XmCreateScrolledText(form, text_name, args, arg));
    XtManageChild(text);

    XtAddCallback(text, XmNgainPrimaryCallback, 
		  set_source_argCB, XtPointer(false));
    XtAddCallback(text, XmNmotionVerifyCallback,
		  set_source_argCB, XtPointer(true));
    XtAddCallback(text, XmNmotionVerifyCallback, 
		  CheckScrollCB, XtPointer(0));
    XtAddCallback(text, XmNmodifyVerifyCallback,
		  InhibitModificationCB, XtPointer(0));
		  
    InstallTextTips(text);

    // Fetch scrollbar ID and add callbacks
    Widget scrollbar = 0;
    XtVaGetValues(XtParent(text), XmNverticalScrollBar, &scrollbar, NULL);
    if (scrollbar != 0)
    {
	XtAddCallback(scrollbar, XmNincrementCallback,     CheckScrollCB, 0);
	XtAddCallback(scrollbar, XmNdecrementCallback,     CheckScrollCB, 0);
	XtAddCallback(scrollbar, XmNpageIncrementCallback, CheckScrollCB, 0);
	XtAddCallback(scrollbar, XmNpageDecrementCallback, CheckScrollCB, 0);
	XtAddCallback(scrollbar, XmNtoTopCallback,         CheckScrollCB, 0);
	XtAddCallback(scrollbar, XmNtoBottomCallback,      CheckScrollCB, 0);
	XtAddCallback(scrollbar, XmNdragCallback,          CheckScrollCB, 0);
	XtAddCallback(scrollbar, XmNvalueChangedCallback,  CheckScrollCB, 0);
    }
}



// ***************************************************************************
//

// Set current execution position, based on the GDB position info
// POSITION; no arg means clear current position.
// STOPPED indicates that the program just stopped.
// SIGNALED indicates that the program received a signal.
void SourceView::show_execution_position (string position, bool stopped,
					  bool signaled, bool silent)
{
    if (stopped)
    {
	at_lowest_frame = true;
	signal_received = signaled;
    }

    if (position == "")
    {
	if (!display_glyphs)
	{
	    // Remove old marker
	    XmTextReplace (source_text_w,
			   last_pos + indent_amount(source_text_w) - 1,
			   last_pos + indent_amount(source_text_w),
			   " ");

	    if (last_start_highlight)
		XmTextSetHighlight (source_text_w,
				    last_start_highlight, last_end_highlight,
				    XmHIGHLIGHT_NORMAL);

	}
	last_pos = last_start_highlight = last_end_highlight = 0;
	last_execution_file = "";
	last_execution_line = 0;
	update_glyphs();
	return;
    }

    string file_name = current_file_name;

    if (position.contains(':'))
    {
	file_name = position.before(":");
	position  = position.after(":");
    }

    int line = get_positive_nr(position);
    if (line < 0)
	return;

    if (!file_matches(file_name, current_file_name))
	read_file(file_name, line, silent);

    if (file_matches(file_name, current_file_name))
    {
	if (!display_glyphs && indent_amount(source_text_w) > 0)
	{
	    // Remove old marker
	    XmTextReplace (source_text_w,
			   last_pos + indent_amount(source_text_w) - 1,
			   last_pos + indent_amount(source_text_w),
			   " ");
	}

	// Show current position
	_show_execution_position(file_name, line, silent);
    }
}

// Unset current execution position (program terminated)
void SourceView::clear_execution_position()
{
    show_execution_position();
    last_execution_pc = "";
    last_shown_pc     = "";
    update_glyphs();
}


void SourceView::_show_execution_position(string file, int line, bool silent)
{
    last_execution_file = file;
    last_execution_line = line;

    if (!file_matches(file, current_file_name))
	read_file(file, line, silent);
    else if (line >= 1 && line <= line_count)
	add_to_history(file, line);

    if (line < 1 || line > line_count)
	return;

    XmTextPosition pos = pos_of_line(line);
    SetInsertionPosition(source_text_w, 
			 pos + indent_amount(source_text_w), false);

    // Mark current line
    if (!display_glyphs && indent_amount(source_text_w) > 0)
    {
	// Set new marker
	XmTextReplace (source_text_w,
		       pos + indent_amount(source_text_w) - 1,
		       pos + indent_amount(source_text_w),
		       ">");
    }

    XmTextPosition pos_line_end = 0;
    if (current_source != "")
	pos_line_end = current_source.index('\n', pos) + 1;

    if (!display_glyphs && 
	(pos != last_start_highlight || pos_line_end != last_end_highlight))
    {
	if (last_start_highlight)
	{
	    XmTextSetHighlight (source_text_w,
				last_start_highlight, last_end_highlight,
				XmHIGHLIGHT_NORMAL);
	}

	XmTextSetHighlight (source_text_w,
			    pos, pos_line_end,
			    XmHIGHLIGHT_SELECTED);
    }

    last_pos             = pos;
    last_start_highlight = pos;
    last_end_highlight   = pos_line_end;

    update_glyphs();
}


// ***************************************************************************
//
void SourceView::show_position (string position, bool silent)
{
    string file_name = current_file_name;

    if (position.contains(':'))
    {
	file_name = position.before(':');
	position  = position.after(':');
    }
    int line = get_positive_nr(position);

    if (!file_matches(file_name, current_file_name))
	read_file(file_name, line, false, silent);
    add_to_history(file_name, line);

    // Fenster scrollt an Position
    if (line > 0 && line <= line_count)
    {
	XmTextPosition pos = pos_of_line(line);
	SetInsertionPosition(source_text_w, 
			     pos + indent_amount(source_text_w), true);

	last_pos = pos;
    }
}



// ***************************************************************************
// Process reply on 'info breakpoints'.
// Update breakpoints in BP_BAP, adding new ones or deleting existing ones.
// Update breakpoint display by calling REFRESH_BP_DISP.
//
void SourceView::process_info_bp (string& info_output,
				  const string& break_arg)
{
    // DEC dbx issues empty lines, which causes trouble
    info_output.gsub("\n\n", "\n");

    // SGI dbx issues `Process PID' before numbers
#if RUNTIME_REGEX
    static regex rxprocess1("Process[ \t]+[0-9]+:[ \t]*");
#endif
    info_output.gsub(rxprocess1, "");

    last_info_output = info_output;

    switch (gdb->type())
    {
    case GDB:
	// If this is no breakpoint info, process it as GDB message
	if (!info_output.contains("Num", 0) && 
	    !info_output.contains("No breakpoints", 0))
	    check_remainder(info_output);
	break;
    default:
	break;
    }
				    
    VarIntArray bps_not_read;
    MapRef ref;
    int i;
    for (i = bp_map.first_key(ref); i != 0; i = bp_map.next_key(ref))
	bps_not_read += i;

    bool changed = false;
    bool added   = false;

    while (info_output != "") 
    {
	int bp_nr = -1;
	switch(gdb->type())
	{
	case GDB:
	    if (!has_nr(info_output))
	    {
		// Skip this line
		info_output = info_output.after('\n');
		continue;
	    }
	    bp_nr = get_positive_nr (info_output);
	    break;

	case DBX:
	    {
		// SGI IRIX DBX issues `Process PID: ' 
		// before status lines.
#if RUNTIME_REGEX
		static regex rxprocess2("Process[ \t]+[0-9]+:");
#endif
		if (info_output.contains(rxprocess2, 0))
		    info_output = info_output.after(':');
		read_leading_blanks(info_output);
		    
		if (!info_output.contains('(', 0)
		    && !info_output.contains('[', 0))
		{
		    // No breakpoint info - skip this line
		    info_output = info_output.after('\n');
		    continue;
		}
		string bp_nr_s = info_output.after(0);
		bp_nr = get_positive_nr (bp_nr_s);
	    }
	    break;


	case XDB:
	    bp_nr = get_positive_nr(info_output);
	    break;
	}

	if (bp_nr <= 0)
	{
	    info_output = info_output.after('\n');
	    continue;
	}

	if (bp_map.contains (bp_nr))
	{
	    bps_not_read -= bp_nr;
	    if (bp_map.get(bp_nr)->update(info_output)
		&& (bp_map.get(bp_nr)->position_changed()
		    || bp_map.get(bp_nr)->enabled_changed()))
	    {
		changed = true;
	    }
	}
	else
	{
	    // New breakpoint
	    changed = true;
	    BreakPoint* new_bp = new BreakPoint(info_output, break_arg);
	    bp_map.insert (bp_nr, new_bp);

	    if (!added)
	    {
		added = true;
		// Select this breakpoint only
		MapRef ref;
		for (BreakPoint* bp = bp_map.first(ref);
		     bp != 0;
		     bp = bp_map.next(ref))
		{
		    bp->selected() = false;
		}
	    }

	    new_bp->selected() = true;
	}

	max_breakpoint_number_seen = max(max_breakpoint_number_seen, bp_nr);
    }

    // Delete all breakpoints not found now
    for (i = 0; i < bps_not_read.size(); i++)
    {
	delete bp_map.get(bps_not_read[i]);
	bp_map.del(bps_not_read[i]);
	changed = true;
    }

    if (changed)
	refresh_bp_disp();

    // Set up breakpoint editor contents
    process_breakpoints(last_info_output);
}

int SourceView::next_breakpoint_number()
{
    return max_breakpoint_number_seen + 1;
}


// ***************************************************************************
//
void SourceView::process_info_line_main(string& info_output)
{
    clear_file_cache();
    clear_code_cache();
    clear_dbx_lookup_cache();
    current_file_name = "";

    switch (gdb->type())
    {
    case GDB:
    case XDB:
	{
	    PosBuffer pos_buffer;
	    pos_buffer.filter(info_output);
	    pos_buffer.answer_ended();
	    if (pos_buffer.pos_found())
		show_position(pos_buffer.get_position());
	    if (pos_buffer.pc_found())
		show_pc(pos_buffer.get_pc());
	}
	break;

    case DBX:
	{
	    show_position(info_output);
	    info_output = "";
	}
	break;
    }

    // Strip 'Line <n> of <file> starts at <address>...' info
    // Strip 'No symbol table is loaded.' info
    String strips[] = {"Line ", "No symbol table is loaded."};

    for (int i = 0; i < int(XtNumber(strips)); i++)
    {
	int line = info_output.index(strips[i]);
	if (line >= 0)
	{
	    int end_line = info_output.index('\n', line);
	    if (end_line >= 0)
		info_output(line, end_line - line) = "";
	}
    }

    check_remainder(info_output);
}

void SourceView::check_remainder(string& info_output)
{
    // Any remaining input is an informative GDB message.
    // Strip newlines around the message and post it.

    while (info_output.length() > 0 && 
	   info_output[0] == '\n')
	info_output = info_output.after(0);
    while (info_output.length() > 0 && 
	   info_output[info_output.length() - 1] == '\n')
	info_output = info_output.before(int(info_output.length() - 1));

    if (info_output != "")
	post_gdb_message(info_output, source_text_w);
}

// ***************************************************************************
//
void SourceView::lookup(string s, bool silent)
{
    if (s != "" && isspace(s[0]))
	s = s.after(rxwhite);

    if (s == "")
    {
	// Empty argument given
	if (last_execution_pc != "")
	{
	    // Show last PC
	    show_pc(last_execution_pc, XmHIGHLIGHT_SELECTED);
	}
	else
	{
	    // Show cursor position
	    SetInsertionPosition(code_text_w,
				 XmTextGetInsertionPosition(code_text_w));
	}

	if (last_execution_file != "")
	{
	    // Show last execution position
	    _show_execution_position(last_execution_file, 
				     last_execution_line,
				     silent);
	}
	else
	{
	    // Show cursor position
	    SetInsertionPosition(source_text_w,
				 XmTextGetInsertionPosition(source_text_w));
	}
    }
    else if (s[0] != '0' && isdigit(s[0]))
    {
	// Line number given
	int line = atoi(s);
	if (line > 0 && line <= line_count)
	{
	    if (gdb->type() == GDB)
	    {
		Command c("info line " + current_source_name()
			  + ":" + itostring(line));
		c.verbose = !silent;
		gdb_command(c);
	    }
	    else
		show_position(full_path(current_file_name) 
			      + ":" + itostring(line));
	}
	else
	{
	    if (!silent)
		post_error("No line " 
			   + itostring(line) + " in current source.",
			   "no_such_line_error", source_text_w);
	}
    }
    else if (s[0] == '#')
    {
	// Breakpoint given
	string nr_str = s.after('#');
	int nr = get_positive_nr(nr_str);
	if (nr >= 0)
	{
	    MapRef ref;
	    BreakPoint *bp;
	    for (bp = bp_map.first(ref); bp != 0; bp = bp_map.next(ref))
	    {
		if (nr == bp->number())
		{
		    if (bp->file_name() != "")
		    {
			show_position(bp->file_name() + ":" + 
				      itostring(bp->line_nr()));
			show_pc(bp->address());
		    }
		    else
		    {
			show_position(itostring(bp->line_nr()));
			show_pc(bp->address());
		    }
		    break;
		}
	    }

	    if (bp == 0 && !silent)
		post_error("No breakpoint number " + itostring(nr) + ".", 
			   "no_such_breakpoint_error", source_text_w);
	}
    }
    else if (s.contains(":") && !s.contains("::"))
    {
	// File:line given
	if (gdb->type() == GDB)
	{
	    Command c("info line " + s);
	    c.verbose = !silent;
	    gdb_command(c);
	}
	else
	    show_position(s);
    }
    else
    {
	// Function or *address given
	switch (gdb->type())
	{
	case GDB:
	{
	    if (s[0] == '0')	// Address given
		s = "*" + s;
	    if (s.length() > 0 && s[0] != '\'' && s[0] != '*')
		s = string('\'') + s + '\'';
	    Command c("info line " + s);
	    c.verbose = !silent;
	    gdb_command(c);
	    break;
	}

	case DBX:
	{
	    string pos = dbx_lookup(s, silent);
	    if (pos != "")
		show_position(pos);
	    break;
	}

	case XDB:
	{
	    Command c("v " + s);
	    c.verbose = !silent;
	    gdb_command(c);
	    break;
	}
	}
    }
}


// ***************************************************************************
// Current directory
// ***************************************************************************

void SourceView::process_pwd(string& pwd_output)
{
    strip_final_blanks(pwd_output);

    while (pwd_output != "")
    {
	string pwd;
	if (pwd_output.contains('\n'))
	{
	    pwd        = pwd_output.before('\n');
	    pwd_output = pwd_output.after('\n');
	}
	else
	{
	    pwd        = pwd_output;
	    pwd_output = "";
	}

	switch (gdb->type())
	{
	case GDB:			// 'Working directory PATH.'
	    if (pwd.contains("Working directory", 0))
	    {
		pwd = pwd.before('.', -1);
		pwd = pwd.after(' ', -1);
	    }
	    // FALL THROUGH

	case XDB:
	case DBX:		// 'PATH'
	    if (pwd.contains('/', 0) && !pwd.contains(" "))
	    {
		current_pwd = pwd;
		process_cd(current_pwd);
		return;
	    }
	    break;
	}
    }
}

// ***************************************************************************
// History
// ***************************************************************************

// Add position to history
void SourceView::add_to_history(const string& file_name, int line)
{
    if (source_history_locked)
    {
	source_history_locked = false;
	return;
    }

    string entry = file_name + ":" + itostring(line);

    string last_entry;
    if (history.size() > 0)
    {
	last_entry = history[history.size() - 1];
	if (last_entry.freq(':') == 2)
	{
	    int last_colon = last_entry.index(':', -1);
	    last_entry = last_entry.before(last_colon);
	}
    }

    if (entry != last_entry)
    {
	history += entry;
	history_position = history.size();
    }

    if (last_start_secondary_highlight || last_end_secondary_highlight)
    {
	XmTextSetHighlight(source_text_w,
			   last_start_secondary_highlight,
			   last_end_secondary_highlight,
			   XmHIGHLIGHT_NORMAL);

	last_start_secondary_highlight = last_end_secondary_highlight = 0;
    }

#if 0
    clog << "Position history:\n";
    for (int i = 0; i < history.size(); i++)
	clog << i << (i == history_position - 1 ? "* " : "  ") 
	     << history[i] << "\n";
    clog << "\n";
#endif
}

// Add position to history
void SourceView::add_to_history(const string& address)
{
    if (code_history_locked)
    {
	code_history_locked = false;
	return;
    }

    if (history.size() > 0)
    {
	string& last_entry = history[history.size() - 1];
	if (last_entry.freq(':') < 2)
	{
	    // Append address to this position
	    last_entry += ":" + address;
	}
	else
	{
	    // Address already there
	    int last_colon = last_entry.index(':', -1);
	    string last_address = last_entry.after(last_colon);
	    if (address != last_address)
	    {
		// Add new entry
		string new_entry = last_entry.through(last_colon) + address;

		history += new_entry;
		history_position = history.size();
	    }
	}
    }
    else
    {
	// No source position yet: add address
	history += "::" + address;
	history_position = history.size();
    }

#if 0
    clog << "Position history:\n";
    for (int i = 0; i < history.size(); i++)
	clog << i << (i == history_position - 1 ? "* " : "  ") 
	     << history[i] << "\n";
    clog << "\n";
#endif
}

// Lookup entry from position history
void SourceView::goto_entry(string entry)
{
    string file_name = entry.before(':');
    string line_str  = entry.after(':');
    int line         = atoi(line_str);
    string address   = line_str.after(':');

    // Show position in status line
    string msg = "";
    if (file_name != "")
	msg = "File " + quote(file_name);
    if (line != 0)
    {
	if (msg == "")
	    msg = "Line ";
	else
	    msg += ", line ";
	msg += itostring(line);
    }
    if (address != "")
    {
	if (msg == "")
	    msg = "Address ";
	else
	    msg += ", address ";
	msg += address;
    }
    set_status(msg);


    if (file_name != "")
    {
	// Lookup source
	if (!file_matches(file_name, current_file_name))
	{
	    source_history_locked = true;
	    read_file(file_name, line);
	}

	if (line > 0 && line <= line_count)
	{
	    XmTextSetHighlight(source_text_w,
			       last_start_secondary_highlight,
			       last_end_secondary_highlight,
			       XmHIGHLIGHT_NORMAL);

	    last_start_secondary_highlight = pos_of_line(line);
	    last_end_secondary_highlight   = last_start_secondary_highlight;
	    if (current_source != "")
	    {
		last_end_secondary_highlight 
		    = current_source.index('\n', 
					   last_start_secondary_highlight) + 1;
	    }

	    XmHighlightMode mode = XmHIGHLIGHT_SECONDARY_SELECTED;

	    if (line == last_execution_line
		&& file_name == last_execution_file)
	    {
		mode = display_glyphs ? 
		    XmHIGHLIGHT_NORMAL : XmHIGHLIGHT_SELECTED;
	    }

	    XmTextSetHighlight(source_text_w,
			       last_start_secondary_highlight,
			       last_end_secondary_highlight,
			       mode);

	    XmTextPosition pos = 
		last_start_secondary_highlight + indent_amount(source_text_w);
	    SetInsertionPosition(source_text_w, pos, true);
	}
    }

    if (address != "")
    {
	// Lookup address
	code_history_locked = true;
	show_pc(address, 
		address == last_execution_pc ? 
		XmHIGHLIGHT_SELECTED : XmHIGHLIGHT_SECONDARY_SELECTED);
    }

#if 0
    clog << "Position history:\n";
    for (int i = 0; i < history.size(); i++)
	clog << i << (i == history_position - 1 ? "* " : "  ") 
	     << history[i] << "\n";
    clog << "\n";
#endif
}

void SourceView::go_back()
{
    if (history_position == history.size())
	--history_position;
    if (history_position > 0)
	goto_entry(history[--history_position]);
    else
	set_status("No previous source position.");
}

void SourceView::go_forward()
{
    if (history_position + 1 < history.size())
	goto_entry(history[++history_position]);
    else
	set_status("No next source position.");
}

// Clear history
void SourceView::clear_history()
{
    static StringArray empty;
    history               = empty;
    history_position      = 0;
    code_history_locked   = false;
    source_history_locked = false;
}


// ***************************************************************************
// Searching
// ***************************************************************************

void SourceView::find(const string& s, 
		      SourceView::SearchDirection direction,
		      bool words_only,
		      Time time)
{
    int matchlen = s.length();
    int pos = -1;
    XmTextPosition cursor = XmTextGetInsertionPosition(source_text_w);
    XmTextPosition initial_cursor = cursor;
    int wraps = 0;

    if (current_source == "")
    {
	post_error("No source.", "no_source_error", source_text_w);
	return;
    }

    // Make sure we don't re-find the currently found word
    XmTextPosition startpos;
    XmTextPosition endpos;

    if (XmTextGetSelectionPosition(source_text_w, &startpos, &endpos))
    {
	switch (direction)
	{
	case forward:
	    if (cursor == startpos
		&& cursor < XmTextPosition(current_source.length()))
		cursor++;
	    break;
	case backward:
	    if (cursor == endpos && cursor > 0)
		cursor--;
	    break;
	}
    }

    // Go and find the word
    for (;;) {
	switch (direction)
	{
	case forward:
	    pos = current_source.index(s, cursor);
	    if (pos < 0)
	    {
		if (wraps++)
		    break;
		pos = current_source.index(s, 0);
	    }
	    break;
	case backward:
	    pos = current_source.index(s, 
				       cursor - current_source.length() - 1);
	    if (pos < 0)
	    {
		if (wraps++)
		    break;
		pos = current_source.index(s, -1);
	    }
	    break;
	}

	if (pos < 0)
	    break;		// String not found or double wrap

	// Set the cursor to the appropriate position
	switch (direction)
	{
	case forward:
	    cursor = pos + matchlen;
	    break;
	case backward:
	    cursor = pos;
	    break;
	}

	if (words_only)
	{
	    // Make sure the found string is not part of a larger word
	    if (pos > 0 && pos < int(current_source.length()))
	    {
		if (isid(current_source[pos]) && isid(current_source[pos - 1]))
		    continue;
	    }

	    if (pos + matchlen < int(current_source.length()))
	    {
		if (isid(current_source[pos + matchlen - 1]) && 
		    isid(current_source[pos + matchlen]))
		    continue;
	    }
	}

	// We got it!
	break;
    }

    if (pos > 0)
    {
	string msg;

	// Highlight occurrence
	XmTextSetSelection(source_text_w, pos, pos + matchlen, time);

	// Set position
	SetInsertionPosition(source_text_w, cursor, false);

	if (cursor == initial_cursor)
	{
	    // Unchanged position
	    msg = "No other occurrences of " + quote(s) + ".";
	}
	else
	{
	    int line_nr;
	    bool in_text;
	    int bp_nr;
	    string address;

	    if (!get_line_of_pos(source_text_w, pos, line_nr, 
				 address, in_text, bp_nr))
		line_nr = line_count;

	    msg = "Found " + quote(s) + " in " + line_of_cursor();
	    if (wraps)
		msg += " (wrapped)";
	}

	set_status(msg);
    }
    else
    {
	post_warning(quote(s) + " not found.", "source_find_error", 
		     source_text_w);
    }
}



// ***************************************************************************
// Return current source name
string SourceView::current_source_name()
{
    string source = "";

    switch (gdb->type())
    {
    case GDB:
	// GDB internally recognizes only `source names', i.e., the
	// source file name as compiled into the executable.
	if (source_name_cache[current_file_name] == "")
	{
	    // Try the current source.
	    string ans = gdb_question("info source");
	    if (ans != NO_GDB_ANSWER)
	    {
		ans = ans.before('\n');
		ans = ans.after(' ', -1);

		if (base_matches(ans, current_file_name))
		{
		    // For security, we request that source and current
		    // file have the same basename.
		    source_name_cache[current_file_name] = ans;
		}
		else
		{
		    // The current source does not match the current file.
		    // Try all sources.
		    static string all_sources = "<ALL SOURCES>";

		    if (source_name_cache[all_sources] == "")
		    {
			ans = gdb_question("info sources");
			if (ans != NO_GDB_ANSWER)
			{
			    // Create a newline-separated list of sources
			    string new_ans;
			    string line;

			    while (ans != "")
			    {
				line = ans.before('\n');
				ans = ans.after('\n');

				if (line == "" || line.contains(':', -1))
				    continue;

				line.gsub(", ", "\n");
				new_ans += line + '\n';
			    }

			    ans = new_ans;
			    source_name_cache[all_sources] = ans;
			}
		    }

		    ans = source_name_cache[all_sources];
		    if (ans != "")
		    {
			int n = ans.freq('\n');
			string *sources = new string[n + 1];
			split(ans, sources, n + 1, '\n');

			for (int i = 0; i < n + 1; i++)
			{
			    if (base_matches(sources[i], current_file_name))
			    {
				const string& src = sources[i];
				source_name_cache[current_file_name] = src;
				break;
			    }
			}
			
			delete[] sources;

			if (source_name_cache[current_file_name] == "")
			{
			    // No such source text.  Store the base name
			    // such that GDB is not asked again.
			    string base = basename(current_file_name.chars());
			    source_name_cache[current_file_name] = base;
			}
		    }
		}
	    }
	}

	source = source_name_cache[current_file_name];
	break;

    case DBX:
    case XDB:
	if (app_data.use_source_path)
	{
	    // DBX and XDB use full file names.
	    source = full_path(current_file_name);
	}
	break;
    }

    // In case this does not work, use the current base name.
    if (source == "")
	source = basename(current_file_name.chars());

    return source;
}

// ***************************************************************************
//
string SourceView::line_of_cursor()
{
    XmTextPosition pos = XmTextGetInsertionPosition(source_text_w);

    string s = current_source_name();
    if (s == "")
	return "";		// No source

    int line_nr;
    bool in_text;
    int bp_nr;
    string address;

    if (get_line_of_pos(source_text_w, pos, line_nr, address, in_text, bp_nr))
	return s + ":" + itostring(line_nr);    // Cursor within source
    else
	return s + ":" + itostring(line_count);	// Cursor in last line
}

string SourceView::file_of_cursor()
{
    string pos = line_of_cursor();
    return full_path(current_file_name) + pos.from(':');
}


//-----------------------------------------------------------------------------
// Handle mouse selections
//----------------------------------------------------------------------------

void SourceView::setSelection(XtPointer client_data, XtIntervalId *)
{
    Widget w = (Widget)client_data;

    assert(XmIsText(w));

    XmTextSetSelection(w, selection_startpos, selection_endpos, 
		       selection_time);

    set_source_argCB(w, XtPointer(false), 0);
}

void SourceView::startSelectWordAct (Widget text_w, XEvent* e, 
				     String *params, Cardinal *num_params)
{
    selection_event = *e;

    XtCallActionProc(text_w, "grab-focus", e, params, *num_params);

    if (e->type != ButtonPress && e->type != ButtonRelease)
	return;

    XButtonEvent *event = (XButtonEvent *) e;

    XmTextPosition pos = XmTextXYToPos (text_w, event->x, event->y);

    XmTextPosition startpos, endpos;

    find_word_bounds(text_w, pos, startpos, endpos);

    selection_click    = true;
    selection_startpos = startpos;
    selection_endpos   = endpos;
    selection_pos      = pos;
    selection_time     = time(e);

    XtAppAddTimeOut(XtWidgetToApplicationContext(text_w), 0, setSelection, 
		    (XtPointer)text_w);
}

void SourceView::endSelectWordAct (Widget text_w, XEvent* e, 
				   String *params, Cardinal *num_params)
{
    selection_event = *e;

    XtCallActionProc(text_w, "extend-end", e, params, *num_params);
    
    if (e->type != ButtonPress && e->type != ButtonRelease)
	return;
    
    XButtonEvent *event = (XButtonEvent *) e;

    XmTextPosition pos = XmTextXYToPos (text_w, event->x, event->y);

    XmTextPosition startpos, endpos;
    if (XmTextGetSelectionPosition(text_w, &startpos, &endpos))
    {
	selection_startpos = startpos;
	selection_endpos   = endpos;
    }

    selection_pos      = pos;
    selection_time     = time(e);

    XtAppAddTimeOut(XtWidgetToApplicationContext(text_w), 0, setSelection,
		   (XtPointer)text_w);
}


//-----------------------------------------------------------------------------
// Handle source popup
//----------------------------------------------------------------------------

void SourceView::set_text_popup_label(int item, const string& arg, bool sens)
{
    Widget w = text_popup[item].widget;
    MString label = MString(text_cmd_labels[item]) + tt(arg);

    XtVaSetValues(w, XmNlabelString, label.xmstring(), NULL);
    XtSetSensitive(w, sens);
}

void SourceView::set_text_popup_resource(int item, const string& arg)
{
    if (lesstif_version < 1000)
    {
	// Set up resources for yet-to-be-created popup menu
	string db = string(DDD_CLASS_NAME "*text_popup.") 
	    + text_popup[item].name + "." + XmNlabelString + ": "
	    + "@rm " + text_cmd_labels[item] 
	    + " @tt " + arg;

	XrmDatabase res = XrmGetStringDatabase(db.chars());
	XrmDatabase target = XtDatabase(XtDisplay(source_text_w));
	XrmMergeDatabases(res, &target);
    }
}


void SourceView::srcpopupAct (Widget w, XEvent* e, String *, Cardinal *)
{
    if (e->type != ButtonPress && e->type != ButtonRelease)
	return;

    if (!is_source_widget(w) && !is_code_widget(w))
	return;

    XButtonEvent* event = (XButtonEvent *) e;

    Position x = event->x;
    Position y = event->y;

    if (w != source_text_w && w != code_text_w)
    {
	// Called from a glyph: add glyph position to event position
	Position xw, yw;
	XtVaGetValues(w, 
		      XmNx, &xw,
		      XmNy, &yw,
		      NULL);
	x += xw;
	y += yw;
    }


    // Get the position
    Widget text_w;
    if (is_source_widget(w))
	text_w = source_text_w;
    else if (is_code_widget(w))
	text_w = code_text_w;
    else
	return;

    XmTextPosition pos = XmTextXYToPos (text_w, x, y);

    // Move the insertion cursor to this position, but don't disturb the
    // selection
    XmTextPosition left, right;
    Boolean have_selection = XmTextGetSelectionPosition(text_w, &left, &right);
    if (have_selection && pos >= left && pos <= right)
    {
	// Do not scroll here.  Do not use SetInsertionPosition().
	XmTextSetInsertionPosition(text_w, pos);
	XmTextSetSelection(text_w, left, right, time(e));
    }
    else
    {
	// Do not scroll here.  Do not use SetInsertionPosition().
	XmTextClearSelection(text_w, time(e));
	XmTextSetInsertionPosition(text_w, pos);
    }

    int line_nr;
    bool in_text;
    static int bp_nr;
    static string address;
    bool pos_found = get_line_of_pos(w, pos, line_nr, address, in_text, bp_nr);
    bool right_of_text = 
	pos < XmTextPosition(current_text(w).length()) 
	&& current_text(w)[pos] == '\n';

    if (pos_found && bp_nr != 0)
    {
	// Clicked on breakpoint: create breakpoint menu
	static Widget bp_popup_w = 0;

	if (bp_popup_w == 0) {
	    bp_popup_w = MMcreatePopupMenu (text_w, "bp_popup", bp_popup);
	    MMaddCallbacks (bp_popup, XtPointer(&bp_nr));
	    InstallButtonTips(bp_popup_w);
	}

	// Grey out unsupported functions
	XtSetSensitive(bp_popup[BPItms::Disable].widget, 
		       gdb->has_disable_command());
	XtSetSensitive(bp_popup[BPItms::IgnoreCount].widget,
		       gdb->has_ignore_command());
	MString label(bp_map.get(bp_nr)->enabled() ? 
		      "Disable Breakpoint" : "Enable Breakpoint");
	XtVaSetValues(bp_popup[BPItms::Disable].widget,
		      XmNlabelString, label.xmstring(),
		      NULL);

	XmMenuPosition (bp_popup_w, event);
	XtManageChild (bp_popup_w);
    }
    else if (pos_found 
	     && (line_nr > 0 || address != "") 
	     && (!in_text || right_of_text))
    {
	// Create popup menu for selected line
	static Widget line_popup_w = 0;
	if (line_popup_w == 0)
	{
	    line_popup_w = MMcreatePopupMenu (w, "line_popup", line_popup);
	    MMaddCallbacks (line_popup, XtPointer(&address));
	    InstallButtonTips(line_popup_w);

	    XtSetSensitive(line_popup[LineItms::SetTempBP].widget, 
			   gdb->type() != DBX || gdb->has_when_command());
	    XtSetSensitive(line_popup[LineItms::TempNContBP].widget,
			   gdb->type() != DBX || gdb->has_when_command());
	}

	if (is_source_widget(w))
	    address = current_source_name() + ":" + itostring(line_nr);
	else
	    address = string('*') + address;
	XmMenuPosition (line_popup_w, event);
	XtManageChild (line_popup_w);
    }
    else
    {
	// Determine surrounding token (or selection) and create popup
	static string word;
	static string ref_word;

	XmTextPosition startpos = 0;
	XmTextPosition endpos   = 0;

	if (pos_found)
	    word = get_word_at_pos(text_w, pos, startpos, endpos);

	// Popup specific word menu
	ref_word = gdb->dereferenced_expr(word);

	string current_arg = word;
	shorten(current_arg, max_popup_expr_length);
	string current_ref_arg = gdb->dereferenced_expr(current_arg);

	if (lesstif_version < 1000)
	{
	    set_text_popup_resource(TextItms::Break,    current_arg);
	    set_text_popup_resource(TextItms::Clear,    current_arg);
	    set_text_popup_resource(TextItms::Print,    current_arg);
	    set_text_popup_resource(TextItms::Disp,     current_arg);
	    set_text_popup_resource(TextItms::PrintRef, current_ref_arg);
	    set_text_popup_resource(TextItms::DispRef,  current_ref_arg);
	    set_text_popup_resource(TextItms::Whatis,   current_arg);
	    set_text_popup_resource(TextItms::Lookup,   current_arg);
	}

	Widget text_popup_w = 
	    MMcreatePopupMenu(text_w, "text_popup", text_popup);
	MMaddCallbacks (text_popup, XtPointer(&word));
	InstallButtonTips(text_popup_w);

	// The popup menu is destroyed immediately after having popped down.
	Widget shell = XtParent(text_popup_w);
	XtAddCallback(shell, XtNpopdownCallback, DestroyThisCB, shell);

	bool sens = (word.length() > 0);
	set_text_popup_label(TextItms::Break,    current_arg, sens);
	set_text_popup_label(TextItms::Clear,    current_arg, sens);
	set_text_popup_label(TextItms::Print,    current_arg, sens);
	set_text_popup_label(TextItms::Disp,     current_arg, sens);
	set_text_popup_label(TextItms::PrintRef, current_ref_arg, sens);
	set_text_popup_label(TextItms::DispRef,  current_ref_arg, sens);
	set_text_popup_label(TextItms::Whatis,   current_arg, sens);
	set_text_popup_label(TextItms::Lookup,   current_arg, sens);

	XmMenuPosition (text_popup_w, event);
	XtManageChild (text_popup_w);
    }
}



//-----------------------------------------------------------------------------
// Breakpoint selection
//----------------------------------------------------------------------------

void SourceView::NewBreakpointDCB(Widget w, XtPointer, XtPointer call_data)
{
    XmSelectionBoxCallbackStruct *cbs = 
	(XmSelectionBoxCallbackStruct *)call_data;

    String _input;
    XmStringGetLtoR(cbs->value, MSTRING_DEFAULT_CHARSET, &_input);
    string input(_input);
    XtFree(_input);

    int i = 0;
    while (i < int(input.length()) && isspace(input[i]))
	i++;
    input = input.from(i);
    if (input == "")
	return;

    create_bp(input, w);
}

void SourceView::NewBreakpointCB(Widget, XtPointer, XtPointer)
{
    static Widget new_breakpoint_dialog = 0;
    if (new_breakpoint_dialog == 0)
    {
	new_breakpoint_dialog = 
	    verify(XmCreatePromptDialog(source_text_w,
					"new_breakpoint_dialog",
					NULL, 0));
	Delay::register_shell(new_breakpoint_dialog);

	if (lesstif_version <= 79)
	    XtUnmanageChild(XmSelectionBoxGetChild(new_breakpoint_dialog,
						   XmDIALOG_APPLY_BUTTON));

	XtAddCallback(new_breakpoint_dialog,
		      XmNhelpCallback,
		      ImmediateHelpCB,
		      NULL);
	XtAddCallback(new_breakpoint_dialog,
		      XmNokCallback,
		      NewBreakpointDCB,
		      NULL);
    }
    manage_and_raise(new_breakpoint_dialog);
}


// Edit breakpoint condition
void SourceView::EditBreakpointConditionDCB(Widget, 
					    XtPointer client_data, 
					    XtPointer call_data)
{
    if (breakpoint_list_w == 0)
	return;

    XmSelectionBoxCallbackStruct *cbs = 
	(XmSelectionBoxCallbackStruct *)call_data;
    String input;
    XmStringGetLtoR(cbs->value, MSTRING_DEFAULT_CHARSET, &input);

    if (client_data == 0)
    {
	IntArray breakpoint_nrs;
	getDisplayNumbers(breakpoint_list_w, breakpoint_nrs);
	for (int i = 0; i < breakpoint_nrs.size(); i++)
	    set_bp_cond(breakpoint_nrs[i], input);
    }
    else
    {
	int bp_nr = *((int *)client_data);
	set_bp_cond(bp_nr, input);
    }

    XtFree(input);
}

void SourceView::EditBreakpointConditionCB(Widget,
					   XtPointer client_data,
					   XtPointer)
{
    if (breakpoint_list_w == 0)
	return;

    static Widget edit_breakpoint_condition_dialog = 0;
    if (edit_breakpoint_condition_dialog == 0)
    {
	edit_breakpoint_condition_dialog = 
	    verify(XmCreatePromptDialog(source_text_w,
					"edit_breakpoint_condition_dialog",
					NULL, 0));
	Delay::register_shell(edit_breakpoint_condition_dialog);

	if (lesstif_version <= 79)
	    XtUnmanageChild(XmSelectionBoxGetChild(
		edit_breakpoint_condition_dialog, XmDIALOG_APPLY_BUTTON));

	XtAddCallback(edit_breakpoint_condition_dialog,
		      XmNhelpCallback,
		      ImmediateHelpCB,
		      NULL);
    }

    XtRemoveAllCallbacks(edit_breakpoint_condition_dialog,
			 XmNokCallback);

    XtAddCallback(edit_breakpoint_condition_dialog,
		  XmNokCallback,
		  EditBreakpointConditionDCB,
		  client_data);

    int bp_nr;
    int *bp_nr_ptr = (int *)client_data;
    if (bp_nr_ptr == 0)
    {
	IntArray breakpoint_nrs;
	getDisplayNumbers(breakpoint_list_w, breakpoint_nrs);
	bp_nr = breakpoint_nrs[0];
    }
    else
    {
	bp_nr = *bp_nr_ptr;
    }

    if (bp_nr > 0)
    {
	string cond = "";
	MapRef ref;
	for (BreakPoint* bp = bp_map.first(ref);
	     bp != 0;
	     bp = bp_map.next(ref))
	{
	    if (bp->number() == bp_nr)
	    {
		cond = bp->condition();
		break;
	    }
	}

	MString xmcond(cond);
	XtVaSetValues(edit_breakpoint_condition_dialog,
		      XmNtextString, xmcond.xmstring(),
		      NULL);

	Widget text = XmSelectionBoxGetChild(edit_breakpoint_condition_dialog,
					     XmDIALOG_TEXT);
	XmTextSetSelection(text, 0, cond.length(), 
			   XtLastTimestampProcessed(XtDisplay(text)));
    }

    manage_and_raise(edit_breakpoint_condition_dialog);
}



// Edit breakpoint ignore count
void SourceView::EditBreakpointIgnoreCountDCB(Widget, 
					      XtPointer client_data, 
					      XtPointer call_data)
{
    if (breakpoint_list_w == 0)
	return;

    XmSelectionBoxCallbackStruct *cbs = 
	(XmSelectionBoxCallbackStruct *)call_data;
    String input;
    XmStringGetLtoR(cbs->value, MSTRING_DEFAULT_CHARSET, &input);
    int count = atoi(input);
    XtFree(input);

    if (client_data == 0)
    {
	IntArray breakpoint_nrs;
	getDisplayNumbers(breakpoint_list_w, breakpoint_nrs);
	for (int i = 0; i < breakpoint_nrs.size(); i++)
	    gdb_command(gdb->ignore_command(itostring(breakpoint_nrs[i]), 
					    count));
    }
    else
    {
	int bp_nr = *((int *)client_data);
	gdb_command(gdb->ignore_command(itostring(bp_nr), count));
    }
}

void SourceView::EditBreakpointIgnoreCountCB(Widget,
					     XtPointer client_data,
					     XtPointer)
{
    if (breakpoint_list_w == 0)
	return;

    static Widget edit_breakpoint_ignore_count_dialog = 0;
    if (edit_breakpoint_ignore_count_dialog == 0)
    {
	edit_breakpoint_ignore_count_dialog = 
	    verify(XmCreatePromptDialog(source_text_w,
					"edit_breakpoint_ignore_count_dialog",
					NULL, 0));
	Delay::register_shell(edit_breakpoint_ignore_count_dialog);

	if (lesstif_version <= 79)
	    XtUnmanageChild(XmSelectionBoxGetChild(
		edit_breakpoint_ignore_count_dialog, XmDIALOG_APPLY_BUTTON));

	XtAddCallback(edit_breakpoint_ignore_count_dialog,
		      XmNhelpCallback,
		      ImmediateHelpCB,
		      NULL);
    }

    XtRemoveAllCallbacks(edit_breakpoint_ignore_count_dialog,
			 XmNokCallback);

    XtAddCallback(edit_breakpoint_ignore_count_dialog,
		  XmNokCallback,
		  EditBreakpointIgnoreCountDCB,
		  client_data);

    int bp_nr;
    int *bp_nr_ptr = (int *)client_data;
    if (bp_nr_ptr == 0)
    {
	IntArray breakpoint_nrs;
	getDisplayNumbers(breakpoint_list_w, breakpoint_nrs);
	bp_nr = breakpoint_nrs[0];
    }
    else
    {
	bp_nr = *bp_nr_ptr;
    }

    string ignore = "";
    if (bp_nr > 0)
    {
	MapRef ref;
	for (BreakPoint* bp = bp_map.first(ref);
	     bp != 0;
	     bp = bp_map.next(ref))
	{
	    if (bp->number() == bp_nr)
	    {
		ignore = bp->ignore_count();
		break;
	    }
	}
    }

    MString xmignore(ignore);
    XtVaSetValues(edit_breakpoint_ignore_count_dialog,
		  XmNtextString, xmignore.xmstring(),
		  NULL);
    Widget text = 
	XmSelectionBoxGetChild(edit_breakpoint_ignore_count_dialog,
			       XmDIALOG_TEXT);
    XmTextSetSelection(text, 0, ignore.length(), 
		       XtLastTimestampProcessed(XtDisplay(text)));

    manage_and_raise(edit_breakpoint_ignore_count_dialog);
}


void SourceView::BreakpointCmdCB(Widget,
				 XtPointer client_data,
				 XtPointer)
{
    if (breakpoint_list_w == 0)
	return;

    string cmd = (String)client_data;

    if (cmd == "enable")
	cmd = gdb->enable_command();
    else if (cmd == "disable")
	cmd = gdb->disable_command();
    else if (cmd == "delete")
	cmd = gdb->delete_command();

    IntArray breakpoint_nrs;
    getDisplayNumbers(breakpoint_list_w, breakpoint_nrs);
    if (breakpoint_nrs.size() > 0)
    {
	for (int i = 0; i < breakpoint_nrs.size(); i++)
	    cmd += " " + itostring(breakpoint_nrs[i]);

	gdb_command(cmd);
    }
}

void SourceView::LookupBreakpointCB(Widget, XtPointer, XtPointer)
{
    if (breakpoint_list_w == 0)
	return;

    IntArray breakpoint_nrs;
    getDisplayNumbers(breakpoint_list_w, breakpoint_nrs);
    if (breakpoint_nrs.size() == 1)
	lookup("#" + itostring(breakpoint_nrs[0]));
}

// Handle breakpoint info
void SourceView::process_breakpoints(string& info_breakpoints_output)
{
    if (breakpoint_list_w == 0)
	return;

    read_leading_blanks(info_breakpoints_output);
    strip_final_blanks(info_breakpoints_output);
    if (info_breakpoints_output == "")
	info_breakpoints_output = "No breakpoints.";

    int count = info_breakpoints_output.freq('\n') + 1;

    string *breakpoint_list = new string[count];
    bool *selected          = new bool[count];

    split(info_breakpoints_output, breakpoint_list, count, '\n');

    while (count > 0 && breakpoint_list[count - 1] == "")
	count--;

    bool select = false;
    for (int i = 0; i < count; i++)
    {
	int bp_number = get_positive_nr(breakpoint_list[i]);
	if (bp_number > 0)
	{
	    MapRef ref;
	    for (BreakPoint* bp = bp_map.first(ref);
		 bp != 0;
		 bp = bp_map.next(ref))
	    {
		if (bp->number() == bp_number)
		{
		    select = bp->selected();
		    break;
		}
	    }
	}

	selected[i] = select;
	setup_where_line(breakpoint_list[i]);
    }

    setLabelList(breakpoint_list_w, breakpoint_list, selected, count,
		 gdb->type() == GDB && count > 1, false);
    UpdateBreakpointButtonsCB(breakpoint_list_w, XtPointer(0), XtPointer(0));

    delete[] breakpoint_list;
    delete[] selected;
}

void SourceView::UpdateBreakpointButtonsCB(Widget, XtPointer, 
					   XtPointer call_data)
{
    (void) call_data;		// Use it

    if (edit_breakpoints_dialog_w == 0)
	return;

    IntArray breakpoint_nrs;
    getDisplayNumbers(breakpoint_list_w, breakpoint_nrs);

    // Update selection
    MapRef ref;
    for (BreakPoint* bp = bp_map.first(ref); bp != 0; bp = bp_map.next(ref))
	bp->selected() = false;

    for (int i = 0; i < breakpoint_nrs.size(); i++)
    {
	int bp_number = breakpoint_nrs[i];
	MapRef ref;
	for (BreakPoint* bp = bp_map.first(ref);
	     bp != 0;
	     bp = bp_map.next(ref))
	{
	    if (bp->number() == bp_number)
	    {
		bp->selected() = true;
		break;
	    }
	}
    }

#if 0
    if (call_data != 0)
    {
	// Update status line
	if (breakpoint_nrs.size() == 1)
	    set_status_mstring(help_on_bp(breakpoint_nrs[0], true));
	else
	    set_status("");
    }
#endif

    // Update buttons
    XtSetSensitive(bp_area[BPButtons::Lookup].widget,
		   breakpoint_nrs.size() == 1);
    XtSetSensitive(bp_area[BPButtons::Enable].widget,      
		   gdb->has_enable_command() && breakpoint_nrs.size() > 0);
    XtSetSensitive(bp_area[BPButtons::Disable].widget,     
		   gdb->has_disable_command() && breakpoint_nrs.size() > 0);
    XtSetSensitive(bp_area[BPButtons::Condition].widget,   
		   breakpoint_nrs.size() > 0);
    XtSetSensitive(bp_area[BPButtons::IgnoreCount].widget, 
		   gdb->has_ignore_command() && breakpoint_nrs.size() > 0);
    XtSetSensitive(bp_area[BPButtons::Delete].widget,
		   breakpoint_nrs.size() > 0);
}

void SourceView::EditBreakpointsCB(Widget, XtPointer, XtPointer)
{
    manage_and_raise(edit_breakpoints_dialog_w);
}



//-----------------------------------------------------------------------------
// Stack frame selection
//----------------------------------------------------------------------------

void SourceView::StackDialogPoppedDownCB (Widget, XtPointer, XtPointer)
{
    stack_dialog_popped_up = false;
}

void SourceView::SelectFrameCB (Widget w, XtPointer, XtPointer call_data)
{
    XmListCallbackStruct *cbs = (XmListCallbackStruct *)call_data;

    int count = 0;
    XtVaGetValues(w,
		  XmNitemCount, &count,
		  NULL);

    XtSetSensitive(up_w,   cbs->item_position > 1);
    XtSetSensitive(down_w, cbs->item_position < count);

    switch (gdb->type())
    {
    case GDB:
	// GDB frame output is caught by our routines.
	gdb_command(gdb->frame_command(count - cbs->item_position));
	break;
    
    case XDB:
	// XDB frame output is caught by our routines.
	gdb_command(gdb->frame_command(cbs->item_position - 1));
	break;

    case DBX:
	if (gdb->has_frame_command())
	{
	    // Issue `frame' command
	    gdb_command(gdb->frame_command(count - cbs->item_position + 1));
	}
	else
	{
	    // Some DBXes lack a `frame' command.
	    // Use `up N'/`down N' instead.
	    int offset = cbs->item_position - last_frame_pos;
	    if (offset == -1)
		gdb_command("up");
	    else if (offset < 0)
		gdb_command("up " + itostring(-offset));
	    else if (offset == 1)
		gdb_command("down");
	    else if (offset > 0)
		gdb_command("down " + itostring(offset));

	    // Call `set_frame_pos' now.
	    frame_pos_locked = false;
	    set_frame_pos(0, cbs->item_position);

	    // Ignore the call after `up'/`down' command.
	    frame_pos_locked = (offset != 0);
	}
	break;
    }
}

void SourceView::refresh_stack_frames()
{
    // Allow unlimited time to find out where we are
    string where_s = gdb_question(gdb->where_command(), -1, true);
    if (where_s == NO_GDB_ANSWER)
	where_s = "No stack.";
    process_where(where_s);

    if (gdb->has_frame_command())
    {
	string frame = gdb_question(gdb->frame_command());
	process_frame(frame);
    }
}

void SourceView::ViewStackFramesCB(Widget, XtPointer, XtPointer)
{
    refresh_stack_frames();
    manage_and_raise(stack_dialog_w);
    stack_dialog_popped_up = true;
}

// Remove file paths and argument lists from `where' output
void SourceView::setup_where_line(string& line)
{
    const int min_width = 40;

    // Remove file paths (otherwise line can be too long for DBX)
    //   ... n.b. with templates, line can still be rather long
#if RUNTIME_REGEX
    static regex rxfilepath("[^\"\' /]*/");
#endif
    line.gsub(rxfilepath, "");

    // Shorten argument lists `(a = 1, b = 2, ...)' to `()'
#if RUNTIME_REGEX
    static regex rxarglist("[(][^0-9][^)]*[)]");
#endif
    int start = index(line, rxarglist, "(");
    if (start > 0)
    {
	int end = line.index(')', -1);
	if (end > start)
	line = line.through(start) + line.from(end);
    }

    if (int(line.length()) < min_width)
	line += replicate(' ', min_width - line.length());
}

void SourceView::process_where (string& where_output)
{
    int count          = where_output.freq('\n') + 1;
    string *frame_list = new string[count];
    bool *selected     = new bool[count];

    split(where_output, frame_list, count, '\n');

    while (count > 0 && frame_list[count - 1] == "")
	count--;

    if (gdb->type() != XDB)
    {
	// Invert list such that `Up' and `Down' make sense
	for (int i = 0; i < count / 2; i++)
	{
	    string tmp = frame_list[i];
	    frame_list[i] = frame_list[count - i - 1];
	    frame_list[count - i - 1] = tmp;
	}
    }

    // Make sure we have a minimum width
    for (int i = 0; i < count; i++)
    {
	selected[i] = false;
	setup_where_line(frame_list[i]);
    }

    setLabelList(frame_list_w, frame_list, selected, count, false, false);
    set_frame_pos(0, 0);

    delete[] frame_list;
    delete[] selected;
}

void SourceView::process_frame (string& frame_output)
{
    if (frame_output != "" 
	&& (frame_output[0] == '#' 
	    || gdb->type() == DBX
	    || gdb->type() == XDB))
    {
	string frame_nr;

	switch (gdb->type())
	{
	case GDB:
	    frame_nr = frame_output.after(0);
	    break;

	case DBX:
	    frame_nr = frame_output;

	    // Sun DBX 4.0 issues `=>' before current frame
	    if (frame_nr.contains("=>", 0))
		frame_nr = frame_nr.after("=>");
	    break;

	case XDB:
	    frame_nr = frame_output.after(" = ", -1);
	    break;
	}

	int frame = get_positive_nr(frame_nr);

	if (gdb->type() == DBX)
	    frame--;		    // GDB uses origin-0, DBX uses origin-1

	at_lowest_frame = (frame == 0);

	int count         = 0;
	int top_item      = 0;
	int visible_items = 0;
	XtVaGetValues(frame_list_w,
		      XmNitemCount, &count,
		      XmNtopItemPosition, &top_item,
		      XmNvisibleItemCount, &visible_items,
		      NULL);

	int pos = 1;
	switch (gdb->type())
	{
	case GDB:
	case DBX:
	    pos = count - frame;
	    break;

	case XDB:
	    pos = frame + 1;
	    break;
	}

	ListSetAndSelectPos(frame_list_w, pos);

	XtSetSensitive(up_w,   pos > 1);
	XtSetSensitive(down_w, pos < count);

	update_glyphs();
	current_frame = frame;
    }
    else
    {
	XtSetSensitive(up_w,   False);
	XtSetSensitive(down_w, False);
	current_frame = -1;
    }
}

// Set frame manually to function FUNC; return TRUE if successful
bool SourceView::set_frame_func(const string& func)
{
    int count = 0;
    XmStringTable items;

    XtVaGetValues(frame_list_w,
		  XmNitemCount, &count,
		  XmNitems, &items,
		  NULL);

    for (int i = count - 1; i >= 0; i--)
    {
	String _item;
	XmStringGetLtoR(items[i], LIST_CHARSET, &_item);
	string item(_item);
	XtFree(_item);

	int func_index  = item.index(func);
	int paren_index = item.index('(');

	if (func_index >= 0 &&
	    (func_index < paren_index || paren_index < 0))
	{
	    set_frame_pos(0, i + 1);
	    return true;
	}
    }

    return false;
}

// Set frame manually: ARG = 0: POS, ARG = +/- N: down/up N levels
void SourceView::set_frame_pos(int arg, int pos)
{
    if (frame_pos_locked)
    {
	frame_pos_locked = false;
	return;
    }

    int items = 0;
    XtVaGetValues(frame_list_w, XmNitemCount, &items, NULL);

    if (pos == 0)
	pos = items;
    if (arg != 0)
    {
	int *position_list;
	int position_count;
	if (XmListGetSelectedPos(frame_list_w,
				 &position_list, &position_count))
	{
	    if (position_count == 1)
		pos = position_list[0] + arg;
	    XtFree((char *)position_list);
	} else
	    return;
	if (position_count != 1 || pos < 1 || pos > items)
	    return;
    }

    ListSetAndSelectPos(frame_list_w, pos);

    last_frame_pos = pos;

    XtSetSensitive(up_w,   pos > 1);
    XtSetSensitive(down_w, pos < items);
}

bool SourceView::where_required()    { return stack_dialog_popped_up; }
bool SourceView::register_required() { return register_dialog_popped_up; }
bool SourceView::thread_required()   { return thread_dialog_popped_up; }



//-----------------------------------------------------------------------------
// Register stuff
//----------------------------------------------------------------------------

void SourceView::process_registers(string& register_output)
{
    register_output.gsub(";\n", "\n");
    register_output.gsub("; ", "\n");
    register_output.gsub(";", "\n");
    register_output.gsub("\n\n", "\n");

    int count             = register_output.freq('\n') + 1;
    string *register_list = new string[count];
    bool *selected        = new bool[count];

    split(register_output, register_list, count, '\n');

    while (count > 0 && register_list[count - 1] == "")
	count--;

    for (int i = 0; i < count; i++)
    {
	tabto(register_list[i], 26);
	untabify(register_list[i]);
	selected[i] = false;
    }

    setLabelList(register_list_w, register_list, selected, count,
		 false, false);

    delete[] register_list;
    delete[] selected;
}

void SourceView::refresh_registers()
{
    string registers = gdb_question(refresh_registers_command());
    if (registers == NO_GDB_ANSWER)
	registers = "No registers.";
    process_registers(registers);
}

string SourceView::refresh_registers_command()
{
    return gdb->regs_command(all_registers);
}

void SourceView::ViewRegistersCB(Widget, XtPointer, XtPointer)
{
    refresh_registers();
    manage_and_raise(register_dialog_w);
    register_dialog_popped_up = true;
}

void SourceView::RegisterDialogPoppedDownCB (Widget, XtPointer, XtPointer)
{
    register_dialog_popped_up = false;
}

void SourceView::SelectRegisterCB (Widget, XtPointer, XtPointer call_data)
{
    XmListCallbackStruct *cbs = (XmListCallbackStruct *)call_data;

    // Get the selected line
    String _item;
    XmStringGetLtoR(cbs->item, LIST_CHARSET, &_item);
    string item(_item);
    XtFree(_item);

    if (item != "" && item[item.length() - 1] != '.')
    {
	item = "/x $" + item.through(rxalphanum);
	source_arg->set_string(item);
    }
}

//-----------------------------------------------------------------------------
// Thread stuff
//----------------------------------------------------------------------------

void SourceView::process_threads(string& threads_output)
{
    if (threads_output == NO_GDB_ANSWER 
	|| threads_output == ""
	|| threads_output.matches(rxwhite))
	threads_output = "No threads.\n";

    int count           = threads_output.freq('\n') + 1;
    string *thread_list = new string[count];
    bool *selected      = new bool[count];

    split(threads_output, thread_list, count, '\n');

    while (count > 0 && thread_list[count - 1] == "")
	count--;

    for (int i = 0; i < count; i++)
    {
	selected[i] = thread_list[i].contains('*', 0);
	if (selected[i])
	    thread_list[i] = thread_list[i].after(0);
	read_leading_blanks(thread_list[i]);
	setup_where_line(thread_list[i]);
    }

    setLabelList(thread_list_w, thread_list, selected, count, false, false);

    delete[] thread_list;
    delete[] selected;
}

void SourceView::refresh_threads()
{
    string threads = gdb_question("info threads");
    process_threads(threads);
}

void SourceView::ViewThreadsCB(Widget, XtPointer, XtPointer)
{
    refresh_threads();
    manage_and_raise(thread_dialog_w);
    thread_dialog_popped_up = true;
}

void SourceView::ThreadDialogPoppedDownCB (Widget, XtPointer, XtPointer)
{
    thread_dialog_popped_up = false;
}

void SourceView::SelectThreadCB (Widget, XtPointer, XtPointer)
{
    // Get the selected threads
    IntArray threads;
    getDisplayNumbers(thread_list_w, threads);

    if (threads.size() == 1)
	gdb_command("thread " + itostring(threads[0]));
}

//-----------------------------------------------------------------------------
// Get Line in GDB format
//----------------------------------------------------------------------------

string SourceView::get_line(string position)
{
    string file_name = current_file_name;

    if (position.contains(':'))
    {
	file_name = position.before(':');
	position  = position.after(':');
    }
    int line = get_positive_nr(position);

    // Sanity check: make sure the line # isn't too big
    line = min(line, line_count);
    if (line < 1)
	return "";

    if (!file_matches(file_name, current_file_name))
	read_file(file_name, line);

    XmTextPosition start = pos_of_line(line) + indent_amount(source_text_w);
    XmTextPosition end   = current_source.index('\n', start);
    if (end < 0)
	end = current_source.length();

    string text = current_source.at(int(start), end - start);

    ostrstream buf;
    buf << line << '\t' << text;
    return string(buf);
}


//----------------------------------------------------------------------------
// Glyph stuff
//----------------------------------------------------------------------------

// Move text cursor at glyph position
void SourceView::MoveCursorToGlyphPosCB(Widget w, 
					XtPointer, 
					XtPointer call_data)
{
    XmPushButtonCallbackStruct *info = (XmPushButtonCallbackStruct *)call_data;
    XEvent *e = info->event;
    if (e->type != ButtonPress && e->type != ButtonRelease)
	return;

    Widget text_w;
    if (is_source_widget(w))
	text_w = source_text_w;
    else if (is_code_widget(w))
	text_w = code_text_w;
    else
	return;

    // Set up event such that it applies to the source window
    XButtonEvent *event = (XButtonEvent *) e;

    Position x, y;
    XtVaGetValues(w, 
		  XmNx, &x,
		  XmNy, &y,
		  NULL);
    event->x += x;
    event->y += y;
    event->window = XtWindow(text_w);

    // Invoke action for source window
    String *params = { 0 };
    XtCallActionProc(text_w, "source-start-select-word", e, params, 0);
}

#if XmVERSION >= 2
const int motif_offset = 1;  // Motif 2.0 adds a 1 pixel border around glyphs
#else
const int motif_offset = 0;  // Motif 1.x does not
#endif

// Create glyph in FORM_W named NAME from given BITS
Widget SourceView::create_glyph(Widget form_w,
				String name,
				char */* bits */, int width, int height)
{
    Arg args[20];
    Cardinal arg = 0;
    XtSetArg(args[arg], XmNmappedWhenManaged,  False);         arg++;
    XtSetArg(args[arg], XmNtopAttachment,      XmATTACH_FORM); arg++;
    XtSetArg(args[arg], XmNleftAttachment,     XmATTACH_FORM); arg++;
    XtSetArg(args[arg], XmNrecomputeSize,      False);         arg++;
    XtSetArg(args[arg], XmNmarginBottom,       0);             arg++;
    XtSetArg(args[arg], XmNmarginTop,          0);             arg++;
    XtSetArg(args[arg], XmNmarginLeft,         0);             arg++;
    XtSetArg(args[arg], XmNmarginRight,        0);             arg++;
    XtSetArg(args[arg], XmNmarginWidth,        0);             arg++;
    XtSetArg(args[arg], XmNmarginHeight,       0);             arg++;
    XtSetArg(args[arg], XmNshadowThickness,    0);             arg++;
    XtSetArg(args[arg], XmNhighlightThickness, 0);             arg++;
    XtSetArg(args[arg], XmNborderWidth,        0);             arg++;
    XtSetArg(args[arg], XmNlabelType,  XmPIXMAP);              arg++;
    XtSetArg(args[arg], XmNmultiClick, XmMULTICLICK_DISCARD);  arg++;
    XtSetArg(args[arg], XmNalignment, XmALIGNMENT_BEGINNING);  arg++;
    XtSetArg(args[arg], XmNuserData,           XtPointer(0));  arg++;
    Widget w = verify(XmCreatePushButton(form_w, name, args, arg));
    XtRealizeWidget(w);
    XtManageChild(w);

    unsigned char unit_type;
    XtVaGetValues(w, XmNunitType, &unit_type, NULL);
    int new_width  = XmConvertUnits(w, XmHORIZONTAL, XmPIXELS, 
				    width + 1 + motif_offset, unit_type);
    int new_height = XmConvertUnits(w, XmVERTICAL, XmPIXELS, 
				    height + 1 + motif_offset, unit_type);

    Pixel background;
    XtVaGetValues(w, XmNbackground, &background, NULL);

    arg = 0;
    XtSetArg(args[arg], XmNwidth,       new_width);  arg++;
    XtSetArg(args[arg], XmNheight,      new_height); arg++;
    XtSetArg(args[arg], XmNfillOnArm,   True);       arg++;
    XtSetArg(args[arg], XmNarmColor,    background); arg++;
    XtSetValues(w, args, arg);
    
    XtAddCallback(w, XmNarmCallback, MoveCursorToGlyphPosCB, 0);

    InstallButtonTips(w);
    return w;
}

// Return height of a single line
int SourceView::line_height(Widget text_w)
{
    static int source_height = 0;
    static int code_height   = 0;
    if (text_w == source_text_w && source_height > 0)
	return source_height;
    else if (text_w == code_text_w && code_height > 0)
	return code_height;

    bool ok;

    XmTextPosition top = XmTextGetTopCharacter(text_w);
    Position top_x, top_y;
    ok = XmTextPosToXY(text_w, top, &top_x, &top_y);
    if (!ok)
	return 0;

    string& text = current_text(text_w);
    XmTextPosition second = text.index('\n', top) + 1;
    Position second_x, second_y;
    ok = XmTextPosToXY(text_w, second, &second_x, &second_y);
    if (!ok)
	return 0;

    int height = abs(second_y - top_y);

    if (text_w == source_text_w)
	source_height = height;
    else if (text_w == code_text_w)
	code_height = height;

    return height;
}


// If false, don't change glyphs - just check if they would change
bool SourceView::change_glyphs = true;

// All glyphs that have changed during update_glyphs_now()
WidgetArray SourceView::changed_glyphs;

// Unmap glyph W
void SourceView::unmap_glyph(Widget w)
{
    if (w == 0)
	return;

    assert(is_code_widget(w) || is_source_widget(w));

    XtPointer user_data;
    XtVaGetValues(w, XmNuserData, &user_data, NULL);
    if (user_data == 0)
	return;			// Already unmapped

    if (change_glyphs)
    {
	// If we unmapped the glyph during a drag operation, the drag
	// would be broken.  Move it to an invisible position instead.
	XtVaSetValues(w,
		      XmNleftOffset, -100,
		      XmNtopOffset,  -100,
		      XmNuserData, XtPointer(0),
		      NULL);
    }

    changed_glyphs += w;
}

// Map glyph W in (X, Y)
void SourceView::map_glyph(Widget& w, Position x, Position y)
{
    while (w == 0)
	CreateGlyphsWorkProc(0);

    assert(is_code_widget(w) || is_source_widget(w));

    // clog << "Mapping glyph at (" << x << ", " << y << ")\n";

    Widget text_w;
    if (is_source_widget(w))
	text_w = source_text_w;
    else
	text_w = code_text_w;

    XtPointer user_data;
    Dimension height              = 0;
    Dimension border_width        = 0;
    Dimension margin_height       = 0;
    Dimension shadow_thickness    = 0;
    Dimension highlight_thickness = 0;
    int old_x                     = 0;
    int old_y                     = 0; 
    XtVaGetValues(w,
		  XmNheight,             &height,
		  XmNborderWidth,        &border_width,
		  XmNmarginHeight,       &margin_height,
		  XmNshadowThickness,    &shadow_thickness,
		  XmNhighlightThickness, &highlight_thickness,
		  XmNuserData,           &user_data,
		  XmNleftOffset,         &old_x,
		  XmNtopOffset,          &old_y,
		  NULL);
    Dimension glyph_height = 
	height + border_width + margin_height
	+ shadow_thickness + highlight_thickness;

    y += -glyph_height + line_height(text_w) / 2 - 2;

    if (x != old_x || y != old_y)
    {
	if (change_glyphs)
	    XtVaSetValues(w, XmNleftOffset, x, XmNtopOffset, y, NULL);
	changed_glyphs += w;
    }

    if (user_data != 0)
	return;			// Already mapped

    if (change_glyphs)
    {
	XtMapWidget(w);
	XtVaSetValues(w, XmNuserData, XtPointer(1), NULL);
	changed_glyphs += w;
    }
}


// True if code/source glyphs need to be updated
bool SourceView::update_code_glyphs   = false;
bool SourceView::update_source_glyphs = false;

// Update glyphs for widget W (0: all)
void SourceView::update_glyphs(Widget w)
{
    static XtWorkProcId update_glyph_id = 0;
    static time_t update_glyph_called   = 0;

    if (w == 0)
	update_source_glyphs = update_code_glyphs = true;
    else if (is_source_widget(w))
	update_source_glyphs = true;
    else if (is_code_widget(w))
	update_code_glyphs = true;

    if (update_glyph_id == 0)
    {
	update_glyph_id = 
	    XtAppAddTimeOut(XtWidgetToApplicationContext(source_text_w), 0,
			    UpdateGlyphsWorkProc, XtPointer(&update_glyph_id));
	update_glyph_called = time((time_t *)0);
    }
}


// Invoked by scrolling keys
void SourceView::updateGlyphsAct(Widget, XEvent*, String *, Cardinal *)
{
    CheckScrollWorkProc(0, 0);
}

// Invoked whenever the text widget may be about to scroll
void SourceView::CheckScrollCB(Widget, XtPointer, XtPointer)
{
    static XtIntervalId check_scroll_id = 0;
    static time_t check_scroll_called = 0;

    if (check_scroll_id == 0)
    {
	check_scroll_id = 
	    XtAppAddTimeOut(XtWidgetToApplicationContext(source_text_w), 0,
			    CheckScrollWorkProc, XtPointer(&check_scroll_id));
	check_scroll_called = time((time_t *)0);
    }
}
    
void SourceView::CheckScrollWorkProc(XtPointer client_data, XtIntervalId *id)
{
    (void) id;			// Use it

    XtIntervalId *timer = (XtIntervalId *)client_data;
    if (timer != 0)
    {
	assert(*timer == *id);
	*timer = 0;
    }

    XmTextPosition old_top = last_top;
    last_top = XmTextGetTopCharacter(source_text_w);

    XmTextPosition old_top_pc = last_top_pc;
    last_top_pc = XmTextGetTopCharacter(code_text_w);

    if (old_top != last_top && old_top_pc != last_top_pc)
	update_glyphs();
    else if (old_top != last_top)
	update_glyphs(source_text_w);
    else if (old_top_pc != last_top_pc)
	update_glyphs(code_text_w);
}


// Pixel offsets

// Horizontal arrow offset (pixels)
int SourceView::arrow_x_offset = -5;

// Horizontal breakpoint symbol offset (pixels)
int SourceView::stop_x_offset = +6;

// Additional offset for multiple breakpoints (pixels)
int SourceView::multiple_stop_x_offset = stop_width + (2 * motif_offset - 2);


// Glyph locations: x[0] is source, x[1] is code
Widget SourceView::plain_arrows[2]  = {0, 0};
Widget SourceView::grey_arrows[2]   = {0, 0};
Widget SourceView::signal_arrows[2] = {0, 0};
Widget SourceView::temp_arrows[2]   = {0, 0};
Widget SourceView::temp_stops[2]    = {0, 0};
Widget SourceView::plain_stops[2][MAX_GLYPHS + 1];
Widget SourceView::grey_stops[2][MAX_GLYPHS + 1];



// Create glyphs in the background
Boolean SourceView::CreateGlyphsWorkProc(XtPointer)
{
    static bool all_done = false;

    if (all_done)
	return True;

    int k;
    for (k = 0; k < 2; k++)
    {
	// On the Form widget, later children are displayed on top of
	// earlier children.  A stop sign hiding an arrow gives more
	// pleasing results than vice-versa, so place arrow glyph
	// below sign glyphs.

	Widget form_w = k ? code_form_w : source_form_w;

	if (plain_arrows[k] == 0)
	{
	    plain_arrows[k] = 
		create_glyph(form_w, "plain_arrow",
			     arrow_bits, 
			     arrow_width,
			     arrow_height);
	    return False;
	}

	if (grey_arrows[k] == 0)
	{
	    grey_arrows[k] = 
		create_glyph(form_w, "grey_arrow",
			     grey_arrow_bits, 
			     grey_arrow_width, 
			     grey_arrow_height);
	    return False;
	}

	if (signal_arrows[k] == 0)
	{
	    signal_arrows[k] = 
		create_glyph(form_w, "signal_arrow",
			     signal_arrow_bits, 
			     signal_arrow_width,
			     signal_arrow_height);
	    return False;
	}

	if (temp_arrows[k] == 0)
	{
	    temp_arrows[k] = 
		create_glyph(form_w, "temp_arrow",
			     temp_arrow_bits, 
			     temp_arrow_width,
			     temp_arrow_height);
	    return False;
	}
    }
   
    for (k = 0; k < 2; k++)
    {
	Widget form_w = k ? code_form_w : source_form_w;

	int i;
	for (i = 0; i < MAX_GLYPHS; i++)
	{
	    if (grey_stops[k][i] == 0)
	    {
		grey_stops[k][i] = 
		    create_glyph(form_w, "grey_stop",
				 grey_stop_bits, 
				 grey_stop_width,
				 grey_stop_height);
		return False;
	    }
	}

	for (i = 0; i < MAX_GLYPHS; i++)
	{
	    if (plain_stops[k][i] == 0)
	    {
		plain_stops[k][i] = 
		    create_glyph(form_w, "plain_stop",
				 stop_bits, 
				 stop_width,
				 stop_height);
		return False;
	    }
	}

	if (temp_stops[k] == 0)
	{
	    temp_stops[k] = 
		create_glyph(form_w, "temp_stop",
			     temp_stop_bits, 
			     temp_stop_width,
			     temp_stop_height);
	    return False;
	}
    }

    all_done = true;
    return True;
}

// Map stop sign in W at position POS.  Get widget from STOPS[COUNT];
// store location in POSITIONS.  Return mapped widget (0 if none)
Widget SourceView::map_stop_at(Widget w, XmTextPosition pos,
			       Widget stops[], int& count,
			       TextPositionArray& positions)
{
    assert (is_source_widget(w) || is_code_widget(w));

    Position x, y;
    Boolean pos_displayed = XmTextPosToXY(w, pos, &x, &y);
    if (pos_displayed)
    {
	while (stops[count] == 0)
	{
	    if (CreateGlyphsWorkProc(0))
		break;
	}

	Widget glyph = stops[count] ? stops[count++] : 0;

	if (glyph != 0)
	{
	    for (int i = 0; i < positions.size(); i++)
		if (pos == positions[i])
		    x += multiple_stop_x_offset;

	    map_glyph(glyph, x + stop_x_offset, y);
	    positions += pos;
	    return glyph;
	}
    }

    return 0;
}

// Map arrow in W at POS.  Return mapped arrow widget (0 if none)
Widget SourceView::map_arrow_at(Widget w, XmTextPosition pos)
{
    assert (is_source_widget(w) || is_code_widget(w));

    Position x, y;
    Boolean pos_displayed = (pos != XmTextPosition(-1) 
			     && XmTextPosToXY(w, pos, &x, &y));

    int k = int(is_code_widget(w));

    Widget& signal_arrow = signal_arrows[k];
    Widget& plain_arrow  = plain_arrows[k];
    Widget& grey_arrow   = grey_arrows[k];

    while (signal_arrow == 0 || plain_arrow == 0 || grey_arrow == 0)
    {
	if (CreateGlyphsWorkProc(0))
	    break;
    }

    if (pos_displayed)
    {
	if (at_lowest_frame && signal_received)
	{
	    map_glyph(signal_arrow, x + arrow_x_offset, y);
	    unmap_glyph(plain_arrow);
	    unmap_glyph(grey_arrow);
	    return signal_arrow;
	}
	else if (at_lowest_frame)
	{
	    map_glyph(plain_arrow, x + arrow_x_offset, y);
	    unmap_glyph(signal_arrow);
	    unmap_glyph(grey_arrow);
	    return plain_arrow;
	}
	else
	{
	    map_glyph(grey_arrow, x + arrow_x_offset, y);
	    unmap_glyph(signal_arrow);
	    unmap_glyph(plain_arrow);
	    return grey_arrow;
	}
    }
    else
    {
	unmap_glyph(signal_arrow);
	unmap_glyph(plain_arrow);
	unmap_glyph(grey_arrow);
    }
    return 0;
}

// Copy glyph foreground and background colors from ORIGIN to W
void SourceView::copy_colors(Widget w, Widget origin)
{
    if (origin == 0)
	return;

    Pixel background, foreground;
    XtVaGetValues(origin,
		  XmNforeground, &foreground,
		  XmNbackground, &background,
		  NULL);

    Pixmap pixmap = 
	XmGetPixmap(XtScreen(w), XtName(w), foreground, background);
    if (pixmap != XmUNSPECIFIED_PIXMAP)
    {
	Pixmap old_pixmap;
	XtVaGetValues(w, XmNlabelPixmap, &old_pixmap, NULL);
	XmDestroyPixmap(XtScreen(w), old_pixmap);

	XtVaSetValues(w, XmNlabelPixmap, pixmap, NULL);
    }
}

// Map temporary stop sign at position POS.  If ORIGIN is given, use
// colors from ORIGIN.
Widget SourceView::map_temp_stop_at(Widget w, XmTextPosition pos, 
				    Widget origin)
{
    assert (is_source_widget(w) || is_code_widget(w));
    Position x, y;
    Boolean pos_displayed = (pos != XmTextPosition(-1) 
			     && XmTextPosToXY(w, pos, &x, &y));

    int k = int(is_code_widget(w));

    Widget& temp_stop = temp_stops[k];

    while (temp_stop == 0)
    {
	if (CreateGlyphsWorkProc(0))
	    break;
    }

    copy_colors(temp_stop, origin);

    if (pos_displayed)
    {
	if (origin)
	    XtVaGetValues(origin, XmNx, &x, NULL);
	else
	    x += stop_x_offset;

	map_glyph(temp_stop, x, y);
    }
    else
	unmap_glyph(temp_stop);

    return temp_stop;
}

// Map temporary arrow at position POS.  If ORIGIN is given, use
// colors from ORIGIN.
Widget SourceView::map_temp_arrow_at(Widget w, XmTextPosition pos, 
				     Widget origin)
{
    assert (is_source_widget(w) || is_code_widget(w));
    Position x, y;
    Boolean pos_displayed = (pos != XmTextPosition(-1) 
			     && XmTextPosToXY(w, pos, &x, &y));

    int k = int(is_code_widget(w));

    Widget& temp_arrow = temp_arrows[k];

    while (temp_arrow == 0)
    {
	if (CreateGlyphsWorkProc(0))
	    break;
    }

    copy_colors(temp_arrow, origin);

    if (pos_displayed)
	map_glyph(temp_arrow, x + arrow_x_offset, y);
    else
	unmap_glyph(temp_arrow);

    return temp_arrow;
}



// Update glyphs after interval
void SourceView::UpdateGlyphsWorkProc(XtPointer client_data, XtIntervalId *id)
{
    (void) id;			// Use it

    // Allow new invocations
    XtIntervalId *proc_id = ((XtIntervalId *) client_data);
    if (proc_id != 0)
    {
	assert(*proc_id == *id);
	*proc_id = 0;
    }

    XtAppContext app_context = XtWidgetToApplicationContext(source_text_w);
    if (XtAppPending(app_context) & (XtIMXEvent | XtIMAlternateInput))
    {
	// Other events pending - check if we shall change something
	const WidgetArray& glyphs = glyphs_to_be_updated();

	if (glyphs.size() > 0)
	{
	    // Change is imminent - unmap all glyphs that will change
	    // and try again in 10ms
	    for (int i = 0; i < glyphs.size(); i++)
		unmap_glyph(glyphs[i]);

	    XtIntervalId new_id = 
		XtAppAddTimeOut(app_context, 10,
				UpdateGlyphsWorkProc, client_data);
	    if (proc_id != 0)
		*proc_id = new_id;
	    return;
	}
    }

    change_glyphs = true;
    update_glyphs_now();
}


// The function that does the real work
void SourceView::update_glyphs_now()
{
    // clog << "Updating glyphs...";

    WidgetArray empty;
    changed_glyphs = empty;

    if (update_source_glyphs)
    {
	// Show current execution position
	XmTextPosition pos = XmTextPosition(-1);

	if (display_glyphs
	    && base_matches(current_file_name, last_execution_file)
	    && line_count > 0
	    && last_execution_line > 0
	    && last_execution_line <= line_count)
	{
	    pos = pos_of_line(last_execution_line);
	}

	map_arrow_at(source_text_w, pos);
    }

    if (update_code_glyphs)
    {
	// Show current PC
	XmTextPosition pos = XmTextPosition(-1);

	if (display_glyphs && last_execution_pc != "")
	    pos = find_pc(last_execution_pc);

	map_arrow_at(code_text_w, pos);
    }

    // Map breakpoint glyphs
    for (int k = 0; k < 2; k++)
    {
	if (k == 0 && !update_source_glyphs)
	    continue;
	if (k == 1 && !update_code_glyphs)
	    continue;

	int plain_count = 0;
	int grey_count  = 0;

	if (display_glyphs)
	{
	    TextPositionArray positions;
	    
	    MapRef ref;
	    for (BreakPoint *bp = bp_map.first(ref);
		 bp != 0;
		 bp = bp_map.next(ref))
	    {
		Widget& bp_glyph = k ? bp->code_glyph() : bp->source_glyph();
		Widget text_w    = k ? code_text_w      : source_text_w;
		bp_glyph = 0;

		XmTextPosition pos;
		if (k == 0)
		{
		    // Find source position
		    if (!bp_matches(bp)
			|| line_count <= 0
			|| bp->line_nr() <= 0
			|| bp->line_nr() > line_count)
			continue;

		    pos = pos_of_line(bp->line_nr());
		}
		else
		{
		    // Find code position
		    if (bp->type() != BREAKPOINT)
			continue;

		    pos = find_pc(bp->address());
		}


		if (bp->enabled())
		    bp_glyph = map_stop_at(text_w, pos, plain_stops[k],
					   plain_count, positions);
		else
		    bp_glyph = map_stop_at(text_w, pos, grey_stops[k],
					   grey_count, positions);
	    }
	}

	// Unmap remaining breakpoint glyphs
	Widget w;
	while ((w = plain_stops[k][plain_count++]))
	    unmap_glyph(w);
	while ((w = grey_stops[k][grey_count++]))
	    unmap_glyph(w);
    }

    if (change_glyphs)
    {
	update_source_glyphs = false;
	update_code_glyphs   = false;
    }

    // clog << "done.\n";
}


// Return all glyphs that would change
const WidgetArray& SourceView::glyphs_to_be_updated()
{
    change_glyphs = false;
    update_glyphs_now();
    change_glyphs = true;

    // clog << "Glyphs to be updated:";
    // for (int i = 0; i < changed_glyphs.size(); i++)
    //     clog << " " << XtName(changed_glyphs[i]);
    // clog << "\n";

    return changed_glyphs;
}


// Change setting of display_glyphs
void SourceView::set_display_glyphs(bool set)
{
    if (display_glyphs != set)
    {
	if (XtIsRealized(source_text_w))
	{
	    display_glyphs = false;	
	    show_execution_position();
	    UpdateGlyphsWorkProc(0, 0);

	    display_glyphs = true;
	    refresh_bp_disp();
	}

	display_glyphs = set;

	if (XtIsRealized(source_text_w))
	{
	    StatusDelay delay(set ? "Enabling glyphs" : "Disabling glyphs");

	    refresh_bp_disp();
	    lookup();
	}
    }
}

// Change setting of display_line_numbers
void SourceView::set_display_line_numbers(bool set)
{
    if (display_line_numbers != set)
    {
	display_line_numbers = set;

	if (XtIsRealized(source_text_w))
	{
	    StatusDelay delay(set ? "Enabling line numbers" : 
			      "Disabling line numbers");
	    reload();
	}
    }
}

// Return help on a glyph
MString SourceView::help_on_glyph(Widget w, bool detailed)
{
    XmTextPosition dummy;
    return help_on_pos(w, 0, dummy, detailed);
}

// Return help on a breakpoint position
MString SourceView::help_on_pos(Widget w, XmTextPosition pos, 
				XmTextPosition& ref, bool detailed)
{
    if (w == 0)
	return MString(0, true);

    int line_nr;
    bool in_text;
    int bp_nr;
    string address;
    bool pos_found = get_line_of_pos(w, pos, line_nr, address, in_text, bp_nr);

    if (!pos_found || bp_nr == 0)
	return MString(0, true);

    ref = pos_of_line(line_nr) + indent_amount(source_text_w) - 1;
    return help_on_bp(bp_nr, detailed);
}

// Return help on a glyph
MString SourceView::help_on_bp(int bp_nr, bool detailed)
{
    BreakPoint *bp = bp_map.get(bp_nr);
    if (bp == 0)
	return MString(0, true);

    MString info;
    switch (bp->type())
    {
    case BREAKPOINT:
	info = rm("Breakpoint ");
	break;
    case WATCHPOINT:
	info = rm("Watchpoint ");
	break;
    }
    info += tt(bp->number_str());

    if (detailed)
    {
	if (bp->enabled())
	    info += rm(" (enabled");
	else
	    info += rm(" (disabled");

	string infos = bp->infos();
	read_leading_blanks(infos);
	strip_final_blanks(infos);
	infos.gsub("\n", "; ");

	if (bp->infos() != "")
	    info += rm("; " + infos);
	info += rm(")");
    }

    return info;
}


// Glyph drag & drop

XmTextPosition SourceView::glyph_position(Widget w, XEvent *e, bool normalize)
{
    BoxPoint p = point(e);
    if (w != source_text_w && w != code_text_w)
    {
	// Called from a glyph: add glyph position to event position
	Position xw, yw;
	XtVaGetValues(w, 
		      XmNx, &xw,
		      XmNy, &yw,
		      NULL);
	p[X] += xw;
	p[Y] += yw;
    }

    // Get the position
    Widget text_w;
    if (is_source_widget(w))
	text_w = source_text_w;
    else if (is_code_widget(w))
	text_w = code_text_w;
    else
	return XmTextPosition(-1);

    XmTextPosition pos = XmTextXYToPos(text_w, p[X], p[Y]);

    // Stay within viewable text +/-1 row, such that we don't scroll too fast
    short rows = 0;
    XmTextPosition current_top = 0;
    XtVaGetValues(text_w,
		  XmNrows, &rows,
		  XmNtopCharacter, &current_top,
		  NULL);

    const string& text = current_text(text_w);
    XmTextPosition current_bottom = current_top;
    while (current_bottom < int(text.length()) && rows > 0)
	if (text[current_bottom++] == '\n')
	    rows--;

    if (pos < current_top)
	pos = max(current_top - 1, 0);
    else if (pos > current_bottom)
	pos = min(current_bottom + 1, text.length());

    if (normalize)
    {
	const string& text = current_text(w);
	pos = min(pos, text.length());
	while (pos > 0 && text[pos - 1] != '\n')
	    pos--;
    }

    return pos;
}

// Data associated with current drag operation
// The Glyph being dragged
Widget SourceView::current_drag_origin     = 0;

// The breakpoint being dragged, or 0 if execution position
int    SourceView::current_drag_breakpoint = -1;

void SourceView::dragGlyphAct (Widget w, XEvent *e, String *, Cardinal *)
{
    if (e->type != ButtonPress && e->type != ButtonRelease)
	return;

    Widget text_w;
    if (is_source_widget(w))
	text_w = source_text_w;
    else if (is_code_widget(w))
	text_w = code_text_w;
    else
	return;			// Bad widget

    int k;
    for (k = 0; k < 2; k++)
    {
	if (w == grey_arrows[k])
	{
	    set_status("Cannot drag last execution position");
	    return;
	}

	if (w == temp_stops[k] || w == temp_arrows[k])
	{
	    set_status("Cannot drag temp glyph");
	    return;
	}
    }

    static Cursor move_cursor = XCreateFontCursor(XtDisplay(w), XC_fleur);

    // clog << "Dragging " << XtName(w) << " [" << w << "]\n";

    XDefineCursor(XtDisplay(w), XtWindow(w), move_cursor);

    map_temp_stop_at(text_w, XmTextPosition(-1));
    map_temp_arrow_at(text_w, XmTextPosition(-1));

    current_drag_origin     = w;
    current_drag_breakpoint = 0;

    // Check for breakpoint
    MapRef ref;
    for (BreakPoint *bp = bp_map.first(ref); bp != 0; bp = bp_map.next(ref))
    {
	if (w == bp->source_glyph() || w == bp->code_glyph())
	{
	    current_drag_breakpoint = bp->number();
	    break;
	}
    }
}

void SourceView::followGlyphAct  (Widget w, XEvent *e, String *, Cardinal *)
{
    if (w != current_drag_origin)
	return;

    Widget text_w;
    if (is_source_widget(w))
	text_w = source_text_w;
    else if (is_code_widget(w))
	text_w = code_text_w;
    else
	return;			// Bad widget

    XmTextPosition pos = glyph_position(w, e);

    // Make sure we see the position
    ShowPosition(text_w, pos);

    // Update glyphs in case we had to scroll
    CheckScrollCB(w, XtPointer(0), XtPointer(0));

    if (current_drag_breakpoint)
	map_temp_stop_at(text_w, pos, w);
    else
	map_temp_arrow_at(text_w, pos, w);
}

void SourceView::dropGlyphAct (Widget w, XEvent *e, String *, Cardinal *)
{
    if (e->type != ButtonPress && e->type != ButtonRelease)
	return;

    if (w != current_drag_origin)
	return;

    Widget text_w;
    if (is_source_widget(w))
	text_w = source_text_w;
    else if (is_code_widget(w))
	text_w = code_text_w;
    else
	return;			// Bad widget

    XUndefineCursor(XtDisplay(w), XtWindow(w));

    // Unmap temp glyph
    map_temp_stop_at(text_w, XmTextPosition(-1));
    map_temp_arrow_at(text_w, XmTextPosition(-1));

    // Show all other glyphs
    update_glyphs();

    int k;
    for (k = 0; k < 2; k++)
	if (w == grey_arrows[k] || w == temp_stops[k] || w == temp_arrows[k])
	    return;

    XmTextPosition pos = glyph_position(w, e);
    if (pos == XmTextPosition(-1))
	return;			// No position

    int line_nr = 0;
    bool in_text;
    int bp_nr;
    string address;
    if (!get_line_of_pos(text_w, pos, line_nr, address, in_text, bp_nr))
	return;			// No location

    if (text_w == code_text_w)
    {
	// Selection from code
	if (address == "")
	    return;		// No address
    }
    else
    {
	// Selection from source
	if (line_nr == 0)
	    return;		// No line
	address = current_source_name() + ':' + itostring(line_nr);
    }

    // clog << "Dropping " << XtName(w) << " [" << w << "] at " 
    //      << address << "\n";

    if (text_w == code_text_w)
    {
	// Selection from code
	if (address == "")
	    return;		// No address
	address = string('*') + address;
    }
    else
    {
	// Selection from source
	if (line_nr == 0)
	    return;		// No line
	address = current_source_name() + ':' + itostring(line_nr);
    }

    if (current_drag_breakpoint)
    {
	// Move breakpoint
	move_bp(current_drag_breakpoint, address, text_w);
    }
    else
    {
	// Move exec pos
	line_popup_set_pcCB(text_w, XtPointer(&address), XtPointer(0));
    }

    // Make sure this position is kept visible
    SetInsertionPosition(text_w, pos);

    current_drag_origin     = 0;
    current_drag_breakpoint = 0;
}



//----------------------------------------------------------------------------
// Machine code stuff
//----------------------------------------------------------------------------

// Clear the code cache
void SourceView::clear_code_cache()
{
    static CodeCache empty;
    code_cache = empty;
    process_disassemble("No code.");
}

#if RUNTIME_REGEX
static regex rxnladdress("\n *" RXADDRESS);
#endif

static string first_address(const string& s)
{
    int idx = index(s, rxnladdress, "\n");
    if (idx < 0)
	return "";
    idx++;

    int eol = s.index('\n', idx);
    if (eol < 0)
	eol = s.length();

    string addr = ((string&)s).at(idx, eol - idx);
    return addr.through(rxaddress);
}

static string last_address(const string& s)
{
    int idx = index(s, rxnladdress, "\n", -1);
    if (idx < 0)
	return "";
    idx++;

    int eol = s.index('\n', idx);
    if (eol < 0)
	eol = s.length();

    string addr = ((string&)s).at(idx, eol - idx);
    return addr.through(rxaddress);
}

void SourceView::set_code(const string& code,
			  const string& start,
			  const string& end)
{
    XmTextSetString(code_text_w, (String)code);
    XmTextSetHighlight (code_text_w, 0, code.length(), XmHIGHLIGHT_NORMAL);
    
    current_code       = code;
    current_code_start = start;
    current_code_end   = end;

    last_pos_pc             = 0;
    last_start_highlight_pc = 0;
    last_end_highlight_pc   = 0;
}

// Process output of `disassemble' command
void SourceView::process_disassemble(const string& disassemble_output)
{
    int count             = ((string&)disassemble_output).freq('\n') + 1;
    string *code_list     = new string[count];

    split((string &)disassemble_output, code_list, count, '\n');

    string indented_code;
    for (int i = 0; i < count; i++)
    {
	string& line = code_list[i];
	untabify(line);
	if (line.length() > 0 && line[0] == '0')
	    line = replicate(' ', code_indent_amount) + line;
	indented_code += line + '\n';
    }

    set_code(indented_code,
	     first_address(disassemble_output),
	     last_address(disassemble_output));

    if (cache_machine_code
	&& current_code_start != ""
	&& current_code_end   != "")
	code_cache += CodeCacheEntry(current_code_start, 
				     current_code_end, 
				     current_code);
}

// Search PC in the current code; return beginning of line if found
XmTextPosition SourceView::find_pc(const string& pc)
{
    if (compare_address(pc, current_code_start) < 0
	|| compare_address(pc, current_code_end) > 0)
	return XmTextPosition(-1);

    XmTextPosition pos = XmTextPosition(-1);

    int i = 0;
    while (i < int(current_code.length()))
    {
	int eol = current_code.index('\n', i);
	if (eol < 0)
	    break;

	int j = i;
	while (j < int(current_code.length()) && isspace(current_code[j]))
	    j++;

	if (j + 2 < int(current_code.length())
	    && (is_address_start(current_code[j])))
	{
	    // Use first word of line as address.  Much faster than
	    // checking address regexps.
	    string address = current_code.at(j, eol - j);
	    int k = 0;
	    while (k < int(address.length()) && !isspace(address[k]))
		k++;
	    address = address.before(k);
	    if (compare_address(pc, address) == 0)
	    {
		pos = i;
		break;
	    }
	}

	i = eol + 1;
    }

    return pos;
}


struct RefreshInfo {
    string pc;
    XmHighlightMode mode;
    Delay *delay;
};

// Process `disassemble' output
void SourceView::refresh_codeOQC(const string& answer, void *client_data)
{
    RefreshInfo *info = (RefreshInfo *)client_data;

    if (answer != NO_GDB_ANSWER)
    {
	process_disassemble(answer);

	if (find_pc(info->pc) != XmTextPosition(-1))
	    show_pc(info->pc, info->mode);
    }

    delete info->delay;
    delete info;
}

// Show program counter location PC
// If MODE is given, highlight PC line.
// STOPPED indicates that the program just stopped.
// SIGNALED indicates that the program just received a signal.
void SourceView::show_pc(const string& pc, XmHighlightMode mode,
			 bool stopped, bool signaled)
{
    last_shown_pc = pc;
    if (mode == XmHIGHLIGHT_SELECTED)
	last_execution_pc = pc;

    if (stopped)
    {
	at_lowest_frame = true;
	signal_received = signaled;
    }

    if (!disassemble)
	return;

	
    // clog << "Showing PC " << pc << "\n";

    XmTextPosition pos = find_pc(pc);

    // While PC not found, look for code in cache
    for (int i = 0; 
	 pos == XmTextPosition(-1) && i < code_cache.size(); 
	 i++)
    {
	const CodeCacheEntry& cce = code_cache[i];
	if (compare_address(pc, cce.start) >= 0 
	    && compare_address(pc, cce.end) <= 0)
	{
	    set_code(cce.code, cce.start, cce.end);
	    pos = find_pc(pc);
	}
    }

    if (pos == XmTextPosition(-1))
    {
	// PC not found in current code: disassemble location
	RefreshInfo *info = new RefreshInfo;
	info->pc    = pc;
	info->mode  = mode;
	info->delay = new StatusDelay("Disassembling location " + pc);

	gdb_command(gdb->disassemble_command(info->pc), 0,
		    refresh_codeOQC, (void *)info);
	return;
    }

    if (pos == XmTextPosition(-1))
	return;

    SetInsertionPosition(code_text_w, pos + code_indent_amount);
    add_to_history(pc);

    XmTextPosition pos_line_end = 0;
    if (current_code != "")
	pos_line_end = current_code.index('\n', pos) + 1;

    // Clear old selections
    if (last_start_secondary_highlight_pc)
    {
	XmTextSetHighlight (code_text_w,
			    last_start_secondary_highlight_pc, 
			    last_end_secondary_highlight_pc,
			    XmHIGHLIGHT_NORMAL);
	last_start_secondary_highlight_pc = 0;
	last_end_secondary_highlight_pc   = 0;
    }

    if (last_start_highlight_pc)
    {
	XmTextSetHighlight (code_text_w,
			    last_start_highlight_pc, 
			    last_end_highlight_pc,
			    XmHIGHLIGHT_NORMAL);
	last_start_highlight_pc = 0;
	last_end_highlight_pc   = 0;
    }

    // Mark current line
    if (mode == XmHIGHLIGHT_SELECTED)
    {
	if (!display_glyphs)
	{
	    // Set new marker
	    if (last_pos_pc)
		XmTextReplace (code_text_w,
			       last_pos_pc + code_indent_amount - 1,
			       last_pos_pc + code_indent_amount,
			       " ");
	    XmTextReplace (code_text_w,
			   pos + code_indent_amount - 1,
			   pos + code_indent_amount,
			   ">");
    
	    if (pos_line_end)
	    {
		XmTextSetHighlight (code_text_w,
				    pos, pos_line_end,
				    XmHIGHLIGHT_SELECTED);

		last_start_highlight_pc = pos;
		last_end_highlight_pc   = pos_line_end;
	    }

	    last_pos_pc = pos;
	}
    }
    else if (mode == XmHIGHLIGHT_SECONDARY_SELECTED)
    {
	if (pos_line_end)
	{
	    XmTextSetHighlight (code_text_w,
				pos, pos_line_end,
				XmHIGHLIGHT_SECONDARY_SELECTED);

	    last_start_secondary_highlight_pc = pos;
	    last_end_secondary_highlight_pc   = pos_line_end;
	}
    }

    if (mode == XmHIGHLIGHT_SELECTED)
	update_glyphs(code_text_w);
}

void SourceView::set_disassemble(bool set)
{
    if (disassemble != set)
    {
	disassemble = set;

	if (!disassemble)
	{
	    XtUnmanageChild(code_form_w);
	}
	else
	{
	    XtManageChild(code_form_w);
	    if (last_execution_pc != "")
		show_pc(last_execution_pc, XmHIGHLIGHT_SELECTED);
	    else if (last_shown_pc != "")
		show_pc(last_shown_pc);
	    else
		lookup(line_of_cursor());
	}
    }
}

void SourceView::set_all_registers(bool set)
{
    if (all_registers != set)
    {
	all_registers = set;

	if (all_registers_w)
	    XmToggleButtonSetState(all_registers_w, (Boolean)set, False);
	if (int_registers_w)
	    XmToggleButtonSetState(int_registers_w, !(Boolean)set, False);

	refresh_registers();
    }
}


// Return `clear ARG' command.  If CLEAR_NEXT is set, attempt to guess
// the next event number and clear this one as well.  (This is useful
// for setting temporary breakpoints, as `delete' must also clear the
// event handler we're about to install.)  Consider only breakpoints
// whose number is >= FIRST_BP.
string SourceView::clear_command(string pos, bool clear_next, int first_bp)
{
    string file = current_file_name;
    string line = pos;

    if (gdb->type() == DBX && !pos.contains(':') && !pos.matches(rxint))
	pos = dbx_lookup(pos);

    if (pos.contains(':'))
    {
	file = pos.before(':');
	line = pos.after(':');
    }

    int line_no = atoi(line);

    if (!clear_next && gdb->has_clear_command())
    {
	switch (gdb->type())
	{
	case GDB:
	    return "clear " + pos;

	case DBX:
	    if (line_no > 0 && file_matches(file, current_file_name))
		return "clear " + line;
	    break;

	default:
	    break;
	}
    }

    int max_bp_nr = -1;
    string bps = "";
    MapRef ref;
    for (BreakPoint* bp = bp_map.first(ref);
	 bp != 0;
	 bp = bp_map.next(ref))
    {
	if (bp->number() >= first_bp
	    && bp->line_nr() == line_no
	    && (bp->file_name() == "" 
		|| file_matches(bp->file_name(), file)))
	    {
		if (bps != "")
		    bps += gdb->has_delete_comma() ? ", " : " ";
		bps += itostring(bp->number());
		max_bp_nr = max(max_bp_nr, bp->number());
	    }
    }

    if (bps == "")
	return "";

    if (clear_next && max_bp_nr >= 0)
    {
	bps += (gdb->has_delete_comma() ? ", " : " ");
	bps += itostring(max_bp_nr + 1);
    }

    return gdb->delete_command(bps);
}

// Some DBXes require `{ COMMAND; }', others `{ COMMAND }'.
string SourceView::command_list(string cmd)
{
    if (gdb->has_when_semicolon())
	return "{ " + cmd + "; }";
    else
	return "{ " + cmd + " }";
}

// Get the position of breakpoint NUM
string SourceView::bp_pos(int num)
{
    BreakPoint *bp = bp_map.get(num);
    if (bp == 0)
	return "";
    else
	return bp->file_name() + ":" + itostring(bp->line_nr());
}


// True iff we have some selection
bool SourceView::have_selection()
{
    XmTextPosition left, right;

    return (XmTextGetSelectionPosition(source_text_w, &left, &right)
	    || XmTextGetSelectionPosition(code_text_w, &left, &right)) 
	&& left != right;
}



//----------------------------------------------------------------------------
// Session stuff
//----------------------------------------------------------------------------

int SourceView::max_breakpoint_number = 99;

// Return DDD commands to restore current state (breakpoints, etc.)
bool SourceView::get_state(ostream& os)
{
    IntArray breakpoint_nrs;
    bool ok = true;

    // Restore breakpoints
    MapRef ref;
    for (BreakPoint *bp = bp_map.first(ref); bp != 0; bp = bp_map.next(ref))
	breakpoint_nrs += bp->number();

    if (breakpoint_nrs.size() > 0)
    {
	sort(breakpoint_nrs);

	// If all breakpoint numbers are less than
	// MAX_BREAKPOINT_NUMBER, insert `dummy' breakpoints that are
	// immediately deleted after creation, such that the
	// breakpoint numbers are preserved.  Otherwise, begin
	// numbering with 1.

	int max_number = breakpoint_nrs[breakpoint_nrs.size() - 1];
	bool restore_old_numbers = max_number < max_breakpoint_number;

	int num = 1;
	for (int i = 0; i < breakpoint_nrs.size(); i++)
	{
	    BreakPoint *bp = bp_map.get(breakpoint_nrs[i]);
	    if (restore_old_numbers)
	    {
		while (num < breakpoint_nrs[i])
		    ok &= bp->get_state(os, num++, true);
		assert(num == breakpoint_nrs[i]);
	    }
	    ok &= bp->get_state(os, num++);
	}
    }

    // Restore current cursor position
    switch (gdb->type())
    {
    case GDB:
	os << "info line " << line_of_cursor() << '\n';
	break;

    case DBX:
	break;			// FIXME

    case XDB:
	os << "v " << line_of_cursor() << '\n';
	break;
    }

    return ok;
}

void SourceView::reset_done(const string& answer, void *)
{
    (void) answer;

    // Breakpoints should be deleted now -- clear all other information
    clear_file_cache();
    clear_code_cache();
    clear_dbx_lookup_cache();
    current_file_name = "";
}

void SourceView::reset()
{
    string del = gdb->delete_command();

    // Delete all breakpoints
    MapRef ref;
    int n = 0;
    for (BreakPoint *bp = bp_map.first(ref); bp != 0; bp = bp_map.next(ref))
    {
	n++;
	del += " " + itostring(bp->number());
    }

    if (n > 0)
    {
	Command c(del);
	c.verbose  = false;
	c.check    = true;
	c.priority = COMMAND_PRIORITY_INIT;
	c.callback = reset_done;
	gdb_command(c);
    }
}
