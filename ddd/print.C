// $Id$ -*- C++ -*-
// Printing dialog

// Copyright (C) 1996-1998 Technische Universitaet Braunschweig, Germany.
// Copyright (C) 2000-2001 Universitaet Passau, Germany.
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

char print_rcsid[] = 
    "$Id$";

#include "print.h"

#include "AppData.h"
#include "DataDisp.h"
#include "DestroyCB.h"
#include "DispGraph.h"
#include "GraphEdit.h"
#include "Graph.h"
#include "LiterateA.h"
#include "TimeOut.h"
#include "Command.h"
#include "cook.h"
#include "cwd.h"
#include "file.h"
#include "filetype.h"
#include "post.h"
#include "regexps.h"
#include "status.h"
#include "string-fun.h"
#include "tempfile.h"
#include "verify.h"
#include "wm.h"
#include "charsets.h"
#include "MakeMenu.h"

#include "LessTifH.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string.h>		// strerror()
#include <errno.h>
#include <unistd.h>

#if defined(IF_MOTIF)
#include <Xm/Xm.h>
#include <Xm/ToggleB.h>
#include <Xm/RowColumn.h>
#include <Xm/Label.h>
#include <Xm/SelectioB.h>
#include <Xm/MessageB.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <Xm/PushB.h>
#include <Xm/FileSB.h>
#endif

#if !defined(IF_XM)
#include <GUI/Widget.h>
#include <GUI/Dialog.h>
#include <GUI/Entry.h>
#include <GUI/RadioButton.h>
#endif

#ifndef R_OK
/* 3b2 doesn't define these according to jthomas@nmsu.edu. */
#define R_OK 04
#define W_OK 02
#endif


//-----------------------------------------------------------------------------
// Printing Dialog
//-----------------------------------------------------------------------------

static string msg(const string& path, bool displays, bool to_file)
{
    string m = "Printing ";
    if (displays)
	m += "graph ";
    else
	m += "plots ";
    if (to_file)
	m += "to ";
    m += quote(path);
    if (!to_file)
	m += " to printer";

    return m;
}

// Print to FILENAME according to given PrintGC
static int print_to_file(const string& filename, PrintGC& gc, 
			 bool selectedOnly, bool displays)
{
#if defined(IF_MOTIF)
    string path = filename;
    if (!filename.contains('/', 0))
	path.prepend(cwd() + '/');

    StatusDelay delay(msg(path, displays, true));

    // Get the graph
    Graph *_graph = graphEditGetGraph(data_disp->graph_edit);
    DispGraph *graph = ptr_cast(DispGraph, _graph);

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

    std::ofstream os(filename.chars());
    if (os.bad())
    {
	FILE *fp = fopen(filename.chars(), "w");
	post_error(string("Cannot open ") 
		   + quote(filename) + ": " + strerror(errno), 
		   "print_failed_error", data_disp->graph_edit);
	if (fp)
	    fclose(fp);
	delay.outcome = strerror(errno);
	return -1;
    }

    if (displays)
    {
	// Print displays
	graph->print(os, graphGC);
    }
    else
    {
	// Print plots
	os.close();
	graph->print_plots(filename, graphGC);
    }
#else
#ifdef NAG_ME
#warning Printing not implemented
#endif
#endif
    return 0;
}

#if defined(IF_MOTIF)
static void deletePrintAgent(XtPointer client_data, XtIntervalId *)
#else
static bool deletePrintAgent(Agent *client_data)
#endif
{
    // Delete agent after use
    Agent *edit_agent = (Agent *)client_data;
    delete edit_agent;
#if !defined(IF_MOTIF)
    return false;
#endif
}

static void unlinkPrintFile(XtPointer client_data, XtIntervalId *)
{
    // Delete temp file after use
    string *tempfile = (string *)client_data;
    unlink(tempfile->chars());
    delete tempfile;
}

static string output_buffer;

static void printDoneHP(Agent *print_agent, void *client_data, void *)
{
    // Don't get called again
    print_agent->removeAllHandlers(InputEOF);
    print_agent->removeAllHandlers(Died);

    // Printing is done: remove temporary file
#if defined(IF_MOTIF)
    XtAppAddTimeOut(XtWidgetToApplicationContext(gdb_w), 0, deletePrintAgent, 
		    XtPointer(print_agent));
    XtAppAddTimeOut(XtWidgetToApplicationContext(gdb_w), 0, unlinkPrintFile, 
		    XtPointer(client_data));
#else
    Glib::signal_idle().connect(sigc::bind(PTR_FUN(deletePrintAgent), print_agent));
#endif

    if (!output_buffer.empty())
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
    if (!output_buffer.empty())
	set_status(output_buffer);
}

// Print according to given PrintGC
static int print_to_printer(string command, PrintGC& gc, 
			    bool selectedOnly, bool displays)
{
    const string tmpfile = tempfile();
    int ret = print_to_file(tmpfile, gc, selectedOnly, displays);
    if (ret)
	return ret;

    StatusDelay delay(msg(tmpfile, displays, false));

    command += " " + tmpfile;

#if defined(IF_XM)
    LiterateAgent *print_agent = 
	new LiterateAgent(XtWidgetToApplicationContext(gdb_w), command);
#else
    LiterateAgent *print_agent = 
	new LiterateAgent(gdb_w->get_main(), command);
#endif

    output_buffer = "";

    string *sp = new string(tmpfile);

    print_agent->removeAllHandlers(Died);
    print_agent->addHandler(InputEOF, printDoneHP, (void *)sp);
    print_agent->addHandler(Died,     printDoneHP, (void *)sp);
    print_agent->addHandler(Input, printOutputHP);
    print_agent->addHandler(Error, printOutputHP);
    print_agent->start();

    return 0;
}

// Local state of print dialog
enum PrintType   { PRINT_POSTSCRIPT, PRINT_FIG };
enum PrintTarget { TARGET_FILE, TARGET_PRINTER };

static bool            print_selected_only = false;
static bool            print_displays      = true;
static bool            print_target = TARGET_PRINTER;
static PostScriptPrintGC print_postscript_gc;
static FigPrintGC        print_xfig_gc;
static PrintType       print_type = PRINT_POSTSCRIPT;

