// $Id$
// Use the Source, Luke.

// Copyright (C) 1995-1998 Technische Universitaet Braunschweig, Germany.
// Written by Dorothea Luetkehaus <luetke@ips.cs.tu-bs.de>
// and Andreas Zeller <zeller@ips.cs.tu-bs.de>.
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
#include "cwd.h"
#include "HelpCB.h"
#include "DestroyCB.h"
#include "charsets.h"
#include "events.h"
#include "cook.h"
#include "filetype.h"
#include "misc.h"
#include "shorten.h"
#include "tabs.h"
#include "wm.h"
#include "TimeOut.h"
#include "logo.h"

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

#if XmVersion >= 2000
#include <Xm/SpinB.h>
#ifndef XmIsSpinBox
#define XmIsSpinBox(w) XtIsSubclass((w), xmSpinBoxWidgetClass)
#endif
#endif

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
#include "AppData.h"
#include "Command.h"
#include "MakeMenu.h"
#include "PosBuffer.h"
#include "charsets.h"
#include "dbx-lookup.h"
#include "ddd.h"
#include "file.h"
#include "fortranize.h"
#include "index.h"
#include "java.h"
#include "mydialogs.h"
#include "options.h"
#include "post.h"
#include "question.h"
#include "regexps.h"
#include "shell.h"
#include "status.h"
#include "string-fun.h"
#include "verify.h"
#include "version.h"
#include "windows.h"
#include "wm.h"




// Additional macros
inline int isid(char c)
{
    return isalnum(c) || c == '_' || c == '$';
}

// Test for regular file - see stat(3)
#ifndef S_ISREG
#define S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#endif


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

struct BPItms { enum Itms {Properties, Disable, Delete, Sep, SetPC}; };
MMDesc SourceView::bp_popup[] =
{
    {"properties",   MMPush, {SourceView::EditBreakpointPropertiesCB}},
    {"disable",      MMPush, {SourceView::bp_popup_disableCB}},
    {"delete",       MMPush, {SourceView::bp_popup_deleteCB}},
    MMSep,
    {"set_pc",       MMPush, {SourceView::bp_popup_set_pcCB}},
    MMEnd
};

struct BPButtons { enum Itms {NewBP, NewWP, Properties, Lookup, 
			      Enable, Disable, Delete}; };
MMDesc SourceView::bp_area[] =
{
    {"new_bp",       MMPush,   {SourceView::NewBreakpointCB}},
    {"new_wp",       MMPush,   {SourceView::NewWatchpointCB}},
    {"properties",   MMPush,   {SourceView::EditBreakpointPropertiesCB, 0}},
    {"lookup",       MMPush,   
     {SourceView::LookupBreakpointCB, XtPointer(0) }},
    {"enable",       MMPush,   {SourceView::BreakpointCmdCB, "enable"  }},
    {"disable",      MMPush,   {SourceView::BreakpointCmdCB, "disable" }},
    {"delete",       MMPush,   {SourceView::BreakpointCmdCB, "delete" }},
    MMEnd
};


