// $Id$ -*- C++ -*-
// Printing dialog

// Copyright (C) 1996-1998 Technische Universitaet Braunschweig, Germany.
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

char print_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "print.h"

#include "AppData.h"
#include "DataDisp.h"
#include "DestroyCB.h"
#include "GraphEdit.h"
#include "Graph.h"
#include "LiterateA.h"
#include "TimeOut.h"
#include "Command.h"
#include "cook.h"
#include "cwd.h"
#include "filetype.h"
#include "post.h"
#include "regexps.h"
#include "status.h"
#include "string-fun.h"
#include "verify.h"
#include "wm.h"
#include "charsets.h"
#include "MakeMenu.h"

#include "LessTifH.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream.h>
#include <fstream.h>
#include <string.h>		// strerror()
#include <errno.h>

#include <Xm/Xm.h>
#include <Xm/ToggleB.h>
#include <Xm/RowColumn.h>
#include <Xm/Label.h>
#include <Xm/SelectioB.h>
#include <Xm/MessageB.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <Xm/PushB.h>


//-----------------------------------------------------------------------------
// Printing Graphs
//-----------------------------------------------------------------------------

// Convert according to given BoxPrintGC
static int convert(string filename, BoxPrintGC& gc, bool selectedOnly)
{
    string path = filename;
    if (!filename.contains('/', 0))
	path.prepend(cwd() + '/');

    StatusDelay delay("Printing graph to " + quote(path));

    // Get the graph
    Graph *graph = graphEditGetGraph(data_disp->graph_edit);

    if (graph->firstNode() == 0)
    {
	post_error("No displays to print.", "print_empty_graph_error", 
		   data_disp->graph_edit);
	delay.outcome = "failed";
	return -1;
    }

    // Get and set the GC
    GraphGC graphGC = graphEditGetGraphGC(data_disp->graph_edit);
    graphGC.printGC = &gc;
    graphGC.printSelectedNodesOnly = selectedOnly;

    ofstream os(filename);
    if (os.bad())
    {
	FILE *fp = fopen(filename, "w");
	post_error(string("Cannot open ") 
		   + quote(filename) + ": " + strerror(errno), 
		   "print_failed_error", data_disp->graph_edit);
	if (fp)
	    fclose(fp);
	delay.outcome = strerror(errno);
	return -1;
    }

    graph->print(os, graphGC);
    return 0;
}

static void deletePrintAgent(XtPointer client_data, XtIntervalId *)
{
    // Delete agent after use
    Agent *edit_agent = (Agent *)client_data;
    delete edit_agent;
}

static void unlinkPrintFile(XtPointer client_data, XtIntervalId *)
{
    // Delete temp file after use
    string *tempfile = (string *)client_data;
    unlink(*tempfile);
    delete tempfile;
}

static string output_buffer;

static void printDoneHP(Agent *print_agent, void *client_data, void *)
{
    // Printing is done: remove temporary file
    XtAppAddTimeOut(XtWidgetToApplicationContext(gdb_w), 0, deletePrintAgent, 
		    XtPointer(print_agent));
    XtAppAddTimeOut(XtWidgetToApplicationContext(gdb_w), 0, unlinkPrintFile, 
		    XtPointer(client_data));

    if (output_buffer != "")
	set_status("");
}

static void printOutputHP(Agent *, void *, void *call_data)
{
    DataLength *input = (DataLength *)call_data;
    output_buffer += string(input->data, input->length);
    while (output_buffer.contains('\n'))
    {
	set_status(output_buffer.before('\n'));
	output_buffer = output_buffer.after('\n');
    }
    if (output_buffer != "")
	set_status(output_buffer);
}

// Print according to given BoxPrintGC
static int print(string command, BoxPrintGC& gc, bool selectedOnly)
{
    string tempfile = tmpnam(0);
    int ret = convert(tempfile, gc, selectedOnly);
    if (ret)
	return ret;

    StatusDelay delay("Printing graph " + quote(tempfile));

    command = command + " " + tempfile;

    LiterateAgent *print_agent = 
	new LiterateAgent(XtWidgetToApplicationContext(gdb_w), command);

    output_buffer = "";

    print_agent->removeAllHandlers(Died);
    print_agent->addHandler(InputEOF, printDoneHP, 
			   (void *)new string(tempfile));
    print_agent->addHandler(Input, printOutputHP);
    print_agent->addHandler(Error, printOutputHP);
    print_agent->start();

    return 0;
}

