// $Id$
// Use the Source, Luke.

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Dorothea Luetkehaus (luetke@ips.cs.tu-bs.de)
// and Andreas Zeller (zeller@ips.cs.tu-bs.de).
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
// 675 Mass Ave, Cambridge, MA 02139, USA.
// 
// DDD is the GDB-based data display debugger.
// Contact ddd@ips.cs.tu-bs.de for details.

#ifdef __GNUG__
#pragma implementation "Assoc.h"
#endif

char SourceView_rcsid[] =
    "$Id$";

//-----------------------------------------------------------------------------
#include "SourceView.h"
#include "IntArray.h"

// includes von Nora-Zeugs
#include "assert.h"
#include "Delay.h"
#include "HelpCB.h"
#include "DestroyCB.h"
#include "MString.h"
#include "events.h"
#include "cook.h"

// includes von Motif-Zeugs
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/MessageB.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <Xm/RowColumn.h>
#include <Xm/PushB.h>
#include <Xm/SelectioB.h>
#include <Xm/List.h>

#if XmVersion < 1002
// _XmTextEnableRedisplay(), _XmTextDisableRedisplay()
extern "C" {
#define new new_w
#define class class_w
#undef MAXINT
#include <Xm/TextP.h>
#undef class
#undef new
}; 
#endif

// sonstige includes
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <iomanip.h>
#include <stdlib.h> // fuer atoi
#include <string.h> // fuer strdup, strchr
#include <ctype.h> // fuer strdup, strchr

// includes eigener Sachen
#include "PosBuffer.h"
#include "string-fun.h"
#include "ddd.h"
#include "version.h"
#include "mydialogs.h"

inline int isid(char c)
{
    return isalnum(c) || c == '_';
}


// ***************************************************************************
//-----------------------------------------------------------------------
// Xt-Zeugs
//-----------------------------------------------------------------------
XtActionsRec SourceView::actions [] = {
    {"source-popup-menu",        SourceView::srcpopupAct},
    {"source-start-select-word", SourceView::startSelectWordAct},
    {"source-end-select-word",   SourceView::endSelectWordAct},
};

//-----------------------------------------------------------------------
// Menues
//-----------------------------------------------------------------------

// Popup-Menues - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
struct LineItms { enum Itms {SetBP, SetTempBP}; };
MMDesc SourceView::line_popup[] =
{
    {"set",      MMPush, {SourceView::line_popup_setCB}},
    {"set_temp", MMPush, {SourceView::line_popup_set_tempCB}},
    MMEnd
};

struct BPItms { enum Itms {Disable, Dummy1, Condition, IgnoreCount, 
			   Dummy2, Delete}; };
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


struct TextItms { enum Itms {Print, Disp, Dummy, Lookup, Break, Clear}; };
static string text_cmd_labels[] =
{"print ", "display ", "", "lookup " , "break ", "clear "};
MMDesc SourceView::text_popup[] =
{
    {"print",  MMPush, {SourceView::text_popup_printCB}},
    {"disp",   MMPush, {SourceView::text_popup_dispCB}},
    MMSep,
    {"lookup", MMPush, {SourceView::text_popup_lookupCB}},
    {"break",  MMPush, {SourceView::text_popup_breakCB}},
    {"clear",  MMPush, {SourceView::text_popup_clearCB}},
    MMEnd
};


//-----------------------------------------------------------------------
// Daten
//-----------------------------------------------------------------------

Widget SourceView::toplevel_w                = 0;
Widget SourceView::source_view_w             = 0;
Widget SourceView::source_text_w             = 0;
Widget SourceView::edit_breakpoints_dialog_w = 0;
Widget SourceView::breakpoint_list_w         = 0;
Widget SourceView::stack_dialog_w            = 0;
Widget SourceView::frame_list_w              = 0;
Widget SourceView::up_w                      = 0;
Widget SourceView::down_w                    = 0;

bool SourceView::stack_dialog_popped_up = false;
bool SourceView::cache_source_files     = true;

int  SourceView::bp_indent_amount = 0;

Map<int, BreakPoint> SourceView::bp_map;

string SourceView::current_file_name = "";
int    SourceView::line_count = 0;
Assoc<int, VarArray<int> >* SourceView::bps_in_line = 0;
XmTextPosition*             SourceView::pos_of_line = 0;
Assoc<string, string> SourceView::file_cache;

string SourceView::current_text;

XmTextPosition SourceView::last_pos = 0;
XmTextPosition SourceView::last_start_highlight = 0;
XmTextPosition SourceView::last_end_highlight = 0;

XmTextPosition SourceView::last_start_secondary_highlight = 0;
XmTextPosition SourceView::last_end_secondary_highlight = 0;

string SourceView::last_execution_file = "";
int    SourceView::last_execution_line = 0;

StringArray SourceView::history;
int SourceView::history_position = 0;
bool SourceView::history_locked = false;

//-----------------------------------------------------------------------
// Helping functions
//-----------------------------------------------------------------------

inline string basename(string file)
{
    if (file.contains('/'))
	return file.after('/', -1);
    else
	return file;
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
	gdb_command("break " + basename(current_file_name) + ":" + 
		    itostring(line_nr), w);
	break;
	
    case DBX:
	gdb_command("file " + current_file_name, w);
	gdb_command("stop at " + itostring(line_nr), w);
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
	gdb_command("tbreak " + basename(current_file_name) + ":" + 
		    itostring(line_nr), w);
	break;

    case DBX:
	gdb_command("file " + current_file_name, w);
	gdb_command("stop at " + itostring(line_nr), w);
	gdb_command("when at " + itostring(line_nr) 
		    + " { clear " + itostring(line_nr) + "; }", w);
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
    gdb_command("delete " + itostring(bp_nr), w);
}


// ***************************************************************************
//
void SourceView::bp_popup_disableCB (Widget w,
				     XtPointer client_data,
				     XtPointer call_data)
{
    int bp_nr = *((int *)client_data);

    string cmd;
    if (bp_map.get(bp_nr)->enabled())
	cmd = "disable ";
    else
	cmd = "enable ";

    cmd += itostring(bp_nr);
    gdb_command(cmd, w);
}

// ***************************************************************************
//
void SourceView::text_popup_breakCB (Widget w,
				     XtPointer client_data,
				     XtPointer call_data)
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
    }
}

void SourceView::text_popup_clearCB (Widget w,
				     XtPointer client_data,
				     XtPointer call_data)
{
    string* word_ptr = (string*)client_data;
    assert(word_ptr->length() > 0);
    string pos;

    switch (gdb->type())
    {
    case GDB:
	gdb_command("clear " + *word_ptr, w);
	break;

    case DBX:
	pos = dbx_lookup(*word_ptr);
	if (pos != "")
	{
	    gdb_command("file " + pos.before(':'), w);
	    gdb_command("clear " + pos.after(':'), w);
	}
	break;
    }
}



