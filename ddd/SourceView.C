// $Id$
// Use the Source, Luke.

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Dorothea Luetkehaus (luetke@ips.cs.tu-bs.de) 
// and Andreas Zeller (zeller@ips.cs.tu-bs.de) 
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

#ifdef __GNUG__
#pragma implementation
#pragma implementation "Assoc.h"
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
// Written by David A. Lyons (dlyons@apple.com), January 1994
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
#include "MString.h"
#include "events.h"
#include "cook.h"
#include "misc.h"
#include "wm.h"

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
#include <X11/StringDefs.h>

// System stuff
extern "C" {
#include <sys/types.h>
#include <sys/stat.h>
#include <libiberty.h> // basename()
}
#include <stdio.h>
#include <fcntl.h>
#include <iomanip.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// DDD stuff
#include "PosBuffer.h"
#include "string-fun.h"
#include "ddd.h"
#include "version.h"
#include "mydialogs.h"
#include "verify.h"
#include "commandQ.h"
#include "post.h"
#include "shell.h"
#include "windows.h"
#include "wm.h"
#include "dbx-lookup.h"
#include "question.h"
#include "status.h"
#include "file.h"
#include "AppData.h"

// Glyphs
#include "arrow.xbm"
#include "stop.xbm"
#include "greystop.xbm"
#include "greyarrow.xbm"

// Additional macros
inline int isid(char c)
{
    return isalnum(c) || c == '_';
}

// Test for regular file - see stat(3)
#ifndef S_ISREG
#define S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#endif


// ***************************************************************************
//-----------------------------------------------------------------------
// Xt-Zeugs
//-----------------------------------------------------------------------
XtActionsRec SourceView::actions [] = {
    {"source-popup-menu",        SourceView::srcpopupAct        },
    {"source-start-select-word", SourceView::startSelectWordAct },
    {"source-end-select-word",   SourceView::endSelectWordAct   },
    {"source-update-glyphs",     SourceView::updateGlyphsAct    },
};

//-----------------------------------------------------------------------
// Menues
//-----------------------------------------------------------------------

// Popup-Menues - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
struct LineItms { enum Itms {SetBP, SetTempBP, TempNContBP}; };
MMDesc SourceView::line_popup[] =
{
    {"set",         MMPush, {SourceView::line_popup_setCB}},
    {"set_temp",    MMPush, {SourceView::line_popup_set_tempCB}},
    {"temp_n_cont", MMPush, {SourceView::line_popup_temp_n_contCB}},
    MMEnd
};

struct AddressItms { enum Itms {SetBP, SetTempBP, TempNContBP}; };
MMDesc SourceView::address_popup[] =
{
    {"set",         MMPush, {SourceView::address_popup_setCB}},
    {"set_temp",    MMPush, {SourceView::address_popup_set_tempCB}},
    {"temp_n_cont", MMPush, {SourceView::address_popup_temp_n_contCB}},
    MMEnd
};

struct BPItms { enum Itms {Disable, Sep1, Condition, IgnoreCount, 
			   Sep2, Delete}; };
MMDesc SourceView::bp_popup_gdb[] =
{
    {"disable",      MMPush, {SourceView::bp_popup_disableCB}},
    MMSep,
    {"condition",    MMPush, {SourceView::EditBreakpointConditionCB}},
    {"ignore_count", MMPush, {SourceView::EditBreakpointIgnoreCountCB}},
    MMSep,
    {"delete",       MMPush, {SourceView::bp_popup_deleteCB}},
    MMEnd
};

MMDesc SourceView::bp_popup_dbx[] =
{
    {"delete",       MMPush, {SourceView::bp_popup_deleteCB}},
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
    {"print_ref",  MMPush, {SourceView::text_popup_print_refCB}},
    {"disp_ref",   MMPush, {SourceView::text_popup_disp_refCB}},
    MMSep,
    {"whatis",     MMPush, {SourceView::text_popup_whatisCB}},
    MMSep,
    {"lookup",     MMPush, {SourceView::text_popup_lookupCB}},
    {"break",      MMPush, {SourceView::text_popup_breakCB}},
    {"clear",      MMPush, {SourceView::text_popup_clearCB}},
    MMEnd
};


//-----------------------------------------------------------------------
// Daten
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

bool SourceView::stack_dialog_popped_up    = false;
bool SourceView::register_dialog_popped_up = false;

bool SourceView::cache_source_files     = true;
bool SourceView::cache_machine_code     = true;
bool SourceView::display_glyphs         = true;
bool SourceView::disassemble            = true;

int  SourceView::bp_indent_amount   = 0;
int  SourceView::code_indent_amount = 4;

Map<int, BreakPoint> SourceView::bp_map;

string SourceView::current_file_name = "";
int    SourceView::line_count = 0;
Assoc<int, VarArray<int> > SourceView::bps_in_line;
XmTextPosition*             SourceView::pos_of_line = 0;
StringArray SourceView::bp_addresses;
Assoc<string, string> SourceView::file_cache;
Assoc<string, string> SourceView::source_name_cache;
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
string SourceView::last_execution_pc = "";

StringArray SourceView::history;
int SourceView::history_position = 0;
bool SourceView::code_history_locked = false;
bool SourceView::source_history_locked = false;

bool SourceView::at_lowest_frame = true;

//-----------------------------------------------------------------------
// Helping functions
//-----------------------------------------------------------------------

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

//-----------------------------------------------------------------------
// Methoden
//-----------------------------------------------------------------------

// ***************************************************************************
//
void SourceView::other_fileCB (Widget w,
			       XtPointer,
			       XtPointer)
{
    String filename = XmTextFieldGetString(w);
    read_file (filename);
    XtFree (filename);
}

// ***************************************************************************
//
void SourceView::line_popup_setCB (Widget w,
				   XtPointer client_data,
				   XtPointer)
{
    int line_nr = *((int *)client_data);
    switch (gdb->type())
    {
    case GDB:
	gdb_command("break " + current_source_name() + ":" + 
		    itostring(line_nr), w);
	break;
	
    case DBX:
	gdb_command("file " + full_path(current_file_name), w);
	gdb_command("stop at " + itostring(line_nr), w);
	break;

    case XDB:
	gdb_command("b " + current_source_name() + ":" + 
		    itostring(line_nr), w);
    }
}

void SourceView::address_popup_setCB (Widget w,
				      XtPointer client_data,
				      XtPointer)
{
    string address = *((string *)client_data);
    switch (gdb->type())
    {
    case GDB:
	gdb_command("break *" + address, w);
	break;
	
    case DBX:
	gdb_command("stopi at " + address, w);
	break;

    case XDB:
	gdb_command("b " + address, w);
	break;
    }
}

// ***************************************************************************
//
void SourceView::line_popup_set_tempCB (Widget w,
					XtPointer client_data,
					XtPointer)
{
    int line_nr = *((int *)client_data);
    switch (gdb->type())
    {
    case GDB:
	gdb_command("tbreak " + current_source_name() + ":" + 
		    itostring(line_nr), w);
	break;

    case DBX:
	gdb_command("file " + full_path(current_file_name), w);
	gdb_command("stop at " + itostring(line_nr), w);

	// Make sure we get the number of the temporary breakpoint
	syncCommandQueue();
	{
	    string line = itostring(line_nr);
	    gdb_command("when at " + line + " " 
			+ command_list(clear_command(line)), w);
	}
	break;

    case XDB:
	gdb_command("b " + current_source_name() + ":" + itostring(line_nr)
		    + " \\1t", w);
	break;
    }
}

void SourceView::address_popup_set_tempCB (Widget w,
					   XtPointer client_data,
					   XtPointer)
{
    string address = *((string *)client_data);
    switch (gdb->type())
    {
    case GDB:
	gdb_command("tbreak *" + address, w);
	break;

    case DBX:
	gdb_command("stopi at " + address, w);

	// Make sure we get the number of the temporary breakpoint
	syncCommandQueue();
	gdb_command("when $pc == " + address + " "
		    + command_list(clear_command(address)), w);
	break;

    case XDB:
	gdb_command("ba " + address + " \\1t", w);
	break;
    }
}

// ***************************************************************************
//
void SourceView::line_popup_temp_n_contCB (Widget w,
					   XtPointer client_data,
					   XtPointer call_data)
{
    int line_nr = *((int *)client_data);
    switch (gdb->type())
    {
    case GDB:
	gdb_command("until " + current_source_name() + ":" + 
		    itostring(line_nr), w);
	break;
    
    case DBX:
	line_popup_set_tempCB(w, client_data, call_data);
	gdb_command("cont", w);
	break;

    case XDB:
	gdb_command("c " + current_source_name() + ":" + 
		    itostring(line_nr), w);
	break;
    }
}

void SourceView::address_popup_temp_n_contCB (Widget w,
					      XtPointer client_data,
					      XtPointer call_data)
{
    string address = *((string *)client_data);
    switch (gdb->type())
    {
    case GDB:
	gdb_command("until *" + address, w);
	break;
    
    case DBX:
	line_popup_set_tempCB(w, client_data, call_data);
	gdb_command("cont", w);
	break;

    case XDB:
	gdb_command("c " + address, w);
	break;
    }
}

// ***************************************************************************
//
void SourceView::bp_popup_deleteCB (Widget w,
				    XtPointer client_data,
				    XtPointer)
{
    int bp_nr = *((int *)client_data);

    string cmd;
    switch (gdb->type())
    {
    case GDB:
    case DBX:
	cmd = "delete ";
	break;

    case XDB:
	cmd = "db ";
	break;
    }

    gdb_command(cmd + itostring(bp_nr), w);
}


// ***************************************************************************
//
void SourceView::bp_popup_disableCB (Widget w, 
				     XtPointer client_data,
				     XtPointer)
{
    int bp_nr = *((int *)client_data);

    string cmd;
    switch (gdb->type())
    {
    case GDB:
    case DBX:
	if (bp_map.get(bp_nr)->enabled())
	    cmd = "disable ";
	else
	    cmd = "enable ";
	break;

    case XDB:
	if (bp_map.get(bp_nr)->enabled())
	    cmd = "sb ";
	else
	    cmd = "ab ";
	break;
    }

    cmd += itostring(bp_nr);
    gdb_command(cmd, w);
}