// Local state of print dialog
enum PrintType { PRINT_POSTSCRIPT, PRINT_FIG };

static bool            print_selected_only = false;
static bool            print_to_printer    = true;
static BoxPostScriptGC print_postscript_gc;
static BoxFigGC        print_xfig_gc;
static PrintType       print_type = PRINT_POSTSCRIPT;

static Widget          print_dialog = 0;
static Widget          print_command_field   = 0;
static Widget          print_file_name_field = 0;
static Widget          paper_size_dialog = 0;

static Widget          a4_paper_size;
static Widget          a3_paper_size;
static Widget          letter_paper_size;
static Widget          legal_paper_size;
static Widget          executive_paper_size;
static Widget          custom_paper_size;


// Go and print according to local state
void graphQuickPrintCB(Widget w, XtPointer client_data, XtPointer)
{
    const bool unmanage = ((int)(long)client_data & 1);
    const bool override = ((int)(long)client_data & 2);

    if (print_to_printer)
    {
	static string command;
	command = app_data.print_command;

	if (print_command_field != 0)
	{
	    String c = XmTextFieldGetString(print_command_field);
	    command = c;
	    XtFree(c);
	}

	app_data.print_command = command;
	if (print(command, print_postscript_gc, print_selected_only) == 0)
	{
	    if (unmanage && print_dialog != 0)
		XtUnmanageChild(print_dialog);
	}
    }
    else
    {
	BoxPrintGC *gc_ptr = 0;
	switch (print_type)
	{
	case PRINT_POSTSCRIPT:
	    gc_ptr = &print_postscript_gc;
	    break;

	case PRINT_FIG:
	    gc_ptr = &print_xfig_gc;
	    break;
	}
	BoxPrintGC& gc = *gc_ptr;

	String file = XmTextFieldGetString(print_file_name_field);
	string f = file;
	XtFree(file);

	strip_trailing_space(f);
	if (f == "")
	    return;

	if (access(f, W_OK) || !is_regular_file(f) || override)
	{
	    // File does not exist, is special, or override is on
	    if (convert(f, gc, print_selected_only) == 0)
	    {
		if (unmanage && print_dialog != 0)
		    XtUnmanageChild(print_dialog);
	    }
	}
	else
	{
	    // File exists - request confirmation
	    static Widget confirm_overwrite_dialog = 0;
	    if (confirm_overwrite_dialog)
		DestroyWhenIdle(confirm_overwrite_dialog);
	    confirm_overwrite_dialog = 
		verify(XmCreateQuestionDialog(find_shell(w),
					      "confirm_overwrite_dialog", 
					      NULL, 0));
	    Delay::register_shell(confirm_overwrite_dialog);
	    XtAddCallback(confirm_overwrite_dialog, 
			  XmNokCallback,   graphQuickPrintCB, 
			  XtPointer((int)(long)client_data | 2));
	    XtAddCallback(confirm_overwrite_dialog, 
			  XmNhelpCallback, ImmediateHelpCB, 0);

	    MString question = rm("Overwrite existing file " 
				  + quote(f) + "?");
	    XtVaSetValues (confirm_overwrite_dialog, XmNmessageString, 
			   question.xmstring(), NULL);
	    manage_and_raise(confirm_overwrite_dialog);
	}
    }
}

static string suffix(PrintType print_type)
{
    switch (print_type)
    {
    case PRINT_POSTSCRIPT:
	return ".ps";

    case PRINT_FIG:
	return ".fig";
    }

    return "";
}

static void SetPrintTypeCB(Widget w, XtPointer client_data, XtPointer)
{
    if (!XmToggleButtonGetState(w))
	return;

    string old_suffix = suffix(print_type);
    print_type = PrintType((int)(long)client_data);
    string new_suffix = suffix(print_type);

    String file_name_s = XmTextFieldGetString(print_file_name_field);
    string file_name(file_name_s);
    XtFree(file_name_s);

    if (file_name.contains(old_suffix, -1))
    {
	int idx = file_name.index(old_suffix, -1);
	file_name = file_name.before(idx) + new_suffix;

	XmTextFieldSetString(print_file_name_field, (char *)file_name.chars());
    }
}