// ***************************************************************************
//
void SourceView::text_popup_printCB (Widget w,
				     XtPointer client_data,
				     XtPointer call_data)
{
    string* word_ptr = (string*)client_data;
    assert(word_ptr->length() > 0);

    gdb_command("print " + *word_ptr, w);
}



// ***************************************************************************
//
void SourceView::text_popup_dispCB (Widget w,
				    XtPointer client_data,
				    XtPointer call_data)
{
    string* word_ptr = (string*)client_data;
    assert(word_ptr->length() > 0);

    gdb_command("graph display " + *word_ptr, w);
}


// ***************************************************************************
//
void SourceView::text_popup_lookupCB (Widget w,
				      XtPointer client_data,
				      XtPointer call_data)
{
    string* word_ptr = (string*)client_data;
    lookup(*word_ptr);
}



// ***************************************************************************
//

// If this is true, no motion occurred while selecting
static bool selection_click = false;

static string last_info_output = "";

void SourceView::set_source_argCB (Widget w, 
				   XtPointer client_data, 
				   XtPointer call_data)
{
    if (current_text == "")
	return;

    if (bool(client_data))
	selection_click = false;

    XmTextPosition startPos, endPos;
    if (XmTextGetSelectionPosition(source_text_w, &startPos, &endPos))
    {
	int startIndex = 0;
	if (startPos > 0)
	    startIndex = 
		current_text.index('\n', startPos - current_text.length()) + 1;

	int endIndex = 0;
	if (endPos > 0)
	    endIndex = 
		current_text.index('\n', endPos - current_text.length()) + 1;

	if (selection_click
	    && startIndex == endIndex
	    && startPos < current_text.length()
	    && endPos < current_text.length()
	    && current_text[startPos] != '\n'
	    && current_text[endPos] != '\n'
	    && startPos - startIndex <= bp_indent_amount
	    && endPos - endIndex <= bp_indent_amount)
	{
	    // Selection from line number area: prepend source file name
	    string line = current_text(startIndex, bp_indent_amount);
	    int line_nr = atoi(line);
	    source_arg->set_string(
		basename(current_file_name) + ":" + itostring(line_nr));

	    // If a breakpoint is here, select this one only
	    MapRef ref;
	    for (BreakPoint* bp = bp_map.first(ref);
		 bp != 0;
		 bp = bp_map.next(ref))
	    {
		bp->selected() = 
		    (bp->type() == BREAKPOINT
		     && (bp->file_name() == "" || 
			 basename(bp->file_name()) == 
			 basename(current_file_name))
		     && bp->line_nr() == line_nr);
	    }

	    fill_labels(last_info_output);
	}
	else
	{
	    // Selection from source
	    string s;
	    if (startPos < current_text.length() 
		&& endPos < current_text.length())
		s = current_text(startPos, endPos - startPos);
	    while (s.contains('\n'))
		s = s.after('\n');
	    source_arg->set_string(s);
	}
    }
}


// ***************************************************************************

// Read local file from FILE_NAME
String SourceView::read_local(const string& file_name)
{
    StatusDelay delay("Reading file " + quote(file_name));

    // Make sure the file is a regular text file and open it
    int fd = 0;
    struct stat statb;

    if (stat(file_name, &statb) < 0)
    {
	post_error (file_name + ": " + strerror(errno), 
		    "source_file_error", source_view_w);
	return 0;
    }

#if 0 
    // According to bussieck@moa.math.nat.tu-bs.de (Michael Bussieck),
    // this causes trouble on hppa1.1-hp-hpux9.01.
    if ((statb.st_mode & S_IFMT) != S_IFREG)
    {
	post_error (file_name + ": not a regular file", 
		    "source_file_error", source_view_w);
	return 0;
    }
#endif

    if ((fd = open(file_name, O_RDONLY)) < 0)
    {
	post_error (file_name + ": " + strerror(errno), 
		    "source_file_error", source_view_w);
        return 0;
    }

    // Put the contents of the file in the Text widget by allocating
    // enough space for the entire file and reading the file into the
    // allocated space.
    char* text = XtMalloc(unsigned(statb.st_size + 1));
    if (read(fd, text, statb.st_size) != statb.st_size)
	post_error (file_name + ": " + strerror(errno),
		    "source_trunc_error", source_view_w);
    close(fd);

    text[statb.st_size] = '\0'; // be sure to null-terminate

    if (statb.st_size == 0)
	post_warning("File " + quote(file_name) + " is empty.", 
		     "source_empty_warning");
    return text;
}


// Read (possibly remote) file FILE_NAME; a little slower
String SourceView::read_remote(const string& file_name)
{
    StatusDelay delay("Reading file " + 
		      quote(file_name) + " from remote host");

    string cat_command = sh_command(string("cat") + " " + file_name);

    Agent cat(cat_command);
    cat.start();

    FILE *fp = cat.inputfp();
    if (fp == 0)
	return 0;

    int n = 0;
    String text = XtMalloc(1);

    do {
	text = XtRealloc(text, n + BUFSIZ + 1);
	n += fread(text + n, sizeof(char), BUFSIZ, fp);
    } while (!feof(fp));

    text[n] = '\0';  // be sure to null-terminate

    if (n == 0)
	post_error("Cannot access remote file " + quote(file_name), 
		   "remote_file_error");
    return text;
}


// Read file FILE_NAME via the GDB `list' function
// Really slow, is guaranteed to work for source files.
String SourceView::read_from_gdb(const string& file_name)
{
    if (!gdb->isReadyWithPrompt())
	return 0;

    StatusDelay delay("Reading file " + quote(file_name) + " from debugger");

    string command;
    switch (gdb->type())
    {
    case GDB:
	command = "list " + file_name + ":1,1000000";
	break;

    case DBX:
	command = "list 1,1000000";
	break;
    }
    string listing = gdb_question(command);

    // GDB listings have the format <NUMBER>\t<LINE>.
    // Copy LINE only; line numbers will be re-added later.
    // Note that tabs may be expanded to spaces due to a PTY interface.
    String text = XtMalloc(listing.length());

    int i = 0;
    int j = 0;
    while (i < listing.length())
    {
	if (isdigit(listing[i]))
	{
	    // Skip line number
	    int count = 0;
	    while (i < listing.length() && isdigit(listing[i]))
		i++, count++;

	    // Break at first non-blank character or after 8 characters
	    while (count < 8 && i < listing.length() && listing[i] == ' ')
		i++, count++;

	    // Skip tab character
	    if (count < 8 && i < listing.length() && listing[i] == '\t')
		i++;

	    // Copy line
	    while (i < listing.length() && listing[i] != '\n')
		text[j++] = listing[i++];

	    // Copy newline character
	    text[j++] = '\n';
	    i++;
	}
	else
	{
	    int start = i;

	    // Some other line -- the prompt, maybe?
	    while (i < listing.length() && listing[i] != '\n')
		i++;
	    if (i < listing.length())
		i++;

	    string msg = listing.from(start);
	    post_gdb_message(msg, source_view_w);
	}
    }

    text[j] = '\0';  // be sure to null-terminate
    return text;
}