#if defined(IF_XM)
static Widget          print_dialog = 0;
static Widget          print_command_field   = 0;
static Widget          print_file_name_field = 0;
static Widget 	       print_file_name_box   = 0;
static Widget          paper_size_dialog = 0;
#else
static GUI::Dialog    *print_dialog = 0;
static GUI::Entry     *print_command_field   = 0;
static GUI::Entry     *print_file_name_field = 0;
static GUI::Box       *print_file_name_box   = 0;
static GUI::Dialog    *paper_size_dialog = 0;
#endif

#if defined(IF_XM)
static Widget  a4_paper_size;
static Widget  a3_paper_size;
static Widget  letter_paper_size;
static Widget  legal_paper_size;
static Widget  executive_paper_size;
static Widget  custom_paper_size;
#else
static GUI::CheckButton * a4_paper_size;
static GUI::CheckButton * a3_paper_size;
static GUI::CheckButton * letter_paper_size;
static GUI::CheckButton * legal_paper_size;
static GUI::CheckButton * executive_paper_size;
static GUI::CheckButton * custom_paper_size;
#endif

// Go and print according to local state

#if defined(IF_MOTIF)

void PrintAgainCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    const bool unmanage = ((long)client_data & 1);
    const bool override = ((long)client_data & 2);

    switch (print_target)
    {
    case TARGET_PRINTER:
    {
	static string command;
	command = app_data.print_command;

	if (print_command_field != 0)
	{
#if defined(IF_MOTIF)
	    String c = XmTextFieldGetString(print_command_field);
	    command = c;
	    XtFree(c);
#else
	    command = print_command_field->get_text().c_str();
#endif
	}

	app_data.print_command = command.chars();
	if (print_to_printer(command, print_postscript_gc, 
			     print_selected_only, print_displays) == 0)
	{
	    if (unmanage && print_dialog != 0) {
#if defined(IF_XM)
		XtUnmanageChild(print_dialog);
#else
		print_dialog->hide();
#endif
	    }
	}

	break;
    }
    
    case TARGET_FILE:
    {
	PrintGC *gc_ptr = 0;
	switch (print_type)
	{
	case PRINT_POSTSCRIPT:
	    gc_ptr = &print_postscript_gc;
	    break;

	case PRINT_FIG:
	    gc_ptr = &print_xfig_gc;
	    break;
	}
	PrintGC& gc = *gc_ptr;

#if defined(IF_MOTIF)
	String file = XmTextFieldGetString(print_file_name_field);
	string f = file;
	XtFree(file);
#else
	string f(print_file_name_field->get_text().c_str());
#endif

	strip_trailing_space(f);
	if (f.empty())
	    return;

	if (access(f.chars(), W_OK) || !is_regular_file(f) || override)
	{
	    // File does not exist, is special, or override is on
	    if (print_to_file(f, gc, print_selected_only, print_displays) == 0)
	    {
		if (unmanage && print_dialog != 0) {
#if defined(IF_XM)
		    XtUnmanageChild(print_dialog);
#else
		    print_dialog->hide();
#endif
		}
	    }
	}
	else
	{
	    // File exists - request confirmation
	    static Widget confirm_overwrite_dialog = 0;
	    if (confirm_overwrite_dialog)
		DestroyWhenIdle(confirm_overwrite_dialog);
	    confirm_overwrite_dialog = 
		verify(
		    XmCreateQuestionDialog(find_shell(w),
					   XMST("confirm_overwrite_dialog"), 
					   ArgList(0), 0));
	    Delay::register_shell(confirm_overwrite_dialog);
	    XtAddCallback(confirm_overwrite_dialog, 
			  XmNokCallback, PrintAgainCB, 
			  XtPointer((int)(long)client_data | 2));
	    XtAddCallback(confirm_overwrite_dialog, 
			  XmNhelpCallback, ImmediateHelpCB, 0);

	    MString question = rm("Overwrite existing file " 
				  + quote(f) + "?");
	    XtVaSetValues (confirm_overwrite_dialog, XmNmessageString, 
			   question.xmstring(), XtPointer(0));
	    manage_and_raise(confirm_overwrite_dialog);
	}

	break;
    }
    }
}

#endif

#if !defined(IF_XM)

void PrintAgainCB1(GUI::Button *w, long client_data)
{
#if defined(IF_XMMM)
    PrintAgainCB(w->internal(), (XtPointer)client_data, (XtPointer)0);
#else
    std::cerr << "PrintAgainCB not implemented\n";
#endif
}

#endif


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

static void set_print_file_name(const string& name)
{
#if defined(IF_MOTIF)
    XmTextFieldSetString(print_file_name_field, XMST(name.chars()));

    XmTextPosition last_pos = 
	XmTextFieldGetLastPosition(print_file_name_field);
    XmTextFieldSetInsertionPosition(print_file_name_field, last_pos);
    XmTextFieldShowPosition(print_file_name_field, 0);
    XmTextFieldShowPosition(print_file_name_field, last_pos);
#else
    std::cerr << "set_print_file_name unimplemented: " << name << "\n";
#endif
}

#if defined(IF_XM)

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

	set_print_file_name(file_name);
    }
}

#else

static void SetPrintTypeCB(GUI::Widget *w, PrintType client_data)
{
    std::cerr << "SetPrintTypeCB not implemented\n";
}

#endif

#if defined(IF_XM)

static void SetSensitiveCB(Widget w, XtPointer client_data, XtPointer)
{
    if (XmToggleButtonGetState(w))
	set_sensitive(Widget(client_data), true);
}

#else

static void SetSensitiveCB(GUI::Widget *w, GUI::Widget *client_data)
{
    std::cerr << "SetSensitive not implemented\n";
}

#endif

#if defined(IF_XM)

static void TakeFocusCB(Widget w, XtPointer client_data, XtPointer)
{
    if (XmToggleButtonGetState(w))
	XmProcessTraversal(Widget(client_data), XmTRAVERSE_CURRENT);
}

#else