static void SetSensitiveCB(Widget w, XtPointer client_data, XtPointer)
{
    if (XmToggleButtonGetState(w))
	set_sensitive(Widget(client_data), True);
}

static void TakeFocusCB(Widget w, XtPointer client_data, XtPointer)
{
    if (XmToggleButtonGetState(w))
	XmProcessTraversal(Widget(client_data), XmTRAVERSE_CURRENT);
}

static void UnsetSensitiveCB(Widget w, XtPointer client_data, XtPointer)
{
    if (XmToggleButtonGetState(w))
	set_sensitive(Widget(client_data), False);
}

static void SetPrintSelectedNodesCB(Widget w, XtPointer, XtPointer)
{
    print_selected_only = XmToggleButtonGetState(w);
}

static void SetPrintTargetCB(Widget w, XtPointer, XtPointer)
{
    print_to_printer = XmToggleButtonGetState(w);
}

static void set_paper_size_string(string s)
{
    Widget text = XmSelectionBoxGetChild(paper_size_dialog, XmDIALOG_TEXT);
    XmTextSetString(text, s);

    static string current_paper_size;
    current_paper_size = s;
    app_data.paper_size = current_paper_size;
}

static void SetGCA3(Widget w, XtPointer, XtPointer)
{
    if (XmToggleButtonGetState(w))
    {
	BoxPostScriptGC a4;

	print_postscript_gc.hsize = a4.vsize;
	print_postscript_gc.vsize = a4.hsize * 2;
	set_paper_size_string("297mm x 420mm");
    }
}

static void SetGCA4(Widget w, XtPointer, XtPointer)
{
    if (XmToggleButtonGetState(w))
    {
	BoxPostScriptGC a4;

	print_postscript_gc.hsize = a4.hsize;
	print_postscript_gc.vsize = a4.vsize;
	set_paper_size_string("210mm x 297mm");
    }
}

static void SetGCLetter(Widget w, XtPointer, XtPointer)
{
    BoxPostScriptGC gc;

    if (XmToggleButtonGetState(w))
    {
	print_postscript_gc.hsize = 72 * 8 + 72 / 2 - gc.hoffset * 2;
	print_postscript_gc.vsize = 72 * 11         - gc.voffset * 2;
	set_paper_size_string("8.5in x 11in");
    }
}

static void SetGCLegal(Widget w, XtPointer, XtPointer)
{
    BoxPostScriptGC gc;

    if (XmToggleButtonGetState(w))
    {
	print_postscript_gc.hsize = 72 * 8 + 72 / 2 - gc.hoffset * 2;
	print_postscript_gc.vsize = 72 * 14         - gc.voffset * 2;
	set_paper_size_string("8.5in x 14in");
    }
}

static void SetGCExecutive(Widget w, XtPointer, XtPointer)
{
    BoxPostScriptGC gc;

    if (XmToggleButtonGetState(w))
    {
	print_postscript_gc.hsize = 72 * 7 + 72 / 2 - gc.hoffset * 2;
	print_postscript_gc.vsize = 72 * 10         - gc.voffset * 2;
	set_paper_size_string("7.5in x 10in");
    }
}