// Read file FILE_NAME and format it
String SourceView::read_indented(string& file_name)
{
    Delay delay;

    String text;
    if (!remote_gdb())
	text = read_local(file_name);
    else
	text = read_remote(file_name);

    if (text == 0 || text[0] == '\0')
    {
	string source_name = basename(file_name);
	text = read_from_gdb(source_name);

	if (text != 0 && text[0] != '\0')
	{
#if 0
	    post_warning("File was read as source from GDB.",
			 "source_file_from_gdb_warning");
#endif
	    file_name = source_name;
	}
    }

    if (text == 0 || text[0] == '\0')
	return 0;

    // Im Text Platz fuer bpoint-Informationen schaffen.
    int lines = 0;
    char* text_ptr = text; // durchlaeuft text
    while (*text_ptr != '\0')
	if (*text_ptr++ == '\n')
	    lines++;

    int indented_text_length = text_ptr - text + bp_indent_amount * lines;
    if (text_ptr > text && text_ptr[-1] != '\n')
    {
	// ein Zeichen mehr fuer '\n' am Ende
	indented_text_length += 1;
    }

    String indented_text = XtMalloc(indented_text_length + 1);

    char line_no_s[bp_indent_amount];
    for (int i = 0; i < bp_indent_amount; i++)
	line_no_s[i] = ' ';

    text_ptr = text;
    char *pos_ptr = indented_text; // Schreibposition in indented_text
    while (*text_ptr != '\0')
    {
	// Increase line number
	for (int i = bp_indent_amount - 2; i >= 0; i--)
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
	while (*text_ptr != '\0' && *text_ptr != '\n')
	    *pos_ptr++ = *text_ptr++;

	// '\n' bzw. '\0' kopieren
	if (*text_ptr == '\0')
	{
	    // text doesn't end in '\n'
	    *pos_ptr++ = '\n';
	}
	else
	{
	    *pos_ptr++ = *text_ptr++;
	}
    }

    assert (pos_ptr - indented_text == indented_text_length);
    *pos_ptr = '\0';

    XtFree(text);

    return indented_text;
}


// Read file FILE_NAME into current_text; get it from the cache if possible
int SourceView::read_current(string& file_name)
{
    if (cache_source_files && file_cache.has(file_name))
    {
	current_text = file_cache[file_name];
    }
    else
    {
	String indented_text = read_indented(file_name);
	if (indented_text == 0)
	    return -1;

	current_text = indented_text;
	XtFree(indented_text);

	file_cache[file_name] = current_text;
    }

    // Setup global parameters

    // Number of lines
    line_count = current_text.freq('\n');

    // Line positions
    if (pos_of_line != 0) {
	delete[] pos_of_line;
    }
    pos_of_line = new XmTextPosition[line_count + 2];
    pos_of_line[0] = XmTextPosition(0);
    pos_of_line[1] = XmTextPosition(0);

    int l = 2;
    for (int i = 0; i < current_text.length() - 1; i++)
	if (current_text[i] == '\n')
	    pos_of_line[l++] = XmTextPosition(i + 1);

    assert(l == line_count + 1);

    return 0;
}

// Clear the file cache
void SourceView::clear_file_cache()
{
    static Assoc<string, string> empty;
    file_cache = empty;
}

void SourceView::read_file (string file_name, int initial_line)
{
    if (file_name == "")
	return;

    // Read in current_text
    int error = read_current(file_name);
    if (error)
	return;

    add_to_history(file_name, initial_line);

    // The remainder may take some time...
    Delay delay;

    // Set cursor position
    short rows = 0;
    XtVaGetValues(source_text_w,
		  XmNrows, &rows,
		  NULL);

    XmTextPosition initial_pos = 0;
    if (initial_line > 0 && initial_line <= line_count)
	initial_pos = pos_of_line[initial_line] + bp_indent_amount;

    XmTextPosition initial_top = 0;
    if (initial_line > rows / 2 && initial_line <= line_count)
	initial_top = pos_of_line[initial_line - rows / 2];

    // Set string and initial line
#if XmVersion >= 1002
    XmTextDisableRedisplay(source_text_w);
#else
    _XmTextDisableRedisplay(XmTextWidget(source_text_w), FALSE);
#endif
    XtVaSetValues(source_text_w,
		  XmNvalue, String(current_text),
		  XmNcursorPosition, initial_pos,
		  XmNtopCharacter, initial_top,
		  NULL);
#if XmVersion >= 1002
    XmTextEnableRedisplay(source_text_w);
#else
    _XmTextEnableRedisplay(XmTextWidget(source_text_w));
#endif

    if (file_name != current_file_name) {
	current_file_name = file_name;
	update_title();
    }

    // Breakpoint-Anzeige aktualisieren
    if (bps_in_line) {
	// ist jetzt ungueltig
	delete bps_in_line;
	bps_in_line = 0;
    }

    refresh_bp_disp();

    XtManageChild(source_view_w);
    XtManageChild(arg_cmd_w);

    ostrstream os;
    os << "File " << quote(file_name) << " " 
       << line_count << " lines, "
       << current_text.length() << " characters";

    string status(os);
    set_status(status);

    XmTextClearSelection(source_text_w, 
			 XtLastTimestampProcessed(XtDisplay(source_text_w)));
    XmTextSetHighlight(source_text_w,
		       0, XmTextGetLastPosition(source_text_w),
		       XmHIGHLIGHT_NORMAL);
    last_pos = last_start_highlight = last_end_highlight = 0;
    last_start_secondary_highlight = last_end_secondary_highlight = 0;

    if (source_view_shell)
    {
	static bool initial_popup = true;
	if (initial_popup)
	{
	    popup_shell(source_view_shell);
	    initial_popup = false;
	}
    }
}

void SourceView::update_title()
{
    if (toplevel_w == 0)
	return;

    string title = string(DDD_NAME) + ": " + current_file_name;
    String title_s = title;

    string icon = string(DDD_NAME) + ": " + basename(current_file_name);
    String icon_s = icon;

    XtVaSetValues(toplevel_w,
		  XmNtitle, title_s,
		  XmNiconName, icon_s,
		  NULL);
}