static void TakeFocusCB(GUI::Widget *w, GUI::Widget *client_data)
{
    std::cerr << "TakeFocusCB not implemented\n";
}

#endif

#if defined(IF_XM)

static void UnsetSensitiveCB(Widget w, XtPointer client_data, XtPointer)
{
    if (XmToggleButtonGetState(w))
	set_sensitive(Widget(client_data), false);
}

#else

static void UnsetSensitiveCB(GUI::Widget *w, GUI::Widget *client_data)
{
    std::cerr << "UnsetSensitiveCB not implemented\n";
}

#endif

#if defined(IF_XM)

static void SetPrintDisplaysCB(Widget w, XtPointer client_data, XtPointer)
{
    if (!XmToggleButtonGetState(w))
	return;

    print_displays = bool((int)(long)client_data);
}

#else

static void SetPrintDisplaysCB(GUI::CheckButton *w, bool client_data)
{
    std::cerr << "SetPrintDisplaysCB not implemented\n";
}

#endif

#if defined(IF_XM)

static void SetPrintSelectedNodesCB(Widget w, XtPointer, XtPointer)
{
    print_selected_only = XmToggleButtonGetState(w);
}


static void SetPrintTargetCB(Widget w, XtPointer client_data, XtPointer)
{
    if (!XmToggleButtonGetState(w))
	return;

    print_target = PrintTarget((int)(long)client_data);
}

#else

static void SetPrintSelectedNodesCB(GUI::CheckButton *w)
{
    print_selected_only = w->get_active();
}

static void SetPrintTargetCB(GUI::CheckButton *w, long client_data)
{
    if (!XmToggleButtonGetState(w))
	return;

    print_target = PrintTarget(client_data);
}

#endif

static void set_paper_size_string(const char *s)
{
#if defined(IF_MOTIF)
#if defined(IF_XM)
    Widget text = XmSelectionBoxGetChild(paper_size_dialog, XmDIALOG_TEXT);
#else
    Widget text = XmSelectionBoxGetChild(paper_size_dialog->internal(), XmDIALOG_TEXT);
#endif
    XmTextSetString(text, XMST(s));

    static string current_paper_size;
    current_paper_size = s;
    app_data.paper_size = current_paper_size.chars();
#else
    std::cerr << "set_paper_size_string " << s << "\n";
#endif
}

#if defined(IF_XM)

static void SetGCColorCB(Widget w, XtPointer, XtPointer)
{
    print_postscript_gc.color = XmToggleButtonGetState(w);
}

static void SetGCA3(Widget w, XtPointer, XtPointer)
{
    if (XmToggleButtonGetState(w))
    {
	PostScriptPrintGC a4;

	print_postscript_gc.hsize = a4.vsize;
	print_postscript_gc.vsize = a4.hsize * 2;
	set_paper_size_string("297mm x 420mm");
    }
}

static void SetGCA4(Widget w, XtPointer, XtPointer)
{
    if (XmToggleButtonGetState(w))
    {
	PostScriptPrintGC a4;

	print_postscript_gc.hsize = a4.hsize;
	print_postscript_gc.vsize = a4.vsize;
	set_paper_size_string("210mm x 297mm");
    }
}

#else

static void SetGCColorCB(GUI::CheckButton *w)
{
    print_postscript_gc.color = w->get_active();
}

static void SetGCA3(GUI::CheckButton *w)
{
    if (w->get_active())
    {
	PostScriptPrintGC a4;

	print_postscript_gc.hsize = a4.vsize;
	print_postscript_gc.vsize = a4.hsize * 2;
	set_paper_size_string("297mm x 420mm");
    }
}

static void SetGCA4(GUI::CheckButton *w)
{
    if (w->get_active())
    {
	PostScriptPrintGC a4;

	print_postscript_gc.hsize = a4.hsize;
	print_postscript_gc.vsize = a4.vsize;
	set_paper_size_string("210mm x 297mm");
    }
}

#endif

#if defined(IF_XM)

static void SetGCLetter(Widget w, XtPointer, XtPointer)
{
    PostScriptPrintGC gc;

    if (XmToggleButtonGetState(w))
    {
	print_postscript_gc.hsize = 72 * 8 + 72 / 2 - gc.hoffset * 2;
	print_postscript_gc.vsize = 72 * 11         - gc.voffset * 2;
	set_paper_size_string("8.5in x 11in");
    }
}

static void SetGCLegal(Widget w, XtPointer, XtPointer)
{
    PostScriptPrintGC gc;

    if (XmToggleButtonGetState(w))
    {
	print_postscript_gc.hsize = 72 * 8 + 72 / 2 - gc.hoffset * 2;
	print_postscript_gc.vsize = 72 * 14         - gc.voffset * 2;
	set_paper_size_string("8.5in x 14in");
    }
}

static void SetGCExecutive(Widget w, XtPointer, XtPointer)
{
    PostScriptPrintGC gc;

    if (XmToggleButtonGetState(w))
    {
	print_postscript_gc.hsize = 72 * 7 + 72 / 2 - gc.hoffset * 2;
	print_postscript_gc.vsize = 72 * 10         - gc.voffset * 2;
	set_paper_size_string("7.5in x 10in");
    }
}

#else

static void SetGCLetter(GUI::CheckButton *w)
{
    PostScriptPrintGC gc;

    if (w->get_active())
    {
	print_postscript_gc.hsize = 72 * 8 + 72 / 2 - gc.hoffset * 2;
	print_postscript_gc.vsize = 72 * 11         - gc.voffset * 2;
	set_paper_size_string("8.5in x 11in");
    }
}

static void SetGCLegal(GUI::CheckButton *w)
{
    PostScriptPrintGC gc;

    if (w->get_active())
    {
	print_postscript_gc.hsize = 72 * 8 + 72 / 2 - gc.hoffset * 2;
	print_postscript_gc.vsize = 72 * 14         - gc.voffset * 2;
	set_paper_size_string("8.5in x 14in");
    }
}