// Convert single unit to points
static int points(string s)
{
    int points = 0;

    while (s != "")
    {
	char *start = s;
	char *tailptr;
	double value = strtod(start, &tailptr);
	int value_len = (int)(tailptr - start);
	if (value_len == 0)
	{
	    // No size
	    return -1;
	}
	s = s.from(value_len);

	// Read unit
	string unit = s;
	if (unit.contains(rxdouble))
	    unit = unit.before(rxdouble);

	strip_space(unit);
	unit.downcase();

	if (unit.contains("es", -1))
	    unit = unit.before("es");
	if (unit.contains("s", -1))
	    unit = unit.before("s");

	double factor = 0.0;
	if (unit == "point" || unit == "pt")
	    factor = 1.0;
	else if (unit == "inch" || unit == "in" || unit == "\"")
	    factor = 72.0;
	else if (unit == "foot" || unit == "feet" 
		 || unit == "ft" || unit == "\'")
	    factor = 72.0 * 12;
	else if (unit == "yard" || unit == "yd")
	    factor = 72.0 * 12 * 3;
	else if (unit == "mile" || unit == "mi")
	    factor = 72.0 * 12 * 5280;
	else if (unit == "mm" || unit == "millimeter")
	    factor = 72.0 * 1/2.54 * 1/10;
	else if (unit == "cm" || unit == "centimeter")
	    factor = 72.0 * 1/2.54;
	else if (unit == "m" || unit == "meter")
	    factor = 72.0 * 1/2.54 * 100;
	else if (unit == "km" || unit == "kilometer")
	    factor = 72.0 * 1/2.54 * 100000;
	else if (unit == "parsec")                              // ;-)
	    factor = 72.0 * 1/2.54 * 100000 * 3.085678e+13;
	else
	{
	    // Invalid unit
	    return -1;
	}

	if (s.contains(rxdouble))
	    s = s.from(rxdouble);
	else
	    s = "";

	if (double(points) + factor * value > double(INT_MAX))
	{
	    // Too large
	    return -1;
	}

	points += int(factor * value);
    }

    if (points <= 0)
    {
	// Too small
	return -1;
    }

    return points;
}

inline bool near(int i, int j)
{
    return abs(i - j) <= 2;
}

static void get_paper_size(string s, int& hsize, int& vsize)
{
    char delim = '\0';

    if (s.contains('\327'))	// \327 is the times symbol
	delim = '\327';
    else if (s.contains('x'))
	delim = 'x';
    else if (s.contains('X'))
	delim = 'X';

    if (delim == '\0')
    {
	// Bad spec
	hsize = -1;
	vsize = -1;
	return;
    }
    
    string s_hsize = s.before(delim);
    string s_vsize = s.after(delim);

    hsize = points(s_hsize);
    vsize = points(s_vsize);
}

static bool set_paper_size(string s)
{
    int hsize, vsize;
    get_paper_size(s, hsize, vsize);

    if (hsize <= 0 || vsize <= 0)
	return false;		// Error

    BoxPostScriptGC gc;

    print_postscript_gc.hsize = hsize - gc.hoffset * 2;
    print_postscript_gc.vsize = vsize - gc.voffset * 2;

    if (near(hsize, 594) && near(vsize, 840))
	XmToggleButtonSetState(a4_paper_size, True, True);
    else if (near(hsize, 840) && near(vsize, 1188))
	XmToggleButtonSetState(a3_paper_size, True, True);
    else if (hsize == 72 * 8 + 72 / 2 && vsize == 72 * 11)
	XmToggleButtonSetState(letter_paper_size, True, True);
    else if (hsize == 72 * 8 + 72 / 2 && vsize == 72 * 14)
	XmToggleButtonSetState(legal_paper_size, True, True);
    else if (hsize == 72 * 7 + 72 / 2 && vsize == 72 * 10)
	XmToggleButtonSetState(executive_paper_size, True, True);
    else
    {
	XmToggleButtonSetState(a4_paper_size, False, False);
	XmToggleButtonSetState(a3_paper_size, False, False);
	XmToggleButtonSetState(letter_paper_size, False, False);
	XmToggleButtonSetState(legal_paper_size, False, False);
	XmToggleButtonSetState(executive_paper_size, False, False);
	XmToggleButtonSetState(custom_paper_size, True, False);
    }

    set_paper_size_string(s);

    return true;
}

static void SetPaperSizeCB(Widget w, XtPointer, XtPointer call_data)
{
    XmFileSelectionBoxCallbackStruct *cbs =
	(XmFileSelectionBoxCallbackStruct *)call_data;

    string s = "";
    char *value = 0;

    if (XmStringGetLtoR(cbs->value, MSTRING_DEFAULT_CHARSET, &value)
	&& value != 0)
    {
	s = value;
	XtFree(value);
    }

    int ok = true;
    if (s != "")
	ok = set_paper_size(s);

    if (ok)
	XtUnmanageChild(w);
}

static void CheckPaperSizeCB(Widget text, XtPointer client_data, XtPointer)
{
    Widget ok_button = Widget(client_data);
    String value;
    XtVaGetValues(text, XmNvalue, &value, NULL);
    string size(value);
    XtFree(value);

    int hsize, vsize;
    get_paper_size(size, hsize, vsize);

    set_sensitive(ok_button, hsize >= 0 && vsize >= 0);
}