// ***************************************************************************
// Bringt Breakpoint-Anzeige auf den aktuellen Stand (gemaess bp_map).
//
void SourceView::refresh_bp_disp ()
{
    if (current_text == "")
	return;

    // Alte Breakpoint-Darstellungen ueberschreiben - - - - - - - - - - - - -
    if (bps_in_line != 0)
    {
	for (AssocIter<int, VarArray<int> > b_i_l_iter = *(bps_in_line);
	     b_i_l_iter.ok();
	     b_i_l_iter = b_i_l_iter.next())
	{
	    int line_nr = b_i_l_iter.key();
	    if (line_nr < 0 || line_nr > line_count)
		continue;

	    string s(current_text.at(int(pos_of_line[line_nr]),
				     bp_indent_amount - 1));

	    XmTextReplace (source_text_w,
			   pos_of_line[line_nr],
			   pos_of_line[line_nr] + bp_indent_amount - 1,
			   String(s));
	}

	delete bps_in_line;
    }

    // neue Breakpoint-Darstellungen erzeugen - - - - - - - - - - - - - - - -
    bps_in_line = new Assoc<int, VarArray<int> >;

    // alle das File betreffende Breakpoints raussuchen.
    MapRef ref;
    for (BreakPoint* bp = bp_map.first(ref);
	 bp != 0;
	 bp = bp_map.next(ref))
    {
	if (bp->type() == BREAKPOINT && 
	    (bp->file_name() == "" || 
	     basename(bp->file_name()) == basename(current_file_name)))
	{
	    (*bps_in_line)[bp->line_nr()] += bp->number();
	}
    }

    // fuer alle Zeilen mit Breakpoints ...
    for (AssocIter<int, VarArray<int> > b_i_l_iter = *(bps_in_line);
	 b_i_l_iter.ok();
	 b_i_l_iter = b_i_l_iter.next())
    {
	int line_nr = b_i_l_iter.key();
	if (line_nr < 0 || line_nr > line_count)
	    continue;

	XmTextPosition pos = pos_of_line[line_nr];

	string insert_string = "";

	// Darstellung fuer alle Breakpoints der Zeile
	int i;
	for (i = 0; i < (*bps_in_line)[line_nr].size(); i++) {
	    BreakPoint* bp =
		bp_map.get((*bps_in_line)[line_nr][i]);
	    assert (bp);
	    insert_string += bp->enabled() ? '#' : '_';
	    insert_string += bp->number_str();
	    insert_string += bp->enabled() ? '#' : '_';
	}
	if (insert_string.length() >= bp_indent_amount - 1) {
	    insert_string =
		insert_string.before(bp_indent_amount - 1);
	}
	else
	{
	    for (i = insert_string.length();
		 i < bp_indent_amount-1;
		 i++)
	    {
		insert_string += current_text[pos + i];
	    }
	}
	assert(insert_string.length() == bp_indent_amount - 1);

	XmTextReplace (source_text_w,
		       pos, pos + bp_indent_amount - 1,
		       String(insert_string));
    }
}




// ***************************************************************************
// Findet zu pos die passende Zeilennummer.
// ist in_text!=0, so ist *in_text==true wenn pos im Quelltext-Bereich ist.
// ist bp_nr!=0, so ist *bp_nr die Nr des Brekpoints, der an Position pos
// dargestellt wird, 0 sonst.
//
bool SourceView::get_line_of_pos (XmTextPosition pos,
				  int*     line_nr_ptr,
				  bool* in_text,
				  int*     bp_nr_ptr)
{
    bool found = false;
    XmTextPosition line_pos = 0;
    XmTextPosition next_line_pos = 0;
    *line_nr_ptr = 1;

    while (!found
	   && line_count >= *line_nr_ptr) {

	next_line_pos =(line_count >= (*line_nr_ptr + 1)) ?
	    pos_of_line[*line_nr_ptr + 1] :
	    XmTextGetLastPosition (source_text_w) + 1;

	if (pos < (line_pos + bp_indent_amount - 1)) {
	    // pos liegt in dieser Zeile im Breakpoint-Anzeige-Bereich.
	    // ---------------------------------------------------------------
	    found = true;
	    if (in_text)
		*in_text = false;
	    if (bp_nr_ptr) {
		// Pruefen ob auf breakpoint geklickt wurde
		if ((*bps_in_line)[*line_nr_ptr].size() >= 1) {

		    // Breakpoint(s) in der Zeile
		    // ermitteln, auf welchen gedrueckt wurde.
		    *bp_nr_ptr = 0;
		    int i;
		    XmTextPosition bp_disp_pos = line_pos;
		    for (i = 0;
			 i < (*bps_in_line)[*line_nr_ptr].size();
			 i++) {
			BreakPoint* bp = bp_map.get(
				(*bps_in_line)[*line_nr_ptr][i]);
			assert (bp);
			bp_disp_pos += 2; //fuer '#' bzw. '_';
			bp_disp_pos += bp->number_str().length();
			if (pos < bp_disp_pos) {
			    *bp_nr_ptr =
				 (*bps_in_line)[*line_nr_ptr][i];
			    break; // for-Schleife fertig
			}
		    }
		}
		else {
		    *bp_nr_ptr = 0;
		}
	    }
	}
	else if (pos < next_line_pos) {
	    // pos liegt in dieser Zeile im Text-Bereich.
	    // ---------------------------------------------------------------
	    found = true;
	    if (in_text)
		*in_text = true;
	    if (bp_nr_ptr)
		*bp_nr_ptr = 0;
	}
	else {
	    // pos liegt in einer der folgenden Zeilen.
	    // ---------------------------------------------------------------
	    line_pos = next_line_pos;
	    (*line_nr_ptr)++;
	}
    }
    return found;
}