struct TextItms { 
    enum Itms {
	Print, 
	Disp, 
	Watch,
	Dummy1, 
	PrintRef, 
	DispRef,
	WatchRef,
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
    "Watch ",
    "",
    "Print ", 
    "Display ", 
    "Watch ",
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
    {"watch",      MMPush | MMUnmanaged, {SourceView::text_popup_watchCB}},
    MMSep,
    {"printRef",   MMPush, {SourceView::text_popup_print_refCB}},
    {"dispRef",    MMPush, {SourceView::text_popup_disp_refCB}},
    {"watchRef",   MMPush | MMUnmanaged, {SourceView::text_popup_watch_refCB}},
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

#include "icons/glyphs/arrow.xbm"
static XImage arrow_image = {
    arrow_width,		// width
    arrow_height,		// height
    0,				// xoffset
    XYBitmap,			// format
    (char *)arrow_bits,		// data
    MSBFirst,			// byte_order
    8,				// bitmap_unit
    LSBFirst,			// bitmap_bit_order
    8,				// bitmap_pad
    1,				// depth
    2				// bytes_per_line
};

#include "icons/glyphs/greyarrow.xbm"
static XImage grey_arrow_image = {
    grey_arrow_width,		// width
    grey_arrow_height,		// height
    0,				// xoffset
    XYBitmap,			// format
    (char *)grey_arrow_bits,	// data
    MSBFirst,			// byte_order
    8,				// bitmap_unit
    LSBFirst,			// bitmap_bit_order
    8,				// bitmap_pad
    1,				// depth
    2				// bytes_per_line
};

#include "icons/glyphs/signalarrow.xbm"
static XImage signal_arrow_image = {
    signal_arrow_width,		// width
    signal_arrow_height,	// height
    0,				// xoffset
    XYBitmap,			// format
    (char *)signal_arrow_bits,	// data
    MSBFirst,			// byte_order
    8,				// bitmap_unit
    LSBFirst,			// bitmap_bit_order
    8,				// bitmap_pad
    1,				// depth
    2				// bytes_per_line
};

#include "icons/glyphs/temparrow.xbm"
static XImage temp_arrow_image = {
    temp_arrow_width,		// width
    temp_arrow_height,		// height
    0,				// xoffset
    XYBitmap,			// format
    (char *)temp_arrow_bits,	// data
    MSBFirst,			// byte_order
    8,				// bitmap_unit
    LSBFirst,			// bitmap_bit_order
    8,				// bitmap_pad
    1,				// depth
    2				// bytes_per_line
};

#include "icons/glyphs/stop.xbm"
static XImage stop_image = {
    stop_width,			// width
    stop_height,		// height
    0,				// xoffset
    XYBitmap,			// format
    (char *)stop_bits,		// data
    MSBFirst,			// byte_order
    8,				// bitmap_unit
    LSBFirst,			// bitmap_bit_order
    8,				// bitmap_pad
    1,				// depth
    2				// bytes_per_line
};

#include "icons/glyphs/greystop.xbm"
static XImage grey_stop_image = {
    grey_stop_width,		// width
    grey_stop_height,		// height
    0,				// xoffset
    XYBitmap,			// format
    (char *)grey_stop_bits,	// data
    MSBFirst,			// byte_order
    8,				// bitmap_unit
    LSBFirst,			// bitmap_bit_order
    8,				// bitmap_pad
    1,				// depth
    2				// bytes_per_line
};

#include "icons/glyphs/tempstop.xbm"
static XImage temp_stop_image = {
    temp_stop_width,		// width
    temp_stop_height,		// height
    0,				// xoffset
    XYBitmap,			// format
    (char *)temp_stop_bits,	// data
    MSBFirst,			// byte_order
    8,				// bitmap_unit
    LSBFirst,			// bitmap_bit_order
    8,				// bitmap_pad
    1,				// depth
    2				// bytes_per_line
};

#include "icons/glyphs/cond.xbm"
static XImage cond_image = {
    cond_width,			// width
    cond_height,		// height
    0,				// xoffset
    XYBitmap,			// format
    (char *)cond_bits,		// data
    MSBFirst,			// byte_order
    8,				// bitmap_unit
    LSBFirst,			// bitmap_bit_order
    8,				// bitmap_pad
    1,				// depth
    2				// bytes_per_line
};

#include "icons/glyphs/greycond.xbm"
static XImage grey_cond_image = {
    grey_cond_width,		// width
    grey_cond_height,		// height
    0,				// xoffset
    XYBitmap,			// format
    (char *)grey_cond_bits,	// data
    MSBFirst,			// byte_order
    8,				// bitmap_unit
    LSBFirst,			// bitmap_bit_order
    8,				// bitmap_pad
    1,				// depth
    2				// bytes_per_line
};

#include "icons/glyphs/tempcond.xbm"
static XImage temp_cond_image = {
    temp_cond_width,		// width
    temp_cond_height,		// height
    0,				// xoffset
    XYBitmap,			// format
    (char *)temp_cond_bits,	// data
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
StringStringAssoc SourceView::file_name_cache;
CodeCache SourceView::code_cache;

string SourceView::current_source;
string SourceView::current_code;
string SourceView::current_code_start;
string SourceView::current_code_end;

string SourceView::current_pwd        = cwd();
string SourceView::current_class_path = ".";

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

int SourceView::indent_amount(Widget w, int pos)
{
    assert(is_source_widget(w) || is_code_widget(w));

    int indent = 0;
    if (is_code_widget(w))
	indent = code_indent_amount;
    else if (display_line_numbers)
	indent = source_indent_amount + line_indent_amount;
    else
	indent = source_indent_amount;

    if (pos >= 0)
    {
	const string& text = current_text(w);
	while (pos < int(text.length()) && text[pos] == ' ')
	{
	    pos++;
	    indent++;
	}
    }

    return indent;
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

	case JDB:
	{
	    if (address.contains(":"))
		gdb_command("stop at " + address);
	    else
		gdb_command("stop in " + address);
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
	delete_bp(bp_nr);
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
    case JDB:
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

	case JDB:
	    break;		// Unsupported
	}

	// Jump to the new address and clear the breakpoint again
	last_jump_address = a;
	Command c(gdb->jump_command(address), w);
	c.callback = clearJumpBP;
	c.data     = XtPointer(old_max_breakpoint_number_seen);
	gdb_command(c);
    }
    else if (gdb->has_assign_command())
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

void SourceView::move_bp(int bp_nr, const string& a, Widget w, bool copy)
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

	if (bp_matches(bp, file, line))
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

    if (!copy)
    {
	// Delete old breakpoint
	delete_bp(bp_nr);
    }
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
    delete_bp(bp_nr);
}


// ***************************************************************************
//
void SourceView::bp_popup_deleteCB (Widget w,
				    XtPointer client_data,
				    XtPointer)
{
    int bp_nr = *((int *)client_data);
    delete_bp(bp_nr, w);
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

// Convert NRS to a list of numbers
string SourceView::numbers(IntArray& nrs)
{
    if (all_bps(nrs))
	return "";		// In GDB, no arg means `all'

    string cmd = ""; 
    for (int i = 0; i < nrs.size(); i++)
    {
	if (i > 0)
	    cmd += " ";
	cmd += itostring(nrs[i]);
    }
    return cmd;
}

// Return true if NRS contains all breakpoints and
// a GDB delete/disable/enable command can be given without args.
bool SourceView::all_bps(IntArray& nrs)
{
    if (gdb->type() != GDB || nrs.size() < 2)
	return false;

    IntArray work(nrs);

    MapRef ref;
    BreakPoint *bp = 0;
    for (bp = bp_map.first(ref); bp != 0; bp = bp_map.next(ref))
    {
	bool found = false;
	for (int i = 0; !found && i < work.size(); i++)
	{
	    if (bp->number() == work[i])
		found = true;
	}

	if (!found)
	    return false;
    }

    return true;
}

void SourceView::enable_bps(IntArray& nrs, Widget w)
{
    if (gdb->has_enable_command())
	gdb_command(gdb->enable_command(numbers(nrs)), w);
}

void SourceView::disable_bps(IntArray& nrs, Widget w)
{
    if (gdb->has_disable_command())
	gdb_command(gdb->disable_command(numbers(nrs)), w);
}

void SourceView::delete_bps(IntArray& nrs, Widget w)
{
    if (gdb->recording() && gdb->has_clear_command())
    {
	// While recording, prefer commands without explicit numbers.
        for (int i = 0; i < nrs.size(); i++)
	{
	    BreakPoint *bp = bp_map.get(nrs[i]);
	    if (bp != 0)
		gdb_command(clear_command(bp->pos()));
	}
    }
    else if (gdb->has_delete_command())
    {
        for (int i = 0; i < nrs.size(); i++)
	    gdb_command(delete_command(nrs[i]));
    }
    else
    {
	gdb_command(gdb->delete_command(numbers(nrs)), w);
    }
}

string SourceView::delete_command(int bp_nr)
{
    if (gdb->has_delete_command())
    {
	return gdb->delete_command(itostring(bp_nr));
    }
    else if (gdb->has_clear_command())
    {
	BreakPoint *bp = bp_map.get(bp_nr);
	if (bp != 0)
	    return clear_command(bp->pos());
    }

    return "";			// No way to delete a breakpoint (*sigh*)
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
    create_bp(fortranize(*word_ptr, true), w);
}

void SourceView::text_popup_clearCB (Widget w, 
				     XtPointer client_data, 
				     XtPointer)
{
    string* word_ptr = (string*)client_data;
    clear_bp(fortranize(*word_ptr, true), w);
}



// ***************************************************************************
//
void SourceView::text_popup_printCB (Widget w, 
				     XtPointer client_data, 
				     XtPointer)
{
    string* word_ptr = (string*)client_data;
    assert(word_ptr->length() > 0);

    gdb_command(gdb->print_command(fortranize(*word_ptr), false), w);
}

void SourceView::text_popup_print_refCB (Widget w, 
					 XtPointer client_data, XtPointer)
{
    string* word_ptr = (string*)client_data;
    assert(word_ptr->length() > 0);

    gdb_command(gdb->print_command(gdb->dereferenced_expr(
	fortranize(*word_ptr)), false), w);
}


// ***************************************************************************
//
void SourceView::text_popup_watchCB (Widget w, 
				     XtPointer client_data, 
				     XtPointer)
{
    string* word_ptr = (string*)client_data;
    assert(word_ptr->length() > 0);

    gdb_command(gdb->watch_command(fortranize(*word_ptr)), w);
}

void SourceView::text_popup_watch_refCB (Widget w, 
					 XtPointer client_data, XtPointer)
{
    string* word_ptr = (string*)client_data;
    assert(word_ptr->length() > 0);

    gdb_command(gdb->watch_command(gdb->dereferenced_expr(
	fortranize(*word_ptr))), w);
}


// ***************************************************************************
//
void SourceView::text_popup_dispCB (Widget w, XtPointer client_data, XtPointer)
{
    string* word_ptr = (string*)client_data;
    assert(word_ptr->length() > 0);

    gdb_command("graph display " + fortranize(*word_ptr), w);
}

void SourceView::text_popup_disp_refCB (Widget w, 
					XtPointer client_data, XtPointer)
{
    string* word_ptr = (string*)client_data;
    assert(word_ptr->length() > 0);

    gdb_command("graph display " + 
		gdb->dereferenced_expr(fortranize(*word_ptr)), w);
}

// ***************************************************************************
//
void SourceView::text_popup_whatisCB (Widget w, XtPointer client_data, 
				      XtPointer)
{
    string* word_ptr = (string*)client_data;
    assert(word_ptr->length() > 0);

    gdb_command(gdb->whatis_command(fortranize(*word_ptr)), w);
}

// ***************************************************************************
//
void SourceView::text_popup_lookupCB (Widget, XtPointer client_data, XtPointer)
{
    string* word_ptr = (string*)client_data;
    lookup(fortranize(*word_ptr, true));
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

    file += '/';

    if (!file.contains('/', 0))
        file = current_pwd + "/" + file;

    /* CvE, Jul 10, 1997
     *
     * Repeatedly remove patterns like /dir1/../ from the file name.
     * Note that a number of /../ patterns may follow each other, like
     * in "/dir1/dir1/dir3/../../../"
     */
    unsigned int file_length = file.length();
    unsigned int prev_file_length;
    do {
        prev_file_length = file_length;
        file.gsub(rxdotdot, "/");
        file_length = file.length();
    } while (file_length != prev_file_length);

    /* CvE, Jul 10, 1997
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
        file_length = file.length();
    } while (file_length != prev_file_length);

    file.gsub("//", "/");

    if (file.contains('/', -1))
	file = file.before(int(file.length() - 1));

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
    if (gdb->type() == JDB)
	return file1 == file2;

    if (gdb->type() == GDB || app_data.use_source_path)
	return file1 == file2 || full_path(file1) == full_path(file2);

    return base_matches(file1, file2);
}

bool SourceView::is_current_file(const string& file)
{
    if (gdb->type() == JDB)
	return file == current_source_name();
    else
	return file_matches(file, current_file_name);
}

bool SourceView::base_matches(const string& file1, const string& file2)
{
    return string(basename(file1.chars())) == string(basename(file2.chars()));
}

// Check if BP occurs in the current source text
bool SourceView::bp_matches(BreakPoint *bp, int line)
{
    return bp_matches(bp, current_source_name(), line) || 
	bp_matches(bp, current_file_name, line);
}

bool SourceView::bp_matches(BreakPoint *bp, const string& file, int line)
{
    return bp->type() == BREAKPOINT && 
	(line == 0 || bp->line_nr() == line) &&
	(bp->file_name() == "" || file_matches(bp->file_name(), file));
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

    if (!have_selection || (app_data.source_editing && startPos == endPos))
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

    if (!have_selection && call_data == 0)
    {
	// Still no selection?  We're probably called from setSelection();
	// use the last selected word instead.
	startPos = selection_startpos;
	endPos   = selection_endpos;
	have_selection = True;
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
		    bp->selected() = bp_matches(bp, line_nr);
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

BreakPoint *SourceView::breakpoint_at(string arg)
{
    MapRef ref;
    for (BreakPoint* bp = bp_map.first(ref); bp != 0; bp = bp_map.next(ref))
    {
	if (bp->type() != BREAKPOINT)
	    continue;

	if (arg.matches(rxint))
	{
	    // Line number for current source given
	    if (bp_matches(bp, atoi(arg)))
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

		if (bp_matches(bp, file, atoi(line)))
		    return bp;
	    }
	}
    }

    return 0;
}

BreakPoint *SourceView::watchpoint_at(string expr)
{
    for (int trial = 0; trial <= 2; trial++)
    {
	MapRef ref;
	for (BreakPoint* bp = bp_map.first(ref); bp != 0; 
	     bp = bp_map.next(ref))
	{
	    if (bp->type() != WATCHPOINT)
		continue;

	    switch (trial)
	    {
	    case 0:
		if (bp->expr() == expr)
		{
		    // Expression matches exactly
		    return bp;
		}
		break;

	    case 1:
		if (bp->expr().contains('(') && bp->expr().before('(') == expr)
		{
		    // Expr matches EXPR(...)  (e.g. a qualified function name)
		    return bp;
		}

	    case 2:
		if (bp->expr().contains("`" + expr, -1) ||
		    bp->expr().contains("::" + expr, -1))
		{
		    // Expr matches ...`EXPR (a Sun DBX identifier)
		    // or ...::EXPR (an SGI DBX identifier)
		    return bp;
		}
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



//-----------------------------------------------------------------------
// Error handling
//-----------------------------------------------------------------------

StringArray SourceView::bad_files;
bool SourceView::new_bad_file(const string& file_name)
{
    for (int i = 0; i < bad_files.size(); i++)
	if (file_name == bad_files[i])
	    return false;
    bad_files += file_name;
    return true;
}

void SourceView::post_file_error(const string& file_name,
				 string text, String name,
				 Widget origin)
{
    if (new_bad_file(file_name))
	post_error(text, name, origin);
}

void SourceView::post_file_warning(const string& file_name,
				   string text, String name,
				   Widget origin)
{
    if (new_bad_file(file_name))
	post_warning(text, name, origin);
}




//-----------------------------------------------------------------------
// Read file
//-----------------------------------------------------------------------

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
	delay.outcome = strerror(errno);
	if (!silent)
	    post_file_error(file_name, 
			    file_name + ": " + delay.outcome, 
			    "source_file_error", source_text_w);
        return 0;
    }

    struct stat statb;
    if (fstat(fd, &statb) < 0)
    {
	delay.outcome = strerror(errno);
	if (!silent)
	    post_file_error(file_name,
			    file_name + ": " + delay.outcome, 
			    "source_file_error", source_text_w);
	return 0;
    }

    // Avoid loading from directory, socket, device, or otherwise.
    if (!S_ISREG(statb.st_mode))
    {
	delay.outcome = "not a regular file";
	if (!silent)
	    post_file_error(file_name,
			    file_name + ": " + delay.outcome, 
			    "source_file_error", source_text_w);
	return 0;
    }

    // Put the contents of the file in the Text widget by allocating
    // enough space for the entire file and reading the file into the
    // allocated space.
    char* text = XtMalloc(unsigned(statb.st_size + 1));
    if ((length = read(fd, text, statb.st_size)) != statb.st_size)
    {
	delay.outcome = "truncated";
	if (!silent)
	    post_file_error(file_name,
			    file_name + ": " + delay.outcome,
			    "source_trunc_error", source_text_w);
    }
    close(fd);

    text[statb.st_size] = '\0'; // be sure to null-terminate

    if (statb.st_size == 0)
    {
	delay.outcome = "empty file";
	if (!silent)
	    post_file_warning(file_name,
			      file_name + ": " + delay.outcome,
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
    {
	delay.outcome = "failed";
	return 0;
    }

    String text = XtMalloc(1);

    do {
	text = XtRealloc(text, length + BUFSIZ + 1);
	length += fread(text + length, sizeof(char), BUFSIZ, fp);
    } while (!feof(fp));

    text[length] = '\0';  // be sure to null-terminate

    if (length == 0)
    {
	if (!silent)
	    post_file_error(file_name,
			    "Cannot access remote file " + quote(file_name), 
			    "remote_file_error", source_text_w);
	delay.outcome = "failed";
    }

    return text;
}

// Read class CLASS_NAME
String SourceView::read_class(const string& class_name, 
			      string& file_name, SourceOrigin& origin,
			      long& length, bool silent)
{
    StatusDelay delay("Loading class " + quote(class_name));
    
    String text = 0;
    length = 0;

    file_name = java_class_file(class_name);

    if (file_name != "")
    {
	if (remote_gdb())
	    text = read_remote(file_name, length, true);
	else
	{
	    file_name = full_path(file_name);
	    text = read_local(file_name, length, true);
	}
    }

    if (text != 0 && length != 0)
    {
	// Save class name for further reference
	source_name_cache[file_name] = class_name;
	origin = remote_gdb() ? ORIGIN_REMOTE : ORIGIN_LOCAL;
	return text;
    }
    else
    {
	// Could not load class
	file_name = class_name;
	origin = ORIGIN_NONE;
	delay.outcome = "failed";
	if (!silent)
	    post_file_error(class_name,
			    "Cannot access class " + quote(class_name),
			    "class_error", source_text_w);

	return 0;
    }
}


// Read file FILE_NAME via the GDB `list' function
// Really slow, is guaranteed to work for source files.
String SourceView::read_from_gdb(const string& file_name, long& length, 
				 bool /* silent */)
{
    length = 0;
    if (!gdb->isReadyWithPrompt())
	return 0;
    if (gdb->type() == JDB)
	return 0;		// Won't work with JDB

    StatusDelay delay("Reading file " + quote(file_name) + 
		      " from " + gdb->title());

    string command;
    switch (gdb->type())
    {
    case GDB:
	command = "list " + file_name + ":1,1000000";
	break;

    case DBX:
	command = "list 1,1000000";
	break;

    case JDB:
	command = "list " + file_name;
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

	if (i < int(listing.length()) && isdigit(listing[i]))
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

    if (length == 0)
	delay.outcome = "failed";

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

    if (gdb->type() == JDB && !file_name.contains('/'))
    {
	// FILE_NAME is a class name.  Search class in JDB `use' path.
	text = read_class(file_name, full_file_name, origin, length, true);
    }

    if (text == 0 || length == 0)
    {
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
	if (gdb->type() == JDB)
	    text = read_class(file_name, full_file_name, origin, 
			      length, false);
	else if (!remote_gdb())
	    text = read_local(full_file_name, length, false);
	else
	    text = read_remote(full_file_name, length, false);
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
    string requested_file_name = file_name;

    if (cache_source_files && !force_reload && file_cache.has(file_name))
    {
	current_source = file_cache[file_name];
	current_origin = origin_cache[file_name];
	file_name      = file_name_cache[file_name];

	if (gdb->type() == JDB)
	{
	    // In JDB, a single source may contain multiple classes.
	    // Store current class name FILE_NAME as source name.
	    source_name_cache[file_name] = requested_file_name;
	}
    }
    else
    {
	long length = 0;
	SourceOrigin orig;
	String indented_text = read_indented(file_name, length, orig, silent);
	if (indented_text == 0 || length == 0)
	    return -1;		// Failure

	current_source = string(indented_text, length);
	current_origin = orig;
	XtFree(indented_text);

	if (current_source.length() > 0)
	{
	    file_cache[file_name]             = current_source;
	    origin_cache[file_name]           = current_origin;
	    file_name_cache[file_name]        = file_name;

	    if (file_name != requested_file_name)
	    {
		file_cache[requested_file_name]      = current_source;
		origin_cache[requested_file_name]    = current_origin;
		file_name_cache[requested_file_name] = file_name;
	    }
	}

	int null_count = current_source.freq('\0');
	if (null_count > 0 && !silent)
	    post_warning(file_name + ": binary file",
			 "source_binary_warning", source_text_w);
    }

    // Untabify current source, using the current tab width
    untabify_if_needed(current_source, tab_width, 
		       indent_amount(source_text_w));

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
    file_name_cache   = string_empty;

    static StringOriginAssoc origin_empty;
    origin_cache      = origin_empty;

    static StringArray bad_files_empty;
    bad_files         = bad_files_empty;
}

void SourceView::reload()
{
    // Reload current file
    if (current_file_name == "")
	return;

    string file;
    if (gdb->type() == JDB)
	file = line_of_cursor();
    else
	file = file_of_cursor();

    string line = file.after(':');
    file        = file.before(':');

    // StatusDelay delay("Reloading " + quote(file));

    read_file(file, atoi(line), true);
}

// Change tab width
void SourceView::set_tab_width(int width)
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

// Change indentation
void SourceView::set_indent(int source_indent, int code_indent)
{
    if (source_indent < 0 || code_indent < 0)
	return;

    if (source_indent == source_indent_amount &&
	code_indent == code_indent_amount)
	return;

    StatusDelay delay("Reformatting");

    if (source_indent != source_indent_amount)
    {
	source_indent_amount = source_indent;
	reload();
    }

    if (code_indent != code_indent_amount)
    {
	code_indent_amount = code_indent;

	clear_code_cache();
	show_pc(last_shown_pc);
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

    // Set current file name
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

    if (app_data.source_window)
    {
	static bool popped_up = false;

	if (!popped_up)
	{
	    // Make sure source is visible
	    if (source_view_shell != 0 || app_data.tty_mode)
	    {
		Widget shell = (source_view_shell != 0) ? 
		    source_view_shell : command_shell;
		initial_popup_shell(shell);
	    }

	    if (!app_data.command_toolbar)
		initial_popup_shell(tool_shell);

	    gdbOpenSourceWindowCB(source_text_w, 0, 0);

	    popped_up = true;
	}
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



//-----------------------------------------------------------------------
// Breakpoint handling
//-----------------------------------------------------------------------

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

	int pos = pos_of_line(line_nr);
	int indent = indent_amount(source_text_w, pos);

	if (indent > 0)
	{
	    string s(current_source.at(pos, indent - 1));

	    if (s.length() > 0)
		XmTextReplace(source_text_w,
			      pos_of_line(line_nr),
			      pos_of_line(line_nr) + s.length(),
			      (String)s);
	}
    }

    static IntIntArrayAssoc empty_bps;
    bps_in_line = empty_bps;

    if (display_glyphs)
	return;

    // Find all breakpoints referring to this file
    MapRef ref;
    for (BreakPoint* bp = bp_map.first(ref); bp != 0; bp = bp_map.next(ref))
    {
	if (bp_matches(bp))
	    bps_in_line[bp->line_nr()] += bp->number();
    }

    // Show breakpoints in text
    for (IntIntArrayAssocIter b_i_l_iter(bps_in_line);
	 b_i_l_iter.ok();
	 b_i_l_iter++)
    {
	int line_nr = b_i_l_iter.key();
	if (line_nr < 0 || line_nr > line_count)
	    continue;

	XmTextPosition pos = pos_of_line(line_nr);
	int indent = indent_amount(source_text_w, pos);

	if (indent > 0)
	{
	    // Display all breakpoints in a line
	    VarIntArray& bps = bps_in_line[line_nr];

	    string insert_string = "";
	    for (int i = 0; i < bps.size(); i++)
	    {
		BreakPoint *bp = bp_map.get(bps[i]);
		insert_string += bp->symbol();
	    }

	    if (int(insert_string.length()) >= indent - 1)
	    {
		insert_string = insert_string.before(indent - 1);
	    }
	    else
	    {
		for (int i = insert_string.length(); i < indent - 1; i++)
		{
		    insert_string += current_source[pos + i];
		}
	    }

	    assert(int(insert_string.length()) == indent - 1);

	    if (insert_string.length() > 0)
		XmTextReplace(source_text_w, pos, 
			      pos + indent - 1,
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
	int indent = indent_amount(code_text_w, pos);
	if (indent > 0)
	{
	    string spaces = replicate(' ', indent);
	    XmTextReplace(code_text_w, pos, pos + indent, (String)spaces);
	}
    }

    static StringArray empty;
    bp_addresses = empty;

    if (display_glyphs)
	return;

    // Collect all addresses
    MapRef ref;
    for (BreakPoint *bp = bp_map.first(ref); bp != 0;
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
		insert_string += bp->symbol();
	}

	int indent = indent_amount(code_text_w, pos);
	if (indent > 0)
	{
	    insert_string += replicate(' ', indent);
	    insert_string = insert_string.before(indent);

	    XmTextReplace(code_text_w, pos, pos + indent, 
			  (String)insert_string);
	}
    }
}


//-----------------------------------------------------------------------
// Position management
//-----------------------------------------------------------------------

// Find the line number at POS
// LINE_NR becomes the line number at POS
// IN_TEXT becomes true iff POS is in the source area
// BP_NR is the number of the breakpoint at POS (none: 0)
// Return false iff failure
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

	    if (pos == line_pos
		|| left_of_first_nonblank
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

	if (pos == line_pos || pos - line_pos < indent_amount(text_w))
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
// Find word around POS.  STARTPOS is the first character, ENDPOS + 1
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
    if (offset == 0 || offset < indent_amount(text_w))
    {
	// Do not select words in breakpoint area
	return;
    }

    // Find end of word
    while (endpos < XmTextPosition(text.length()) && isid(text[endpos]))
	endpos++;

    // Find start of word
    if (startpos >= XmTextPosition(text.length()))
	startpos = XmTextPosition(text.length() - 1);

    while (startpos > 0)
    {
	while (startpos > 0 && isid(text[startpos - 1]))
	    startpos--;

	if (startpos > 2 && 
	    isid(text[startpos - 2]) &&
	    text[startpos - 1] == '.')
	{
	    // Select A.B as a whole
	    startpos -= 1;
	}
	else if (startpos > 3 && 
		 isid(text[startpos - 3]) &&
		 text[startpos - 2] == '-' &&
		 text[startpos - 1] == '>')
	{
	    // Select A->B as a whole
	    startpos -= 2;
	}
	else
	    break;
    }
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

    strip_space(word);

    return word;
}



//----------------------------------------------------------------------------
// Create GDB requests and evaluate replies
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------------

static void InstallImage(XImage *image, const string& name)
{
    Boolean ok = XmInstallImage(image, name);
    if (!ok)
	cerr << "Could not install " << quote(name) << " bitmap\n";
}


SourceView::SourceView(Widget parent)
{
    XtAppContext app_context = XtWidgetToApplicationContext(parent);

    // Find application shell
    toplevel_w = parent;
    while (toplevel_w != 0 && !XtIsWMShell(toplevel_w))
	toplevel_w = XtParent(toplevel_w);

    // Install glyph images
    InstallImage(&arrow_image,        "plain_arrow");
    InstallImage(&grey_arrow_image,   "grey_arrow");
    InstallImage(&signal_arrow_image, "signal_arrow");
    InstallImage(&temp_arrow_image,   "temp_arrow");
    InstallImage(&stop_image,         "plain_stop");
    InstallImage(&cond_image,         "plain_cond");
    InstallImage(&grey_stop_image,    "grey_stop");
    InstallImage(&grey_cond_image,    "grey_cond");
    InstallImage(&temp_stop_image,    "temp_stop");
    InstallImage(&temp_cond_image,    "temp_cond");

    // Setup actions
    XtAppAddActions (app_context, actions, XtNumber (actions));

    // Create source code window
    create_text(parent, "source", app_data.source_editing,
		source_form_w, source_text_w);
    XtManageChild(source_form_w);

    // Create machine code window
    create_text(parent, "code", false, code_form_w, code_text_w);
    if (disassemble)
	XtManageChild(code_form_w);
}

void SourceView::create_shells()
{
    Widget parent = XtParent(source_form_w);
    XtAppContext app_context = XtWidgetToApplicationContext(parent);

    // Create breakpoint editor
    Arg args[10];
    Cardinal arg = 0;

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
    MMaddCallbacks(bp_area);
    MMaddHelpCallback(bp_area, ImmediateHelpCB);

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

    if (gdb->type() != JDB)
    {
	XtUnmanageChild(XmSelectionBoxGetChild(thread_dialog_w, 
					       XmDIALOG_OK_BUTTON));
	XtUnmanageChild(XmSelectionBoxGetChild(thread_dialog_w, 
					       XmDIALOG_APPLY_BUTTON));
    }

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
		  XmNcancelCallback, UnmanageThisCB, thread_dialog_w);
    XtAddCallback(thread_dialog_w,
		  XmNcancelCallback, ThreadDialogPoppedDownCB, 0);
    XtAddCallback(thread_dialog_w,
		  XmNokCallback,     ThreadCommandCB, "suspend");
    XtAddCallback(thread_dialog_w,
		  XmNapplyCallback,  ThreadCommandCB, "resume");
    XtAddCallback(thread_dialog_w,
		  XmNhelpCallback, ImmediateHelpCB, 0);

    // Create remaining glyphs in the background
    XtAppAddWorkProc (app_context, CreateGlyphsWorkProc, XtPointer(0));
}

// Check for modifications
void SourceView::CheckModificationCB(Widget, XtPointer client_data, 
				     XtPointer call_data)
{
    bool editable = bool(client_data);
    XmTextVerifyCallbackStruct *cbs = (XmTextVerifyCallbackStruct *)call_data;
    if (!editable && cbs != 0 && cbs->event != 0)
    {
	cbs->doit = False;
	return;
    }

    // Follow text modifications here... (FIXME)
}

// Create source or code window
void SourceView::create_text(Widget parent, const string& base, bool editable,
			     Widget& form, Widget& text)
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
    XtSetArg(args[arg], XmNcursorPositionVisible, True);          arg++;

    if (lesstif_version <= 82)
    {
	// LessTif 0.81 has a bad implementation of auto-show
	// position: rather than scrolling only when needed, the line
	// containing the cursor is *always* scrolled such that it
	// becomes the first line.  Hence, disable the LessTif
	// auto-show mechanism and rely on the DDD ones.
	XtSetArg(args[arg], XmNautoShowCursorPosition, False);    arg++;
    }
    else
    {
	XtSetArg(args[arg], XmNautoShowCursorPosition, True);     arg++;
    }

    if (lesstif_version <= 1000)
    {
	// LessTif has trouble with non-editable text windows: cursor
	// movement is inhibited.
	editable = true;
    }
    XtSetArg(args[arg], XmNeditable, editable); arg++;

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
		  CheckModificationCB, XtPointer(editable));
		  
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



//-----------------------------------------------------------------------
// Position management
//-----------------------------------------------------------------------

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
	    int indent = indent_amount(source_text_w, last_pos);
	    if (indent > 0)
	    {
		string no_marker = " ";
		XmTextReplace (source_text_w,
			       last_pos + indent - no_marker.length(),
			       last_pos + indent,
			       (String)no_marker);
	    }

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

    if (!is_current_file(file_name))
	read_file(file_name, line, silent);

    if (is_current_file(file_name))
    {
	int indent = indent_amount(source_text_w, last_pos);

	if (!display_glyphs && indent > 0)
	{
	    // Remove old marker
	    string no_marker = " ";
	    XmTextReplace (source_text_w,
			   last_pos + indent - no_marker.length(),
			   last_pos + indent,
			   (String)no_marker);
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

    if (!is_current_file(file))
	read_file(file, line, silent);

    if (!is_current_file(file) || line < 1 || line > line_count)
	return;

    add_to_history(file, line);

    XmTextPosition pos = pos_of_line(line);
    int indent = indent_amount(source_text_w, pos);
    SetInsertionPosition(source_text_w, pos + indent, false);

    // Mark current line
    if (!display_glyphs && indent > 0)
    {
	// Set new marker
	string marker = ">";
	XmTextReplace (source_text_w,
		       pos + indent - marker.length(),
		       pos + indent,
		       (String)marker);
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


void SourceView::show_position (string position, bool silent)
{
    string file_name = current_file_name;

    if (position.contains(':'))
    {
	file_name = position.before(':');
	position  = position.after(':');
    }
    int line = get_positive_nr(position);

    if (!is_current_file(file_name))
	read_file(file_name, line, false, silent);


    // Have window scroll to correct position
    if (is_current_file(file_name))
    {
	if (line == 0 && gdb->type() == JDB)
	{
	    // Scroll to current class
	    int pos = java_class_start(current_source, current_source_name());

	    if (pos >= 0)
	    {
		int line_nr = 0;
		bool in_text;
		int bp_nr;
		string address;

		if (get_line_of_pos(source_text_w, pos, line_nr, address, 
				    in_text, bp_nr))
		{
		    line = line_nr;
		}
	    }
	}
	   
	if (line > 0 && line <= line_count)
	{
	    add_to_history(file_name, line);
    
	    XmTextPosition pos = pos_of_line(line);
	    int indent = indent_amount(source_text_w, pos);
	    SetInsertionPosition(source_text_w, pos + indent, true);
		
	    last_pos = pos;
	}
    }
}



//-----------------------------------------------------------------------
// Process GDB output
//-----------------------------------------------------------------------

// Process reply on 'info breakpoints'.
// Update breakpoints in BP_BAP, adding new ones or deleting existing ones.
// Update breakpoint display by calling REFRESH_BP_DISP.
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
    string keep_me = "";

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
		strip_leading_space(info_output);
		    
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

	case JDB:
	{
	    // JDB has no breakpoint numbers.  Check if we already have one.
	    bp_nr = jdb_breakpoint(info_output);
	    if (bp_nr == 0)
		bp_nr = max_breakpoint_number_seen + 1;	// new breakpoint
	    if (bp_nr < 0)
	    {
		// Not a breakpoint
		string line = info_output.before('\n');
		if (!line.contains("Current breakpoints set"))
		    keep_me += line;
		
		// Skip this line
		info_output = info_output.after('\n');
		continue;
	    }
	    break;
	}
	}

	if (bp_nr <= 0)
	{
	    info_output = info_output.after('\n');
	    continue;
	}

	if (bp_map.contains (bp_nr))
	{
	    // Update existing breakpoint
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
	    BreakPoint* new_bp = new BreakPoint(info_output, break_arg, bp_nr);
	    bp_map.insert(bp_nr, new_bp);

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

    // Keep this stuff for further processing
    info_output = keep_me;

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

    // Set up existing panels
    update_properties_panels();
}

int SourceView::next_breakpoint_number()
{
    return max_breakpoint_number_seen + 1;
}


// Process GDB `info line main' output
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
    case JDB:
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



//-----------------------------------------------------------------------
// Locate position
//-----------------------------------------------------------------------

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
	    switch (gdb->type())
	    {
		case GDB:
		{
		    Command c("info line " + current_source_name()
			      + ":" + itostring(line));
		    c.verbose = !silent;
		    gdb_command(c);
		    break;
		}
		
	    case JDB:
		show_position(current_source_name()
			      + ":" + itostring(line));
		break;

	    default:
		show_position(full_path(current_file_name) 
			      + ":" + itostring(line));
		break;
	    }
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
		    show_position(bp->pos());
		    show_pc(bp->address());
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
	case JDB:
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



//-----------------------------------------------------------------------
// Process current working directory
//-----------------------------------------------------------------------

void SourceView::process_pwd(string& pwd_output)
{
    strip_trailing_space(pwd_output);

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
	case JDB:
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


//-----------------------------------------------------------------------
// Process current use path
//-----------------------------------------------------------------------

void SourceView::process_use(string& use_output)
{
    strip_space(use_output);
    current_class_path = use_output;

    clear_file_cache();
    reload();
}



//-----------------------------------------------------------------------
// Position history
//-----------------------------------------------------------------------

// Add position to history
void SourceView::add_to_history(const string& file_name, int line)
{
    if (source_history_locked)
    {
	source_history_locked = false;
	return;
    }

    string source_name = file_name;
    switch (gdb->type())
    {
    case GDB:
    case JDB:
	// Use source names instead.
	if (source_name_cache.has(file_name))
	    source_name = source_name_cache[file_name];
	break;

    default:
	break;
    }

    string entry = source_name + ":" + itostring(line);

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
	if (!is_current_file(file_name))
	{
	    source_history_locked = true;
	    read_file(file_name, line);
	}

	if (is_current_file(file_name) && line > 0 && line <= line_count)
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

	    int indent = indent_amount(source_text_w, 
				       last_start_secondary_highlight);
	    XmTextPosition pos = last_start_secondary_highlight + indent;
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



//-----------------------------------------------------------------------
// Searching
//-----------------------------------------------------------------------

void SourceView::find(const string& s, 
		      SourceView::SearchDirection direction,
		      bool words_only,
		      bool case_sensitive,
		      Time time)
{
    int matchlen = s.length();
    int pos = -1;
    XmTextPosition cursor = XmTextGetInsertionPosition(source_text_w);
    XmTextPosition initial_cursor = cursor;
    int wraps = 0;

    if (!have_source())
    {
	post_error("No source.", "no_source_error", source_text_w);
	return;
    }

    string key  = s;
    string text = current_source;
    if (!case_sensitive)
    {
	// FIXME: This should be done according to the current locale
	key.downcase();
	text.downcase();
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
		&& cursor < XmTextPosition(text.length()))
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
	    pos = text.index(key, cursor);
	    if (pos < 0)
	    {
		if (wraps++)
		    break;
		pos = text.index(key, 0);
	    }
	    break;
	case backward:
	    pos = text.index(key, cursor - text.length() - 1);
	    if (pos < 0)
	    {
		if (wraps++)
		    break;
		pos = text.index(key, -1);
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
	    // Make sure the occurrence is not part of a larger word
	    if (pos > 0 && pos < int(text.length()))
	    {
		if (isid(text[pos]) && isid(text[pos - 1]))
		    continue;
	    }

	    if (pos + matchlen < int(text.length()))
	    {
		if (isid(text[pos + matchlen - 1]) && 
		    isid(text[pos + matchlen]))
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

	    string occurrence = current_source(pos, matchlen);
	    msg = "Found " + quote(occurrence) + " in " + line_of_cursor();
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




//-----------------------------------------------------------------------
// Return source name
//-----------------------------------------------------------------------

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

    case JDB:
	if (source_name_cache.has(current_file_name))
	{
	    // Use the source name as stored by read_class()
	    source = source_name_cache[current_file_name];
	}
	if (source == "")
	{
	    source = basename(current_file_name.chars());
	    strip_java_suffix(source);
	}
	break;
    }

    // In case this does not work, use the current base name.
    if (source == "")
	source = basename(current_file_name.chars());

    return source;
}

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

    XButtonEvent *event = &e->xbutton;

    XmTextPosition pos = XmTextXYToPos (text_w, event->x, event->y);

    XmTextPosition startpos, endpos;
    if (app_data.source_editing)
	startpos = endpos = pos;
    else
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
    
    XButtonEvent *event = &e->xbutton;

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
    if (lesstif_version <= 82)
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

// Get relative coordinates of GLYPH in TEXT
void SourceView::translate_glyph_pos(Widget glyph, Widget text, int& x, int& y)
{
    int dest_x, dest_y;
    Window child;
    XTranslateCoordinates(XtDisplay(glyph), 
			  XtWindow(glyph), XtWindow(text), 
			  x, y, &dest_x, &dest_y, &child);

    x = dest_x;
    y = dest_y;
}

// Popup button3 source menu
void SourceView::srcpopupAct (Widget w, XEvent* e, String *, Cardinal *)
{
    if (e->type != ButtonPress && e->type != ButtonRelease)
	return;

    Widget text_w;
    if (is_source_widget(w))
	text_w = source_text_w;
    else if (is_code_widget(w))
	text_w = code_text_w;
    else
	return;

    XButtonEvent* event = &e->xbutton;

    int x = event->x;
    int y = event->y;

    if (w != source_text_w && w != code_text_w)
    {
	// Called from a glyph: translate glyph position to text position
	translate_glyph_pos(w, text_w, x, y);
    }

    // Get the position
    XmTextPosition pos = XmTextXYToPos(text_w, x, y);

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
	static Widget bp_popup_w      = 0;
	static Widget bp_popup_parent = 0;

	if (lesstif_version < 1000 && w != bp_popup_parent)
	{
	    // LessTif wants this menu re-created every time the
	    // parent has changed.  Otherwise, it gets insensitive.
	    if (bp_popup_w != 0)
		XtDestroyWidget(bp_popup_w);
	    bp_popup_w = 0;
	}

	if (bp_popup_w == 0)
	{
	    bp_popup_parent = w;
	    bp_popup_w = MMcreatePopupMenu(w, "bp_popup", bp_popup);
	    MMaddCallbacks (bp_popup, XtPointer(&bp_nr));
	    MMaddHelpCallback(bp_popup, ImmediateHelpCB);
	    InstallButtonTips(bp_popup_w);
	}

	// Grey out unsupported functions
	XtSetSensitive(bp_popup[BPItms::Disable].widget, 
		       gdb->has_disable_command());
	XtSetSensitive(bp_popup[BPItms::SetPC].widget,
		       gdb->has_jump_command() || gdb->has_assign_command());

	MString label(bp_map.get(bp_nr)->enabled() ? 
		      "Disable Breakpoint" : "Enable Breakpoint");
	XtVaSetValues(bp_popup[BPItms::Disable].widget,
		      XmNlabelString, label.xmstring(),
		      NULL);

	XmMenuPosition(bp_popup_w, event);
	XtManageChild(bp_popup_w);
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
	    MMaddCallbacks(line_popup, XtPointer(&address));
	    MMaddHelpCallback(line_popup, ImmediateHelpCB);
	    InstallButtonTips(line_popup_w);

	    XtSetSensitive(line_popup[LineItms::SetTempBP].widget, 
			   gdb->has_temporary_breakpoints());
	    XtSetSensitive(line_popup[LineItms::SetPC].widget,
			   gdb->has_jump_command() || 
			   gdb->has_assign_command());
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

	XmTextPosition startpos = 0;
	XmTextPosition endpos   = 0;

	if (pos_found)
	    word = get_word_at_pos(text_w, pos, startpos, endpos);

	// Popup specific word menu
	string current_arg = word;
	shorten(current_arg, max_popup_expr_length);
	string current_ref_arg = gdb->dereferenced_expr(current_arg);

	if (lesstif_version <= 82)
	{
	    set_text_popup_resource(TextItms::Print,    current_arg);
	    set_text_popup_resource(TextItms::Disp,     current_arg);
	    set_text_popup_resource(TextItms::Watch,    current_arg);
	    set_text_popup_resource(TextItms::PrintRef, current_ref_arg);
	    set_text_popup_resource(TextItms::DispRef,  current_ref_arg);
	    set_text_popup_resource(TextItms::WatchRef, current_ref_arg);
	    set_text_popup_resource(TextItms::Whatis,   current_arg);
	    set_text_popup_resource(TextItms::Lookup,   current_arg);
	    set_text_popup_resource(TextItms::Break,    current_arg);
	    set_text_popup_resource(TextItms::Clear,    current_arg);
	}

	Widget text_popup_w = 
	    MMcreatePopupMenu(text_w, "text_popup", text_popup);
	MMaddCallbacks(text_popup, XtPointer(&word));
	MMaddHelpCallback(text_popup, ImmediateHelpCB);
	InstallButtonTips(text_popup_w);

	// The popup menu is destroyed immediately after having popped down.
	Widget shell = XtParent(text_popup_w);
	XtAddCallback(shell, XtNpopdownCallback, DestroyThisCB, shell);

	bool has_arg = (word.length() > 0);
	bool has_watch = has_arg && gdb->has_watch_command();
	set_text_popup_label(TextItms::Print,    current_arg, has_arg);
	set_text_popup_label(TextItms::Disp,     current_arg, has_arg);
	set_text_popup_label(TextItms::Watch,    current_arg, has_watch);
	set_text_popup_label(TextItms::PrintRef, current_ref_arg, has_arg);
	set_text_popup_label(TextItms::DispRef,  current_ref_arg, has_arg);
	set_text_popup_label(TextItms::WatchRef, current_ref_arg, has_watch);
	set_text_popup_label(TextItms::Whatis,   current_arg, has_arg);
	set_text_popup_label(TextItms::Lookup,   current_arg, has_arg);
	set_text_popup_label(TextItms::Break,    current_arg, has_arg);
	set_text_popup_label(TextItms::Clear,    current_arg, has_arg);

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

WatchMode SourceView::selected_watch_mode = WATCH_CHANGE;

void SourceView::SetWatchModeCB(Widget, XtPointer client_data, 
				XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    if (info->set)
	selected_watch_mode = WatchMode(client_data);
}

void SourceView::NewWatchpointDCB(Widget w, XtPointer, XtPointer call_data)
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

    gdb_command(gdb->watch_command(input, selected_watch_mode), w);
}

void SourceView::NewWatchpointCB(Widget, XtPointer, XtPointer)
{
    static Widget new_watchpoint_dialog = 0;
    if (new_watchpoint_dialog == 0)
    {
	static Widget cwatch_w, rwatch_w, awatch_w;

	static MMDesc wp_modes[] =
	{
	    { "cwatch",  MMPush, { SetWatchModeCB, XtPointer(WATCH_CHANGE) }, 
	      NULL, &cwatch_w },
	    { "rwatch",  MMPush, { SetWatchModeCB, XtPointer(WATCH_READ) }, 
	      NULL, &rwatch_w },
	    { "awatch", MMPush, { SetWatchModeCB, XtPointer(WATCH_ACCESS)},
	      NULL, &awatch_w},
	    MMEnd
	};

	static MMDesc wp_menu[] = 
	{
	    { "set",      MMLabel },
	    { "method",   MMOptionMenu, MMNoCB, wp_modes },
	    { "on",       MMLabel },
	    MMEnd
	};

	new_watchpoint_dialog = 
	    verify(XmCreatePromptDialog(source_text_w,
					"new_watchpoint_dialog",
					NULL, 0));
	Delay::register_shell(new_watchpoint_dialog);

	if (lesstif_version <= 79)
	    XtUnmanageChild(XmSelectionBoxGetChild(new_watchpoint_dialog,
						   XmDIALOG_APPLY_BUTTON));
	XtUnmanageChild(XmSelectionBoxGetChild(new_watchpoint_dialog,
					       XmDIALOG_SELECTION_LABEL));

	XtAddCallback(new_watchpoint_dialog,
		      XmNhelpCallback,
		      ImmediateHelpCB,
		      NULL);
	XtAddCallback(new_watchpoint_dialog,
		      XmNokCallback,
		      NewWatchpointDCB,
		      NULL);

	Widget panel = MMcreateButtonPanel(new_watchpoint_dialog, 
					   "panel", wp_menu);
	XtVaSetValues(panel, 
		      XmNorientation, XmHORIZONTAL,
		      XmNborderWidth,  0,
		      XmNentryBorder,  0,
		      XmNspacing,      0,
		      XmNmarginWidth,  0,
		      XmNmarginHeight, 0,
		      NULL);
	MMaddCallbacks(wp_menu);
	MMaddHelpCallback(wp_menu, ImmediateHelpCB);

	XtSetSensitive(cwatch_w, (gdb->has_watch_command() & WATCH_CHANGE) 
		       == WATCH_CHANGE);
	XtSetSensitive(rwatch_w, (gdb->has_watch_command() & WATCH_READ) 
		       == WATCH_READ);
	XtSetSensitive(awatch_w, (gdb->has_watch_command() & WATCH_ACCESS) 
		       == WATCH_ACCESS);

	// Initialize: use CWATCH as default menu item
	XtCallActionProc(cwatch_w, "ArmAndActivate", 
			 (XEvent *)0, (String *)0, 0);
    }

    manage_and_raise(new_watchpoint_dialog);
}


//-----------------------------------------------------------------------------
// Breakpoint properties
//----------------------------------------------------------------------------

struct BreakpointPropertiesInfo {
    IntArray nrs;		// The affected breakpoints
    Widget dialog;		// The widgets of the properties panel
    Widget title;
    Widget enabled;
    Widget temp;
    Widget lookup;
    Widget ignore;
    Widget condition;
    Widget record;
    Widget end;
    Widget edit;
    Widget editor;
    XtIntervalId timer;
    bool spin_locked;
    bool sync_commands;
    BreakpointPropertiesInfo *next; // Next info in list

    static BreakpointPropertiesInfo *all; // List of all infos

    BreakpointPropertiesInfo()
	: nrs(),
	  dialog(0), title(0), enabled(0), temp(0), lookup(0), 
	  ignore(0), condition(0), record(0), edit(0), editor(0),
	  timer(0), spin_locked(false), sync_commands(false), next(all)
    {
	all = this;
    }

    ~BreakpointPropertiesInfo()
    {
	if (all == this)
	{
	    all = next;
	}
	else
	{
	    BreakpointPropertiesInfo *info = all;
	    while (info->next != this)
		info = info->next;
	    info->next = next;
	}
    }
};

BreakpointPropertiesInfo *BreakpointPropertiesInfo::all = 0;

void SourceView::DeleteInfoCB(Widget w, XtPointer client_data, 
			      XtPointer call_data)
{
    BreakpointPropertiesInfo *info = 
	(BreakpointPropertiesInfo *)client_data;

    if (gdb->recording())
    {
	gdb->removeHandler(Recording, RecordingHP, XtPointer(info));
	gdb_command("\003");	// Abort recording
    }

    if (XtIsManaged(XtParent(info->editor)))
    {
	// Finish entering commands
	EditBreakpointCommandsCB(w, client_data, call_data);
    }

    delete info;
}

void SourceView::update_properties_panels()
{
    // Update all panels
    BreakpointPropertiesInfo *info = BreakpointPropertiesInfo::all;
    while (info != 0)
    {
	update_properties_panel(info);
	info = info->next;
    }
}

void SourceView::update_properties_panel(BreakpointPropertiesInfo *info)
{
    // Remove breakpoints from list
    int i;
    for (i = 0; i < info->nrs.size(); i++)
    {
	MapRef ref;
	BreakPoint *bp = 0;
	for (bp = bp_map.first(ref);
	     bp != 0;
	     bp = bp_map.next(ref))
	{
	    if (bp->number() == info->nrs[i])
		break;
	}

	if (bp == 0)
	{
	    // Breakpoint not found -- mark as deleted
	    info->nrs[i] = 0;
	}
    }
    IntArray new_nrs;

    for (i = 0; i < info->nrs.size(); i++)
    {
	if (info->nrs[i] != 0)
	    new_nrs += info->nrs[i];
    }

    info->nrs = new_nrs;

    if (info->nrs.size() == 0)
    {
	// No breakpoint left -- destroy dialog shell
	XtUnmanageChild(info->dialog);
	return;
    }

    // Use first breakpoint for getting values
    BreakPoint *bp = 0;
    MapRef ref;
    for (bp = bp_map.first(ref);
	 bp != 0;
	 bp = bp_map.next(ref))
    {
	if (bp->number() == info->nrs[0])
	    break;
    }
    assert(bp != 0);

    // Set titles
    string what;
    switch (bp->type())
    {
    case BREAKPOINT:
	what = "Breakpoint";
	break;

    case WATCHPOINT:
	what = "Watchpoint";
	break;
    }

    string label;
    if (info->nrs.size() == 1)
    {
	label = what + " " + itostring(info->nrs[0]);
    }
    else
    {
	label = what + "s ";
	for (i = 0; i < info->nrs.size(); i++)
	{
	    if (i > 0)
	    {
		if (info->nrs.size() == 2)
		    label += " and ";
		else if (i == info->nrs.size() - 1)
		    label += ", and ";
		else
		    label += ", ";
	    }
	    label += itostring(info->nrs[i]);
	}
    }

    set_label(info->title, label);

    MString title = string(DDD_NAME) + ": Properties: " + label;
    XtVaSetValues(info->dialog, XmNdialogTitle,
		  title.xmstring(), NULL);

    // Set values
    XtVaSetValues(info->enabled, XmNset, bp->enabled(), NULL);
    XtVaSetValues(info->temp,    XmNset, bp->dispo() != BPKEEP, NULL);

    string commands = "";
    for (i = 0; i < bp->commands().size(); i++)
    {
	string cmd = bp->commands()[i];
	strip_auto_command_prefix(cmd);
	commands += cmd + "\n";
    }

    XmTextSetString(info->editor, commands);

    bool lock = info->spin_locked;
    info->spin_locked = true;
#if XmVersion >= 2000
    if (XmIsSpinBox(XtParent(info->ignore)))
    {
	XtVaSetValues(info->ignore, XmNposition, bp->ignore_count(), NULL);
    }
    else
#endif
    {
	string ignore = itostring(bp->ignore_count());
	if (ignore == "0")
	    ignore = "";

	XmTextFieldSetString(info->ignore, (String)ignore);
    }
    info->spin_locked = lock;

    XmTextFieldSetString(info->condition, (String)bp->condition());

    set_sensitive(info->enabled,   gdb->has_disable_command());
    set_sensitive(info->temp,
		  gdb->type() == GDB && bp->dispo() == BPKEEP);
    set_sensitive(info->ignore,    gdb->has_ignore_command());
    set_sensitive(info->condition, true);

    bool can_record = gdb->type() == GDB && !gdb->recording();
    set_sensitive(info->record,    can_record);
    set_sensitive(info->end,       gdb->recording());
    set_sensitive(info->edit,      can_record);
    set_sensitive(info->editor,    can_record);

    if (info->sync_commands)
    {
	for (int i = 1; i < info->nrs.size(); i++)
	    set_bp_commands(info->nrs[i], bp->commands());
	info->sync_commands = false;
    }
}

// Edit breakpoint properties
void SourceView::EditBreakpointPropertiesCB(Widget, 
					    XtPointer client_data, 
					    XtPointer)
{
    if (breakpoint_list_w == 0)
	return;

    BreakpointPropertiesInfo *info = new BreakpointPropertiesInfo;
    info->spin_locked = true;

    if (client_data == 0)
    {
	IntArray breakpoint_nrs;
	getDisplayNumbers(breakpoint_list_w, info->nrs);
    }
    else
    {
	info->nrs += *((int *)client_data);
    }

    // Check for first breakpoint
    MapRef ref;
    BreakPoint *bp = 0;
    for (bp = bp_map.first(ref);
	 bp != 0;
	 bp = bp_map.next(ref))
    {
	if (bp->number() == info->nrs[0])
	{
	    break;
	}
    }
    if (bp == 0)
	return;			// No such breakpoint

    Arg args[10];
    int arg = 0;
    XtSetArg(args[arg], XmNautoUnmanage, False); arg++;
    info->dialog = 
	verify(XmCreatePromptDialog(source_text_w,
				    "breakpoint_properties",
				    args, arg));
    XtVaSetValues(info->dialog, XmNdefaultButton, Widget(0), NULL);


    // Remove old prompt
    Widget text = XmSelectionBoxGetChild(info->dialog, XmDIALOG_TEXT);
    XtUnmanageChild(text);
    Widget old_label = 
	XmSelectionBoxGetChild(info->dialog, XmDIALOG_SELECTION_LABEL);
    XtUnmanageChild(old_label);

    Delay::register_shell(info->dialog);

    if (lesstif_version <= 79)
	XtUnmanageChild(XmSelectionBoxGetChild(info->dialog, 
					       XmDIALOG_APPLY_BUTTON));

    MMDesc commands_menu[] =
    {
	{ "record", MMPush, \
	  { RecordBreakpointCommandsCB, XtPointer(info) }, 0, &info->record },
	{ "end",    MMPush | MMInsensitive, \
	  { RecordBreakpointCommandsCB, XtPointer(info) }, 0, &info->end },
	{ "edit",   MMPush | MMInsensitive, \
	  { EditBreakpointCommandsCB, XtPointer(info) }, 0, &info->edit },
	MMEnd
    };

    MMDesc enabled_menu[] = 
    {
	{ "enabled",   MMToggle, 
	  { ToggleBreakpointEnabledCB, XtPointer(info) }, 0, &info->enabled },
	{ "temporary", MMToggle, 
	  { ToggleBreakpointTempCB, XtPointer(info) }, 0, &info->temp },
	{ "lookup",    MMPush, 
	  { LookupBreakpointCB, XtPointer(info) }, 0, &info->lookup },
	MMEnd
    };

    MMDesc panel_menu[] = 
    {
	{ "title", MMButtonPanel, MMNoCB, enabled_menu },
	{ "condition", MMEnterField,
	  { SetBreakpointConditionCB, XtPointer(info) }, 0, &info->condition },
	{ "ignore", MMSpinField,
	  { SetBreakpointIgnoreCountCB, XtPointer(info) }, 0, &info->ignore },
	{ "commands", MMButtonPanel, MMNoCB, commands_menu },
	MMEnd
    };

    arg = 0;
    XtSetArg(args[arg], XmNorientation, XmHORIZONTAL); arg++;
    Widget form = XmCreateRowColumn(info->dialog, "form", args, arg);
    XtManageChild(form);

    Widget panel = MMcreatePanel(form, "panel", panel_menu);

    XtVaSetValues(panel,
		  XmNmarginWidth,    0,
		  XmNmarginHeight,   0,
		  NULL);

    arg = 0;
    XtSetArg(args[arg], XmNeditMode, XmMULTI_LINE_EDIT); arg++;
    info->editor = XmCreateScrolledText(form, "text", args, arg);
    XtUnmanageChild(XtParent(info->editor));
    XtManageChild(info->editor);

    info->title = panel_menu[0].label;
    MMaddCallbacks(panel_menu, XtPointer(info));

    update_properties_panel(info);

    MMadjustPanel(panel_menu);

    XtAddCallback(info->dialog, XmNokCallback,
		  UnmanageThisCB, info->dialog);
    XtAddCallback(info->dialog, XmNhelpCallback,    
		  ImmediateHelpCB, NULL);
    XtAddCallback(info->dialog, XmNunmapCallback,
		  DestroyThisCB, XtParent(info->dialog));
    XtAddCallback(info->dialog, XmNdestroyCallback,
		  DeleteInfoCB,  XtPointer(info));
    XtAddCallback(info->dialog, XmNcancelCallback,
		  DeleteBreakpointCB, XtPointer(info));

    manage_and_raise(info->dialog);
    info->spin_locked = false;
}

// Set breakpoint condition
void SourceView::SetBreakpointConditionCB(Widget w,
					  XtPointer client_data, 
					  XtPointer)
{
    BreakpointPropertiesInfo *info = 
	(BreakpointPropertiesInfo *)client_data;

    String cond = XmTextFieldGetString(info->condition);

    for (int i = 0; i < info->nrs.size(); i++)
    {
	set_bp_cond(info->nrs[i], cond, w);
    }

    XtFree(cond);
}

// Set breakpoint ignore count
void SourceView::SetBreakpointIgnoreCountCB(Widget w,
					    XtPointer client_data, 
					    XtPointer call_data)
{
    XmAnyCallbackStruct *cbs = (XmAnyCallbackStruct *)call_data;
    BreakpointPropertiesInfo *info = 
	(BreakpointPropertiesInfo *)client_data;

    if (info->spin_locked)
	return;			// Ignore the SetValue change

    int delay = 500;		// Wait until the SpinBox stops spinning
    if (cbs->reason == XmCR_ACTIVATE)
	delay = 0;

#if XmVersion < 2000
    // Don't care about changing values in non-SpinBoxes
    if (cbs->reason == XmCR_VALUE_CHANGED)
	return;
#endif

    if (info->timer != 0)
    {
	XtRemoveTimeOut(info->timer);
	info->timer = 0;
    }

    info->timer = XtAppAddTimeOut(XtWidgetToApplicationContext(w),
				  delay,
				  SetBreakpointIgnoreCountNowCB, 
				  client_data);
}

void SourceView::SetBreakpointIgnoreCountNowCB(XtPointer client_data, 
					       XtIntervalId *id)
{
    BreakpointPropertiesInfo *info = 
	(BreakpointPropertiesInfo *)client_data;

    assert (info->timer == *id);
    (void) id;			// Use it
    info->timer = 0;

    String _count = XmTextFieldGetString(info->ignore);
    int count = atoi(_count);
    XtFree(_count);

    for (int i = 0; i < info->nrs.size(); i++)
    {
	gdb_command(gdb->ignore_command(itostring(info->nrs[i]), count));
    }
}


// Toggle breakpoint enabled state
void SourceView::ToggleBreakpointEnabledCB(Widget, 
					   XtPointer client_data, 
					   XtPointer call_data)
{
    XmToggleButtonCallbackStruct *cbs = 
	(XmToggleButtonCallbackStruct *)call_data;
    BreakpointPropertiesInfo *info = 
	(BreakpointPropertiesInfo *)client_data;

    if (cbs->set)
	enable_bps(info->nrs);
    else
	disable_bps(info->nrs);
}

// Toggle breakpoint temp state
void SourceView::ToggleBreakpointTempCB(Widget, 
					XtPointer client_data, 
					XtPointer call_data)
{
    XmToggleButtonCallbackStruct *cbs = 
	(XmToggleButtonCallbackStruct *)call_data;
    BreakpointPropertiesInfo *info = 
	(BreakpointPropertiesInfo *)client_data;

    if (cbs->set)
	gdb_command("enable delete " + numbers(info->nrs));
    else
    {
	// How do we make a temp breakpoint non-temporary?  (FIXME)
    }
}


// Delete Breakpoint
void SourceView::DeleteBreakpointCB(Widget, XtPointer client_data, XtPointer)
{
    BreakpointPropertiesInfo *info = 
	(BreakpointPropertiesInfo *)client_data;

    delete_bps(info->nrs);
}

// Record breakpoint commands
void SourceView::RecordBreakpointCommandsCB(Widget w,
					    XtPointer client_data, 
					    XtPointer)
{
    BreakpointPropertiesInfo *info = 
	(BreakpointPropertiesInfo *)client_data;
    bool record = (w == info->record);

    if (record)
    {
	gdb->removeHandler(Recording, RecordingHP, XtPointer(info));
	gdb->addHandler(Recording, RecordingHP, XtPointer(info));
	gdb_command("commands " + itostring(info->nrs[0]), w);
    }
    else			// end
    {
	gdb_command("end", w);
    }
}

// Log recording state
void SourceView::RecordingHP(Agent *, void *client_data, void *call_data)
{
    BreakpointPropertiesInfo *info = 
	(BreakpointPropertiesInfo *)client_data;
    bool recording = bool(call_data);

    // Refresh buttons
    update_properties_panel(info);

    if (!recording)
    {
	// Recording is over.  Don't get called again.
	gdb->removeHandler(Recording, RecordingHP, XtPointer(info));

	// Upon next panel update, propagate command to other breakpoints
	info->sync_commands = true;
    }
}

// Set breakpoint commands
void SourceView::set_bp_commands(IntArray& nrs, const StringArray& commands,
				 Widget origin)
{
    for (int i = 0; i < nrs.size(); i++)
    {
	// Check for breakpoint
	MapRef ref;
	BreakPoint *bp = 0;
	for (bp = bp_map.first(ref); bp != 0; bp = bp_map.next(ref))
	{
	    if (bp->number() == nrs[i])
		break;
	}
	if (bp == 0)
	    continue;		// No such breakpoint

	if (commands.size() == bp->commands().size())
	{
	    bool same_commands = true;
	    for (int j = 0; same_commands && j < bp->commands().size(); j++)
	    {
		string c1 = bp->commands()[j];
		strip_auto_command_prefix(c1);
		string c2 = commands[j];
		strip_auto_command_prefix(c2);

		if (c1 != c2)
		    same_commands = false;
	    }

	    if (same_commands)
		continue;	// Commands unchanged
	}

	gdb_command("commands " + itostring(nrs[i]), origin);
	for (int j = 0; j < commands.size(); j++)
	    gdb_command(commands[j], origin);
	gdb_command("end", origin);
    }
}


// Edit breakpoint commands
void SourceView::EditBreakpointCommandsCB(Widget w,
					  XtPointer client_data, 
					  XtPointer)
{
    BreakpointPropertiesInfo *info = 
	(BreakpointPropertiesInfo *)client_data;

    if (XtIsManaged(XtParent(info->editor)))
    {
	XtUnmanageChild(XtParent(info->editor));
	MString label = "Edit " + MString(">>", "small");
	set_label(info->edit, label);

	String _commands = XmTextGetString(info->editor);
	string cmd = _commands;
	XtFree(_commands);

	if (!cmd.contains('\n', -1))
	    cmd += '\n';
	StringArray commands;
	while (cmd != "")
	{
	    string c = cmd.before('\n');
	    if (c != "")
		commands += c;
	    cmd = cmd.after('\n');
	}
	set_bp_commands(info->nrs, commands, w);
    }
    else
    {
	XtManageChild(XtParent(info->editor));
	MString label = "Edit " + MString("<<", "small");
	set_label(info->edit, label);
    }
}

void SourceView::edit_breakpoint_properties(int bp_nr)
{
    static int n;
    n = bp_nr;
    EditBreakpointPropertiesCB(source_text_w, XtPointer(&n), 0);
}





//-----------------------------------------------------------------------------
// Breakpoint commands
//----------------------------------------------------------------------------

void SourceView::BreakpointCmdCB(Widget,
				 XtPointer client_data,
				 XtPointer)
{
    if (breakpoint_list_w == 0)
	return;

    IntArray nrs;
    getDisplayNumbers(breakpoint_list_w, nrs);
    if (nrs.size() == 0)
        return;

    string cmd = (String)client_data;

    if (cmd == "delete")
        delete_bps(nrs);
    else if (cmd == "enable")
	enable_bps(nrs);
    else if (cmd == "disable")
	disable_bps(nrs);
}

void SourceView::LookupBreakpointCB(Widget, XtPointer client_data, XtPointer)
{
    if (breakpoint_list_w == 0)
	return;

    IntArray breakpoint_nrs;

    if (client_data == 0)
    {
	getDisplayNumbers(breakpoint_list_w, breakpoint_nrs);
    }
    else
    {
	BreakpointPropertiesInfo *info = 
	    (BreakpointPropertiesInfo *)client_data;
	breakpoint_nrs = info->nrs;
    }
    if (breakpoint_nrs.size() < 1)
	return;

    BreakPoint *bp = bp_map.get(breakpoint_nrs[0]);
    if (bp == 0)
	return;

    switch (bp->type())
    {
    case BREAKPOINT:
	lookup("#" + itostring(breakpoint_nrs[0]));
	break;

    case WATCHPOINT:
	gdb_command(gdb->print_command(bp->expr(), false));
	break;
    }
}

// Return breakpoint of BP_INFO; 0 if new; -1 if none
int SourceView::jdb_breakpoint(const string& bp_info)
{
    int colon = bp_info.index(':');
    if (colon < 0)
	return -1;		// No breakpoint

    string class_name = bp_info.before(colon);
    strip_leading_space(class_name);
    int line = get_positive_nr(bp_info.after(colon));
    if (line <= 0 || class_name.contains(' '))
	return -1;		// No breakpoint

    MapRef ref;
    for (BreakPoint* bp = bp_map.first(ref); bp != 0; bp = bp_map.next(ref))
	if (bp_matches(bp, class_name, line))
	    return bp->number(); // Existing breakpoint

    return 0;		       // New breakpoint
}


// Handle breakpoint info
void SourceView::process_breakpoints(string& info_breakpoints_output)
{
    if (breakpoint_list_w == 0)
	return;

    strip_space(info_breakpoints_output);
    if (info_breakpoints_output == "")
    {
	if (gdb->has_watch_command())
	    info_breakpoints_output = "No breakpoints or watchpoints.";
	else
	    info_breakpoints_output = "No breakpoints.";
    }

    int count = info_breakpoints_output.freq('\n') + 1;

    string *breakpoint_list = new string[count];
    bool *selected          = new bool[count];

    split(info_breakpoints_output, breakpoint_list, count, '\n');

    while (count > 0 && breakpoint_list[count - 1] == "")
	count--;

    bool select = false;
    for (int i = 0; i < count; i++)
    {
	string& bp_info = breakpoint_list[i];
	if (gdb->type() == JDB)
	{
	    // JDB has no breakpoint numbers -- insert our own
	    int bp_nr = jdb_breakpoint(bp_info);
	    if (bp_nr > 0)
	    {
		string s = itostring(bp_nr) + "    ";
		bp_info.prepend(s.at(0, 4));
	    }
	}

	// Select number
	int bp_number = get_positive_nr(bp_info);
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
	strip_auto_command_prefix(bp_info);
	setup_where_line(bp_info);
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
    BreakPoint *bp;
    for (bp = bp_map.first(ref); bp != 0; bp = bp_map.next(ref))
	bp->selected() = false;

    for (int i = 0; i < breakpoint_nrs.size(); i++)
    {
	int bp_number = breakpoint_nrs[i];
	for (bp = bp_map.first(ref); bp != 0; bp = bp_map.next(ref))
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

    // Count selected ones
    BreakPoint *selected_bp = 0;
    int selected = breakpoint_nrs.size();
    int selected_enabled = 0;
    int selected_disabled = 0;
    for (bp = bp_map.first(ref); bp != 0; bp = bp_map.next(ref))
    {
	if (bp->selected())
	{
	    selected_bp = bp;
	    if (bp->enabled())
		selected_enabled++;
	    else
		selected_disabled++;
	}
    }

    // Update buttons
    XtSetSensitive(bp_area[BPButtons::NewWP].widget, gdb->has_watch_command());
    XtSetSensitive(bp_area[BPButtons::Lookup].widget, selected == 1);
    XtSetSensitive(bp_area[BPButtons::Enable].widget,      
		   gdb->has_enable_command() && selected_disabled > 0);
    XtSetSensitive(bp_area[BPButtons::Disable].widget,     
		   gdb->has_disable_command() && selected_enabled > 0);
    XtSetSensitive(bp_area[BPButtons::Properties].widget, selected > 0);
    XtSetSensitive(bp_area[BPButtons::Delete].widget, selected > 0);

    if (selected == 1)
    {
	switch (selected_bp->type())
	{
	case BREAKPOINT:
	    set_label(bp_area[BPButtons::Lookup].widget, "Lookup", 
		      LOOKUP_ICON);
	    break;

	case WATCHPOINT:
	    set_label(bp_area[BPButtons::Lookup].widget, "Print", PRINT_ICON);
	    break;
	}
    }
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
    case JDB:
	if (gdb->has_frame_command())
	{
	    // Issue `frame' command
	    gdb_command(gdb->frame_command(count - cbs->item_position + 1));
	}
	else
	{
	    // JDB and some DBXes lack a `frame' command.
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

    if (gdb->type() != JDB)
    {
	// Remove file paths (otherwise line can be too long for DBX)
	//   ... n.b. with templates, line can still be rather long
#if RUNTIME_REGEX
	static regex rxfilepath("[^\"\' /]*/");
#endif
	line.gsub(rxfilepath, "");
    }

    if (gdb->type() != JDB)
    {
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
    }

    if (int(line.length()) < min_width)
	line += replicate(' ', min_width - line.length());
}

// Return current JDB frame; 0 if none
inline int jdb_frame()
{
    return get_positive_nr(gdb->prompt().from("["));
}

// Return current JDB thread; "" if none
inline string jdb_thread()
{
    return gdb->prompt().before("[");
}

void SourceView::process_where(string& where_output)
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

    // JDB does not report frames above the current one.  Hence, we
    // only update the reported frames and others unchanged.
    if (gdb->type() == JDB && jdb_frame() != 1)
	updateLabelList(frame_list_w, frame_list, count);
    else
	setLabelList(frame_list_w, frame_list, selected, count, false, false);
    set_frame_pos(0, 0);

    delete[] frame_list;
    delete[] selected;
}

void SourceView::process_frame (string& frame_output)
{
    if (frame_output != "" 
	&& (frame_output[0] == '#' || gdb->type() != GDB))
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

	case JDB:
	    frame_nr = frame_output.after("[");
	    break;
	}

	int frame = get_positive_nr(frame_nr);

	// In GDB, the lowest frame is #0, in DBX and JDB, it is #1
	if (gdb->type() == DBX || gdb->type() == JDB)
	    frame--;

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
	case JDB:
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
	untabify_if_needed(register_list[i]);
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

string SourceView::current_threadgroup = "system";

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

    switch (gdb->type())
    {
    case GDB:
    {
	for (int i = 0; i < count; i++)
	{
	    selected[i] = thread_list[i].contains('*', 0);
	    if (selected[i])
		thread_list[i] = thread_list[i].after(0);
	    strip_leading_space(thread_list[i]);
	    setup_where_line(thread_list[i]);
	}
	break;
    }

    case JDB:
    {
	string current_thread = jdb_thread();
	current_threadgroup = "";
	for (int i = 0; i < count; i++)
	{
	    selected[i] = false;
	    string& item = thread_list[i];

	    if (item.contains("Group ", 0))
	    {
		// Format is: `Group THREADGROUP:'

		if (current_threadgroup != "")
		{
		    // Multiple threadgroups are shown
		    current_threadgroup = "system";
		}
		else
		{
		    current_threadgroup = item.after(" ");
		    strip_leading_space(current_threadgroup);
		    current_threadgroup = current_threadgroup.before(":");
		}
	    }
	    else
	    {
		// Format is: ` NUMBER. (CLASS)ADDRESS NAME STATE'

		int addr_index = item.index("(");
		if (addr_index < 0)
		    addr_index = item.index("0x");

		if (addr_index >= 0)
		{
		    // If is the current thread, select it
		    string thread = item.after("(");
		    thread = thread.after(" ");
		    strip_leading_space(thread);

		    if (thread.contains(current_thread + " ", 0))
			selected[i] = true;

		    // Leave only ` NUMBER. NAME STATE'
		    int info_index = addr_index;
		    while (info_index < int(item.length()) &&
			   item[info_index] != ' ')
			info_index++;
		    while (info_index < int(item.length()) &&
			   item[info_index] == ' ')
			info_index++;
		    item = item.before(addr_index) + item.from(info_index);

		    // Give more verbose output on system threads
		    if (item.contains("runni", -1))
			item += "ng";
		    else if (item.contains("suspe", -1))
			item += "nded";
		    else if (item.contains("cond.", -1))
			item += " waiting";
		}
	    }
	}
	break;
    }

    case DBX:
    case XDB:
    {
	for (int i = 0; i < count; i++)
	    selected[i] = false;
	break;
    }
    }

    setLabelList(thread_list_w, thread_list, selected, count, false, false);

    delete[] thread_list;
    delete[] selected;
}

void SourceView::refresh_threads(bool all_threadgroups)
{
    switch (gdb->type())
    {
    case GDB:
    {
	string threads = gdb_question("info threads");
	process_threads(threads);
	break;
    }
    case JDB:
    {
	if (all_threadgroups)
	{
	    // In JDB, `threadgroup system' seems to make `threads' list
	    // the threads of *all* threadgroups, not only system threads.
	    // This command will also automatically trigger an update.
	    gdb_command("threadgroup system");
	    syncCommandQueue();
	}

	string threads = gdb_question("threads");
	process_threads(threads);
	break;
    }
    case DBX:
    case XDB:
	// No threads.
	break;
    }
}

void SourceView::ViewThreadsCB(Widget, XtPointer, XtPointer)
{
    refresh_threads(true);
    manage_and_raise(thread_dialog_w);
    thread_dialog_popped_up = true;
}

void SourceView::ThreadDialogPoppedDownCB(Widget, XtPointer, XtPointer)
{
    thread_dialog_popped_up = false;
}

void SourceView::ThreadCommandCB(Widget w, XtPointer client_data, XtPointer)
{
    string command = (char *)client_data;

    // Get the selected threads
    IntArray threads;
    getDisplayNumbers(thread_list_w, threads);

    for (int i = 0; i < threads.size(); i++)
	command += " " + itostring(threads[i]);

    gdb_command(command, w);
}

void SourceView::SelectThreadCB(Widget w, XtPointer, XtPointer)
{
    // Get the selected threads
    IntArray threads;
    getDisplayNumbers(thread_list_w, threads);

    if (threads.size() == 1)
    {
	// Make single thread the default thread.
	gdb_command("thread " + itostring(threads[0]), w);
    }
    else if (threads.size() == 0 && gdb->type() == JDB)
    {
	// Check if we have selected a threadgroup
	XmStringTable selected_items;
	int selected_items_count = 0;

	XtVaGetValues(thread_list_w,
		      XmNselectedItemCount, &selected_items_count,
		      XmNselectedItems, &selected_items,
		      NULL);

	if (selected_items_count == 1)
	{
	    String _item;
	    XmStringGetLtoR(selected_items[0], LIST_CHARSET, &_item);
	    string item(_item);
	    XtFree(_item);
	    
	    // Output has the form `Group jtest.main:'
	    if (item.contains("Group ", 0))
	    {
		string threadgroup = item.after(" ");
		strip_leading_space(threadgroup);
		threadgroup = threadgroup.before(":");

		if (threadgroup == current_threadgroup)
		    threadgroup = "system"; // show all threadgroups

		gdb_command("threadgroup " + threadgroup, w);
	    }
	}
    }
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

    if (!is_current_file(file_name))
	read_file(file_name, line);
    if (!is_current_file(file_name))
	return "";

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


// Whether to cache glyph images
bool SourceView::cache_glyph_images = true;

// Change number of glyphs
void SourceView::set_max_glyphs (int nmax)
{
    WidgetArray empty(nmax);

    for (int k = 0; k < 2; k++)
    {
	int i;

	// Destroy old widgets...
	for (i = 0; i < plain_stops[k].size(); i++)
	{
	    if (plain_stops[k][i] != 0)
		XtDestroyWidget(plain_stops[k][i]);
	}
	for (i = 0; i < grey_stops[k].size(); i++)
	{
	    if (grey_stops[k][i] != 0)
		XtDestroyWidget(grey_stops[k][i]);
	}
	for (i = 0; i < plain_conds[k].size(); i++)
	{
	    if (plain_conds[k][i] != 0)
		XtDestroyWidget(plain_conds[k][i]);
	}
	for (i = 0; i < grey_conds[k].size(); i++)
	{
	    if (grey_stops[k][i] != 0)
		XtDestroyWidget(grey_conds[k][i]);
	}

	// ...make array empty...
	plain_stops[k] = empty;
	grey_stops[k]  = empty;
	plain_conds[k] = empty;
	grey_conds[k]  = empty;

	// ...and make room for new widgets.  The last one is a null pointer.
	for (i = 0; i < nmax + 1; i++)
	{
	    plain_stops[k] += Widget(0);
	    grey_stops[k]  += Widget(0);
	    plain_conds[k] += Widget(0);
	    grey_conds[k]  += Widget(0);
	}
    }
}


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

    if (!XtIsRealized(text_w))
	return;

    // Set up event such that it applies to the source window
    XButtonEvent *event = &e->xbutton;

    translate_glyph_pos(w, text_w, event->x, event->y);
    event->window = XtWindow(text_w);

    // Invoke action for source window
    String *params = { 0 };
    XtCallActionProc(text_w, "source-start-select-word", e, params, 0);
}


// Create a pixmap from BITS suitable for the widget W
Pixmap SourceView::pixmap(Widget w, unsigned char *bits, int width, int height)
{
    Pixel foreground, background;

    XtVaGetValues(w,
		  XmNforeground, &foreground,
		  XmNbackground, &background,
		  NULL);

    int depth = PlanesOfScreen(XtScreen(w));
    Pixmap pix = XCreatePixmapFromBitmapData(XtDisplay(w), XtWindow(w), 
					     (char *)bits, width, height, 
					     foreground, background, depth);
    return pix;
}


#if XmVERSION >= 2
const int motif_offset = 1;  // Motif 2.0 adds a 1 pixel border around glyphs
#else
const int motif_offset = 0;  // Motif 1.x does not
#endif

// Create glyph in FORM_W named NAME from given BITS
Widget SourceView::create_glyph(Widget form_w,
				String name,
				unsigned char *bits,
				int width, int height)
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

    if (XtIsRealized(form_w))
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
    if (!cache_glyph_images)
    {
	Pixmap pix = pixmap(w, bits, width, height);
	XtSetArg(args[arg], XmNlabelPixmap, pix); arg++;
    }
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
void SourceView::unmap_glyph(Widget glyph)
{
    if (glyph == 0)
	return;

    assert(is_code_widget(glyph) || is_source_widget(glyph));

    XtPointer user_data;
    XtVaGetValues(glyph, XmNuserData, &user_data, NULL);
    if (user_data == 0)
	return;			// Already unmapped

    if (change_glyphs)
    {
	const Position invisible_x = -100;
	const Position invisible_y = -100;

	// Unmapping the glyph while dragging breaks the drag.
	// Move the glyph to an invisible position instead.
	XtVaSetValues(glyph,
		      XmNleftOffset, invisible_x,
		      XmNtopOffset,  invisible_y,
		      XmNuserData, XtPointer(0),
		      NULL);

	if (lesstif_version < 1000)
	{
	    // LessTif wants it the hard way.
	    XtMoveWidget(glyph, invisible_x, invisible_y);
	}
	// log_glyph(glyph);
    }

    changed_glyphs += glyph;
}

// Map glyph GLYPH in (X, Y)
void SourceView::map_glyph(Widget& glyph, Position x, Position y)
{
    while (glyph == 0)
	CreateGlyphsWorkProc(0);

    assert(is_code_widget(glyph) || is_source_widget(glyph));

    // clog << "Mapping glyph at (" << x << ", " << y << ")\n";

    Widget text_w;
    if (is_source_widget(glyph))
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
    XtVaGetValues(glyph,
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

    y -= (line_height(text_w) + glyph_height) / 2 - 2;

    if (x != old_x || y != old_y)
    {
	if (change_glyphs)
	{
	    if (lesstif_version < 1000)
	    {
		// LessTif wants it the hard way.
		XtMoveWidget(glyph, x, y);
	    }

	    XtVaSetValues(glyph, XmNleftOffset, x, XmNtopOffset, y, NULL);
	    // log_glyph(glyph);
	}
	changed_glyphs += glyph;
    }

    if (user_data != 0)
	return;			// Already mapped

    if (change_glyphs)
    {
	XtMapWidget(glyph);
	XtVaSetValues(glyph, XmNuserData, XtPointer(1), NULL);
	// log_glyph(glyph);
	changed_glyphs += glyph;
    }
}


// True if code/source glyphs need to be updated
bool SourceView::update_code_glyphs   = false;
bool SourceView::update_source_glyphs = false;

// Update glyphs for widget GLYPH (0: all)
void SourceView::update_glyphs(Widget glyph)
{
    static XtWorkProcId update_glyph_id = 0;

    if (glyph == 0)
	update_source_glyphs = update_code_glyphs = true;
    else if (is_source_widget(glyph))
	update_source_glyphs = true;
    else if (is_code_widget(glyph))
	update_code_glyphs = true;

    if (update_glyph_id != 0)
	XtRemoveTimeOut(update_glyph_id);

    update_glyph_id = 
	XtAppAddTimeOut(XtWidgetToApplicationContext(source_text_w), 1,
			UpdateGlyphsWorkProc, XtPointer(&update_glyph_id));
}


// Invoked by scrolling keys
void SourceView::updateGlyphsAct(Widget w, XEvent*, String *, Cardinal *)
{
    CheckScrollCB(w, 0, 0);
}

// Invoked whenever the text widget may be about to scroll
void SourceView::CheckScrollCB(Widget, XtPointer, XtPointer)
{
    static XtIntervalId check_scroll_id = 0;

    if (check_scroll_id != 0)
    {
	XtRemoveTimeOut(check_scroll_id);
	check_scroll_id = 0;
    }

    check_scroll_id = 
	XtAppAddTimeOut(XtWidgetToApplicationContext(source_text_w),
			app_data.glyph_update_delay,
			CheckScrollWorkProc, XtPointer(&check_scroll_id));
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
Widget SourceView::temp_conds[2]    = {0, 0};
WidgetArray SourceView::plain_stops[2];
WidgetArray SourceView::grey_stops[2];
WidgetArray SourceView::plain_conds[2];
WidgetArray SourceView::grey_conds[2];


// Create glyphs in the background
Boolean SourceView::CreateGlyphsWorkProc(XtPointer)
{
    int k;
    for (k = 0; k < 2; k++)
    {
	// On the Form widget, later children are displayed on top of
	// earlier children.  A stop sign hiding an arrow gives more
	// pleasing results than vice-versa, so place arrow glyph
	// below sign glyphs.

	Widget form_w = k ? code_form_w : source_form_w;

	if (form_w == 0)
	    continue;

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

	if (form_w == 0)
	    continue;

	int i;
	for (i = 0; i < grey_stops[k].size() - 1; i++)
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

	for (i = 0; i < grey_conds[k].size() - 1; i++)
	{
	    if (grey_conds[k][i] == 0)
	    {
		grey_conds[k][i] = 
		    create_glyph(form_w, "grey_cond",
				 grey_cond_bits, 
				 grey_cond_width,
				 grey_cond_height);
		return False;
	    }
	}

	for (i = 0; i < plain_stops[k].size() - 1; i++)
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

	for (i = 0; i < plain_conds[k].size() - 1; i++)
	{
	    if (plain_conds[k][i] == 0)
	    {
		plain_conds[k][i] = 
		    create_glyph(form_w, "plain_cond",
				 cond_bits, 
				 cond_width,
				 cond_height);
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

	if (temp_conds[k] == 0)
	{
	    temp_conds[k] = 
		create_glyph(form_w, "temp_cond",
			     temp_cond_bits, 
			     temp_cond_width,
			     temp_cond_height);
	    return False;
	}
    }

    return True;		// all done
}

// Map stop sign GLYPH at position POS.  Get widget from STOPS[COUNT];
// store location in POSITIONS.  Return mapped widget (0 if none)
Widget SourceView::map_stop_at(Widget glyph, XmTextPosition pos,
			       WidgetArray& stops, int& count,
			       TextPositionArray& positions)
{
    assert (is_source_widget(glyph) || is_code_widget(glyph));

    Position x, y;
    Boolean pos_displayed = XmTextPosToXY(glyph, pos, &x, &y);
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
	else
	{
	    // Max number of glyphs exceeded
	    string msg = "Out of glyphs (used " + 
		itostring(stops.size() - 1) + " of " +
		itostring(stops.size() - 1) + ")";

	    set_status(msg);

	    static bool warning_posted = false;

	    if (!warning_posted)
	    {
		post_warning(msg, "out_of_glyphs_warning", glyph);
		warning_posted = true;
	    }
	}
    }

    return 0;
}

// Map arrow in GLYPH at POS.  Return mapped arrow widget (0 if none)
Widget SourceView::map_arrow_at(Widget glyph, XmTextPosition pos)
{
    assert (is_source_widget(glyph) || is_code_widget(glyph));

    Position x, y;
    Boolean pos_displayed = (pos != XmTextPosition(-1) 
			     && XmTextPosToXY(glyph, pos, &x, &y));

    int k = int(is_code_widget(glyph));

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

// Copy glyph foreground and background colors from ORIGIN to GLYPH
void SourceView::copy_colors(Widget glyph, Widget origin)
{
    if (origin == 0)
	return;

    Pixel background, foreground;
    XtVaGetValues(origin,
		  XmNforeground, &foreground,
		  XmNbackground, &background,
		  NULL);

    Pixmap pixmap = 
	XmGetPixmap(XtScreen(glyph), XtName(glyph), foreground, background);
    if (pixmap != XmUNSPECIFIED_PIXMAP)
    {
	Pixmap old_pixmap;
	XtVaGetValues(glyph, XmNlabelPixmap, &old_pixmap, NULL);
	XmDestroyPixmap(XtScreen(glyph), old_pixmap);

	XtVaSetValues(glyph, XmNlabelPixmap, pixmap, NULL);
    }
}

// Map temporary stop sign at position POS.  If ORIGIN is given, use
// colors from ORIGIN.
Widget SourceView::map_temp_stop_at(Widget glyph, XmTextPosition pos, 
				    Widget origin)
{
    assert (is_source_widget(glyph) || is_code_widget(glyph));
    Position x, y;
    Boolean pos_displayed = (pos != XmTextPosition(-1) 
			     && XmTextPosToXY(glyph, pos, &x, &y));

    int k = int(is_code_widget(glyph));

    if (pos_displayed)
    {
	bool cond = (origin != 0 && string(XtName(origin)).contains("cond"));

	Widget& temp_stop = cond ? temp_conds[k] : temp_stops[k];
	
	while (temp_stop == 0)
	{
	    if (CreateGlyphsWorkProc(0))
		break;
	}

	copy_colors(temp_stop, origin);

	if (origin)
 	    XtVaGetValues(origin, XmNx, &x, NULL);
	else
	    x += stop_x_offset;

	map_glyph(temp_stop, x, y);
	unmap_glyph(cond ? temp_stops[k] : temp_conds[k]);

	return temp_stop;
    }
    else
    {
	unmap_glyph(temp_stops[k]);
	unmap_glyph(temp_conds[k]);

	return 0;
    }
}

// Map temporary arrow at position POS.  If ORIGIN is given, use
// colors from ORIGIN.
Widget SourceView::map_temp_arrow_at(Widget glyph, XmTextPosition pos, 
				     Widget origin)
{
    assert (is_source_widget(glyph) || is_code_widget(glyph));
    Position x, y;
    Boolean pos_displayed = (pos != XmTextPosition(-1) 
			     && XmTextPosToXY(glyph, pos, &x, &y));

    int k = int(is_code_widget(glyph));

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

	if (display_glyphs &&
	    (is_current_file(last_execution_file) ||
	     base_matches(last_execution_file, current_file_name)) &&
	     line_count > 0 &&
	     last_execution_line > 0 &&
	     last_execution_line <= line_count)
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

	int plain_stops_count = 0;
	int grey_stops_count  = 0;
	int plain_conds_count = 0;
	int grey_conds_count  = 0;

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

		if (bp->condition() != "" || bp->ignore_count() != 0)
		{
		    if (bp->enabled())
			bp_glyph = map_stop_at(text_w, pos, plain_conds[k],
					       plain_conds_count, positions);
		    else
			bp_glyph = map_stop_at(text_w, pos, grey_conds[k],
					       grey_conds_count, positions);
		}
		else
		{
		    if (bp->enabled() && bp->ignore_count() == 0)
			bp_glyph = map_stop_at(text_w, pos, plain_stops[k],
					       plain_stops_count, positions);
		    else
			bp_glyph = map_stop_at(text_w, pos, grey_stops[k],
					       grey_stops_count, positions);
		}
	    }
	}

	// Unmap remaining breakpoint glyphs
	Widget glyph;
	while ((glyph = plain_stops[k][plain_stops_count++]))
	    unmap_glyph(glyph);
	while ((glyph = grey_stops[k][grey_stops_count++]))
	    unmap_glyph(glyph);
	while ((glyph = plain_conds[k][plain_conds_count++]))
	    unmap_glyph(glyph);
	while ((glyph = grey_conds[k][grey_conds_count++]))
	    unmap_glyph(glyph);
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
	// Save current execution position
	string file   = last_execution_file;
	int    line   = last_execution_line;
	string pc     = last_execution_pc;
	bool stopped  = at_lowest_frame;
	bool signaled = signal_received;

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
	    if (file != "")
		show_execution_position(file + ":" + itostring(line), 
					stopped, signaled);
	    if (pc != "")
		show_pc(pc, XmHIGHLIGHT_SELECTED);
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
MString SourceView::help_on_glyph(Widget glyph, bool detailed)
{
    XmTextPosition dummy;
    return help_on_pos(glyph, 0, dummy, detailed);
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
	strip_space(infos);
	infos.gsub("\n", "; ");

	if (bp->infos() != "")
	    info += rm("; " + infos);

	switch (bp->dispo())
	{
	case BPKEEP:
	    break;
	case BPDEL:
	    info += rm("; delete when reached");
	    break;
	case BPDIS:
	    info += rm("; disable when reached");
	    break;
	}
	info += rm(")");
    }

    return info;
}


// Glyph drag & drop

XmTextPosition SourceView::glyph_position(Widget glyph, XEvent *e, 
					  bool normalize)
{
    Widget text_w;
    if (is_source_widget(glyph))
	text_w = source_text_w;
    else if (is_code_widget(glyph))
	text_w = code_text_w;
    else
	return XmTextPosition(-1);

    BoxPoint p = point(e);
    if (glyph != source_text_w && glyph != code_text_w)
    {
	// Called from a glyph: add glyph position to event position
	translate_glyph_pos(glyph, text_w, p[X], p[Y]);
    }

    // Get the position
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
	const string& text = current_text(glyph);
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

void SourceView::dragGlyphAct(Widget glyph, XEvent *e, String *, Cardinal *)
{
    if (e->type != ButtonPress && e->type != ButtonRelease)
	return;

    Widget text_w;
    if (is_source_widget(glyph))
	text_w = source_text_w;
    else if (is_code_widget(glyph))
	text_w = code_text_w;
    else
	return;			// Bad widget

    if (!XtIsRealized(text_w))
	return;

    int k;
    for (k = 0; k < 2; k++)
    {
	if (glyph == grey_arrows[k])
	{
	    // Cannot drag last execution position
	    return;
	}
	else if (glyph == plain_arrows[k])
	{
	    if (!gdb->has_jump_command() && !gdb->has_assign_command())
	    {
		// Execution position cannot be dragged
		return;
	    }
	}
	else if (glyph == temp_stops[k] || 
		 glyph == temp_conds[k] || 
		 glyph == temp_arrows[k])
	{
	    // Temp glyph cannot be dragged
	    return;
	}
    }

    static Cursor move_cursor = XCreateFontCursor(XtDisplay(glyph), XC_fleur);

    // clog << "Dragging " << XtName(glyph) << " [" << glyph << "]\n";

    XDefineCursor(XtDisplay(glyph), XtWindow(glyph), move_cursor);

    unmap_temp_stop(text_w);
    unmap_temp_arrow(text_w);

    current_drag_origin     = glyph;
    current_drag_breakpoint = 0;

    // Check for breakpoint
    MapRef ref;
    for (BreakPoint *bp = bp_map.first(ref); bp != 0; bp = bp_map.next(ref))
    {
	if (glyph == bp->source_glyph() || glyph == bp->code_glyph())
	{
	    current_drag_breakpoint = bp->number();
	    break;
	}
    }
}

void SourceView::followGlyphAct(Widget glyph, XEvent *e, String *, Cardinal *)
{
    if (glyph != current_drag_origin)
	return;

    Widget text_w;
    if (is_source_widget(glyph))
	text_w = source_text_w;
    else if (is_code_widget(glyph))
	text_w = code_text_w;
    else
	return;			// Bad widget

    XmTextPosition pos = glyph_position(glyph, e);

    // Make sure we see the position
    ShowPosition(text_w, pos);

    // Update glyphs in case we had to scroll
    CheckScrollCB(glyph, XtPointer(0), XtPointer(0));

    if (current_drag_breakpoint)
	map_temp_stop_at(text_w, pos, glyph);
    else
	map_temp_arrow_at(text_w, pos, glyph);
}

void SourceView::dropGlyphAct (Widget glyph, XEvent *e, 
			       String *params, Cardinal *num_params)
{
    if (e->type != ButtonPress && e->type != ButtonRelease)
	return;

    if (glyph != current_drag_origin)
	return;

    Widget text_w;
    if (is_source_widget(glyph))
	text_w = source_text_w;
    else if (is_code_widget(glyph))
	text_w = code_text_w;
    else
	return;			// Bad widget

    if (!XtIsRealized(text_w))
	return;

    XUndefineCursor(XtDisplay(glyph), XtWindow(glyph));

    // Unmap temp glyphs
    unmap_temp_stop(text_w);
    unmap_temp_arrow(text_w);

    // Show all other glyphs
    update_glyphs();

    int k;
    for (k = 0; k < 2; k++)
	if (glyph == grey_arrows[k] || 
	    glyph == temp_stops[k] || 
	    glyph == temp_conds[k] || 
	    glyph == temp_arrows[k])
	    return;

    XmTextPosition pos = glyph_position(glyph, e);
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

    // clog << "Dropping " << XtName(glyph) << " [" << glyph << "] at " 
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

    string p = "move";
    if (num_params != 0 && *num_params == 1)
	p = params[0];
    if (num_params != 0 && *num_params > 1)
	cerr << "source-drop-glyph: too many parameters\n";
    p.downcase();

    bool copy = false;
    if (p == "move")
	copy = false;
    else if (p == "copy")
	copy = true;
    else
	cerr << "source-drop-glyph: unknown parameter " << quote(p) << "\n";

    if (current_drag_breakpoint)
    {
	// Move breakpoint
	move_bp(current_drag_breakpoint, address, text_w, copy);
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

// Report glyph state (for debugging)
void SourceView::log_glyph(Widget glyph, int n)
{
    if (glyph == 0)
	return;
    
    int left = 0;
    int top  = 0;
    Position x = 0;
    Position y = 0;
    XtPointer user_data;
    XtVaGetValues(glyph,
		  XmNuserData,           &user_data,
		  XmNleftOffset,         &left,
		  XmNtopOffset,          &top,
		  XmNx,                  &x,
		  XmNy,                  &y,
		  NULL);

    clog << XtName(glyph);
    if (n >= 0)
	clog << "s[" << n << "]";
    clog << ": ";
    if (user_data)
	clog << "mapped";
    else
	clog << "unmapped";

    clog << " at (" << left << ", " << top << " / "
	 << x << ", " << y << ")\n";
}

void SourceView::log_glyphs()
{
    for (int k = 0; k < 2; k++)
    {
	if (k && !disassemble)
	    continue;

	if (k == 0)
	    clog << "Source glyphs:\n";
	else
	    clog << "\nCode glyphs:\n";

	int i;
	for (i = 0; i < plain_stops[k].size() - 1; i++)
	    log_glyph(plain_stops[k][i], i);
	for (i = 0; i < grey_stops[k].size() - 1; i++)
	    log_glyph(grey_stops[k][i], i);
	for (i = 0; i < plain_conds[k].size() - 1; i++)
	    log_glyph(plain_conds[k][i], i);
	for (i = 0; i < grey_conds[k].size() - 1; i++)
	    log_glyph(grey_conds[k][i], i);
	log_glyph(plain_arrows[k]);
	log_glyph(grey_arrows[k]);
	log_glyph(signal_arrows[k]);
	log_glyph(temp_arrows[k]);
	log_glyph(temp_stops[k]);
	log_glyph(temp_conds[k]);
    }
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
	untabify_if_needed(line);
	if (line.length() > 0 && line[0] == '0')
	    line = replicate(' ', indent_amount(code_text_w)) + line;
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
    StatusDelay *delay;

    RefreshInfo(const string& p, XmHighlightMode m, const string& msg);

private:
    RefreshInfo(const RefreshInfo& info)
	: pc(info.pc), mode(info.mode), delay(0)
    {
	assert(0);
    }
    RefreshInfo& operator = (const RefreshInfo&)
    {
	assert(0); return *this;
    }
};

// EGCS 1.0 wants this to be non-inlined
RefreshInfo::RefreshInfo(const string& p, XmHighlightMode m, const string& msg)
    : pc(p), mode(m), delay(new StatusDelay(msg))
{}

// Process `disassemble' output
void SourceView::refresh_codeOQC(const string& answer, void *client_data)
{
    RefreshInfo *info = (RefreshInfo *)client_data;

    if (answer == NO_GDB_ANSWER)
    {
	info->delay->outcome = "failed";
    }
    else
    {
	process_disassemble(answer);

	if (find_pc(info->pc) != XmTextPosition(-1))
	    show_pc(info->pc, info->mode);
    }

    delete info->delay;
    delete info;
}

void SourceView::normalize_address(string& addr)
{
    addr.downcase();
    if (addr.contains("0", 0))
	addr = addr.after("0");
    if (addr.contains("x", 0))
	addr = addr.after("x");
    if (addr.contains("h'", 0))
	addr = addr.after("h'");
    if (addr.contains("h", -1))
	addr = addr.before(int(addr.length()) - 1);
    addr.prepend("0x");
}

string SourceView::make_address(long pc)
{
    ostrstream os;
    os << "0x" << setbase(16) << (unsigned long)pc;
    return string(os);
}

// Return FUNCTION and OFFSET at ADDRESS
void SourceView::get_func_at(const string& address, string& func, int& offset)
{
    // GDB issues /i lines in the format
    // `ADDR <FUNC[+OFFSET]> INSTRUCTIONS', as in
    // `0xef7be49c <_IO_file_underflow+128>:\torcc  %o0, %g0, %o2\n'
    offset = 0;
    func = gdb_question("x /i " + address);
    if (func == NO_GDB_ANSWER)
	return;

    // Find func
    func = func.after("<");
    func = func.before(">");

    // Find offset
    int plus = func.index('+');
    if (plus >= 0)
    {
	offset = atoi(func.chars() + plus + 1);
	func = func.before(plus);
    }
}

// Return TRUE iff the function at PC is larger than MAX_SIZE.
bool SourceView::function_is_larger_than(string pc, int max_size)
{
    if (gdb->type() != GDB)
	return false;

    // Get function name at PC
    normalize_address(pc);
    string pc_func;
    int pc_offset;
    get_func_at(pc, pc_func, pc_offset);
    if (pc_func == NO_GDB_ANSWER)
	return true;		// In doubt, treat function as `too large'.

    if (pc_offset > max_size)
    {
	// We're already more than MAX_SIZE bytes away from the
	// function start: function is too large.
	return true;
    }

    // Get the function name at function start + MAX_SIZE.  If this is
    // the same name as the name at start, the function is too large.
    long pc_l = strtol(pc.chars(), NULL, 0);
    long next_l = pc_l - pc_offset + max_size;
    string next = make_address(next_l);

    string next_func;
    int next_offset;
    get_func_at(next, next_func, next_offset);

    if (next_func == NO_GDB_ANSWER)
	return true;		// In doubt, treat function as `too large'.

    if (pc_func == next_func)
    {
	// We're still within the same function: function is too large.
	return true;
    }

    return false;
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

	string start = pc;
	string end   = "";
	if (app_data.max_disassemble > 0 && 
	    function_is_larger_than(pc, app_data.max_disassemble))
	{
	    // Disassemble only MAX_DISASSEMBLE bytes after PC
	    long pc_l = strtol(pc.chars(), NULL, 0);
	    end = make_address(pc_l + app_data.max_disassemble);
	}

	string msg = "Disassembling location " + start;
	if (end != "")
	    msg += " to " + end;

	RefreshInfo *info = new RefreshInfo(pc, mode, msg);

	gdb_command(gdb->disassemble_command(start, end), 0,
		    refresh_codeOQC, (void *)info);
	return;
    }

    if (pos == XmTextPosition(-1))
	return;

    SetInsertionPosition(code_text_w, pos + indent_amount(code_text_w));
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
	    int indent = indent_amount(code_text_w);
	    string marker = ">";
	    if (last_pos_pc)
	    {
		string no_marker = replicate(' ', marker.length());
		XmTextReplace (code_text_w,
			       last_pos_pc + indent - no_marker.length(),
			       last_pos_pc + indent,
			       (String)no_marker);
	    }

	    XmTextReplace (code_text_w,
			   pos + indent - marker.length(),
			   pos + indent,
			   (String)marker);
    
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
	    unmanage_paned_child(code_form_w);
	}
	else
	{
	    manage_paned_child(code_form_w);

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
	case JDB:
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
	    && bp_matches(bp, file, line_no))
	    {
		if (bps != "")
		    bps += gdb->wants_delete_comma() ? ", " : " ";
		bps += itostring(bp->number());
		max_bp_nr = max(max_bp_nr, bp->number());
	    }
    }

    if (bps == "")
	return "";

    if (clear_next && max_bp_nr >= 0)
    {
	bps += (gdb->wants_delete_comma() ? ", " : " ");
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
	return bp->pos();
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
    case JDB:
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
    // Delete all breakpoints
    if (gdb->has_delete_command())
    {
	string del = gdb->delete_command();

	MapRef ref;
	int n = 0;
	for (BreakPoint *bp = bp_map.first(ref); bp != 0; 
	     bp = bp_map.next(ref))
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
    else if (gdb->has_clear_command())
    {
	MapRef ref;
	for (BreakPoint *bp = bp_map.first(ref); bp != 0; 
	     bp = bp_map.next(ref))
	{
	    Command c(clear_command(bp->pos()));
	    c.verbose  = false;
	    c.check    = true;
	    c.priority = COMMAND_PRIORITY_INIT;
	    c.callback = reset_done;
	    gdb_command(c);
	}
    }
}