static void SetGCExecutive(GUI::CheckButton *w)
{
    PostScriptPrintGC gc;

    if (w->get_active())
    {
	print_postscript_gc.hsize = 72 * 7 + 72 / 2 - gc.hoffset * 2;
	print_postscript_gc.vsize = 72 * 10         - gc.voffset * 2;
	set_paper_size_string("7.5in x 10in");
    }
}

#endif

// Convert single unit to points
static int points(string s)
{
    int points = 0;

    while (!s.empty())
    {
	const char *start = s.chars();
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

static void get_paper_size(const string& s, int& hsize, int& vsize)
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

static bool set_paper_size(const string& s)
{
    int hsize, vsize;
    get_paper_size(s, hsize, vsize);

    if (hsize <= 0 || vsize <= 0)
	return false;		// Error

    PostScriptPrintGC gc;

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

    set_paper_size_string(s.chars());

    return true;
}

#if defined(IF_XM)

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
    if (!s.empty())
	ok = set_paper_size(s);

    if (ok)
	XtUnmanageChild(w);
}

#else

static void SetPaperSizeCB(GUI::Dialog *w)
{
#ifdef NAG_ME
#warning SetPaperSizeCB not implemented.
#endif
}

#endif

#if defined(IF_XM)

static void CheckPaperSizeCB(Widget text, XtPointer client_data, XtPointer call_data)
{
    Widget ok_button = Widget(client_data);
    String value;
    XtVaGetValues(text, XmNvalue, &value, XtPointer(0));
    string size(value);
    XtFree(value);

    int hsize, vsize;
    get_paper_size(size, hsize, vsize);

    set_sensitive(ok_button, hsize >= 0 && vsize >= 0);
}

#else

static void CheckPaperSizeCB(GUI::Entry *text, GUI::Button *ok_button)
{
    string size(text->get_text().c_str());

    int hsize, vsize;
    get_paper_size(size, hsize, vsize);

    set_sensitive(ok_button, hsize >= 0 && vsize >= 0);
}

#endif

#if defined(IF_XM)

static void ResetPaperSizeCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    set_paper_size(app_data.paper_size);
    XtUnmanageChild(w);
}

#else

static void ResetPaperSizeCB(GUI::Dialog *w)
{
    set_paper_size(app_data.paper_size);
    w->hide();
}

#endif

#if defined(IF_XM)

static void SetGCCustom(Widget w, XtPointer, XtPointer)
{
    if (!XmToggleButtonGetState(w))
	return;

    manage_and_raise(paper_size_dialog);
}

static void SetGCOrientation(Widget w, XtPointer client_data, XtPointer)
{
    if (!XmToggleButtonGetState(w))
	return;

    print_postscript_gc.orientation = 
	PostScriptPrintGC::Orientation((int)(long)client_data);
}

#else

static void SetGCCustom(GUI::CheckButton *w)
{
    if (!XmToggleButtonGetState(w))
	return;

    manage_and_raise1(paper_size_dialog);
}

static void SetGCOrientation(GUI::RadioButton *w, long client_data)
{
    if (!w->get_active())
	return;

    print_postscript_gc.orientation = 
	PostScriptPrintGC::Orientation(client_data);
}

#endif

#if defined(IF_XM)

static void SetPrintFileNameCB(Widget w, XtPointer, XtPointer)
{
    string target = get_file(w, XtPointer(0), XtPointer(0));
    if (!target.empty())
    {
	set_print_file_name(target);
	XtUnmanageChild(w);
    }
}

#else

static void SetPrintFileNameCB(GUI::FileSelectionDialog *w)
{
    string target = get_file(w);
    if (!target.empty())
    {
	set_print_file_name(target);
	XtUnmanageChild(w);
    }
}

#endif

#if defined(IF_XM)

static void BrowseNameCB(Widget w, XtPointer, XtPointer)
{
    Delay delay;

    static Widget dialog = 0;

    static MString pattern;

    String file = XmTextFieldGetString(print_file_name_field);
    string f = file;
    XtFree(file);

    if (f.contains('.'))
	pattern = "*" + f.from('.', -1);
    else
    {
	switch (print_type)
	{
	case PRINT_POSTSCRIPT:
	    pattern = "*.ps";
	    break;

	case PRINT_FIG:
	    pattern = "*.fig";
	    break;
	}
    }

    Arg args[10];
    Cardinal arg = 0;

    if (dialog == 0)
    {
	XtSetArg(args[arg], XmNpattern, pattern.xmstring()); arg++;
	dialog = 
	    verify(XmCreateFileSelectionDialog(find_shell(w), 
					       XMST("browse_print"), 
					       args, arg));

	Delay::register_shell(dialog);
	XtAddCallback(dialog, XmNokCallback, SetPrintFileNameCB, 0);
	XtAddCallback(dialog, XmNcancelCallback, UnmanageThisCB, 
		      XtPointer(dialog));
	XtAddCallback(dialog, XmNhelpCallback, ImmediateHelpCB, XtPointer(0));
    }
    else
    {
	XtSetArg(args[arg], XmNpattern, pattern.xmstring()); arg++;
	XtSetValues(dialog, args, arg);
    }

    manage_and_raise(dialog);
}

#else

static void BrowseNameCB(GUI::Widget *w)
{
    Delay delay;

    static GUI::FileSelectionDialog *dialog = 0;

    static MString pattern;

    string f(print_file_name_field->get_text().c_str());

    if (f.contains('.'))
	pattern = "*" + f.from('.', -1);
    else
    {
	switch (print_type)
	{
	case PRINT_POSTSCRIPT:
	    pattern = "*.ps";
	    break;

	case PRINT_FIG:
	    pattern = "*.fig";
	    break;
	}
    }


    if (dialog == 0)
    {
	dialog = new GUI::FileSelectionDialog(*find_shell1(w), 
					      "browse_print",
					      GUI::FileActionSave);
#ifdef NAG_ME
#warning Set file filter.
#endif

	Delay::register_shell(dialog);
	GUI::Button *button;
	button = dialog->add_button("OK");
	button->signal_clicked().connect(sigc::bind(sigc::ptr_fun(SetPrintFileNameCB), dialog));
	button = dialog->add_button("Cancel");
	button->signal_clicked().connect(sigc::bind(sigc::ptr_fun(UnmanageThisCB), dialog));
    }
    else
    {
#ifdef NAG_ME
#warning Set file filters?
#endif
    }

    manage_and_raise(dialog);
}