// ***************************************************************************
//
void SourceView::find_word_bounds (const XmTextPosition pos,
				   XmTextPosition& startpos,
				   XmTextPosition& endpos)
{
    startpos = endpos = pos;

    while (startpos > 0 && startpos < current_text.length()
	   && isid(current_text[startpos - 1]))
	startpos--;

    while (endpos < current_text.length() && isid(current_text[endpos]))
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
void SourceView::refresh_bpsOQC (string answer, void* data)
{
    process_info_bp (answer);
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
    XtSetArg (args[arg], XmNselectionArrayCount, 1); arg++;
    source_text_w = XmCreateScrolledText (parent,
					  "source_text_w",
					  args, arg);
    source_view_w = source_text_w;
    XtManageChild(source_text_w);
    XtAddCallback(source_text_w, XmNgainPrimaryCallback,
		  set_source_argCB, XtPointer(false));
    XtAddCallback(source_text_w, XmNmotionVerifyCallback,
		  set_source_argCB, XtPointer(true));
    XtAppAddActions (app_context, actions, XtNumber (actions));

    // XtManageChild (source_view_w);


    // Create breakpoint editor
    arg = 0;
    XtSetArg(args[arg], XmNvisibleItemCount, 0); arg++;
    edit_breakpoints_dialog_w =
	XmCreatePromptDialog(source_text_w, "edit_breakpoints_dialog", 
			     args, arg);
    Delay::register_shell(edit_breakpoints_dialog_w);

    XtUnmanageChild(XmSelectionBoxGetChild(edit_breakpoints_dialog_w,
					   XmDIALOG_TEXT));
    XtUnmanageChild(XmSelectionBoxGetChild(edit_breakpoints_dialog_w,
					   XmDIALOG_CANCEL_BUTTON));
    XtUnmanageChild(XmSelectionBoxGetChild(edit_breakpoints_dialog_w,
					   XmDIALOG_SELECTION_LABEL));

    Widget form1 = 
	XmCreateRowColumn(edit_breakpoints_dialog_w, "form1", NULL, 0);

    Widget label =
	XmCreateLabel(form1, "breakpoints", NULL, 0);

    Widget form2 = XmCreateRowColumn(form1, "form2", NULL, 0);

    arg = 0;
    breakpoint_list_w = 
	XmCreateScrolledList(form2, "breakpoints", args, arg);
    Widget buttons = MMcreateWorkArea(form2, "buttons", bp_area);
    MMaddCallbacks (bp_area);

    XtManageChild (buttons);
    XtManageChild (breakpoint_list_w);
    XtManageChild (form2);
    XtManageChild (label);
    XtManageChild (form1);

    XtAddCallback(breakpoint_list_w,
		  XmNsingleSelectionCallback,
		  UpdateBreakpointButtonsCB,
		  0);
    XtAddCallback(breakpoint_list_w,
		  XmNmultipleSelectionCallback,
		  UpdateBreakpointButtonsCB,
		  0);
    XtAddCallback(breakpoint_list_w,
		  XmNextendedSelectionCallback,
		  UpdateBreakpointButtonsCB,
		  0);
    XtAddCallback(breakpoint_list_w,
		  XmNbrowseSelectionCallback,
		  UpdateBreakpointButtonsCB,
		  0);
    XtAddCallback(edit_breakpoints_dialog_w,
		  XmNokCallback,
		  UnmanageThisCB,
		  edit_breakpoints_dialog_w);
    XtAddCallback(edit_breakpoints_dialog_w,
		  XmNhelpCallback,
		  ImmediateHelpCB,
		  0);

    // Create stack view
    arg = 0;
    XtSetArg(args[arg], XmNautoUnmanage, False); arg++;
    stack_dialog_w =
	XmCreateSelectionDialog(source_text_w, "stack_dialog", args, arg);
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
}


// ***************************************************************************
//

void SourceView::show_execution_position (string position)
{
    if (position == "")
    {
	// Alte Markierung entfernen
	String sep = " ";
	XmTextReplace (source_text_w,
		       last_pos + bp_indent_amount - 1,
		       last_pos + bp_indent_amount,
		       sep);

	if (last_start_highlight)
	    XmTextSetHighlight (source_text_w,
				last_start_highlight, last_end_highlight,
				XmHIGHLIGHT_NORMAL);

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

    if (file_name != current_file_name)
	read_file(file_name, line);

    if (file_name == current_file_name)
    {
	if (bp_indent_amount > 0)
	{
	    // Alte Markierung entfernen
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

    if (file != current_file_name)
	read_file(file, line);
    else if (line >= 1 && line <= line_count)
	add_to_history(file, line);

    if (line < 1 || line > line_count)
	return;

    XmTextPosition pos = pos_of_line[line];
    XmTextSetInsertionPosition(source_text_w, pos + bp_indent_amount);
    XmTextShowPosition(source_text_w, pos + bp_indent_amount);

    // akt. Zeile markieren
    if (bp_indent_amount > 0) {
	// neue Markierung setzen
	String marker = ">";
	XmTextReplace (source_text_w,
		       pos + bp_indent_amount - 1,
		       pos + bp_indent_amount,
		       marker);
    }

    XmTextPosition pos_line_end = 0;
    if (current_text != "")
	pos_line_end = current_text.index('\n', pos) + 1;

    if (pos != last_start_highlight || pos_line_end != last_end_highlight)
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

    if (file_name != current_file_name)
	read_file(file_name, line);
    add_to_history(file_name, line);

    // Fenster scrollt an Position
    if (line > 0 && line <= line_count)
    {
	int pre_line = line;
	if (line > 3)
	    pre_line = line - 3; // Show the previous three lines as well

	XmTextPosition pos = pos_of_line[line];
	XmTextSetInsertionPosition(source_text_w, pos + bp_indent_amount);

	XmTextPosition pre_pos = pos_of_line[pre_line];
	XmTextShowPosition(source_text_w, pre_pos + bp_indent_amount);
	XmTextShowPosition(source_text_w, pos + bp_indent_amount);

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
    bool changed = false;

    while (info_output != "") 
    {
	int bp_nr;
	string bp_nr_s;
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
	    if (!info_output.contains('(', 0))
	    {
		// Skip this line
		info_output = info_output.after('\n');
		continue;
	    }
	    bp_nr_s = info_output.after('(');
	    bp_nr = get_positive_nr (bp_nr_s);
	    break;
	}

	if (bp_nr < 0)
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
    clear_dbx_lookup_cache();
    current_file_name = "";

    switch (gdb->type())
    {
    case GDB:
    {
	PosBuffer pos_buffer;
	pos_buffer.filter(info_output);
	pos_buffer.answer_ended();
	if (pos_buffer.pos_found()) {
	    show_position(pos_buffer.get_position());
	}
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

    for (int i = 0; i < XtNumber(strips); i++)
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
    // Strip newlines at beginning and end; post the message.

    while (info_output.length() > 0 && 
	   info_output[0] == '\n')
	info_output = info_output.after(0);
    while (info_output.length() > 0 && 
	   info_output[info_output.length() - 1] == '\n')
	info_output = info_output.before(int(info_output.length() - 1));

    if (info_output != "")
	post_gdb_message(info_output, source_view_w);
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
	if (last_execution_file == "")
	{
	    post_error("No current execution position.", 
		       "source_position_error", source_text_w);
	    return;
	}

	_show_execution_position(last_execution_file, last_execution_line);
    }
    else if (isdigit(s[0]))
    {
	// Line number given
	int line = atoi(s);
	if (line > 0 && line <= line_count)
	{
	    show_position(current_file_name + ":" + itostring(line));
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
	    for (BreakPoint* bp = bp_map.first(ref);
		 bp != 0;
		 bp = bp_map.next(ref))
	    {
		if (nr == bp->number())
		{
		    if (bp->file_name() != "")
			show_position(bp->file_name() + ":" + 
				      itostring(bp->line_nr()));
		    else
			show_position(itostring(bp->line_nr()));
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
	show_position(s);
    }
    else
    {
	// Function given
	switch (gdb->type())
	{
	case GDB:
	    if (s.length() > 0 && s[0] != '\'')
		s = '\'' + s + '\'';
	    gdb_command("info line " + s);
	    break;
	case DBX:
	    gdb_command("func " + s);
	    break;
	}
    }
}


// ***************************************************************************
// History
// ***************************************************************************

void SourceView::add_to_history(string file_name, int line)
{
    if (history_locked)
    {
	history_locked = false;
	return;
    }

    string entry = file_name + ":" + itostring(line);

    if (history_position >= history.size() ||
	history[history_position] != entry)
    {
	history_position++;
	if (history_position >= history.size())
	    history += entry;
	else
	    history[history_position] = entry;
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
	clog << i << (i == history_position ? "* " : "  ") 
	     << history[i] << "\n";
    clog << "\n";
#endif
}

void SourceView::goto_entry(string entry)
{
    string file_name = entry.before(':');
    string line_str = entry.after(':');
    int line = atoi(line_str);

    if (file_name != current_file_name)
    {
	history_locked = true;
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
	if (current_text != "")
	{
	    last_end_secondary_highlight 
		= current_text.index('\n', last_start_secondary_highlight) + 1;
	}

	XmHighlightMode mode = XmHIGHLIGHT_SECONDARY_SELECTED;

	if (line == last_execution_line && file_name == last_execution_file)
	    mode = XmHIGHLIGHT_SELECTED;

	XmTextSetHighlight(source_text_w,
			   last_start_secondary_highlight,
			   last_end_secondary_highlight,
			   mode);

	XmTextPosition pos = 
	    last_start_secondary_highlight + bp_indent_amount;
	XmTextSetInsertionPosition(source_text_w, pos);
	XmTextShowPosition(source_text_w, pos);
    }
}

void SourceView::go_back()
{
    if (history_position > 0 && history.size() > 0)
	goto_entry(history[--history_position]);
}

void SourceView::go_forward()
{
    if (history_position < history.size() - 1)
	goto_entry(history[++history_position]);
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

    if (current_text == "")
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
	    if (cursor == startpos && cursor < current_text.length())
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
	    pos = current_text.index(s, cursor);
	    if (pos < 0)
	    {
		if (wraps++)
		    break;
		pos = current_text.index(s, 0);
	    }
	    break;
	case backward:
	    pos = current_text.index(s, cursor - current_text.length() - 1);
	    if (pos < 0)
	    {
		if (wraps++)
		    break;
		pos = current_text.index(s, -1);
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
	    if (pos > 0 && pos < current_text.length())
	    {
		if (isid(current_text[pos]) && isid(current_text[pos - 1]))
		    continue;
	    }

	    if (pos + matchlen < current_text.length())
	    {
		if (isid(current_text[pos + matchlen - 1]) && 
		    isid(current_text[pos + matchlen]))
		    continue;
	    }
	}

	// We got it!
	break;
    }

    if (pos > 0)
    {
	XmTextSetSelection(source_text_w, pos, pos + matchlen, time);
	XmTextSetInsertionPosition(source_text_w, cursor);
	XmTextShowPosition(source_text_w, cursor);
    }
    else
    {
	post_warning(quote(s) + " not found.", "source_find_error", 
		     source_text_w);
    }
}

// ***************************************************************************
//
string SourceView::line_of_cursor()
{
    XmTextPosition pos = XmTextGetInsertionPosition(source_text_w);

    int line_nr;
    bool in_text;
    int bp_nr;

    if (get_line_of_pos (pos, &line_nr, &in_text, &bp_nr) == false)
	return "";

    return basename(current_file_name) + ":" + itostring(line_nr);
}


//-----------------------------------------------------------------------------
// Reaktion auf Klicken der linken Maus-Taste
//----------------------------------------------------------------------------

static XmTextPosition selection_startpos;
static XmTextPosition selection_endpos;
static XmTextPosition selection_pos;
static Time selection_time;

void SourceView::setSelection(XtPointer client_data, XtIntervalId *timer)
{
    XmTextSetSelection(source_text_w, 
		       selection_startpos, selection_endpos, selection_time);
    XmTextSetInsertionPosition(source_text_w, selection_pos);
    XmTextShowPosition(source_text_w, selection_pos);
    set_source_argCB(source_text_w, XtPointer(false), 0);
}

void SourceView::startSelectWordAct (Widget w, XEvent* e, 
				     String *params, Cardinal *num_params)
{
    XtCallActionProc(w, "grab-focus", e, params, *num_params);

    if (e->type != ButtonPress)
	return;
    
    XButtonEvent *event = (XButtonEvent *) e;

    XmTextPosition pos = XmTextXYToPos (source_text_w, event->x, event->y);

    XmTextPosition startpos, endpos;

    find_word_bounds(pos, startpos, endpos);

    selection_click    = true;
    selection_startpos = startpos;
    selection_endpos   = endpos;
    selection_pos      = pos;
    selection_time     = time(e);

    XtAppAddTimeOut(XtWidgetToApplicationContext(w), 0, setSelection, 0);
}

void SourceView::endSelectWordAct (Widget w, XEvent* e, 
				   String *params, Cardinal *num_params)
{
    XtCallActionProc(w, "extend-end", e, params, *num_params);

    if (e->type != ButtonPress)
	return;
    
    XButtonEvent *event = (XButtonEvent *) e;

    XmTextPosition pos = XmTextXYToPos (source_text_w, event->x, event->y);

    XmTextPosition startpos, endpos;
    if (XmTextGetSelectionPosition(source_text_w, &startpos, &endpos))
    {
	selection_startpos = startpos;
	selection_endpos   = endpos;
    }

    selection_pos      = pos;
    selection_time     = time(e);

    XtAppAddTimeOut(XtAppContext(w), 0, setSelection, 0);
}

//-----------------------------------------------------------------------------
// Reaktion auf Klicken der rechten Maus-Taste
//----------------------------------------------------------------------------
void SourceView::srcpopupAct (Widget w, XEvent* e, String* str, Cardinal* c)
{
    if (e->type != ButtonPress)
	return;
    
    XButtonEvent* event = (XButtonEvent *) e;

    XmTextPosition pos = XmTextXYToPos (source_text_w,
					event->x,
					event->y);

    static int line_nr;
    bool in_text;
    static int bp_nr;
    bool pos_found =
	get_line_of_pos (pos, &line_nr, &in_text, &bp_nr);

    if (!pos_found) // Wahrsch, noch ein Text-File geladen
	return; 

    if (bp_nr != 0) {
	// Auf Breakpoint-Anzeige geklickt: Popup-Menu fuer Breakpoint oeffnen
	static Widget bp_popup_w = 0;

	if (bp_popup_w == 0) {
	    MMDesc *bp_popup = 0;
	    switch (gdb->type())
	    {
	    case GDB:
		bp_popup = bp_popup_gdb;
		break;
	    case DBX:
		bp_popup = bp_popup_dbx;
		break;
	    }

	    bp_popup_w = MMcreatePopupMenu (w, "bp_popup", bp_popup);
	    MMaddCallbacks (bp_popup, XtPointer (&bp_nr));
	}

	switch (gdb->type())
	{
	case GDB:
	{
	    MString label(bp_map.get(bp_nr)->enabled() ? 
			  "Disable Breakpoint" : "Enable Breakpoint");
	    XtVaSetValues(bp_popup_gdb[BPItms::Disable].widget,
			  XmNlabelString, XmString(label),
			  NULL);
	}
	break;

	case DBX:
	    break;
	}

	XmMenuPosition (bp_popup_w, event);
	XtManageChild (bp_popup_w);
    }
    else if (!in_text) {
	// Popup-Menu fuer Zeile line_nr oeffnen
	static Widget line_popup_w = 0;
	if (line_popup_w == 0) {
	    line_popup_w = MMcreatePopupMenu (w,
					      "line_popup",
					      line_popup);
	    MMaddCallbacks (line_popup, XtPointer(&line_nr));
	}
	XmMenuPosition (line_popup_w, event);
	XtManageChild (line_popup_w);

    }
    else {
	// 'Umgebenden' C-String ermitteln, und Popup dafuer oeffnen
	static string word;

	XmTextPosition startpos;
	XmTextPosition endpos;

	find_word_bounds(pos, startpos, endpos);
	word = "";
	if (startpos < current_text.length() && startpos < endpos)
	    word = current_text(int(startpos), int(endpos - startpos));
	
	Widget text_popup_w = MMcreatePopupMenu(w, "text_popup", text_popup);
	MMaddCallbacks (text_popup, XtPointer(&word));

	// The popup menu is destroyed immediately after having popped down.
	Widget shell = XtParent(text_popup_w);
	XtAddCallback(shell, XmNpopdownCallback, DestroyThisCB, shell);

	MString current_arg(word, "tt");

	Arg args[5];
	int arg = 0;
	MString label = text_cmd_labels[TextItms::Break] + current_arg;
	XtSetArg (args[arg], XmNlabelString, XmString(label));arg++;
	XtSetValues(text_popup[TextItms::Break].widget, args, arg);

	arg = 0;
	label = text_cmd_labels[TextItms::Clear] + current_arg;
	XtSetArg (args[arg], XmNlabelString, XmString(label));arg++;
	XtSetValues(text_popup[TextItms::Clear].widget, args, arg);

	arg = 0;
	label = text_cmd_labels[TextItms::Print] + current_arg;
	XtSetArg (args[arg], XmNlabelString, XmString(label));arg++;
	XtSetValues(text_popup[TextItms::Print].widget, args, arg);

	arg = 0;
	label = text_cmd_labels[TextItms::Disp] + current_arg;
	XtSetArg (args[arg], XmNlabelString, XmString(label));arg++;
	XtSetValues(text_popup[TextItms::Disp].widget, args, arg);

	arg = 0;
	label = text_cmd_labels[TextItms::Lookup] + current_arg;
	XtSetArg (args[arg], XmNlabelString, XmString(label));arg++;
	XtSetValues(text_popup[TextItms::Lookup].widget, args, arg);

	bool sens = (word.length() > 0);
	XtSetSensitive (text_popup[TextItms::Break].widget,  sens);
	XtSetSensitive (text_popup[TextItms::Clear].widget,  sens);
	XtSetSensitive (text_popup[TextItms::Print].widget,  sens);
	XtSetSensitive (text_popup[TextItms::Disp].widget,   sens);
	XtSetSensitive (text_popup[TextItms::Lookup].widget, sens);

	XmMenuPosition (text_popup_w, event);
	XtManageChild (text_popup_w);
    }
}

Widget SourceView::widget() { return source_view_w; }



//-----------------------------------------------------------------------------
// Breakpoint selection
//----------------------------------------------------------------------------

void SourceView::NewBreakpointDCB(Widget w, 
				 XtPointer client_data, 
				 XtPointer call_data)
{
    XmSelectionBoxCallbackStruct *cbs = 
	(XmSelectionBoxCallbackStruct *)call_data;

    String _input;
    XmStringGetLtoR(cbs->value, MSTRING_DEFAULT_CHARSET, &_input);
    string input(_input);
    XtFree(_input);

    int i = 0;
    while (i < input.length() && isspace(input[i]))
	i++;
    input = input.from(i);
    if (input == "")
	return;
	   
    switch (gdb->type())
    {
    case GDB:
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
    }
}

void SourceView::NewBreakpointCB(Widget w,
				 XtPointer client_data,
				 XtPointer call_data)
{
    static Widget new_breakpoint_dialog = 0;
    if (new_breakpoint_dialog == 0)
    {
	new_breakpoint_dialog = 
	    XmCreatePromptDialog(source_text_w, 
				 "new_breakpoint_dialog",
				 NULL, 0);
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
void SourceView::EditBreakpointConditionDCB(Widget w, 
					    XtPointer client_data, 
					    XtPointer call_data)
{
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

void SourceView::EditBreakpointConditionCB(Widget w,
					   XtPointer client_data,
					   XtPointer call_data)
{
    static Widget edit_breakpoint_condition_dialog = 0;
    if (edit_breakpoint_condition_dialog == 0)
    {
	edit_breakpoint_condition_dialog = 
	    XmCreatePromptDialog(source_text_w, 
				 "edit_breakpoint_condition_dialog",
				 NULL, 0);
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
	if (info == string(-1))
	    return;

	string cond = info.after("stop only if ");
	if (cond.contains('\n'))
	    cond = cond.before("\n");

	MString xmcond(cond);
	XtVaSetValues(edit_breakpoint_condition_dialog,
		      XmNtextString, XmString(xmcond),
		      NULL);
    }

    XtManageChild(edit_breakpoint_condition_dialog);
}



// Edit breakpoint ignore count
void SourceView::EditBreakpointIgnoreCountDCB(Widget w, 
					      XtPointer client_data, 
					      XtPointer call_data)
{
    XmSelectionBoxCallbackStruct *cbs = 
	(XmSelectionBoxCallbackStruct *)call_data;
    String input;
    XmStringGetLtoR(cbs->value, MSTRING_DEFAULT_CHARSET, &input);

    if (input[0] == '\0')
	input = "0";

    if (client_data == 0)
    {
	int *breakpoint_nrs = getDisplayNumbers(breakpoint_list_w);
	for (int i = 0; breakpoint_nrs[i] > 0; i++)
	{
	    gdb_command(string("ignore ") + itostring(breakpoint_nrs[i])
			+ " " + input);
	}
	delete[] breakpoint_nrs;
    }
    else
    {
	int bp_nr = *((int *)client_data);
	gdb_command(string("ignore ") + itostring(bp_nr) + " " + input);
    }

    XtFree(input);
}

void SourceView::EditBreakpointIgnoreCountCB(Widget w,
					     XtPointer client_data,
					     XtPointer call_data)
{
    static Widget edit_breakpoint_ignore_count_dialog = 0;
    if (edit_breakpoint_ignore_count_dialog == 0)
    {
	edit_breakpoint_ignore_count_dialog = 
	    XmCreatePromptDialog(source_text_w, 
				 "edit_breakpoint_ignore_count_dialog",
				 NULL, 0);
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
	string info = 
	    gdb_question("info breakpoint " + itostring(bp_nr));
	if (info == string(-1))
	    return;

	string ignore = info.after("ignore next ");
	ignore = ignore.before(" hits");

	MString xmignore(ignore);
	XtVaSetValues(edit_breakpoint_ignore_count_dialog,
		      XmNtextString, XmString(xmignore),
		      NULL);
    }

    XtManageChild(edit_breakpoint_ignore_count_dialog);
}


void SourceView::BreakpointCmdCB(Widget w,
				 XtPointer client_data,
				 XtPointer call_data)
{
    string cmd = String(client_data);
    int *breakpoint_nrs = getDisplayNumbers(breakpoint_list_w);
    if (breakpoint_nrs[0] > 0)
    {
	for (int i = 0; breakpoint_nrs[i] > 0; i++)
	    cmd += " " + itostring(breakpoint_nrs[i]);

	gdb_command(cmd);
    }
    delete[] breakpoint_nrs;
}

void SourceView::LookupBreakpointCB(Widget w,
				    XtPointer client_data,
				    XtPointer call_data)
{
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
    int count          = info_output.freq('\n') + 1;
    string *label_list = new string[count];
    bool *selected     = new bool[count];

    split(info_output, label_list, count, '\n');

    while (count > 0 && label_list[count - 1] == "")
	count--;

    bool select = false;
    for (int i = 0; i < count; i++)
    {
	if (label_list[i] != "" && 
	    (isdigit(label_list[i][0]) || label_list[i][0] == '('))
	{
	    int bp_number = get_positive_nr(label_list[i]);

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

void SourceView::UpdateBreakpointButtonsCB(Widget w, 
					   XtPointer client_data, 
					   XtPointer call_data)
{
    int *breakpoint_nrs = getDisplayNumbers(breakpoint_list_w);
    for (int count = 0; breakpoint_nrs[count] != 0; count++)
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
		   gdb->type() == GDB && count > 0);
    XtSetSensitive(bp_area[BPButtons::Disable].widget,     
		   gdb->type() == GDB && count > 0);
    XtSetSensitive(bp_area[BPButtons::Condition].widget,   
		   gdb->type() == GDB && count > 0);
    XtSetSensitive(bp_area[BPButtons::IgnoreCount].widget, 
		   gdb->type() == GDB && count > 0);
    XtSetSensitive(bp_area[BPButtons::Delete].widget,
		   count > 0);
}

void SourceView::EditBreakpointsCB(Widget w,
				   XtPointer client_data,
				   XtPointer call_data)
{
    XtManageChild(edit_breakpoints_dialog_w);
}



//-----------------------------------------------------------------------------
// Stack frame selection
//----------------------------------------------------------------------------

void SourceView::StackDialogPoppedDownCB (Widget, XtPointer, XtPointer)
{
    stack_dialog_popped_up = false;
}

void SourceView::SelectFrameCB (Widget w, 
				XtPointer client_data, XtPointer call_data)
{
    XmListCallbackStruct *cbs = (XmListCallbackStruct *)call_data;

    int count = 0;
    XtVaGetValues(w,
		  XmNitemCount, &count,
		  NULL);

    XtSetSensitive(up_w,   cbs->item_position > 1);
    XtSetSensitive(down_w, cbs->item_position < count);

    // Get the selected line
    String _item;
    XmStringGetLtoR(cbs->item, LIST_CHARSET, &_item);
    string item(_item);
    XtFree(_item);
    string pos;

    switch (gdb->type())
    {
    case GDB:
    {
	string reply = gdb_question("frame " + 
				    itostring(count - cbs->item_position));
	if (reply == string(-1))
	{
	    post_gdb_busy(w);
	    return;
	}

	if (item.contains(" at "))
	    pos = item.after(" at ");
	break;
    }
    
    case DBX:
    {
	// DBX lacks a `frame' command.  Use this kludge instead.
	string reply = gdb_question("func main");
	if (reply == string(-1))
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
	    reply = gdb_question("down " + itostring(cbs->item_position - 1));
	    if (reply == string(-1))
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
	break;
    }
    }

    // clog << "pos = " << quote(pos) << "\n";

    if (pos != "")
	show_execution_position(pos);
}

void SourceView::refresh_stack_frames()
{
    string where = gdb_question("where");
    if (where == string(-1))
	where = "No stack.";
    process_where(where);

    if (gdb->type() == GDB)
    {
	string frame = gdb_question("frame");
	process_frame(frame);
    }
}
    
void SourceView::ViewStackFramesCB(Widget, XtPointer, XtPointer)
{
    refresh_stack_frames();
    XtManageChild(stack_dialog_w);
    
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

    // Invert list such that `Up' and `Down' make sense
    for (int i = 0; i < count / 2; i++)
    {
	string tmp = frame_list[i];
	frame_list[i] = frame_list[count - i - 1];
	frame_list[count - i - 1] = tmp;
    }

    // Make sure we have a minimum width
    const int min_width = 40;
    for (i = 0; i < count; i++)
    {
	selected[i] = false;
	if (frame_list[i].length() < min_width)
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
    if (frame_output != "" && frame_output[0] == '#')
    {
	string frame_nr = frame_output.after(0);
	int frame = get_positive_nr(frame_nr);

	int count         = 0;
	int top_item      = 0;
	int visible_items = 0;
	XtVaGetValues(frame_list_w,
		      XmNitemCount, &count,
		      XmNtopItemPosition, &top_item,
		      XmNvisibleItemCount, &visible_items,
		      NULL);
	int pos = count - frame;

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
	XmListGetSelectedPos(frame_list_w, &position_list, &position_count);

	if (position_count == 1)
	    pos = position_list[0] + arg;
	XtFree((char *)position_list);
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

bool SourceView::where_required() { return stack_dialog_popped_up; }