static void ResetPaperSizeCB(Widget w, XtPointer, XtPointer)
{
    set_paper_size(app_data.paper_size);
    XtUnmanageChild(w);
}

static void SetGCCustom(Widget w, XtPointer, XtPointer)
{
    if (!XmToggleButtonGetState(w))
	return;

    manage_and_raise(paper_size_dialog);
}

static void SetGCOrientation(Widget w, XtPointer, XtPointer)
{
    if (XmToggleButtonGetState(w))
	print_postscript_gc.orientation = BoxPostScriptGC::PORTRAIT;
    else
	print_postscript_gc.orientation = BoxPostScriptGC::LANDSCAPE;
}

static void NopCB(Widget, XtPointer, XtPointer)
{}

void graphPrintCB(Widget w, XtPointer, XtPointer)
{
    if (print_dialog != 0)
    {
	// Dialog already created -- pop it up again
	manage_and_raise(print_dialog);
	return;
    }

    Arg args[10];
    Cardinal arg = 0;
    XtSetArg(args[arg], XmNautoUnmanage, False); arg++;
    print_dialog = 
	verify(XmCreatePromptDialog(find_shell(w), "print", args, arg));
    Delay::register_shell(print_dialog);

    if (lesstif_version <= 79)
	XtUnmanageChild(XmSelectionBoxGetChild(print_dialog,
					       XmDIALOG_APPLY_BUTTON));

    XtAddCallback(print_dialog, XmNokCallback,
		  graphQuickPrintCB, XtPointer(1));
    XtAddCallback(print_dialog, XmNapplyCallback,
		  graphQuickPrintCB, XtPointer(0));
    XtAddCallback(print_dialog, XmNcancelCallback, 
		  UnmanageThisCB, XtPointer(print_dialog));
    XtAddCallback(print_dialog, XmNhelpCallback,
		  ImmediateHelpCB, XtPointer(0));

    // Remove old prompt
    XtUnmanageChild(XmSelectionBoxGetChild(print_dialog, XmDIALOG_TEXT));
    XtUnmanageChild(XmSelectionBoxGetChild(print_dialog, 
					   XmDIALOG_SELECTION_LABEL));

    // Create menu
    static Widget print_to_printer;
    static Widget print_to_file;
    static MMDesc print_to_menu[] = 
    {
	{"printer", MMToggle, { SetPrintTargetCB }, NULL, &print_to_printer },
	{"file",    MMToggle, { NopCB }, NULL, &print_to_file },
	MMEnd
    };

    Widget postscript;
    static MMDesc type_menu[] = 
    {
	{"postscript", MMToggle,
	  { SetPrintTypeCB, XtPointer(PRINT_POSTSCRIPT) }, NULL, &postscript},
	{"xfig",       MMToggle,
	  { SetPrintTypeCB, XtPointer(PRINT_FIG) }},
	MMEnd
    };

    Widget print_all;
    static MMDesc what_menu[] = 
    {
	{"all",      MMToggle, { NopCB }, NULL, &print_all },
	{"selected", MMToggle, { SetPrintSelectedNodesCB }},
	MMEnd
    };

    Widget print_portrait;
    static MMDesc orientation_menu[] = 
    {
	{"portrait",  MMToggle, { SetGCOrientation }, NULL, &print_portrait },
	{"landscape", MMToggle, { NopCB }},
	MMEnd
    };

    static MMDesc paper_menu[] = 
    {
	{"a4",        MMToggle, {SetGCA4},        NULL, &a4_paper_size},
	{"a3",        MMToggle, {SetGCA3},        NULL, &a3_paper_size},
	{"letter",    MMToggle, {SetGCLetter},    NULL, &letter_paper_size},
	{"legal",     MMToggle, {SetGCLegal},     NULL, &legal_paper_size},
	{"executive", MMToggle, {SetGCExecutive}, NULL, &executive_paper_size},
	{"custom",    MMToggle, {SetGCCustom},    NULL, &custom_paper_size},
	MMEnd
    };

    Widget file_type;
    static MMDesc menu[] =
    {
	{"to",          MMRadioPanel, MMNoCB, print_to_menu },
	{"command",     MMTextField,  MMNoCB, NULL, &print_command_field },
	{"name", 	MMTextField,  MMNoCB, NULL, &print_file_name_field },
	{"type", 	MMRadioPanel, MMNoCB, type_menu, &file_type },
	{"what",        MMRadioPanel, MMNoCB, what_menu },
	{"orientation", MMRadioPanel, MMNoCB, orientation_menu },
	{"size",        MMRadioPanel, MMNoCB, paper_menu },
	MMEnd
    };

    Widget panel = MMcreatePanel(print_dialog, "options", menu);
    (void) panel;		// Use it
    MMadjustPanel(menu);

    // Add callbacks
    MMaddCallbacks(menu);

    XtAddCallback(print_to_printer, XmNvalueChangedCallback,   
		  SetSensitiveCB,   XtPointer(print_command_field));
    XtAddCallback(print_to_printer, XmNvalueChangedCallback,   
		  SetSensitiveCB,   XtPointer(menu[1].label));

    XtAddCallback(print_to_printer, XmNvalueChangedCallback,
		  UnsetSensitiveCB, XtPointer(print_file_name_field));
    XtAddCallback(print_to_printer, XmNvalueChangedCallback,
		  UnsetSensitiveCB, XtPointer(menu[2].label));
    XtAddCallback(print_to_printer, XmNvalueChangedCallback,
		  UnsetSensitiveCB, XtPointer(file_type));

    XtAddCallback(print_to_printer, XmNvalueChangedCallback,
		  TakeFocusCB,      XtPointer(print_command_field));

    XtAddCallback(print_to_file, XmNvalueChangedCallback,   
		  UnsetSensitiveCB, XtPointer(print_command_field));
    XtAddCallback(print_to_file, XmNvalueChangedCallback,   
		  UnsetSensitiveCB, XtPointer(menu[1].label));

    XtAddCallback(print_to_file, XmNvalueChangedCallback,   
		  SetSensitiveCB,   XtPointer(print_file_name_field));
    XtAddCallback(print_to_file, XmNvalueChangedCallback,
		  SetSensitiveCB,   XtPointer(menu[2].label));
    XtAddCallback(print_to_file, XmNvalueChangedCallback,   
		  SetSensitiveCB,   XtPointer(file_type));

    XtAddCallback(print_to_file, XmNvalueChangedCallback,
		  TakeFocusCB,      XtPointer(print_file_name_field));


    // Create size dialog
    arg = 0;
    XtSetArg(args[arg], XmNautoUnmanage, False); arg++;
    paper_size_dialog = 
	verify(XmCreatePromptDialog(find_shell(w), "paper_size_dialog", 
				    args, arg));
    Delay::register_shell(paper_size_dialog);

    if (lesstif_version <= 79)
	XtUnmanageChild(XmSelectionBoxGetChild(paper_size_dialog,
					       XmDIALOG_APPLY_BUTTON));

    XtAddCallback(paper_size_dialog, XmNokCallback,
		  SetPaperSizeCB, XtPointer(0));
    XtAddCallback(paper_size_dialog, XmNcancelCallback, 
		  ResetPaperSizeCB, XtPointer(0));
    XtAddCallback(paper_size_dialog, XmNhelpCallback,   
		  ImmediateHelpCB, XtPointer(0));

    Widget size = XmSelectionBoxGetChild(paper_size_dialog,
					 XmDIALOG_TEXT);
    Widget ok_button = XmSelectionBoxGetChild(paper_size_dialog, 
					      XmDIALOG_OK_BUTTON);
    XtAddCallback(size, XmNvalueChangedCallback, 
		  CheckPaperSizeCB, XtPointer(ok_button));


    // Set initial state
    XmToggleButtonSetState(print_to_printer, True, True);
    XmToggleButtonSetState(postscript, True, True);
    XmToggleButtonSetState(print_all, True, True);
    XmToggleButtonSetState(print_portrait, True, True);

    bool ok = set_paper_size(app_data.paper_size);
    if (!ok)
	XmToggleButtonSetState(a4_paper_size, True, True);

    string command = string(app_data.print_command) + " ";
    XmTextFieldSetString(print_command_field, command);

    // Gofer it!
    manage_and_raise(print_dialog);
}