#endif

#if defined(IF_XM)

static void PrintCB(Widget parent, bool displays)
{
    print_displays = displays;

    static Widget print_displays_w;
    static Widget print_plots_w;
    static Widget print_selected_w;

    if (print_dialog != 0)
    {
	// Dialog already created -- pop it up again
	XmToggleButtonSetState(print_plots_w, !displays, True);
	XmToggleButtonSetState(print_displays_w, displays, True);
	XmToggleButtonSetState(print_selected_w, 
			       data_disp->have_selection(), True);
	manage_and_raise(print_dialog);
	return;
    }

    Arg args[10];
    Cardinal arg = 0;
    XtSetArg(args[arg], XmNautoUnmanage, False); arg++;
    print_dialog = 
	verify(XmCreatePromptDialog(find_shell(parent),
				    XMST("print"), 
				    args, arg));
    Delay::register_shell(print_dialog);

    if (lesstif_version <= 79)
	XtUnmanageChild(XmSelectionBoxGetChild(print_dialog,
					       XmDIALOG_APPLY_BUTTON));

    XtAddCallback(print_dialog, XmNokCallback,
 		  PrintAgainCB, XtPointer(1));
    XtAddCallback(print_dialog, XmNapplyCallback,
		  PrintAgainCB, XtPointer(0));
    XtAddCallback(print_dialog, XmNcancelCallback, 
		  UnmanageThisCB, XtPointer(print_dialog));
    XtAddCallback(print_dialog, XmNhelpCallback,
		  ImmediateHelpCB, XtPointer(0));

    // Remove old prompt
    XtUnmanageChild(XmSelectionBoxGetChild(print_dialog, XmDIALOG_TEXT));
    XtUnmanageChild(XmSelectionBoxGetChild(print_dialog, 
					   XmDIALOG_SELECTION_LABEL));

    // Create menu
    static Widget print_to_printer_w;
    static Widget print_to_file_w;
    static MMDesc print_to_menu[] = 
    {
	GENTRYL("printer", "printer", MMToggle, 
		BIND(SetPrintTargetCB, TARGET_PRINTER), 
		sigc::bind(sigc::retype(sigc::ptr_fun(SetPrintTargetCB)), TARGET_PRINTER), 
		0, &print_to_printer_w),
	GENTRYL("file", "file", MMToggle, 
		BIND(SetPrintTargetCB, TARGET_FILE), 
		sigc::bind(sigc::retype(sigc::ptr_fun(SetPrintTargetCB)), TARGET_FILE), 
		0, &print_to_file_w),
	MMEnd
    };

    static Widget postscript_w;
    static Widget fig_w;
    static MMDesc type2_menu[] = 
    {
	GENTRYL("postscript", "postscript", MMToggle, 
		BIND(SetPrintTypeCB, PRINT_POSTSCRIPT), 
		sigc::bind(sigc::ptr_fun(SetPrintTypeCB), PRINT_POSTSCRIPT), 
		0, &postscript_w),
	GENTRYL("xfig", "xfig", MMToggle,
		BIND(SetPrintTypeCB, PRINT_FIG),
		sigc::bind(sigc::ptr_fun(SetPrintTypeCB), PRINT_FIG),
		0, &fig_w),
	MMEnd
    };

    static Widget print_color_w;
    static MMDesc type_menu[] =
    {
	GENTRYL("type2", "type2", MMRadioPanel | MMUnmanagedLabel, 
		MMNoCB, MDUMMY, type2_menu, 0),
	GENTRYL("color", "color", MMToggle,
		BIND(SetGCColorCB, 0), 
		sigc::retype(sigc::ptr_fun(SetGCColorCB)), 
		0, &print_color_w),
	MMEnd
    };

    static MMDesc what2_menu[] = 
    {
	GENTRYL("displays", "displays", MMToggle,
		BIND(SetPrintDisplaysCB, true), 
		sigc::bind(sigc::retype(sigc::ptr_fun(SetPrintDisplaysCB)), true), 
		0, &print_displays_w),
	GENTRYL("plots", "plots", MMToggle,
		BIND(SetPrintDisplaysCB, false), 
		sigc::bind(sigc::retype(sigc::ptr_fun(SetPrintDisplaysCB)), false), 
		0, &print_plots_w),
	MMEnd
    };

    static MMDesc what_menu[] = 
    {
	GENTRYL("what2", "what2", MMRadioPanel | MMUnmanagedLabel, 
		MMNoCB, MDUMMY, what2_menu, 0),
	GENTRYL("selected", "selected", MMToggle,
		BIND(SetPrintSelectedNodesCB, 0), 
		sigc::retype(sigc::ptr_fun(SetPrintSelectedNodesCB)), 
		0, &print_selected_w),
	MMEnd
    };

    static Widget print_portrait_w;
    static Widget print_landscape_w;
    static MMDesc orientation_menu[] = 
    {
	GENTRYL("portrait", "portrait", MMRadio, 
		BIND(SetGCOrientation, PostScriptPrintGC::PORTRAIT), 
		sigc::bind(sigc::retype(sigc::ptr_fun(SetGCOrientation)), PostScriptPrintGC::PORTRAIT), 
		0, &print_portrait_w),
	GENTRYL("landscape", "landscape", MMRadio,
		BIND(SetGCOrientation, PostScriptPrintGC::LANDSCAPE),
		sigc::bind(sigc::retype(sigc::ptr_fun(SetGCOrientation)), PostScriptPrintGC::LANDSCAPE),
		0, &print_landscape_w),
	MMEnd
    };

    static MMDesc paper_menu[] = 
    {
	GENTRYL("a4", "a4", MMToggle, 
		BIND(SetGCA4, 0),
		sigc::retype(sigc::ptr_fun(SetGCA4)),
		0, &a4_paper_size),
	GENTRYL("a3", "a3", MMToggle, 
		BIND(SetGCA3, 0),
		sigc::retype(sigc::ptr_fun(SetGCA3)),
		0, &a3_paper_size),
	GENTRYL("letter", "letter", MMToggle, 
		BIND(SetGCLetter, 0),
		sigc::retype(sigc::ptr_fun(SetGCLetter)),
		0, &letter_paper_size),
	GENTRYL("legal", "legal", MMToggle, 
		BIND(SetGCLegal, 0),
		sigc::retype(sigc::ptr_fun(SetGCLegal)),
		0, &legal_paper_size),
	GENTRYL("executive", "executive", MMToggle, 
		BIND(SetGCExecutive, 0),
		sigc::retype(sigc::ptr_fun(SetGCExecutive)),
		0, &executive_paper_size),
	GENTRYL("custom", "custom", MMToggle, 
		BIND(SetGCCustom, 0),
		sigc::retype(sigc::ptr_fun(SetGCCustom)),
		0, &custom_paper_size),
	MMEnd
    };

    static MMDesc name_menu[] = 
    {
	GENTRYL("name", "name", MMTextField | MMUnmanagedLabel,
		MMNoCB, MDUMMY,
		0, &print_file_name_field),
	GENTRYL("browse", "browse", MMPush,
		BIND_0(PTR_FUN(BrowseNameCB)),
		sigc::ptr_fun(BrowseNameCB),
		0, 0),
	MMEnd
    };

    static MMDesc menu[] =
    {
	GENTRYL("to", "to", MMRadioPanel, MMNoCB, MDUMMY, print_to_menu, 0),
	GENTRYL("command", "command", MMTextField, MMNoCB, MDUMMY, 0, &print_command_field),
	GENTRYL("name", "name", MMPanel, MMNoCB, MDUMMY, name_menu, &print_file_name_box),
	MMSep,
	GENTRYL("type", "type", MMPanel, MMNoCB, MDUMMY, type_menu, 0),
	GENTRYL("what", "what", MMPanel, MMNoCB, MDUMMY, what_menu, 0),
	GENTRYL("orientation", "orientation", MMRadioPanel, MMNoCB, MDUMMY, orientation_menu, 0),
	GENTRYL("size", "size", MMRadioPanel, MMNoCB, MDUMMY, paper_menu, 0),
	MMEnd
    };

    Widget panel = MMcreatePanel(print_dialog, "options", menu);
    (void) panel;		// Use it
    MMadjustPanel(menu);

    // Add callbacks
    MMaddCallbacks(menu);

    XtAddCallback(print_to_printer_w, XmNvalueChangedCallback,   
		  SetSensitiveCB,   XtPointer(print_command_field));
    XtAddCallback(print_to_printer_w, XmNvalueChangedCallback,   
		  SetSensitiveCB,   XtPointer(menu[1].label));

    XtAddCallback(print_to_printer_w, XmNvalueChangedCallback,
		  UnsetSensitiveCB, XtPointer(print_file_name_box));
    XtAddCallback(print_to_printer_w, XmNvalueChangedCallback,
		  UnsetSensitiveCB, XtPointer(menu[2].label));
    XtAddCallback(print_to_printer_w, XmNvalueChangedCallback,
		  UnsetSensitiveCB, XtPointer(postscript_w));
    XtAddCallback(print_to_printer_w, XmNvalueChangedCallback,
		  UnsetSensitiveCB, XtPointer(fig_w));

    XtAddCallback(print_to_printer_w, XmNvalueChangedCallback,
		  TakeFocusCB,      XtPointer(print_command_field));

    XtAddCallback(print_to_file_w, XmNvalueChangedCallback,   
		  UnsetSensitiveCB, XtPointer(print_command_field));
    XtAddCallback(print_to_file_w, XmNvalueChangedCallback,   
		  UnsetSensitiveCB, XtPointer(menu[1].label));

    XtAddCallback(print_to_file_w, XmNvalueChangedCallback,   
		  SetSensitiveCB,   XtPointer(print_file_name_box));
    XtAddCallback(print_to_file_w, XmNvalueChangedCallback,
		  SetSensitiveCB,   XtPointer(menu[2].label));
    XtAddCallback(print_to_file_w, XmNvalueChangedCallback,   
		  SetSensitiveCB,   XtPointer(postscript_w));
    XtAddCallback(print_to_file_w, XmNvalueChangedCallback,   
		  SetSensitiveCB,   XtPointer(fig_w));

    XtAddCallback(print_to_file_w, XmNvalueChangedCallback,
		  TakeFocusCB,      XtPointer(print_file_name_field));

    // Create size dialog
    arg = 0;
    XtSetArg(args[arg], XmNautoUnmanage, False); arg++;
    paper_size_dialog = 
	verify(XmCreatePromptDialog(find_shell(parent), 
				    XMST("paper_size_dialog"), 
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
    XmToggleButtonSetState(print_to_printer_w, True, True);
    XmToggleButtonSetState(postscript_w, True, True);
    XmToggleButtonSetState(print_color_w, False, True);
    XmToggleButtonSetState(print_selected_w, False, True);
    XmToggleButtonSetState(print_portrait_w, True, True);
    XmToggleButtonSetState(print_landscape_w, False, True);
    XmToggleButtonSetState(print_plots_w, !displays, True);
    XmToggleButtonSetState(print_displays_w, displays, True);
    XmToggleButtonSetState(print_selected_w, 
			   data_disp->have_selection(), True);

    bool ok = set_paper_size(app_data.paper_size);
    if (!ok)
	XmToggleButtonSetState(a4_paper_size, True, True);

    string command = string(app_data.print_command) + " ";
    XmTextFieldSetString(print_command_field, XMST(command.chars()));

    // Gofer it!
    manage_and_raise(print_dialog);
}

#else

static void PrintCB(GUI::Button *parent, bool displays)
{
    print_displays = displays;

    static GUI::CheckButton *print_displays_w;
    static GUI::CheckButton *print_plots_w;
    static GUI::CheckButton *print_selected_w;

    if (print_dialog != 0)
    {
	// Dialog already created -- pop it up again
	XmToggleButtonSetState(print_plots_w, !displays, True);
	XmToggleButtonSetState(print_displays_w, displays, True);
	XmToggleButtonSetState(print_selected_w, 
			       data_disp->have_selection(), True);
	manage_and_raise1(print_dialog);
	return;
    }

    print_dialog = new GUI::Dialog(find_shell(parent), "print");
    Delay::register_shell(print_dialog);

    GUI::Button *button;
    button = print_dialog->add_button("ok", "OK");
    button->signal_clicked().connect(sigc::bind(sigc::ptr_fun(PrintAgainCB1),
						button, 1L));

    // Remove old prompt
    // Create menu
    static GUI::CheckButton *print_to_printer_w;
    static GUI::CheckButton *print_to_file_w;
    static MMDesc print_to_menu[] = 
    {
	GENTRYL("printer", "printer", MMToggle, 
		BIND(SetPrintTargetCB, TARGET_PRINTER), 
		sigc::bind(sigc::retype(sigc::ptr_fun(SetPrintTargetCB)), TARGET_PRINTER), 
		0, &print_to_printer_w),
	GENTRYL("file", "file", MMToggle, 
		BIND(SetPrintTargetCB, TARGET_FILE), 
		sigc::bind(sigc::retype(sigc::ptr_fun(SetPrintTargetCB)), TARGET_FILE), 
		0, &print_to_file_w),
	MMEnd
    };

    static GUI::CheckButton *postscript_w;
    static GUI::Widget *fig_w;
    static MMDesc type2_menu[] = 
    {
	GENTRYL("postscript", "postscript", MMToggle, 
		BIND(SetPrintTypeCB, PRINT_POSTSCRIPT), 
		sigc::bind(sigc::ptr_fun(SetPrintTypeCB), PRINT_POSTSCRIPT), 
		0, &postscript_w),
	GENTRYL("xfig", "xfig", MMToggle,
		BIND(SetPrintTypeCB, PRINT_FIG),
		sigc::bind(sigc::ptr_fun(SetPrintTypeCB), PRINT_FIG),
		0, &fig_w),
	MMEnd
    };

    static GUI::CheckButton *print_color_w;
    static MMDesc type_menu[] = 
    {
	GENTRYL("type2", "type2", MMRadioPanel | MMUnmanagedLabel, 
		MMNoCB, MDUMMY, type2_menu, 0),
	GENTRYL("color", "color", MMToggle,
		BIND(SetGCColorCB, 0), 
		sigc::retype(sigc::ptr_fun(SetGCColorCB)), 
		0, &print_color_w),
	MMEnd
    };

    static MMDesc what2_menu[] = 
    {
	GENTRYL("displays", "displays", MMToggle,
		BIND(SetPrintDisplaysCB, true), 
		sigc::bind(sigc::retype(sigc::ptr_fun(SetPrintDisplaysCB)), true), 
		0, &print_displays_w),
	GENTRYL("plots", "plots", MMToggle,
		BIND(SetPrintDisplaysCB, false), 
		sigc::bind(sigc::retype(sigc::ptr_fun(SetPrintDisplaysCB)), false), 
		0, &print_plots_w),
	MMEnd
    };

    static MMDesc what_menu[] = 
    {
	GENTRYL("what2", "what2", MMRadioPanel | MMUnmanagedLabel, 
		MMNoCB, MDUMMY, what2_menu, 0),
	GENTRYL("selected", "selected", MMToggle,
		BIND(SetPrintSelectedNodesCB, 0), 
		sigc::retype(sigc::ptr_fun(SetPrintSelectedNodesCB)), 
		0, &print_selected_w),
	MMEnd
    };

    static GUI::CheckButton *print_portrait_w;
    static GUI::CheckButton *print_landscape_w;
    static MMDesc orientation_menu[] = 
    {
	GENTRYL("portrait", "portrait", MMRadio, 
		BIND(SetGCOrientation, PostScriptPrintGC::PORTRAIT), 
		sigc::bind(sigc::retype(sigc::ptr_fun(SetGCOrientation)), PostScriptPrintGC::PORTRAIT), 
		0, &print_portrait_w),
	GENTRYL("landscape", "landscape", MMRadio,
		BIND(SetGCOrientation, PostScriptPrintGC::LANDSCAPE),
		sigc::bind(sigc::retype(sigc::ptr_fun(SetGCOrientation)), PostScriptPrintGC::LANDSCAPE),
		0, &print_landscape_w),
	MMEnd
    };

    static MMDesc paper_menu[] = 
    {
	GENTRYL("a4", "a4", MMToggle, 
		BIND(SetGCA4, 0),
		sigc::retype(sigc::ptr_fun(SetGCA4)),
		0, &a4_paper_size),
	GENTRYL("a3", "a3", MMToggle, 
		BIND(SetGCA3, 0),
		sigc::retype(sigc::ptr_fun(SetGCA3)),
		0, &a3_paper_size),
	GENTRYL("letter", "letter", MMToggle, 
		BIND(SetGCLetter, 0),
		sigc::retype(sigc::ptr_fun(SetGCLetter)),
		0, &letter_paper_size),
	GENTRYL("legal", "legal", MMToggle, 
		BIND(SetGCLegal, 0),
		sigc::retype(sigc::ptr_fun(SetGCLegal)),
		0, &legal_paper_size),
	GENTRYL("executive", "executive", MMToggle, 
		BIND(SetGCExecutive, 0),
		sigc::retype(sigc::ptr_fun(SetGCExecutive)),
		0, &executive_paper_size),
	GENTRYL("custom", "custom", MMToggle, 
		BIND(SetGCCustom, 0),
		sigc::retype(sigc::ptr_fun(SetGCCustom)),
		0, &custom_paper_size),
	MMEnd
    };

    static MMDesc name_menu[] = 
    {
	GENTRYL("name", "name", MMTextField | MMUnmanagedLabel,
		MMNoCB, MDUMMY,
		0, &print_file_name_field),
	GENTRYL("browse", "browse", MMPush,
		BIND_0(PTR_FUN(BrowseNameCB)),
		sigc::ptr_fun(BrowseNameCB),
		0, 0),
	MMEnd
    };

    static MMDesc menu[] =
    {
	GENTRYL("to", "to", MMRadioPanel, MMNoCB, MDUMMY, print_to_menu, 0),
	GENTRYL("command", "command", MMTextField, MMNoCB, MDUMMY, 0, &print_command_field),
	GENTRYL("name", "name", MMPanel, MMNoCB, MDUMMY, name_menu, &print_file_name_box),
	MMSep,
	GENTRYL("type", "type", MMPanel, MMNoCB, MDUMMY, type_menu, 0),
	GENTRYL("what", "what", MMPanel, MMNoCB, MDUMMY, what_menu, 0),
	GENTRYL("orientation", "orientation", MMRadioPanel, MMNoCB, MDUMMY, orientation_menu, 0),
	GENTRYL("size", "size", MMRadioPanel, MMNoCB, MDUMMY, paper_menu, 0),
	MMEnd
    };

    GUI::Widget *panel = MMcreatePanel(print_dialog, "options", menu);
    (void) panel;		// Use it
    MMadjustPanel(menu);

    // Add callbacks
    MMaddCallbacks(menu);

    print_to_printer_w->signal_toggled().connect(sigc::bind(sigc::retype(sigc::ptr_fun(SetSensitiveCB)),
							    print_to_printer_w,
							    print_command_field));
    print_to_printer_w->signal_toggled().connect(sigc::bind(sigc::retype(sigc::ptr_fun(SetSensitiveCB)),
							    print_to_printer_w,
							    menu[1].label));

    print_to_printer_w->signal_toggled().connect(sigc::bind(sigc::retype(sigc::ptr_fun(UnsetSensitiveCB)),
							    print_to_printer_w,
							    print_file_name_box));
    print_to_printer_w->signal_toggled().connect(sigc::bind(sigc::retype(sigc::ptr_fun(UnsetSensitiveCB)),
							    print_to_printer_w,
							    menu[2].label));
    print_to_printer_w->signal_toggled().connect(sigc::bind(sigc::retype(sigc::ptr_fun(UnsetSensitiveCB)),
							    print_to_printer_w,
							    postscript_w));
    print_to_printer_w->signal_toggled().connect(sigc::bind(sigc::retype(sigc::ptr_fun(UnsetSensitiveCB)),
							    print_to_printer_w,
							    fig_w));

    print_to_printer_w->signal_toggled().connect(sigc::bind(sigc::retype(sigc::ptr_fun(TakeFocusCB)),
							    print_to_printer_w,
							    print_command_field));

    print_to_file_w->signal_toggled().connect(sigc::bind(sigc::retype(sigc::ptr_fun(UnsetSensitiveCB)),
							 print_to_file_w,
							 print_command_field));
    print_to_file_w->signal_toggled().connect(sigc::bind(sigc::retype(sigc::ptr_fun(UnsetSensitiveCB)),
							 print_to_file_w,
							 menu[1].label));

    print_to_file_w->signal_toggled().connect(sigc::bind(sigc::ptr_fun(SetSensitiveCB),
							 print_to_file_w,
							 print_file_name_box));
    print_to_file_w->signal_toggled().connect(sigc::bind(sigc::ptr_fun(SetSensitiveCB),
							 print_to_file_w,
							 menu[2].label));
    print_to_file_w->signal_toggled().connect(sigc::bind(sigc::ptr_fun(SetSensitiveCB),
							 print_to_file_w,
							 postscript_w));
    print_to_file_w->signal_toggled().connect(sigc::bind(sigc::ptr_fun(SetSensitiveCB),
							 print_to_file_w,
							 fig_w));

    print_to_file_w->signal_toggled().connect(sigc::bind(sigc::ptr_fun(TakeFocusCB),
							 print_to_file_w,
							 print_file_name_field));

    // Create size dialog
    paper_size_dialog = 
	new GUI::Dialog(find_shell(parent), "paper_size_dialog");
    Delay::register_shell(paper_size_dialog);

    GUI::Entry *entry;
    entry = new GUI::Entry(*paper_size_dialog, GUI::PACK_SHRINK, "entry");
    entry->show();
    GUI::Button *ok_button = paper_size_dialog->add_button("OK");
    ok_button->signal_clicked().connect(sigc::bind(sigc::ptr_fun(SetPaperSizeCB), paper_size_dialog));
    button = paper_size_dialog->add_button("Cancel");
    button->signal_clicked().connect(sigc::bind(sigc::ptr_fun(ResetPaperSizeCB), paper_size_dialog));

    entry->signal_activate().connect(sigc::bind(sigc::ptr_fun(CheckPaperSizeCB), entry, ok_button));

    // Set initial state
    XmToggleButtonSetState(print_to_printer_w, True, True);
    XmToggleButtonSetState(postscript_w, True, True);
    XmToggleButtonSetState(print_color_w, False, True);
    XmToggleButtonSetState(print_selected_w, False, True);
    XmToggleButtonSetState(print_portrait_w, True, True);
    XmToggleButtonSetState(print_landscape_w, False, True);
    XmToggleButtonSetState(print_plots_w, !displays, True);
    XmToggleButtonSetState(print_displays_w, displays, True);
    XmToggleButtonSetState(print_selected_w, 
			   data_disp->have_selection(), True);

    bool ok = set_paper_size(app_data.paper_size);
    if (!ok)
	XmToggleButtonSetState(a4_paper_size, True, True);

    string command = string(app_data.print_command) + " ";
    XmTextFieldSetString(print_command_field, XMST(command.chars()));

    // Gofer it!
    manage_and_raise1(print_dialog);
}

#endif

#if defined(IF_XM)

void PrintGraphCB(Widget w, XtPointer, XtPointer)
{
    PrintCB(w, true);
}

void PrintPlotCB(Widget w, XtPointer, XtPointer)
{
    PrintCB(w, false);
}

#else

void PrintGraphCB(GUI::Button *w)
{
    PrintCB(w, true);
}

void PrintPlotCB(GUI::Button *w)
{
    PrintCB(w, false);
}

#endif