// ***************************************************************************
//
void SourceView::text_popup_breakCB (Widget w,
				     XtPointer client_data,
				     XtPointer)
{
    string* word_ptr = (string*)client_data;
    assert(word_ptr->length() > 0);
    string pos;

    switch (gdb->type())
    {
    case GDB:
	gdb_command("break " + *word_ptr, w);
	break;

    case DBX:
	pos = dbx_lookup(*word_ptr);
	if (pos != "")
	{
	    gdb_command("file " + pos.before(':'), w);
	    gdb_command("stop at " + pos.after(':'), w);
	}
	break;

    case XDB:
	gdb_command("b " + *word_ptr, w);
	break;
    }
}

void SourceView::text_popup_clearCB (Widget w, 
				     XtPointer client_data, 
				     XtPointer)
{
    string* word_ptr = (string*)client_data;
    assert(word_ptr->length() > 0);
    string pos;

    switch (gdb->type())
    {
    case GDB:
    case XDB:
	gdb_command(clear_command(*word_ptr), w);
	break;

    case DBX:
	pos = dbx_lookup(*word_ptr);
	if (pos != "")
	{
	    gdb_command("file " + pos.before(':'), w);
	    gdb_command(clear_command(pos.after(':')), w);
	}
	break;
    }
}



// ***************************************************************************
//
void SourceView::text_popup_printCB (Widget w, 
				     XtPointer client_data, 
				     XtPointer)
{
    string* word_ptr = (string*)client_data;
    assert(word_ptr->length() > 0);

    gdb_command(gdb->print_command(*word_ptr), w);
}

void SourceView::text_popup_print_refCB (Widget w, 
					 XtPointer client_data, XtPointer)
{
    string* word_ptr = (string*)client_data;
    assert(word_ptr->length() > 0);

    gdb_command(gdb->print_command("*(" + *word_ptr + ")"), w);
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

    gdb_command("graph display *(" + *word_ptr + ")", w);
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
    static regex RXdotdot("/[^/]*/\\.\\./");

    if (file == "")
	return current_pwd;

    if (file[0] != '/')
	file = current_pwd + "/" + file;

    file.gsub(RXdotdot, "/");
    file.gsub("/./", "/");

    return file;
}

bool SourceView::file_matches(const string& file1, const string& file2)
{
    return file1 == file2 || full_path(file1) == full_path(file2);
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
				  XtPointer)
{
    string& text = current_text(text_w);
    if (text == "")
	return;

    if (bool(client_data))
	selection_click = false;

    XmTextPosition startPos, endPos;
    if (XmTextGetSelectionPosition(text_w, &startPos, &endPos))
    {
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
	    if (text_w == source_text_w
		&& startPos - startIndex <= bp_indent_amount
		&& endPos - endIndex <= bp_indent_amount)
	    {
		// Selection from line number area: prepend source file name
		string line = text(startIndex, bp_indent_amount);
		int line_nr = atoi(line);
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
	    else if (text_w == code_text_w
		     && startPos - startIndex <= code_indent_amount
		     && endPos - endIndex <= code_indent_amount)
	    {
		// Selection from address area
		int index = text.index("0x", startPos);
		if (index >= 0)
		{
		    string address = text.from(index);
		    address = address.through(rxalphanum);

		    source_arg->set_string(address);

		    // If a breakpoint is here, select this one only
		    MapRef ref;
		    for (BreakPoint* bp = bp_map.first(ref);
			 bp != 0;
			 bp = bp_map.next(ref))
		    {
			bp->selected() = 
			    (bp->type() == BREAKPOINT
			     && (compare_address(address, 
						 bp->address()) == 0));
		    }
		
		    bp_selected = true;
		}
	    }
	}

	if (bp_selected)
	{
	    // Update breakpoint selection
	    fill_labels(last_info_output);
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

	    source_arg->set_string(s);
	}
    }
}


// ***************************************************************************

// Set insertion position for TEXT_W to POS, scrolling nicely
void SourceView::SetInsertionPosition(Widget text_w,
				      XmTextPosition pos, 
				      bool fromTop)
{
    string& text = current_text(text_w);

    // Number of lines to show before or after POS
    const int lines_above = 2;
    const int lines_below = 4;

    short rows = 0;
    XmTextPosition current_top = 0;
    XtVaGetValues(text_w,
		  XmNrows, &rows,
		  XmNtopCharacter, &current_top,
		  NULL);

    // Find current relative row
    short relative_row = 1;
    for (XmTextPosition p = pos; p > current_top; p--)
	if (text[p] == '\n')
	    relative_row++;

    if (relative_row <= lines_above || relative_row >= rows - lines_below)
    {
	// Determine new TOP position
	short n = rows / 2;	// #Lines between new TOP and POS
	if (fromTop || relative_row <= lines_above)
	    n = lines_above;
	else if (relative_row >= rows - lines_below)
	    n = rows - lines_below;

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

    XmTextSetInsertionPosition(text_w, pos);
    XmTextShowPosition(text_w, pos);	// just to make sure
}

// ***************************************************************************

// Read local file from FILE_NAME
String SourceView::read_local(const string& file_name, long& length)
{
    StatusDelay delay("Reading file " + quote(file_name));
    length = 0;

    // Make sure the file is a regular text file and open it
    int fd;
    if ((fd = open(file_name, O_RDONLY)) < 0)
    {
	post_error (file_name + ": " + strerror(errno), 
		    "source_file_error", source_text_w);
        return 0;
    }

    struct stat statb;
    if (fstat(fd, &statb) < 0)
    {
	post_error (file_name + ": " + strerror(errno), 
		    "source_file_error", source_text_w);
	return 0;
    }

    // Avoid loading from directory, socket, device, or otherwise.
    if (!S_ISREG(statb.st_mode))
    {
	post_error (file_name + ": not a regular file", 
		    "source_file_error", source_text_w);
	return 0;
    }

    // Put the contents of the file in the Text widget by allocating
    // enough space for the entire file and reading the file into the
    // allocated space.
    char* text = XtMalloc(unsigned(statb.st_size + 1));
    if ((length = read(fd, text, statb.st_size)) != statb.st_size)
	post_error (file_name + ": " + strerror(errno),
		    "source_trunc_error", source_text_w);
    close(fd);

    text[statb.st_size] = '\0'; // be sure to null-terminate

    if (statb.st_size == 0)
	post_warning("File " + quote(file_name) + " is empty.", 
		     "source_empty_warning", source_text_w);
    return text;
}


// Read (possibly remote) file FILE_NAME; a little slower
String SourceView::read_remote(const string& file_name, long& length)
{
    StatusDelay delay("Reading file " + 
		      quote(file_name) + " from remote host");
    length = 0;

    string cat_command = sh_command(string("cat") + " " + file_name);

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
	post_error("Cannot access remote file " + quote(file_name), 
		   "remote_file_error", source_text_w);
    return text;
}


// Read file FILE_NAME via the GDB `list' function
// Really slow, is guaranteed to work for source files.
String SourceView::read_from_gdb(const string& file_name, long& length)
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
	break;			// FIXME
    }
    string listing = gdb_question(command, -1);

    // GDB listings have the format <NUMBER>\t<LINE>.
    // Copy LINE only; line numbers will be re-added later.
    // Note that tabs may be expanded to spaces due to a PTY interface.
    String text = XtMalloc(listing.length());

    int i = 0;
    length = 0;
    while (i < int(listing.length()))
    {
	while (i < int(listing.length()) && isspace(listing[i]))
	    i++;

	if (isdigit(listing[i]))
	{
	    // Skip line number
	    int count = 0;
	    while (i < int(listing.length()) && isdigit(listing[i]))
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
	    post_gdb_message(msg, source_text_w);
	}
    }

    text[length] = '\0';  // be sure to null-terminate
    return text;
}


// Read file FILE_NAME and format it
String SourceView::read_indented(string& file_name, long& length)
{
    length = 0;
    Delay delay;
    long t;

    file_name = full_path(file_name);

    String text;
    if (!remote_gdb())
	text = read_local(file_name, length);
    else
	text = read_remote(file_name, length);

    if (text == 0 || length == 0)
    {
	string source_name = basename(file_name.chars());
	text = read_from_gdb(source_name, length);

	if (text != 0 && text[0] != '\0')
	{
#if 0
	    post_warning("File was read as source from GDB.",
			 "source_file_from_gdb_warning", source_text_w);
#endif
	    file_name = source_name;
	}
    }

    if (text == 0 || length == 0)
	return 0;

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
    indented_text_length += bp_indent_amount * lines;

    String indented_text = XtMalloc(indented_text_length + 1);

    char *line_no_s = new char[bp_indent_amount];
    for (int i = 0; i < bp_indent_amount; i++)
	line_no_s[i] = ' ';

    t = 0;
    char *pos_ptr = indented_text; // Schreibposition in indented_text
    while (t < length)
    {
	// Increase line number
	int i;
	for (i = bp_indent_amount - 2; i >= 0; i--)
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
	for (i = 0; i < bp_indent_amount; i++)
	    *pos_ptr++ = line_no_s[i];

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
int SourceView::read_current(string& file_name, bool force_reload)
{
    if (cache_source_files && !force_reload && file_cache.has(file_name))
    {
	current_source = file_cache[file_name];
    }
    else
    {
	long length = 0;
	String indented_text = read_indented(file_name, length);
	if (indented_text == 0 || length == 0)
	    return -1;

	current_source = string(indented_text, length);
	XtFree(indented_text);

	file_cache[file_name] = current_source;

	int null_count = current_source.freq('\0');
	if (null_count > 0)
	    post_warning("File " + quote(file_name) + " is a binary file.", 
			 "source_binary_warning", source_text_w);
    }

    // Setup global parameters

    // Number of lines
    line_count = current_source.freq('\n');

    // Line positions
    if (pos_of_line != 0)
    {
	delete[] pos_of_line;
	pos_of_line = 0;
    }
    pos_of_line = new XmTextPosition[line_count + 2];
    pos_of_line[0] = XmTextPosition(0);
    pos_of_line[1] = XmTextPosition(0);

    int l = 2;
    for (int i = 0; i < int(current_source.length()) - 1; i++)
	if (current_source[i] == '\n')
	    pos_of_line[l++] = XmTextPosition(i + 1);

    assert(l == line_count + 1);

    return 0;
}

// Clear the file cache
void SourceView::clear_file_cache()
{
    static Assoc<string, string> empty;
    file_cache        = empty;
    source_name_cache = empty;
}

void SourceView::reload()
{
    // Reload current file
    if (current_file_name == "")
	return;

    string pos  = line_of_cursor();
    string line = pos.after(':');
    string file = full_path(current_file_name);

    StatusDelay delay("Reloading " + quote(file));

    read_file(file, atoi(line), true);
}

void SourceView::read_file (string file_name, 
			    int initial_line,
			    bool force_reload)
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
    int error = read_current(file_name, force_reload);
    if (error)
	return;

    add_to_history(file_name, initial_line);

    // The remainder may take some time...
    Delay delay;

    // Set string and initial line
    XtVaSetValues(source_text_w,
		  XmNvalue, (String)current_source,
		  NULL);

    XmTextPosition initial_pos = 0;
    if (initial_line > 0 && initial_line <= line_count)
	initial_pos = pos_of_line[initial_line] + bp_indent_amount;

    SetInsertionPosition(source_text_w, initial_pos, true);

    current_file_name = file_name;

    // Refresh title
    update_title();

    // Refresh breakpoints
    static Assoc<int, VarArray<int> > empty_bps;
    bps_in_line = empty_bps;
    static StringArray empty_addresses;
    bp_addresses = empty_addresses;
    refresh_bp_disp();

    XtManageChild(source_text_w);

    ostrstream os;
    os << "File " << quote(file_name) << " ";
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
    update_glyphs();

    if (source_view_shell || app_data.tty_mode)
    {
	static bool initial_popup = true;
	if (initial_popup)
	{
	    Widget shell = source_view_shell ? 
				source_view_shell : command_shell;
	    initial_popup_shell(shell);
	    initial_popup_shell(tool_shell);

	    initial_popup = false;
	}
    }
}

void SourceView::update_title()
{
    if (toplevel_w == 0)
	return;

    string title   = string(DDD_NAME) + ": " + current_file_name;
    String title_s = title;

    string icon   = 
	string(DDD_NAME) + ": " + basename(current_file_name.chars());
    String icon_s = icon;

    XtVaSetValues(toplevel_w,
		  XmNtitle, title_s,
		  XmNiconName, icon_s,
		  NULL);
}



// ***************************************************************************
// Bringt Breakpoint-Anzeige auf den aktuellen Stand (gemaess bp_map).
//

// Update breakpoint locations
void SourceView::refresh_bp_disp()
{
    refresh_source_bp_disp();
    refresh_code_bp_disp();
    update_glyphs();
}

void SourceView::refresh_source_bp_disp()
{
    // Alte Breakpoint-Darstellungen ueberschreiben - - - - - - - - - - - - -
    for (AssocIter<int, VarArray<int> > b_i_l_iter(bps_in_line);
	 b_i_l_iter.ok(); 
	 b_i_l_iter++)
    {
	int line_nr = b_i_l_iter.key();
	if (line_nr < 0 || line_nr > line_count)
	    continue;

	string s(current_source.at(int(pos_of_line[line_nr]), 
				   bp_indent_amount - 1));

	XmTextReplace (source_text_w,
		       pos_of_line[line_nr],
		       pos_of_line[line_nr] + bp_indent_amount - 1,
		       (String)s);
    }

    static Assoc<int, VarArray<int> > empty_bps;
    bps_in_line = empty_bps;

    // Find all breakpoints referring to this file
    MapRef ref;
    for (BreakPoint* bp = bp_map.first(ref);
	 bp != 0;
	 bp = bp_map.next(ref))
    {
	if (bp_matches(bp))
	    bps_in_line[bp->line_nr()] += bp->number();
    }

    if (!display_glyphs)
    {
	// Show breakpoints in text
	for (AssocIter<int, VarArray<int> > b_i_l_iter(bps_in_line);
	     b_i_l_iter.ok();
	     b_i_l_iter++)
	{
	    int line_nr = b_i_l_iter.key();
	    if (line_nr < 0 || line_nr > line_count)
		continue;

	    XmTextPosition pos = pos_of_line[line_nr];

	    string insert_string = "";

	    // Darstellung fuer alle Breakpoints der Zeile
	    VarArray<int>& bps = bps_in_line[line_nr];

	    int i;
	    for (i = 0; i < bps.size(); i++)
	    {
		BreakPoint* bp = bp_map.get(bps[i]);
		assert (bp);
		insert_string += bp->enabled() ? '#' : '_';
		insert_string += bp->number_str();
		insert_string += bp->enabled() ? '#' : '_';
	    }
	    if (int(insert_string.length()) >= bp_indent_amount - 1) {
		insert_string =
		    insert_string.before(bp_indent_amount - 1);
	    }
	    else
	    {
		for (i = insert_string.length(); 
		     i < int(bp_indent_amount) - 1; i++)
		{
		    insert_string += current_source[pos + i];
		}
	    }
	    assert(insert_string.length() == unsigned(bp_indent_amount - 1));

	    XmTextReplace (source_text_w, pos, pos + bp_indent_amount - 1,
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
	string insert_string = replicate(' ', code_indent_amount);

	XmTextReplace (code_text_w, pos, pos + code_indent_amount,
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
	    insert_string += replicate(' ', code_indent_amount);
	    insert_string = insert_string.before(code_indent_amount);

	    XmTextReplace (code_text_w, pos, pos + code_indent_amount,
			   (String)insert_string);
	}
    }

    update_glyphs();
}



// ***************************************************************************
// Findet zu pos die passende Zeilennummer.
// in_text==true wenn pos im Quelltext-Bereich ist.
// bp_nr ist die Nr des Breakpoints, der an Position pos
// dargestellt wird, 0 sonst.
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
	return false;

    if (text_w == source_text_w)
    {
	// Search in source code
	XmTextPosition line_pos = 0;
	XmTextPosition next_line_pos = 0;

	while (!found && line_count >= line_nr)
	{
	    next_line_pos = (line_count >= line_nr + 1) ?
		pos_of_line[line_nr + 1] :
		XmTextGetLastPosition (text_w) + 1;

	    if (pos < (line_pos + bp_indent_amount - 1))
	    {
		// Position in breakpoint area
		found = true;
		in_text = false;

		// Check for breakpoints...
		VarArray<int>& bps = bps_in_line[line_nr];
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
			    break; // for-Schleife fertig
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
		// Position is in the following line
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

	if (pos - line_pos < code_indent_amount)
	{
	    // Breakpoint area
	    in_text = false;

	    // Check if we have a breakpoint around here
	    int index = current_code.index("0x", pos);
	    if (index >= 0)
	    {
		address = current_code.from(index);
		address = address.through(rxalphanum);

		VarArray<int> bps;

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
			    break; // for-Schleife fertig
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
//
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
    if ((is_source_widget(text_w) && offset < bp_indent_amount)
	|| (is_code_widget(text_w) && offset < code_indent_amount))
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



//----------------------------------------------------------------------------
// Anfragen stellen und auswerten
//----------------------------------------------------------------------------

// ***************************************************************************
//
void SourceView::refresh_bpsSQ ()
{
    gdb->send_question ("info breakpoints", refresh_bpsOQC, 0);
}

// ***************************************************************************
//
void SourceView::refresh_bpsOQC (const string& answer, void *)
{
    string ans = answer;
    process_info_bp (ans);
}


//----------------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------------

SourceView::SourceView (XtAppContext app_context,
			Widget       parent,
			int          bp_i_a)
{
    assert (bp_indent_amount >= 0);
    bp_indent_amount = bp_i_a;

    // Find application shell
    toplevel_w = parent;
    while (toplevel_w != 0 && !XtIsWMShell(toplevel_w))
	toplevel_w = XtParent(toplevel_w);

    Arg args[10];
    Cardinal arg = 0;

    // Create source code window
    arg = 0;
    XtSetArg(args[arg], XmNmarginHeight, 0); arg++;
    XtSetArg(args[arg], XmNmarginWidth, 0);  arg++;
    source_form_w = 
	verify(XmCreateForm(parent, "source_form_w", args, arg));

    arg = 0;
    XtSetArg(args[arg], XmNselectionArrayCount, 1); arg++;
    XtSetArg(args[arg], XmNtopAttachment,     XmATTACH_FORM); arg++;
    XtSetArg(args[arg], XmNbottomAttachment,  XmATTACH_FORM); arg++;
    XtSetArg(args[arg], XmNleftAttachment,    XmATTACH_FORM); arg++;
    XtSetArg(args[arg], XmNrightAttachment,   XmATTACH_FORM); arg++;
    source_text_w = 
	verify(XmCreateScrolledText(source_form_w, "source_text_w", 
				    args, arg));
    XtManageChild(source_text_w);
    XtManageChild(source_form_w);

    XtAddCallback(source_text_w, XmNgainPrimaryCallback,
		  set_source_argCB, XtPointer(false));
    XtAddCallback(source_text_w, XmNmotionVerifyCallback,
		  set_source_argCB, XtPointer(true));
    XtAddCallback(source_text_w, XmNmotionVerifyCallback,
		  CheckScrollCB, 0);

    // Fetch scrollbar ID
    Widget scrollbar = 0;
    XtVaGetValues(XtParent(source_text_w),
		  XmNverticalScrollBar, &scrollbar,
		  NULL);
    if (scrollbar)
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


    // Create machine code window
    arg = 0;
    XtSetArg(args[arg], XmNmarginHeight, 0); arg++;
    XtSetArg(args[arg], XmNmarginWidth, 0);  arg++;
    code_form_w = 
	verify(XmCreateForm(parent, "code_form_w", args, arg));

    arg = 0;
    XtSetArg(args[arg], XmNselectionArrayCount, 1); arg++;
    XtSetArg(args[arg], XmNtopAttachment,     XmATTACH_FORM); arg++;
    XtSetArg(args[arg], XmNbottomAttachment,  XmATTACH_FORM); arg++;
    XtSetArg(args[arg], XmNleftAttachment,    XmATTACH_FORM); arg++;
    XtSetArg(args[arg], XmNrightAttachment,   XmATTACH_FORM); arg++;
    code_text_w = 
	verify(XmCreateScrolledText(code_form_w, "code_text_w", args, arg));
    XtManageChild(code_text_w);
    if (disassemble)
	XtManageChild(code_form_w);

    XtAddCallback(code_text_w, XmNgainPrimaryCallback,
		  set_source_argCB, XtPointer(false));
    XtAddCallback(code_text_w, XmNmotionVerifyCallback,
		  set_source_argCB, XtPointer(true));
    XtAddCallback(code_text_w, XmNmotionVerifyCallback,
		  CheckScrollCB, 0);

    // Fetch scrollbar ID
    scrollbar = 0;
    XtVaGetValues(XtParent(code_text_w),
		  XmNverticalScrollBar, &scrollbar,
		  NULL);
    if (scrollbar)
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


    // Create breakpoint editor
    arg = 0;
    XtSetArg(args[arg], XmNvisibleItemCount, 0); arg++;
    edit_breakpoints_dialog_w =
	verify(XmCreatePromptDialog(parent, "edit_breakpoints_dialog",
				    args, arg));
    Delay::register_shell(edit_breakpoints_dialog_w);

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

    up_w   = XmSelectionBoxGetChild(stack_dialog_w, XmDIALOG_APPLY_BUTTON);
    down_w = XmSelectionBoxGetChild(stack_dialog_w, XmDIALOG_CANCEL_BUTTON);

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
		  XmNokCallback, UnmanageThisCB, stack_dialog_w);
    XtAddCallback(stack_dialog_w,
		  XmNokCallback, StackDialogPoppedDownCB, 0);
    XtAddCallback(stack_dialog_w,
		  XmNapplyCallback, gdbCommandCB, "up");
    XtAddCallback(stack_dialog_w,
		  XmNcancelCallback, gdbCommandCB, "down");
    XtAddCallback(stack_dialog_w,
		  XmNhelpCallback, ImmediateHelpCB, 0);


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


    // Setup actions
    XtAppAddActions (app_context, actions, XtNumber (actions));

    // Create glyphs in the background
    XtAppAddWorkProc (app_context, CreateGlyphsWorkProc, XtPointer(0));
}


// ***************************************************************************
//

// Set current execution position, based on the GDB position info
// POSITION; no arg means clear current position.
// STOPPED indicates that the program just stopped.
void SourceView::show_execution_position (string position, bool stopped)
{
    if (stopped)
	at_lowest_frame = true;

    if (position == "")
    {
	if (!display_glyphs)
	{
	    // Remove old marker
	    String sep = " ";
	    XmTextReplace (source_text_w,
			   last_pos + bp_indent_amount - 1,
			   last_pos + bp_indent_amount,
			   sep);

	    if (last_start_highlight)
		XmTextSetHighlight (source_text_w,
				    last_start_highlight, last_end_highlight,
				    XmHIGHLIGHT_NORMAL);

	}
	last_pos = last_start_highlight = last_end_highlight = 0;
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
	read_file(file_name, line);

    if (file_matches(file_name, current_file_name))
    {
	if (!display_glyphs && bp_indent_amount > 0)
	{
	    // Remove old marker
	    String sep = " ";
	    XmTextReplace (source_text_w,
			   last_pos + bp_indent_amount - 1,
			   last_pos + bp_indent_amount,
			   sep);
	}

	// Zeilennummer usw. anzeigen
	_show_execution_position(file_name, line);
    }
}


void SourceView::_show_execution_position(string file, int line)
{
    last_execution_file = file;
    last_execution_line = line;

    if (!file_matches(file, current_file_name))
	read_file(file, line);
    else if (line >= 1 && line <= line_count)
	add_to_history(file, line);

    if (line < 1 || line > line_count)
	return;

    XmTextPosition pos = pos_of_line[line];
    SetInsertionPosition(source_text_w, pos + bp_indent_amount, false);

    // Mark current line
    if (!display_glyphs && bp_indent_amount > 0)
    {
	// Set new marker
	String marker = ">";
	XmTextReplace (source_text_w,
		       pos + bp_indent_amount - 1,
		       pos + bp_indent_amount,
		       marker);
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
void SourceView::show_position (string position)
{
    string file_name = current_file_name;

    if (position.contains(':'))
    {
	file_name = position.before(':');
	position  = position.after(':');
    }
    int line = get_positive_nr(position);

    if (!file_matches(file_name, current_file_name))
	read_file(file_name, line);
    add_to_history(file_name, line);

    // Fenster scrollt an Position
    if (line > 0 && line <= line_count)
    {
	XmTextPosition pos = pos_of_line[line];
	SetInsertionPosition(source_text_w, pos + bp_indent_amount, true);

	last_pos = pos;
    }
}



// ***************************************************************************
// Wertet die Ausgabe auf 'info breakpoints' aus.
// Aktualisiert die in bp_map gespeicherten breakpoints, fuegt ggf. neue
// hinzu oder loescht alte.
// Bringt durch Aufruf von refresh_bp_disp die Breakpoint-Anzeige 
// auf den aktuellen Stand.
//
void SourceView::process_info_bp (string& info_output)
{
    // DEC dbx issues empty lines, which causes trouble
    info_output.gsub("\n\n", "\n");

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
				    
    VarArray<int> bps_not_read;
    MapRef ref;
    int i;
    for (i = bp_map.first_key(ref);
	 i != 0;
	 i = bp_map.next_key(ref)) {
	bps_not_read += i;
    }

    // Skip header
    if (gdb->type() == GDB)
	info_output = info_output.after ('\n');

    if (info_output == "")
	info_output = "No breakpoints.";

    bool changed = false;

    while (info_output != "") 
    {
	int bp_nr = -1;
	switch(gdb->type())
	{
	case GDB:
	    if (!isdigit(info_output[0]))
	    {
		// Skip this line
		info_output = info_output.after('\n');
		continue;
	    }
	    bp_nr = get_positive_nr (info_output);
	    break;

	case DBX:
	    {
		if (!info_output.contains('(', 0)
		    && !info_output.contains('[', 0))
		{
		    // Skip this line
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
	    BreakPoint* new_bp = new BreakPoint (info_output);
	    bp_map.insert (bp_nr, new_bp);

	    // Select this breakpoint only
	    MapRef ref;
	    for (BreakPoint* bp = bp_map.first(ref);
		 bp != 0;
		 bp = bp_map.next(ref))
	    {
		bp->selected() = (bp == new_bp);
	    }
	}
    }

    // Delete all breakpoints not found now
    for (i = 0; i < bps_not_read.size(); i++) {
	delete bp_map.get(bps_not_read[i]);
	bp_map.del(bps_not_read[i]);
	changed = true;
    }

    if (changed)
	refresh_bp_disp();

    // Set up breakpoint editor contents
    fill_labels(last_info_output);
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
void SourceView::lookup(string s)
{
    if (s != "" && isspace(s[0]))
	s = s.after(rxwhite);

    if (s == "")
    {
	// Empty argument given
	if (last_execution_pc != "")
	    show_pc(last_execution_pc, XmHIGHLIGHT_SELECTED);

	if (last_execution_file != "")
	    _show_execution_position(last_execution_file, 
				     last_execution_line);
    }
    else if (s[0] != '0' && isdigit(s[0]))
    {
	// Line number given
	int line = atoi(s);
	if (line > 0 && line <= line_count)
	{
	    if (gdb->type() == GDB)
		gdb_command("info line " + current_source_name()
			    + ":" + itostring(line));
	    else
		show_position(full_path(current_file_name) 
			      + ":" + itostring(line));
	}
	else
	{
	    post_error("No such line in current source.", 
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

	    if (bp == 0)
		post_error("No such breakpoint.", 
			   "no_such_breakpoint_error", source_text_w);
	}
    }
    else if (s.contains(":") && !s.contains("::"))
    {
	// File:line given
	if (gdb->type() == GDB)
	    gdb_command("info line " + s);
	else
	    show_position(s);
    }
    else
    {
	// Function or *address given
	switch (gdb->type())
	{
	case GDB:
	    if (s[0] == '0')	// Address given
		s = "*" + s;
	    if (s.length() > 0 && s[0] != '\'' && s[0] != '*')
		s = string('\'') + s + '\'';
	    gdb_command("info line " + s);
	    break;

	case DBX:
	    {
		string pos = dbx_lookup(s);
		if (pos != "")
		    show_position(pos);
	    }
	    break;

	case XDB:
	    gdb_command("v " + s);
	    break;
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

void SourceView::goto_entry(string entry)
{
    string file_name = entry.before(':');
    string line_str = entry.after(':');
    int line = atoi(line_str);
    string address = line_str.after(':');

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

	    last_start_secondary_highlight = pos_of_line[line];
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
		last_start_secondary_highlight + bp_indent_amount;
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
}

void SourceView::go_forward()
{
    if (history_position + 1 < history.size())
	goto_entry(history[++history_position]);
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
	XmTextSetSelection(source_text_w, pos, pos + matchlen, time);
	SetInsertionPosition(source_text_w, cursor, false);
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
			    static regex rxtitle("[^\n]*:\n");
			    ans.gsub(rxtitle, string(""));
			    ans.gsub(", ", "\n");
			    ans.gsub(rxwhite, "\n");

			    static regex rxnls("\n\n*");
			    ans.gsub(rxnls, "\n");

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
	// DBX and XDB use full file names.
	source = full_path(current_file_name);
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

    int line_nr;
    bool in_text;
    int bp_nr;
    string address;

    if (get_line_of_pos(source_text_w,
			pos, line_nr, address, in_text, bp_nr) == false)
	return "";

    return current_source_name() + ":" + itostring(line_nr);
}


//-----------------------------------------------------------------------------
// Reaktion auf Klicken der linken Maus-Taste
//----------------------------------------------------------------------------

static XmTextPosition selection_startpos;
static XmTextPosition selection_endpos;
static XmTextPosition selection_pos;
static Time selection_time;

void SourceView::setSelection(XtPointer client_data, XtIntervalId *)
{
    Widget w = (Widget)client_data;

    XmTextSetSelection(w, selection_startpos, selection_endpos, 
		       selection_time);

    // Do not scroll here.  Do not use SetInsertionPosition().
    XmTextSetInsertionPosition(w, selection_pos);
    XmTextShowPosition(w, selection_pos);

    set_source_argCB(w, XtPointer(false), 0);
}

void SourceView::startSelectWordAct (Widget text_w, XEvent* e, 
				     String *params, Cardinal *num_params)
{
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
// Reaktion auf Klicken der rechten Maus-Taste
//----------------------------------------------------------------------------
void SourceView::srcpopupAct (Widget w, XEvent* e, String *, Cardinal *)
{
    if (e->type != ButtonPress && e->type != ButtonRelease)
	return;

    if (!is_source_widget(w) && !is_code_widget(w))
	return;
    
    string& text = current_text(w);
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

    static int line_nr;
    bool in_text;
    static int bp_nr;
    static string address;
    bool pos_found = 
	get_line_of_pos (w, pos, line_nr, address, in_text, bp_nr);

    if (!pos_found) // Wahrsch. noch ein Text-File geladen
	return; 

    if (bp_nr != 0)
    {
	// Auf Breakpoint-Anzeige geklickt: Popup-Menu fuer Breakpoint oeffnen
	static Widget bp_popup_w = 0;

	if (bp_popup_w == 0) {
	    MMDesc *bp_popup = 0;
	    switch (gdb->type())
	    {
	    case GDB:
	    case XDB:
		bp_popup = bp_popup_gdb;
		break;

	    case DBX:
		bp_popup = bp_popup_dbx;
		break;
	    }

	    bp_popup_w = MMcreatePopupMenu (text_w, "bp_popup", bp_popup);
	    MMaddCallbacks (bp_popup, XtPointer (&bp_nr));
	}

	switch (gdb->type())
	{
	case XDB:
	    // We don't support these items in XDB
	    XtUnmanageChild(bp_popup_gdb[BPItms::Sep1].widget);
	    XtUnmanageChild(bp_popup_gdb[BPItms::Condition].widget);
	    XtUnmanageChild(bp_popup_gdb[BPItms::Sep2].widget);

	    // FALL THROUGH
	case GDB:
	    {
		MString label(bp_map.get(bp_nr)->enabled() ? 
			      "Disable Breakpoint" : "Enable Breakpoint");
		XtVaSetValues(bp_popup_gdb[BPItms::Disable].widget,
			      XmNlabelString, label.xmstring(),
			      NULL);
	    }
	    break;

	case DBX:
	    break;
	}

	XmMenuPosition (bp_popup_w, event);
	XtManageChild (bp_popup_w);
    }
    else if (!in_text) 
    {
	// Popup-Menu fuer Zeile line_nr oeffnen
	if (is_source_widget(w))
	{
	    static Widget line_popup_w = 0;
	    if (line_popup_w == 0)
	    {
		line_popup_w = 
		    MMcreatePopupMenu (w, "line_popup", line_popup);
		MMaddCallbacks (line_popup, XtPointer(&line_nr));
	    }
	    XmMenuPosition (line_popup_w, event);
	    XtManageChild (line_popup_w);
	}
	else if (is_code_widget(w))
	{
	    static Widget address_popup_w = 0;
	    if (address_popup_w == 0)
	    {
		address_popup_w = 
		    MMcreatePopupMenu (w, "address_popup", address_popup);
		MMaddCallbacks (address_popup, XtPointer(&address));
	    }
	    XmMenuPosition (address_popup_w, event);
	    XtManageChild (address_popup_w);
	}
    }
    else
    {
	// 'Umgebenden' C-String ermitteln, und Popup dafuer oeffnen
	static string word;
	static string ref_word;

	XmTextPosition startpos;
	XmTextPosition endpos;

	find_word_bounds(text_w, pos, startpos, endpos);
	word = "";

	if (startpos < XmTextPosition(text.length())
	    && startpos < endpos)
	    word = text(int(startpos), int(endpos - startpos));

	ref_word = gdb->dereferenced_expr(word);

	Widget text_popup_w = 
	    MMcreatePopupMenu(text_w, "text_popup", text_popup);
	MMaddCallbacks (text_popup, XtPointer(&word));

	// The popup menu is destroyed immediately after having popped down.
	Widget shell = XtParent(text_popup_w);
	XtAddCallback(shell, XtNpopdownCallback, DestroyThisCB, shell);

	MString current_arg(word, "tt");
	MString current_ref_arg(ref_word, "tt");

	Arg args[5];
	int arg = 0;
	MString label = MString(text_cmd_labels[TextItms::Break]) 
	    + current_arg;
	XtSetArg (args[arg], XmNlabelString, label.xmstring());arg++;
	XtSetValues(text_popup[TextItms::Break].widget, args, arg);

	arg = 0;
	label = MString(text_cmd_labels[TextItms::Clear]) + current_arg;
	XtSetArg (args[arg], XmNlabelString, label.xmstring());arg++;
	XtSetValues(text_popup[TextItms::Clear].widget, args, arg);

	arg = 0;
	label = MString(text_cmd_labels[TextItms::Print]) + current_arg;
	XtSetArg (args[arg], XmNlabelString, label.xmstring());arg++;
	XtSetValues(text_popup[TextItms::Print].widget, args, arg);

	arg = 0;
	label = MString(text_cmd_labels[TextItms::Disp]) + current_arg;
	XtSetArg (args[arg], XmNlabelString, label.xmstring());arg++;
	XtSetValues(text_popup[TextItms::Disp].widget, args, arg);

	arg = 0;
	label = MString(text_cmd_labels[TextItms::PrintRef]) + current_ref_arg;
	XtSetArg (args[arg], XmNlabelString, label.xmstring());arg++;
	XtSetValues(text_popup[TextItms::PrintRef].widget, args, arg);

	arg = 0;
	label = MString(text_cmd_labels[TextItms::DispRef]) + current_ref_arg;
	XtSetArg (args[arg], XmNlabelString, label.xmstring());arg++;
	XtSetValues(text_popup[TextItms::DispRef].widget, args, arg);

	arg = 0;
	label = MString(text_cmd_labels[TextItms::Whatis]) + current_arg;
	XtSetArg (args[arg], XmNlabelString, label.xmstring());arg++;
	XtSetValues(text_popup[TextItms::Whatis].widget, args, arg);

	arg = 0;
	label = MString(text_cmd_labels[TextItms::Lookup]) + current_arg;
	XtSetArg (args[arg], XmNlabelString, label.xmstring());arg++;
	XtSetValues(text_popup[TextItms::Lookup].widget, args, arg);

	bool sens = (word.length() > 0);
	XtSetSensitive (text_popup[TextItms::Break].widget,    sens);
	XtSetSensitive (text_popup[TextItms::Clear].widget,    sens);
	XtSetSensitive (text_popup[TextItms::Print].widget,    sens);
	XtSetSensitive (text_popup[TextItms::Disp].widget,     sens);
	XtSetSensitive (text_popup[TextItms::PrintRef].widget, sens);
	XtSetSensitive (text_popup[TextItms::DispRef].widget,  sens);
	XtSetSensitive (text_popup[TextItms::Whatis].widget,   sens);
	XtSetSensitive (text_popup[TextItms::Lookup].widget,   sens);

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
	   
    switch (gdb->type())
    {
    case GDB:
	if (input[0] == '0')
	    input = "*" + input; // Address given
	gdb_command("break " + input);
	break;

    case DBX:
	if (isdigit(input[0]))
	{
	    // Line number given
	    gdb_command("stop at " + input);
	}
	else if (input.contains(':') && !input.contains("::"))
	{
	    // File:pos given
	    gdb_command("file " + input.before(':'), w);
	    gdb_command("stop at " + input.after(':'), w);
	}
	else
	{
	    // Symbol given
	    string pos = dbx_lookup(input);
	    if (pos != "")
	    {
		gdb_command("file " + pos.before(':'), w);
		gdb_command("stop at " + pos.after(':'), w);
	    }
	}

    case XDB:
	gdb_command("b " + input);
	break;
    }
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
	XtAddCallback(new_breakpoint_dialog,
		      XmNhelpCallback,
		      ImmediateHelpCB,
		      NULL);
	XtAddCallback(new_breakpoint_dialog,
		      XmNokCallback,
		      NewBreakpointDCB,
		      NULL);
    }
    XtManageChild(new_breakpoint_dialog);
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
	int *breakpoint_nrs = getDisplayNumbers(breakpoint_list_w);
	for (int i = 0; breakpoint_nrs[i] > 0; i++)
	{
	    gdb_command(string("cond ") + itostring(breakpoint_nrs[i])
			+ " " + input);
	}
	delete[] breakpoint_nrs;
    }
    else
    {
	int bp_nr = *((int *)client_data);
	gdb_command(string("cond ") + itostring(bp_nr) + " " + input);
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
	int *breakpoint_nrs = getDisplayNumbers(breakpoint_list_w);
	bp_nr = breakpoint_nrs[0];
	delete[] breakpoint_nrs;
    }
    else
    {
	bp_nr = *bp_nr_ptr;
    }

    if (bp_nr > 0)
    {
	string info = 
	    gdb_question("info breakpoint " + itostring(bp_nr));
	if (info == NO_GDB_ANSWER)
	    return;

	string cond = info.after("stop only if ");
	if (cond.contains('\n'))
	    cond = cond.before("\n");

	MString xmcond(cond);
	XtVaSetValues(edit_breakpoint_condition_dialog,
		      XmNtextString, xmcond.xmstring(),
		      NULL);

	Widget text = XmSelectionBoxGetChild(edit_breakpoint_condition_dialog,
					     XmDIALOG_TEXT);
	XmTextSetSelection(text, 0, cond.length(), 
			   XtLastTimestampProcessed(XtDisplay(text)));
    }

    XtManageChild(edit_breakpoint_condition_dialog);
    raise_shell(edit_breakpoint_condition_dialog);
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

    if (input[0] == '\0')
	input = "0";

    string ignore;
    switch (gdb->type())
    {
    case GDB:
	ignore = "ignore ";
	break;
    case DBX:
	return;			// FIXME
    case XDB:
	ignore = "bc ";
	break;
    }

    if (client_data == 0)
    {
	int *breakpoint_nrs = getDisplayNumbers(breakpoint_list_w);
	for (int i = 0; breakpoint_nrs[i] > 0; i++)
	{
	    gdb_command(ignore + itostring(breakpoint_nrs[i]) + " " + input);
	}
	delete[] breakpoint_nrs;
    }
    else
    {
	int bp_nr = *((int *)client_data);
	gdb_command(ignore + itostring(bp_nr) + " " + input);
    }

    XtFree(input);
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
	int *breakpoint_nrs = getDisplayNumbers(breakpoint_list_w);
	bp_nr = breakpoint_nrs[0];
	delete[] breakpoint_nrs;
    }
    else
    {
	bp_nr = *bp_nr_ptr;
    }

    if (bp_nr > 0)
    {
	string ignore;
	switch (gdb->type())
	{
	case GDB:
	    {
		string info = 
		    gdb_question("info breakpoint " + itostring(bp_nr));
		if (info == NO_GDB_ANSWER)
		    return;

		ignore = info.after("ignore next ");
		ignore = ignore.before(" hits");
	    }
	    break;

	case DBX:
	    break;		// FIXME

	case XDB:
	    {
		MapRef ref;
		for (BreakPoint* bp = bp_map.first(ref);
		     bp != 0;
		     bp = bp_map.next(ref))
		{
		    if (bp->number() == bp_nr && bp->ignore_count() != "")
		    {
			ignore = bp->ignore_count();
			break;
		    }
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
    }

    XtManageChild(edit_breakpoint_ignore_count_dialog);
    raise_shell(edit_breakpoint_ignore_count_dialog);
}


void SourceView::BreakpointCmdCB(Widget,
				 XtPointer client_data,
				 XtPointer)
{
    if (breakpoint_list_w == 0)
	return;

    string cmd = (String)client_data;

    if (gdb->type() == XDB)
    {
	if (cmd == "delete")
	    cmd = "db ";
	else if (cmd == "enable")
	    cmd = "ab";
	else if (cmd == "disable")
	    cmd = "sb";
    }

    int *breakpoint_nrs = getDisplayNumbers(breakpoint_list_w);
    if (breakpoint_nrs[0] > 0)
    {
	for (int i = 0; breakpoint_nrs[i] > 0; i++)
	    cmd += " " + itostring(breakpoint_nrs[i]);

	gdb_command(cmd);
    }
    delete[] breakpoint_nrs;
}

void SourceView::LookupBreakpointCB(Widget, XtPointer, XtPointer)
{
    if (breakpoint_list_w == 0)
	return;

    int *breakpoint_nrs = getDisplayNumbers(breakpoint_list_w);
    if (breakpoint_nrs[0] > 0 && breakpoint_nrs[1] == 0)
    {
	lookup("#" + itostring(breakpoint_nrs[0]));
    }
    delete[] breakpoint_nrs;
}

// Create labels for the list
void SourceView::fill_labels(const string& info_output)
{
    if (breakpoint_list_w == 0)
	return;

    int count = info_output.freq('\n') + 1;
    if (info_output == "")
	count = 0;

    string *label_list = new string[count];
    bool *selected     = new bool[count];

    split(info_output, label_list, count, '\n');

    while (count > 0 && label_list[count - 1] == "")
	count--;

    bool select = false;
    for (int i = 0; i < count; i++)
    {
	int bp_number = get_positive_nr(label_list[i]);
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
    }

    setLabelList(breakpoint_list_w, label_list, selected, count);
    UpdateBreakpointButtonsCB(breakpoint_list_w, XtPointer(0), XtPointer(0));

    delete[] label_list;
    delete[] selected;
}

void SourceView::UpdateBreakpointButtonsCB(Widget, XtPointer, XtPointer)
{
    if (edit_breakpoints_dialog_w == 0)
	return;

    int *breakpoint_nrs = getDisplayNumbers(breakpoint_list_w);
    int count;
    for (count = 0; breakpoint_nrs[count] != 0; count++)
	;

    // Update selection
    MapRef ref;
    for (BreakPoint* bp = bp_map.first(ref);
	 bp != 0;
	 bp = bp_map.next(ref))
	bp->selected() = false;

    for (int i = 0; i < count; i++)
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

    delete[] breakpoint_nrs;

    // Update buttons
    XtSetSensitive(bp_area[BPButtons::Lookup].widget,
		   count == 1);
    XtSetSensitive(bp_area[BPButtons::Enable].widget,      
		   gdb->type() != DBX && count > 0);
    XtSetSensitive(bp_area[BPButtons::Disable].widget,     
		   gdb->type() != DBX && count > 0);
    XtSetSensitive(bp_area[BPButtons::Condition].widget,   
		   gdb->type() == GDB && count > 0);
    XtSetSensitive(bp_area[BPButtons::IgnoreCount].widget, 
		   gdb->type() != DBX && count > 0);
    XtSetSensitive(bp_area[BPButtons::Delete].widget,
		   count > 0);
}

void SourceView::EditBreakpointsCB(Widget, XtPointer, XtPointer)
{
    if (edit_breakpoints_dialog_w)
    {
	XtManageChild(edit_breakpoints_dialog_w);
	raise_shell(edit_breakpoints_dialog_w);
    }
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
	gdb_command(gdb->frame_command(itostring(count - cbs->item_position)));
	break;
    
    case XDB:
	// XDB frame output is caught by our routines.
	gdb_command(gdb->frame_command(itostring(cbs->item_position - 1)));
	break;

    case DBX:
	{
	    string pos;
	    if (!gdb->has_frame_command())
	    {
		// Some DBXes lack a `frame' command.  Use this kludge instead.
		string reply = gdb_question("func main");
		if (reply == NO_GDB_ANSWER)
		{
		    post_gdb_busy(w);
		    return;
		}
		if (cbs->item_position == 1)
		{
		    set_status("Current function is main");
		}
		else
		{
		    reply = 
			gdb_question("down " 
				     + itostring(cbs->item_position - 1));
		    if (reply == NO_GDB_ANSWER)
		    {
			post_gdb_busy(w);
			return;
		    }

		    if (reply.contains("Current", 0))
		    {
			if (reply.contains('\n'))
			    reply = reply.before('\n');
			set_status(reply);
		    }
		    else
			post_gdb_message(reply, w);
		}

		// Get the selected line
		String _item;
		XmStringGetLtoR(cbs->item, LIST_CHARSET, &_item);
		string item(_item);
		XtFree(_item);

		if (item.contains(" in "))
		{
		    string file = item.after(" in ");
		    file = file.after('\"');
		    file = file.before('\"');
		    pos = file + ":";
		}

		if (item.contains("line "))
		{
		    string line_s = item.after("line ");
		    pos += itostring(get_positive_nr(line_s));
		}
	    }
	    else
	    {
		// Issue `frame' command
		string reply = 
		    gdb_question(gdb->frame_command(
			itostring(count - cbs->item_position + 1)));
		if (reply == NO_GDB_ANSWER)
		{
		    post_gdb_busy(w);
		    return;
		}

		// Get current file
		string file = gdb_question("file");

		// Simple sanity check
		strip_final_blanks(file);

		if (file.contains('\n'))
		    file = file.before('\n');
		if (file.contains(' '))
		    file = "";

		if (file != "")
		{
		    // Get current line
		    int line = 0;

		    // Check if line was contained in `frame' reply
		    PosBuffer pos_buffer;
		    pos_buffer.filter(reply);
		    pos_buffer.answer_ended();
		    if (pos_buffer.pos_found())
		    {
			string line_s = pos_buffer.get_position();
			if (line_s.contains(':'))
			    line_s = line_s.after(':');
			line = atoi(line_s);
		    }

		    if (line == 0)
		    {
			// No? Get the current line via `list'
			string listing = gdb_question("list");
			line = atoi(listing);
		    }

		    if (line >= 1)
			pos = file + ":" + itostring(line);
		}
		
	    }

	    if (pos != "")
		show_execution_position(pos);
	}
	break;
    }
}

void SourceView::refresh_stack_frames()
{
    // Allow unlimited time to find out where we are
    string where_s = gdb_question(gdb->where_command(), -1);
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
    XtManageChild(stack_dialog_w);
    raise_shell(stack_dialog_w);
    
    stack_dialog_popped_up = true;
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
    const int min_width = 40;
    for (int i = 0; i < count; i++)
    {
	selected[i] = false;

	// Remove argument list and file paths
	// (otherwise line can be too long for dbx)
	//   ... n.b. with templates, line can still be rather long
	static regex arglist("[(][^)]*[)]");
	static regex filepath("/.*/");
	frame_list[i].gsub(arglist, "()");
	frame_list[i].gsub(filepath, "");

	if (int(frame_list[i].length()) < min_width)
	{
	    frame_list[i] += 
		replicate(' ', min_width - frame_list[i].length());
	}
    }

    setLabelList(frame_list_w, frame_list, selected, count);
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

	XmListSelectPos(frame_list_w, pos, False);
	if (pos == 1)
	    XmListSetPos(frame_list_w, pos);
	else if (pos - 1 < top_item)
	    XmListSetPos(frame_list_w, pos - 1);
	else if (pos + 1 >= top_item + visible_items)
	    XmListSetBottomPos(frame_list_w, pos + 1);

	XtSetSensitive(up_w,   pos > 1);
	XtSetSensitive(down_w, pos < count);

	update_glyphs();
    }
    else
    {
	XtSetSensitive(up_w,   False);
	XtSetSensitive(down_w, False);
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

    int pos  = -1;
    int line = -1;
    string file;
    for (int i = count - 1; i >= 0; i--)
    {
	String _item;
	XmStringGetLtoR(items[i], LIST_CHARSET, &_item);
	string item(_item);
	XtFree(_item);

	if (item.contains(func, 0))
	{
	    pos = i;
	    if (item.contains("line "))
	    {
		string line_s = item.after("line ");
		line = get_positive_nr(line_s);
	    }
	    if (item.contains(" in "))
	    {
		file = item.after(" in ");
		file = file.from('\"');
		file = file.through('\"');
		file = unquote(file);
	    }
	    break;
	}
    }

    if (pos >= 0)
    {
	set_frame_pos(0, pos + 1);
	if (line > 0)
	{
	    string p;
	    if (file != "")
		p = file + ":";
	    p += itostring(line);

	    show_execution_position(p);
	}
    }

    return pos >= 0;
}

// Set frame manually: ARG = 0: POS, ARG = +/- N: down/up N levels
void SourceView::set_frame_pos(int arg, int pos)
{
    int count         = 0;
    int top_item      = 0;
    int visible_items = 0;
    XtVaGetValues(frame_list_w,
		  XmNitemCount, &count,
		  XmNtopItemPosition, &top_item,
		  XmNvisibleItemCount, &visible_items,
		  NULL);

    if (pos == 0)
	pos = count;
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
	if (position_count != 1 || pos < 1 || pos > count)
	    return;
    }

    XmListSelectPos(frame_list_w, pos, False);
    if (pos == 1)
	XmListSetPos(frame_list_w, pos);
    else if (pos - 1 < top_item)
	XmListSetPos(frame_list_w, pos - 1);
    else if (pos + 1 >= top_item + visible_items)
	XmListSetBottomPos(frame_list_w, pos + 1);

    XtSetSensitive(up_w,   pos > 1);
    XtSetSensitive(down_w, pos < count);
}

bool SourceView::where_required()    { return stack_dialog_popped_up; }
bool SourceView::register_required() { return register_dialog_popped_up; }



//-----------------------------------------------------------------------------
// Machine stuff
//----------------------------------------------------------------------------

// Replace first '\t' by filling up spaces until POS is reached
static void tabto(string& s, int pos)
{
    for (int i = 0; unsigned(i) < s.length() && i < pos; i++)
    {
	if (s[i] == '\t')
	{
	    int offset = pos - i;
	    s(i, 1) = replicate(' ', offset);
	    return;
	}
    }
}
    
// Replace all '\t' by filling up spaces until multiple of OFFSET is reached
static void untabify(string& s, int offset = 8)
{
    for (int i = 0; unsigned(i) < s.length(); i++)
    {
	if (s[i] == '\t')
	{
	    int spaces = offset - i % offset;
	    s(i, 1) = replicate(' ', spaces);
	    i += spaces - 1;
	}
    }
}

void SourceView::process_register(string& register_output)
{
    register_output.gsub(";\n", "\n");
    register_output.gsub("; ", "\n");
    register_output.gsub(";", "\n");

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

    setLabelList(register_list_w, register_list, selected, count);

    delete[] register_list;
    delete[] selected;
}

void SourceView::refresh_registers()
{
    string registers = gdb_question("info registers");
    if (registers == NO_GDB_ANSWER)
	registers = "No registers.";
    process_register(registers);
}

void SourceView::ViewRegistersCB(Widget, XtPointer, XtPointer)
{
    refresh_registers();
    XtManageChild(register_dialog_w);
    raise_shell(stack_dialog_w);
    
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
    if (line == 0)
	return "";

    if (!file_matches(file_name, current_file_name))
	read_file(file_name, line);

    XmTextPosition start = pos_of_line[line] + bp_indent_amount;
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


// Return pixmaps suitable for the widget W
static Pixmap pixmap(Widget w, char *bits, int width, int height)
{
    Pixel foreground, background;

    XtVaGetValues(w,
		  XmNforeground, &foreground,
		  XmNbackground, &background,
		  NULL);

    int depth = PlanesOfScreen(XtScreen(w));
    Pixmap pix = XCreatePixmapFromBitmapData(XtDisplay(w),
	XtWindow(w), bits, width, height, foreground, background, depth);

    return pix;
}

#if XmVERSION >= 2
const int motif_offset = 1;  // Motif 2.0 adds a 1 pixel border around glyphs
#else
const int motif_offset = 0;  // Motif 1.x does not
#endif

// Create glyph for text
Widget SourceView::create_glyph(Widget form_w,
				String name,
				char *bits, int width, int height)
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
    Widget w = verify(XmCreatePushButton(form_w, name, args, arg));
    XtRealizeWidget(w);
    XtManageChild(w);

    Pixmap pix = pixmap(w, bits, width, height);

    unsigned char unit_type;
    XtVaGetValues(w, XmNunitType, &unit_type, NULL);
    int new_width  = XmConvertUnits(w, XmHORIZONTAL, XmPIXELS, 
				    width + 1 + motif_offset, unit_type);
    int new_height = XmConvertUnits(w, XmVERTICAL, XmPIXELS, 
				    height + 1 + motif_offset, unit_type);

    arg = 0;
    XtSetArg(args[arg], XmNlabelType, XmPIXMAP); arg++;
    XtSetArg(args[arg], XmNlabelPixmap, pix);    arg++;
    XtSetArg(args[arg], XmNwidth,  new_width);   arg++;
    XtSetArg(args[arg], XmNheight, new_height);  arg++;
    XtSetValues(w, args, arg);
    
    XtAddCallback(w, XmNactivateCallback, MoveCursorToGlyphPosCB, 0);
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

void SourceView::map_glyph(Widget& w, Position x, Position y)
{
    while (w == 0)
	CreateGlyphsWorkProc(0);

    // clog << "Mapping glyph at (" << x << ", " << y << ")\n";

    assert(is_code_widget(w) || is_source_widget(w));

    Widget text_w;
    if (is_source_widget(w))
	text_w = source_text_w;
    else
	text_w = code_text_w;

    Dimension height              = 0;
    Dimension border_width        = 0;
    Dimension margin_height       = 0;
    Dimension shadow_thickness    = 0;
    Dimension highlight_thickness = 0;
    XtVaGetValues(w,
		  XmNheight,             &height,
		  XmNborderWidth,        &border_width,
		  XmNmarginHeight,       &margin_height,
		  XmNshadowThickness,    &shadow_thickness,
		  XmNhighlightThickness, &highlight_thickness,
		  NULL);
    Dimension glyph_height = 
	height + border_width + margin_height
	+ shadow_thickness + highlight_thickness;
    XtVaSetValues(w,
		  XmNleftOffset, x,
		  XmNtopOffset, y - glyph_height + line_height(text_w) / 2 - 2,
		  NULL);
    XtMapWidget(w);
}    

void SourceView::update_glyphs()
{
    static XtWorkProcId proc_id = 0;

    if (proc_id == 0)
    {
	proc_id = 
	    XtAppAddTimeOut(XtWidgetToApplicationContext(source_text_w), 0,
			    UpdateGlyphsWorkProc, XtPointer(&proc_id));
    }
}


static bool checking_scroll = false;

// Invoked by scrolling keys
void SourceView::updateGlyphsAct(Widget, XEvent*, String *, Cardinal *)
{
    CheckScrollWorkProc(0, 0);
}

// Invoked whenever the text widget may be about to scroll
void SourceView::CheckScrollCB(Widget, XtPointer, XtPointer)
{
    if (!checking_scroll)
	XtAppAddTimeOut(XtWidgetToApplicationContext(source_text_w), 0,
			CheckScrollWorkProc, XtPointer(0));
}
    
void SourceView::CheckScrollWorkProc(XtPointer client_data, XtIntervalId *id)
{
    if (checking_scroll)
	return;

    checking_scroll = true;

    XmTextPosition old_top = last_top;
    last_top = XmTextGetTopCharacter(source_text_w);

    XmTextPosition old_top_pc = last_top_pc;
    last_top_pc = XmTextGetTopCharacter(code_text_w);
    if (old_top != last_top || old_top_pc != last_top_pc)
	UpdateGlyphsWorkProc(client_data, id);

    checking_scroll = false;
}

// Maximum number of simultaneous glyphs on the screen
const int max_glyphs = 20;

// Horizontal arrow offset (pixels)
const int arrow_x_offset = -5;

// Horizontal breakpoint symbol offset (pixels)
const int stop_x_offset = +6;

// Additional offset for multiple breakpoints (pixels)
const int multiple_stop_x_offset = stop_width + (2 * motif_offset - 2);


// Glyph locations
static Widget _plain_arrow_w[2] = {0, 0};
static Widget _grey_arrow_w[2]  = {0, 0};
static Widget _plain_stops_w[2][max_glyphs + 1];
static Widget _grey_stops_w[2][max_glyphs + 1];

Boolean SourceView::CreateGlyphsWorkProc(XtPointer)
{
    static bool all_done = false;

    if (all_done)
	return True;

    int k;
    for (k = 0; k < 2; k++)
    {
	// On the Form widget, later children are displayed
	// on top of earlier children.  A stop sign hiding an arrow
	// gives more pleasing results than vice-versa, so place arrow
	// glyph below sign glyphs.

	Widget form_w = k ? code_form_w : source_form_w;

	if (_plain_arrow_w[k] == 0)
	{
	    _plain_arrow_w[k] = 
		create_glyph(form_w, "plain_arrow",
			     arrow_bits, 
			     arrow_width,
			     arrow_height);
	    return False;
	}

	if (_grey_arrow_w[k] == 0)
	{
	    _grey_arrow_w[k] = 
		create_glyph(form_w, "grey_arrow",
			     greyarrow_bits, 
			     greyarrow_width, 
			     greyarrow_height);
	    return False;
	}
    }
   
    for (k = 0; k < 2; k++)
    {
	Widget form_w = k ? code_form_w : source_form_w;

	int i;
	for (i = 0; i < max_glyphs; i++)
	{
	    if (_grey_stops_w[k][i] == 0)
	    {
		_grey_stops_w[k][i] = 
		    create_glyph(form_w, "grey_stop", greystop_bits, 
				 greystop_width, greystop_height);
		return False;
	    }
	}
	for (i = 0; i < max_glyphs; i++)
	{
	    if (_plain_stops_w[k][i] == 0)
	    {
		_plain_stops_w[k][i] = 
		    create_glyph(form_w, "plain_stop", stop_bits, 
				 stop_width, stop_height);
		return False;
	    }
	}
    }

    all_done = true;
    return True;
}

void SourceView::UpdateGlyphsWorkProc(XtPointer client_data, XtIntervalId *)
{
    // clog << "Updating glyphs...\n";

    // Show source position
    // clog << "Source arrow:\n";
    Widget& source_plain_arrow_w = _plain_arrow_w[0];
    Widget& source_grey_arrow_w  = _grey_arrow_w[0];
    Position x, y;
    XmTextPosition pos;
    Boolean pos_displayed = False;

    if (display_glyphs
	&& base_matches(current_file_name, last_execution_file)
	&& pos_of_line != 0
	&& last_execution_line > 0
	&& last_execution_line <= line_count)
    {
	pos = pos_of_line[last_execution_line];
	pos_displayed = XmTextPosToXY(source_text_w, pos, &x, &y);
    }

    if (pos_displayed)
    {
	if (at_lowest_frame)
	{
	    map_glyph(source_plain_arrow_w, x + arrow_x_offset, y);

	    if (source_grey_arrow_w)
		XtUnmapWidget(source_grey_arrow_w);
	}
	else
	{
	    map_glyph(source_grey_arrow_w, x + arrow_x_offset, y);

	    if (source_plain_arrow_w)
		XtUnmapWidget(source_plain_arrow_w);
	}
    }
    else
    {
	if (source_plain_arrow_w)
	    XtUnmapWidget(source_plain_arrow_w);
	if (source_grey_arrow_w)
	    XtUnmapWidget(source_grey_arrow_w);
    }


    // Show PC
    // clog << "Code arrow:\n";
    Widget& code_plain_arrow_w = _plain_arrow_w[1];
    Widget& code_grey_arrow_w  = _grey_arrow_w[1];
    pos_displayed = False;
    if (display_glyphs && last_execution_pc != "")
    {
	pos = find_pc(last_execution_pc);
	if (pos != XmTextPosition(-1))
	    pos_displayed = XmTextPosToXY(code_text_w, pos, &x, &y);
    }

    if (pos_displayed)
    {
	if (at_lowest_frame)
	{
	    map_glyph(code_plain_arrow_w, x + arrow_x_offset, y);
	    if (code_grey_arrow_w)
		XtUnmapWidget(code_grey_arrow_w);
	}
	else
	{
	    map_glyph(code_grey_arrow_w, x + arrow_x_offset, y);
	    if (code_plain_arrow_w)
		XtUnmapWidget(code_plain_arrow_w);
	}
    }
    else
    {
	if (code_plain_arrow_w)
	    XtUnmapWidget(code_plain_arrow_w);
	if (code_grey_arrow_w)
	    XtUnmapWidget(code_grey_arrow_w);
    }


    int plain[2]; plain[0] = 0; plain[1] = 0;
    int grey[2];   grey[0] = 0;  grey[1] = 0;

    if (display_glyphs)
    {
	// Show source breakpoints

	Widget *source_plain_stops_w = _plain_stops_w[0];
	Widget *source_grey_stops_w  = _grey_stops_w[0];
	int& source_p                = plain[0];
	int& source_g                = grey[0];

	VarArray<XmTextPosition> source_stops;

	// Map breakpoint glyphs
	// clog << "Source breakpoints:\n";
	MapRef ref;
	BreakPoint *bp;
	for (bp = bp_map.first(ref);
	     bp != 0;
	     bp = bp_map.next(ref))
	{
	    bp->source_glyph() = 0;

	    if (bp_matches(bp)
		&& pos_of_line != 0
		&& bp->line_nr() > 0
		&& bp->line_nr() <= line_count)
	    {
		pos = pos_of_line[bp->line_nr()];
		pos_displayed = 
		    XmTextPosToXY(source_text_w, pos, &x, &y);
		if (pos_displayed)
		{
		    while (!CreateGlyphsWorkProc(0))
			;

		    Widget glyph = 0;
		    if (bp->enabled())
			glyph = source_plain_stops_w[source_p] ? 
			    source_plain_stops_w[source_p++] : 0;
		    else
			glyph = source_grey_stops_w[source_g] ? 
			    source_grey_stops_w[source_g++] : 0;

		    if (glyph != 0)
		    {
			for (int i = 0; i < source_stops.size(); i++)
			    if (pos == source_stops[i])
				x += multiple_stop_x_offset;

			map_glyph(glyph, x + stop_x_offset, y);
			source_stops += pos;
			bp->source_glyph() = glyph;
		    }
		}
	    }
	}



	// Show code breakpoints

	Widget *code_plain_stops_w = _plain_stops_w[1];
	Widget *code_grey_stops_w  = _grey_stops_w[1];
	int& code_p                = plain[1];
	int& code_g                = grey[1];

	VarArray<XmTextPosition> code_stops;

	// Map breakpoint glyphs
	// clog << "Code breakpoints:\n";
	MapRef ref2;
	for (bp = bp_map.first(ref2);
	     bp != 0;
	     bp = bp_map.next(ref2))
	{
	    bp->code_glyph() = 0;

	    if (bp->type() != BREAKPOINT)
		continue;

	    pos = find_pc(bp->address());
	    pos_displayed = 
		XmTextPosToXY(code_text_w, pos, &x, &y);
	    if (pos_displayed)
	    {
		while (!CreateGlyphsWorkProc(0))
		    ;

		Widget glyph = 0;
		if (bp->enabled())
		    glyph = code_plain_stops_w[code_p] ? 
			code_plain_stops_w[code_p++] : 0;
		else
		    glyph = code_grey_stops_w[code_g] ? 
			code_grey_stops_w[code_g++] : 0;

		if (glyph != 0)
		{
		    for (int i = 0; i < code_stops.size(); i++)
			if (pos == code_stops[i])
			    x += multiple_stop_x_offset;

		    map_glyph(glyph, x + stop_x_offset, y);
		    code_stops += pos;
		    bp->code_glyph() = glyph;
		}
	    }
	}
    }

    for (int k = 0; k < 2; k++)
    {
	// Unmap remaining glyphs
	Widget w;
	while ((w = _plain_stops_w[k][plain[k]++]))
	    XtUnmapWidget(w);
	while ((w = _grey_stops_w[k][grey[k]++]))
	    XtUnmapWidget(w);
    }

    // Allow new invocations
    XtWorkProcId *proc_id = ((XtWorkProcId *) client_data);
    if (proc_id)
	*proc_id = 0;
}

// Change setting of display_glyphs
void SourceView::set_display_glyphs(bool set)
{
    if (display_glyphs != set)
    {
	StatusDelay delay(set ? "Enabling glyphs" : "Disabling glyphs");

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
	    refresh_bp_disp();
	    lookup();
	}
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

static string first_address(string s)
{
    int index = s.index("0x");
    if (index < 0)
	return "";

    s = s.from(index);
    return s.through(rxalphanum);
}

static string last_address(string s)
{
    int index = s.index("\n0x", -1);
    if (index < 0)
	return "";

    s = s.from(index + 1);
    return s.through(rxalphanum);
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
	    && current_code[j] == '0'
	    && current_code[j + 1] == 'x')
	{
	    string line = current_code.at(j, eol - j);
	    string address = line.from("0x");
	    address = line.through(rxalphanum);
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

Delay *SourceView::refresh_code_pending = 0;

// Process `disassemble' output
void SourceView::refresh_codeOQC(const string& answer, void *client_data)
{
    RefreshInfo *info = (RefreshInfo *)client_data;
    process_disassemble(answer);

    if (find_pc(info->pc) != XmTextPosition(-1))
	show_pc(info->pc, info->mode);

    if (refresh_code_pending)
    {
	delete refresh_code_pending;
	refresh_code_pending = 0;
    }
}

void SourceView::refresh_codeWorkProc(XtPointer client_data, XtIntervalId *)
{
    if (!disassemble)
    {
	if (refresh_code_pending)
	{
	    delete refresh_code_pending;
	    refresh_code_pending = 0;
	}
	return;
    }

    RefreshInfo *info = (RefreshInfo *)client_data;
    bool ok = gdb->send_question("disassemble " + info->pc, 
				 refresh_codeOQC, (void *)info);

    if (!ok)
    {
	// Try again in 250ms
	XtAppAddTimeOut(XtWidgetToApplicationContext(source_text_w), 250,
			refresh_codeWorkProc, client_data);
    }
}

static string last_shown_pc;

// Show program counter location PC
void SourceView::show_pc(const string& pc, XmHighlightMode mode)
{
    if (!disassemble)
	return;
	
    // clog << "Showing PC " << pc << "\n";

    last_shown_pc = pc;

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
	static RefreshInfo info;
	info.pc    = pc;
	info.mode  = mode;

	if (refresh_code_pending)
	{
	    // Working proc already started: clear old status line
	    delete refresh_code_pending;
	}
	else
	{
	    // Start new working proc: send `disassemble' command
	    // after the running command completed.
	    XtAppAddTimeOut(XtWidgetToApplicationContext(source_text_w), 0,
			    refresh_codeWorkProc, XtPointer(&info));
	}

	// Show new status
	refresh_code_pending = 
	    new StatusDelay("Disassembling location " + pc);
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
	last_execution_pc = pc;

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
	    last_pos_pc             = pos;
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
	update_glyphs();
}

void SourceView::set_disassemble(bool set)
{
    if (disassemble != set)
    {
	disassemble = set;

	if (!disassemble)
	    XtUnmanageChild(code_form_w);
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

// Some DBXes lack a `clear' command.  Use `delete' instead.
string SourceView::clear_command(string pos)
{
    if (gdb->has_clear_command())
	return "clear " + pos;

    string file = current_file_name;
    string line = pos;

    if (pos.contains(':'))
    {
	file = pos.before(':');
	line = pos.after(':');
    }

    int line_no = atoi(line);

    string bps = "";
    MapRef ref;
    for (BreakPoint* bp = bp_map.first(ref);
	 bp != 0;
	 bp = bp_map.next(ref))
    {
	if (bp->line_nr() == line_no
	    && (bp->file_name() == "" 
		|| file_matches(bp->file_name(), file)))
	    {
		if (bps != "")
		    bps += " ";
		bps += itostring(bp->number());
	    }
    }

    if (bps == "")
	return "";

    string cmd;
    switch (gdb->type())
    {
    case GDB:
    case DBX:
	cmd = "delete ";
	break;

    case XDB:
	cmd = "db ";
	break;
    }

    return cmd + bps;
}

// Some DBXes require `{ COMMAND; }', others `{ COMMAND }'.
string SourceView::command_list(string cmd)
{
    if (gdb->has_when_semicolon())
	return "{ " + cmd + "; }";
    else
	return "{ " + cmd + " }";
}

