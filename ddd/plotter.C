// $Id$ -*- C++ -*-
// Create a plotter interface

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

char plotter_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "plotter.h"

#include "assert.h"
#include "charsets.h"
#include "cook.h"
#include "ddd.h"
#include "exit.h"
#include "findParent.h"
#include "findWindow.h"
#include "file.h"
#include "filetype.h"
#include "fonts.h"
#include "post.h"
#include "print.h"
#include "regexps.h"
#include "simpleMenu.h"
#include "status.h"
#include "strclass.h"
#include "string-fun.h"
#include "verify.h"
#include "version.h"
#include "wm.h"
#include "AppData.h"
#include "Command.h"
#include "Delay.h"
#include "HelpCB.h"
#include "MakeMenu.h"
#include "PlotArea.h"
#include "Swallower.h"
#include "DispValue.h"
#include "DataDisp.h"
#include "DestroyCB.h"

#include <stdio.h>
#include <fstream.h>

#include <Xm/Command.h>
#include <Xm/MainW.h>
#include <Xm/MessageB.h>
#include <Xm/AtomMgr.h>
#include <Xm/FileSB.h>
#include <Xm/Protocols.h>
#include <Xm/DrawingA.h>
#include <Xm/ScrolledW.h>
#include <Xm/SelectioB.h>
#include <Xm/ScrollBar.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <Xm/ToggleB.h>

static void TraceInputHP (Agent *source, void *, void *call_data);
static void TraceOutputHP(Agent *source, void *, void *call_data);
static void TraceErrorHP (Agent *source, void *, void *call_data);
static void SetStatusHP  (Agent *source, void *, void *call_data);
static void PlotterNotFoundHP(Agent *source, void *, void *call_data);

static void CancelPlotCB(Widget, XtPointer, XtPointer);
static void ExposePlotAreaCB(Widget, XtPointer, XtPointer);
static void ResizePlotAreaCB(Widget, XtPointer, XtPointer);

static void SelectPlotCB(Widget, XtPointer, XtPointer);
static void SelectAndPrintPlotCB(Widget, XtPointer, XtPointer);

static void ReplotCB(Widget, XtPointer, XtPointer);
static void PlotCommandCB(Widget, XtPointer, XtPointer);
static void ExportPlotCB(Widget, XtPointer, XtPointer);

static void ToggleOptionCB(Widget, XtPointer, XtPointer);
static void ToggleLogscaleCB(Widget, XtPointer, XtPointer);
static void SetStyleCB(Widget, XtPointer, XtPointer);
static void SetContourCB(Widget, XtPointer, XtPointer);
static void SetViewCB(Widget, XtPointer, XtPointer);


struct PlotWindowInfo {
    DispValue *source;		// The source we depend upon
    PlotAgent *plotter;		// The current Gnuplot instance
    PlotArea *area;		// The area we're drawing in
    Widget shell;		// The shell we're in
    Widget working_dialog;	// The working dialog
    Widget swallower;		// The Gnuplot window
    Widget vsb;			// Vertical scroll bar
    Widget hsb;			// Horizontal scroll bar
    Widget command;		// Command widget
    Widget command_dialog;      // Command dialog
    Widget export_dialog;       // Export dialog
    bool active;		// True if popped up

    // Constructor - just initialize
    PlotWindowInfo()
	: plotter(0), area(0), shell(0), working_dialog(0), swallower(0),
	  vsb(0), hsb(0), command(0), command_dialog(0), 
	  export_dialog(0), active(false)
    {}
};


//-------------------------------------------------------------------------
// Menus
//-------------------------------------------------------------------------

static MMDesc file_menu[] = 
{
    { "command", MMPush, { PlotCommandCB, 0 }, 0, 0, 0, 0 },
    MMSep,
    { "replot",  MMPush, { ReplotCB, 0 }, 0, 0, 0, 0 },
    { "print",   MMPush, { SelectAndPrintPlotCB, 0 }, 0, 0, 0, 0 },
    { "export",  MMPush, { ExportPlotCB, 0 }, 0, 0, 0, 0 },
    MMSep,
    { "close",   MMPush, { CancelPlotCB, 0 }, 0, 0, 0, 0 },
    { "exit",    MMPush, { DDDExitCB, XtPointer(EXIT_SUCCESS) }, 0, 0, 0, 0},
    MMEnd
};

static MMDesc view_menu[] = 
{
    { "border",    MMToggle, { ToggleOptionCB, 0 }, 0, 0, 0, 0 },
    { "time",      MMToggle, { ToggleOptionCB, 0 }, 0, 0, 0, 0 },
    MMSep,
    { "grid",      MMToggle, { ToggleOptionCB, 0 }, 0, 0, 0, 0 },
    { "xzeroaxis", MMToggle, { ToggleOptionCB, 0 }, 0, 0, 0, 0 },
    { "yzeroaxis", MMToggle, { ToggleOptionCB, 0 }, 0, 0, 0, 0 },
    MMEnd
};

static MMDesc contour_menu[] = 
{
    { "base",      MMToggle, { SetContourCB, 0 }, 0, 0, 0, 0 },
    { "surface",   MMToggle, { SetContourCB, 0 }, 0, 0, 0, 0 },
    MMEnd
};

static MMDesc scale_menu[] = 
{
    { "logscale",  MMToggle, { ToggleLogscaleCB, 0 }, 0, 0, 0, 0 },
    MMSep,
    { "xtics",     MMToggle, { ToggleOptionCB, 0 }, 0, 0, 0, 0 },
    { "ytics",     MMToggle, { ToggleOptionCB, 0 }, 0, 0, 0, 0 },
    { "ztics",     MMToggle, { ToggleOptionCB, 0 }, 0, 0, 0, 0 },
    MMEnd
};

static MMDesc plot_menu[] = 
{
    { "points",         MMToggle, { SetStyleCB, 0 }, 0, 0, 0, 0 },
    { "lines",          MMToggle, { SetStyleCB, 0 }, 0, 0, 0, 0 },
    { "lines3d",        MMToggle, { SetStyleCB, 0 }, 0, 0, 0, 0 },
    { "linespoints",    MMToggle, { SetStyleCB, 0 }, 0, 0, 0, 0 },
    { "linespoints3d",  MMToggle | MMUnmanaged, 
                                  { SetStyleCB, 0 }, 0, 0, 0, 0 },
    { "impulses",       MMToggle, { SetStyleCB, 0 }, 0, 0, 0, 0 },
    { "dots",           MMToggle, { SetStyleCB, 0 }, 0, 0, 0, 0 },
    { "steps2d",        MMToggle, { SetStyleCB, 0 }, 0, 0, 0, 0 },
    { "boxes2d",        MMToggle, { SetStyleCB, 0 }, 0, 0, 0, 0 },
    MMEnd
};

static MMDesc menubar[] = 
{
    { "file",     MMMenu,          MMNoCB, file_menu,        0, 0, 0 },
    { "edit",     MMMenu,          MMNoCB, simple_edit_menu, 0, 0, 0 },
    { "plotView", MMMenu,          MMNoCB, view_menu,        0, 0, 0 },
    { "plot",     MMRadioMenu,     MMNoCB, plot_menu,        0, 0, 0 },
    { "scale",    MMMenu,          MMNoCB, scale_menu,       0, 0, 0 },
    { "contour",  MMMenu,          MMNoCB, contour_menu,     0, 0, 0 },
    { "help",     MMMenu | MMHelp, MMNoCB, simple_help_menu, 0, 0, 0 },
    MMEnd
};



//-------------------------------------------------------------------------
// Plotter commands
//-------------------------------------------------------------------------

static void send(PlotWindowInfo *plot, const string& cmd)
{
    data_disp->select(plot->source);
    plot->plotter->write(cmd.chars(), cmd.length());
}

static void send_and_replot(PlotWindowInfo *plot, string cmd)
{
    if (cmd.matches(rxwhite))
	return;

    if (!cmd.contains('\n', -1))
	cmd += "\n";
    if (cmd.contains("help", 0))
	cmd += "\n";		// Exit `help'
    else
	cmd += "replot\n";

    send(plot, cmd);
}


//-------------------------------------------------------------------------
// Set up menu
//-------------------------------------------------------------------------

static string slurp_file(const string& filename)
{
    string s;

    ifstream is(filename);
    if (is.bad())
	return "";

    int c;
    while ((c = is.get()) != EOF)
	s += (unsigned char)c;

    return s;
}

static string plot_settings(PlotWindowInfo *plot)
{
    string settings_file = tmpnam(0);
    string cmd = "save " + quote(settings_file) + "\n";
    send(plot, cmd);

    // Wait for settings file to be created
    Delay delay;

    string settings;
    do {
	sleep(1);
	settings = slurp_file(settings_file);
    } while (!settings.contains("set zero"));

    unlink(settings_file);

    return settings;
}

static void configure_options(PlotWindowInfo *plot, MMDesc *menu, 
			      const string& settings)
{
    for (int i = 0; menu[i].name != 0; i++)
    {
	if ((menu[i].type & MMTypeMask) != MMToggle)
	    continue;

	string name = menu[i].name;

	Widget w = XtNameToWidget(plot->shell, "*" + name);
	XtCallbackProc callback = menu[i].callback.callback;

	bool set = false;
	if (callback == ToggleOptionCB)
	{
	    set = settings.contains("\nset " + name + "\n");
	}
	else if (callback == SetContourCB)
	{
	    if (name == "base")
		set = settings.contains("\nset contour base\n") ||
		    settings.contains("\nset contour both\n");
	    else if (name == "surface")
		set = settings.contains("\nset contour surface\n") ||
		    settings.contains("\nset contour both\n");
	}
	else if (callback == ToggleLogscaleCB)
	{
	    set = settings.contains("\nset logscale ");
	}

	XmToggleButtonSetState(w, set, False);
    }
}

static void configure_plot(PlotWindowInfo *plot)
{
    if (plot->plotter == 0)
	return;

    int ndim = plot->plotter->dimensions();

    // Set up plot menu
    int i;
    for (i = 0; plot_menu[i].name != 0; i++)
    {
	if ((plot_menu[i].type & MMTypeMask) != MMToggle)
	    continue;

	string name = plot_menu[i].name;

	Widget w = XtNameToWidget(plot->shell, "*" + name);

	if (name.contains("2d", -1))
	    XtSetSensitive(w, ndim == 2);
	else if (name.contains("3d", -1))
	    XtSetSensitive(w, ndim >= 3);
	else
	    XtSetSensitive(w, ndim >= 2);
    }

    // Log scale is available only iff all values are non-negative
    Widget logscale = XtNameToWidget(plot->shell, "*logscale");
    XtSetSensitive(logscale, plot->plotter->min_v() >= 0.0);

    // Axes can be toggled in 2d mode only
    Widget xzeroaxis = XtNameToWidget(plot->shell, "*xzeroaxis");
    Widget yzeroaxis = XtNameToWidget(plot->shell, "*yzeroaxis");
    XtSetSensitive(xzeroaxis, ndim <= 2);
    XtSetSensitive(yzeroaxis, ndim <= 2);

    // Z Tics are available in 3d mode only
    Widget ztics = XtNameToWidget(plot->shell, "*ztics");
    XtSetSensitive(ztics, ndim >= 3);

    // Contour drawing is available in 3d mode only
    Widget base    = XtNameToWidget(plot->shell, "*base");
    Widget surface = XtNameToWidget(plot->shell, "*surface");
    XtSetSensitive(base,    ndim >= 3);
    XtSetSensitive(surface, ndim >= 3);

    // Get settings
    string settings = plot_settings(plot);
    configure_options(plot, view_menu,    settings);
    configure_options(plot, contour_menu, settings);
    configure_options(plot, scale_menu,   settings);

    // Get style
    for (i = 0; plot_menu[i].name != 0; i++)
    {
	if ((plot_menu[i].type & MMTypeMask) != MMToggle)
	    continue;

	string name = plot_menu[i].name;

	Widget w = XtNameToWidget(plot->shell, "*" + name);

	bool set = settings.contains("\nset data style " + name + "\n");
	XmToggleButtonSetState(w, set, False);
    }

    // Set scrollbars
    manage_child(plot->hsb, ndim >= 3);
    manage_child(plot->vsb, ndim >= 3);

    int rot_x = 60;
    int rot_z = 30;

    int view_index = settings.index("set view ");
    if (view_index >= 0)
    {
	// `set view <rot_x> {,{<rot_z>}{,{<scale>}{,<scale_z>}}}'
	string view_setting = settings.after("set view ");
	rot_x = atoi(view_setting);
	view_setting = view_setting.after(", ");
	rot_z = atoi(view_setting);
    }

    XtVaSetValues(plot->vsb, XmNvalue, rot_x, NULL);
    XtVaSetValues(plot->hsb, XmNvalue, rot_z, NULL);

    // Check if we can export something
    bool have_source = false;
    bool can_export  = false;
    const StringArray& sources = plot->plotter->data_files();
    for (i = 0; i < sources.size(); i++)
    {
	if (sources[i] != "")
	{
	    if (have_source)
		can_export  = false; // Multiple source files
	    else
		can_export = have_source = true;
	}
    }

    Widget export = XtNameToWidget(plot->shell, "*export");
    set_sensitive(export, have_source);
}



//-------------------------------------------------------------------------
// Decoration stuff
//-------------------------------------------------------------------------

// Start plot
static void popup_plot_shell(PlotWindowInfo *plot)
{
    if (!plot->active && plot->plotter != 0)
    {
	// Pop down working dialog
	if (plot->working_dialog != 0)
	{
	    XtUnmanageChild(plot->working_dialog);
	    XtPopdown(XtParent(plot->working_dialog));
	}

	if (plot->command_dialog != 0)
	    XtUnmanageChild(plot->command_dialog);
	if (plot->export_dialog != 0)
	    XtUnmanageChild(plot->export_dialog);

	// Setup menu
	plot->plotter->removeHandler(Died, PlotterNotFoundHP, 
				     (void *)plot);
	configure_plot(plot);

	XtPopup(plot->shell, XtGrabNone);
	plot->active = true;
    }
}

// Swallow new GNUPLOT window
static void SwallowCB(Widget swallower, XtPointer client_data, 
		      XtPointer call_data)
{
    PlotWindowInfo *plot = (PlotWindowInfo *)client_data;
    SwallowerInfo *info = (SwallowerInfo *)call_data;
    Window window = 
	findWindow(XtDisplay(swallower), info->window, app_data.plot_window);

    if (window != None)
    {
	// We have the window
	XtRemoveCallback(swallower, XtNwindowCreatedCallback, 
			 SwallowCB, client_data);

	XtVaSetValues(swallower, XtNwindow, window, NULL);

	popup_plot_shell(plot);
    }
}

// Swallow again after window has gone.  This happens while printing.
static void SwallowAgainCB(Widget swallower, XtPointer client_data, XtPointer)
{
    XtAddCallback(swallower, XtNwindowCreatedCallback, SwallowCB, client_data);
}


// Cancel plot
static void popdown_plot_shell(PlotWindowInfo *plot)
{
    static bool entered = false;
    if (entered)
	return;

    entered = true;

    if (plot->working_dialog != 0)
    {
	XtUnmanageChild(plot->working_dialog);
	XtPopdown(XtParent(plot->working_dialog));
    }

    if (plot->command_dialog != 0)
	XtUnmanageChild(plot->command_dialog);
    if (plot->export_dialog != 0)
	XtUnmanageChild(plot->export_dialog);

    if (plot->shell != 0)
    {
	XtPopdown(plot->shell);
    }

    plot->active = false;

    entered = false;
}

static void CancelPlotCB(Widget, XtPointer client_data, XtPointer)
{
    static bool entered = false;
    if (entered)
	return;

    entered = true;

    PlotWindowInfo *plot = (PlotWindowInfo *)client_data;
    popdown_plot_shell(plot);

    if (plot->plotter != 0)
    {
	// Terminate plotter
	plot->plotter->removeHandler(Died, PlotterNotFoundHP, client_data);
	plot->plotter->terminate();
	plot->plotter = 0;
    }

    entered = false;
}

static void DeletePlotterCB(XtPointer client_data, XtIntervalId *)
{
    Agent *plotter = (Agent *)client_data;
    delete plotter;
}

static void DeletePlotterHP(Agent *plotter, void *client_data, void *)
{
    // Plotter has died - delete memory
    XtAppAddTimeOut(XtWidgetToApplicationContext(gdb_w), 0, 
		    DeletePlotterCB, XtPointer(plotter));

    plotter->removeHandler(Died, DeletePlotterHP, client_data);

    PlotWindowInfo *plot = (PlotWindowInfo *)client_data;
    assert(plot->plotter == 0 || plot->plotter == plotter);
    plot->plotter = 0;
    popdown_plot_shell(plot);
}

static void GetPlotHP(Agent *, void *client_data, void *call_data)
{
    PlotWindowInfo *plot = (PlotWindowInfo *)client_data;

    // We got the plot commands: Pass them to the plot area
    DataLength *dl = (DataLength *)call_data;
    plot->area->plot(string(dl->data, dl->length));

    // Popup shell if needed
    popup_plot_shell(plot);
}

static void PlotterNotFoundHP(Agent *plotter, void *client_data, void *)
{
    PlotWindowInfo *plot = (PlotWindowInfo *)client_data;
    (void) plot;		// Use it
    assert(plot->plotter == 0 || plot->plotter == plotter);

    plotter->removeHandler(Died, PlotterNotFoundHP, client_data);

    string base = app_data.plot_command;
    if (base.contains(' '))
	base = base.before(' ');

    Arg args[10];
    Cardinal arg = 0;
    MString msg = rm(capitalize(base) + " could not be started.");
    XtSetArg(args[arg], XmNmessageString, msg.xmstring()); arg++;
    Widget dialog = 
	verify(XmCreateErrorDialog(find_shell(),
				   "no_plotter_dialog", args, arg));
    XtUnmanageChild(XmMessageBoxGetChild
		    (dialog, XmDIALOG_CANCEL_BUTTON));
    XtAddCallback(dialog, XmNhelpCallback, ImmediateHelpCB, NULL);

    Delay::register_shell(dialog);
    manage_and_raise(dialog);
}


#define SWALLOWER_NAME "swallower"
#define PLOT_AREA_NAME "area"

static VoidArray plot_infos;

static PlotWindowInfo *new_decoration(const string& name)
{
    PlotWindowInfo *plot = 0;

    // Check whether we can reuse an existing decoration
    for (int i = 0; i < plot_infos.size(); i++)
    {
	PlotWindowInfo *info = (PlotWindowInfo *)plot_infos[i];
	if (info->plotter == 0)
	{
	    // Shell is unused - use this one
	    plot = info;
	    break;
	}
    }

    if (plot == 0)
    {
	plot = new PlotWindowInfo;

	// Create decoration windows
	Arg args[10];
	Cardinal arg = 0;
	XtSetArg(args[arg], XmNallowShellResize, True);       arg++;
	XtSetArg(args[arg], XmNdeleteResponse, XmDO_NOTHING); arg++;

	// Mark shell as `used'
	XtSetArg(args[arg], XmNuserData, XtPointer(True)); arg++;
	plot->shell = verify(XtCreateWidget("plot", topLevelShellWidgetClass,
					    find_shell(), args, arg));

	Atom WM_DELETE_WINDOW = 
	    XmInternAtom(XtDisplay(plot->shell), "WM_DELETE_WINDOW", False);
	XmAddWMProtocolCallback(plot->shell, WM_DELETE_WINDOW, 
				CancelPlotCB, XtPointer(plot));

	arg = 0;
	Widget main_window = XmCreateMainWindow(plot->shell, "main_window", 
						args, arg);
	XtManageChild(main_window);

	MMcreateMenuBar(main_window, "menubar", menubar);
	MMaddCallbacks(menubar, XtPointer(plot));
	MMaddHelpCallback(menubar, ImmediateHelpCB);

	arg = 0;
	XtSetArg(args[arg], XmNscrollingPolicy, XmAPPLICATION_DEFINED); arg++;
	XtSetArg(args[arg], XmNvisualPolicy,    XmVARIABLE);            arg++;
	Widget scroll = 
	    XmCreateScrolledWindow(main_window, "scroll", args, arg);
	XtManageChild(scroll);

	// Create work window
	Widget work;
	if (app_data.builtin_plot_window)
	{
	    arg = 0;
	    work = XmCreateDrawingArea(scroll, PLOT_AREA_NAME, args, arg);
	    XtManageChild(work);

	    plot->area = 
		new PlotArea(work, make_font(app_data, FixedWidthDDDFont));
	    XtVaSetValues(work, XmNuserData, XtPointer(plot->area), NULL);
	}
	else
	{
	    arg = 0;
	    work = plot->swallower = 
		XtCreateManagedWidget(SWALLOWER_NAME, swallowerWidgetClass, 
				      scroll, args, arg);
	}

	// Create scroll bars
	const int slider_size = 20;

	arg = 0;
	XtSetArg(args[arg], XmNorientation, XmHORIZONTAL);      arg++;
	XtSetArg(args[arg], XmNminimum,     0);                 arg++;
	XtSetArg(args[arg], XmNmaximum,     360 + slider_size); arg++;
	plot->hsb = XmCreateScrollBar(scroll, "hsb", args, arg);
	XtManageChild(plot->hsb);

	arg = 0;
	XtSetArg(args[arg], XmNorientation, XmVERTICAL);        arg++;
	XtSetArg(args[arg], XmNminimum,     0);                 arg++;
	XtSetArg(args[arg], XmNmaximum,     180 + slider_size); arg++;
	plot->vsb = XmCreateScrollBar(scroll, "vsb", args, arg);
	XtManageChild(plot->vsb);

	XtAddCallback(plot->hsb, XmNvalueChangedCallback,
		      SetViewCB, XtPointer(plot));
	XtAddCallback(plot->vsb, XmNvalueChangedCallback,
		      SetViewCB, XtPointer(plot));

#if 0
	XtAddCallback(plot->hsb, XmNdragCallback, SetViewCB, XtPointer(plot));
	XtAddCallback(plot->vsb, XmNdragCallback, SetViewCB, XtPointer(plot));
#endif

	XmScrolledWindowSetAreas(scroll, plot->hsb, plot->vsb, work);

	Delay::register_shell(plot->shell);
	InstallButtonTips(plot->shell);

	plot_infos += plot;
    }

    string title = DDD_NAME ": " + name;
    XtVaSetValues(plot->shell,
		  XmNtitle, title.chars(),
		  XmNiconName, title.chars(),
		  NULL);

    if (plot->swallower != 0)
    {
	XtRemoveAllCallbacks(plot->swallower, XtNwindowCreatedCallback);
	XtAddCallback(plot->swallower, XtNwindowCreatedCallback,
		      SwallowCB, XtPointer(plot));
	XtRemoveAllCallbacks(plot->swallower, XtNwindowGoneCallback);
	XtAddCallback(plot->swallower, XtNwindowGoneCallback, 
		      SwallowAgainCB, XtPointer(plot));
    }

    plot->active = false;

    return plot;
}

// Remove all unused decorations from cache
void clear_plot_window_cache()
{
    for (int i = 0; i < plot_infos.size(); i++)
    {
	PlotWindowInfo *info = (PlotWindowInfo *)plot_infos[i];
	if (info->plotter == 0)
	{
	    // Shell is unused -- destroy it
	    XtDestroyWidget(info->shell);
	    info->shell = 0;
	}
	else
	{
	    // A running shell should be destroyed after invocation.
	    // (FIXME)
	}
    }

    static VoidArray empty;
    plot_infos = empty;
}


// Create a new plot window
PlotAgent *new_plotter(string name, DispValue *source)
{
    string cmd = app_data.plot_command;
    cmd.gsub("@FONT@", make_font(app_data, FixedWidthDDDFont));

    // Create shell
    PlotWindowInfo *plot = new_decoration(name);
    plot->source = source;
    XtVaSetValues(plot->shell, XmNuserData, XtPointer(True), NULL);

    // Pop up a working dialog
    static Widget dialog = 0;
    if (dialog == 0)
    {
	Arg args[10];
	Cardinal arg = 0;
	dialog = verify(XmCreateWorkingDialog(find_shell(),
					      "launch_plot_dialog", 
					      args, arg));
	XtUnmanageChild(XmMessageBoxGetChild(dialog,
					     XmDIALOG_OK_BUTTON));
	XtUnmanageChild(XmMessageBoxGetChild(dialog,
					     XmDIALOG_HELP_BUTTON));
    }

    XtRemoveAllCallbacks(dialog, XmNcancelCallback);
    XtAddCallback(dialog, XmNcancelCallback, CancelPlotCB, XtPointer(plot));
    plot->working_dialog = dialog;

    string base = cmd;
    if (base.contains(' '))
	base = cmd.before(' ');
    MString msg = rm("Starting ") + tt(base) + rm("...");
    XtVaSetValues(dialog, XmNmessageString, msg.xmstring(), NULL);
    manage_and_raise(dialog);
    wait_until_mapped(dialog);

    // Invoke plot process
    PlotAgent *plotter = 
	new PlotAgent(XtWidgetToApplicationContext(plot->shell), cmd);

    XtAddCallback(plot->shell, XtNpopdownCallback,
		  CancelPlotCB, XtPointer(plot));

    if (plot->area != 0)
    {
	XtAddCallback(plot->area->widget(), XmNexposeCallback,
		      ExposePlotAreaCB, XtPointer(plot));
	XtAddCallback(plot->area->widget(), XmNresizeCallback,
		      ResizePlotAreaCB, XtPointer(plot));
    }

    string init = app_data.plot_init_commands;
    if (plot->area != 0)
	init.prepend("set term xlib\n");
    else
	init.prepend("set term x11\n");
    if (init != "" && !init.contains('\n', -1))
	init += '\n';

    // Add trace handlers
    plotter->addHandler(Input,  TraceInputHP);     // Gnuplot => DDD
    plotter->addHandler(Output, TraceOutputHP);    // DDD => Gnuplot
    plotter->addHandler(Error,  TraceErrorHP);     // Gnuplot Errors => DDD

    // Show Gnuplot Errors in status line
    plotter->addHandler(Error,  SetStatusHP,       (void *)plot);

    // Handle death
    plotter->addHandler(Died, PlotterNotFoundHP, (void *)plot);
    plotter->addHandler(Died, DeletePlotterHP,   (void *)plot);

    if (plot->area != 0)
	plotter->addHandler(Plot, GetPlotHP, (void *)plot);

    plotter->start_with(init);
    plot->plotter = plotter;

    return plotter;
}


//-------------------------------------------------------------------------
// Drawing stuff
//-------------------------------------------------------------------------

static void ExposePlotAreaCB(Widget, XtPointer client_data, XtPointer)
{
    PlotWindowInfo *plot = (PlotWindowInfo *)client_data;
    plot->area->replot(false);
}

static void ResizePlotAreaCB(Widget, XtPointer client_data, XtPointer)
{
    PlotWindowInfo *plot = (PlotWindowInfo *)client_data;
    plot->area->replot(true);
}


//-------------------------------------------------------------------------
// Selection stuff
//-------------------------------------------------------------------------

static void SelectPlotCB(Widget, XtPointer client_data, XtPointer)
{
    PlotWindowInfo *plot = (PlotWindowInfo *)client_data;

    data_disp->select(plot->source);
}

static void SelectAndPrintPlotCB(Widget w, XtPointer client_data, 
				 XtPointer call_data)
{
    SelectPlotCB(w, client_data, call_data);
    PrintPlotCB(w, client_data, call_data);
}



//-------------------------------------------------------------------------
// Plot again
//-------------------------------------------------------------------------

static void ReplotCB(Widget, XtPointer client_data, XtPointer)
{
    PlotWindowInfo *plot = (PlotWindowInfo *)client_data;

    // This transfers the data once again and replots the whole thing
    plot->source->plot();
}

//-------------------------------------------------------------------------
// Command
//-------------------------------------------------------------------------

// Selection from Command widget
static void DoPlotCommandCB(Widget, XtPointer client_data, XtPointer call_data)
{
    PlotWindowInfo *plot = (PlotWindowInfo *)client_data;
    XmCommandCallbackStruct *cbs = (XmCommandCallbackStruct *)call_data;

    MString xcmd(cbs->value, true);
    string cmd = xcmd.str();

    send_and_replot(plot, cmd);
}

// Apply button
static void ApplyPlotCommandCB(Widget, XtPointer client_data, XtPointer)
{
    PlotWindowInfo *plot = (PlotWindowInfo *)client_data;

    Widget text = XmCommandGetChild(plot->command, XmDIALOG_COMMAND_TEXT);
    String cmd_s = 0;

    if (XmIsTextField(text))
	cmd_s = XmTextFieldGetString(text);
    else if (XmIsText(text))
	cmd_s = XmTextGetString(text);
    else
	assert(0);

    string cmd = cmd_s;
    XtFree(cmd_s);

    send_and_replot(plot, cmd);
}

static void EnableApplyCB(Widget, XtPointer client_data, XtPointer call_data)
{
    Widget apply = (Widget)client_data;
    XmCommandCallbackStruct *cbs = (XmCommandCallbackStruct *)call_data;

    set_sensitive(apply, cbs->length > 0);
}

static void PlotCommandCB(Widget, XtPointer client_data, XtPointer)
{
    PlotWindowInfo *plot = (PlotWindowInfo *)client_data;

    if (plot->command_dialog == 0)
    {
	Arg args[10];
	Cardinal arg = 0;
	Widget dialog = 
	    verify(XmCreatePromptDialog(plot->shell, "plot_command_dialog",
					args, arg));
	Delay::register_shell(dialog);
	plot->command_dialog = dialog;

	Widget apply = XmSelectionBoxGetChild(dialog, XmDIALOG_APPLY_BUTTON);
	XtManageChild(apply);
    
	XtUnmanageChild(XmSelectionBoxGetChild(dialog, 
					       XmDIALOG_OK_BUTTON));
	XtUnmanageChild(XmSelectionBoxGetChild(dialog, 
					       XmDIALOG_SELECTION_LABEL));
	XtUnmanageChild(XmSelectionBoxGetChild(dialog, XmDIALOG_TEXT));

	XtAddCallback(dialog, XmNapplyCallback,
		      ApplyPlotCommandCB, XtPointer(client_data));
	XtAddCallback(dialog, XmNhelpCallback,
		      ImmediateHelpCB, XtPointer(client_data));

	arg = 0;
	Widget command = 
	    verify(XmCreateCommand(dialog, "plot_command", args, arg));
	plot->command = command;
	XtManageChild(command);

	XtAddCallback(command, XmNcommandEnteredCallback, 
		      DoPlotCommandCB, XtPointer(client_data));
	XtAddCallback(command, XmNcommandChangedCallback, 
		      EnableApplyCB, XtPointer(apply));
	set_sensitive(apply, false);
    }

    manage_and_raise(plot->command_dialog);
}


//-------------------------------------------------------------------------
// Export
//-------------------------------------------------------------------------

static void SetCB(Widget, XtPointer client_data, XtPointer)
{
    bool *value = (bool *)client_data;
    *value = true;
}

static void DoExportCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    SelectPlotCB(w, client_data, call_data);

    PlotWindowInfo *plot = (PlotWindowInfo *)client_data;
    string target = get_file(w, client_data, call_data);

    const StringArray& titles  = plot->plotter->data_titles();
    const StringArray& sources = plot->plotter->data_files();

    string source = "";
    string title  = "";
    for (int i = 0; source == "" && i < sources.size(); i++)
    {
	if (sources[i] != "")
	{
	    source = sources[i];
	    title  = titles[i];
	}
    }
    
    if (source == "")
	return;			// This should not happen

    if (access(target, W_OK) == 0 && is_regular_file(target))
    {
	// File exists - request confirmation
	static Widget confirm_overwrite_dialog = 0;
	if (confirm_overwrite_dialog != 0)
	    DestroyWhenIdle(confirm_overwrite_dialog);

	Arg args[10];
	Cardinal arg = 0;
	XtSetArg(args[arg], XmNdialogStyle, 
		 XmDIALOG_FULL_APPLICATION_MODAL); arg++;
	confirm_overwrite_dialog = 
	    verify(XmCreateQuestionDialog(plot->shell,
					  "confirm_overwrite_dialog", 
					  args, arg));
	Delay::register_shell(confirm_overwrite_dialog);

	bool yes = false;
	bool no  = false;
	   
	XtAddCallback(confirm_overwrite_dialog,
		      XmNokCallback, SetCB, XtPointer(&yes));
	XtAddCallback(confirm_overwrite_dialog,
		      XmNcancelCallback, SetCB, XtPointer(&no));
	XtAddCallback(confirm_overwrite_dialog, 
		      XmNhelpCallback, ImmediateHelpCB, 0);

	MString question = rm("Overwrite existing file " 
			      + quote(target) + "?");
	XtVaSetValues (confirm_overwrite_dialog, XmNmessageString, 
		       question.xmstring(), NULL);
	manage_and_raise(confirm_overwrite_dialog);

	XtAppContext app_context = XtWidgetToApplicationContext(plot->shell);
	while (!yes && !no)
	    XtAppProcessEvent(app_context, XtIMAll);

	if (no)
	    return;
    }

    StatusDelay delay("Saving " + title + " data to " + quote(target));

    // Copy SOURCE to TARGET
    ifstream is(source);
    ofstream os(target);

    if (os.bad())
    {
	FILE *fp = fopen(target, "w");
	post_error(string("Cannot open ") 
		   + quote(target) + ": " + strerror(errno), 
		   "export_failed_error", plot->shell);
	if (fp)
	    fclose(fp);
	delay.outcome = strerror(errno);
	return;
    }

    int c;
    while ((c = is.get()) != EOF)
	os.put((unsigned char) c);

    XtUnmanageChild(plot->export_dialog);
}

static void ExportPlotCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    SelectPlotCB(w, client_data, call_data);

    PlotWindowInfo *plot = (PlotWindowInfo *)client_data;

    if (plot->export_dialog == 0)
    {
	Arg args[10];
	Cardinal arg = 0;
	Widget dialog = 
	    verify(XmCreateFileSelectionDialog(plot->shell, 
					       "export_data", args, arg));
	plot->export_dialog = dialog;

	Delay::register_shell(dialog);
	XtAddCallback(dialog, XmNokCallback, DoExportCB, client_data);
	XtAddCallback(dialog, XmNcancelCallback, UnmanageThisCB, 
		      XtPointer(dialog));
	XtAddCallback(dialog, XmNhelpCallback, ImmediateHelpCB, XtPointer(0));
    }

    manage_and_raise(plot->export_dialog);
}


//-------------------------------------------------------------------------
// Settings
//-------------------------------------------------------------------------

static void ToggleOptionCB(Widget w, XtPointer client_data, 
			   XtPointer call_data)
{
    PlotWindowInfo *plot = (PlotWindowInfo *)client_data;
    XmToggleButtonCallbackStruct *cbs = 
	(XmToggleButtonCallbackStruct *)call_data;

    string cmd;
    if (cbs->set)
	cmd = string("set ") + XtName(w);
    else
	cmd = string("set no") + XtName(w);

    send_and_replot(plot, cmd);
}

static void ToggleLogscaleCB(Widget, XtPointer client_data, 
			     XtPointer call_data)
{
    PlotWindowInfo *plot = (PlotWindowInfo *)client_data;
    XmToggleButtonCallbackStruct *cbs = 
	(XmToggleButtonCallbackStruct *)call_data;

    string cmd;
    if (cbs->set)
	cmd = "set logscale ";
    else
	cmd = "set nologscale ";

    if (plot->plotter->dimensions() >= 3)
	cmd += "z";
    else
	cmd += "y";

    send_and_replot(plot, cmd);
}

static void SetStyleCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    PlotWindowInfo *plot = (PlotWindowInfo *)client_data;
    XmToggleButtonCallbackStruct *cbs = 
	(XmToggleButtonCallbackStruct *)call_data;

    if (cbs->set)
    {
	string style = XtName(w);
	string cmd;
	if (style.contains("3d", -1))
	{
	    cmd = "set hidden3d\n";
	    style = style.before("3d");
	}
	else
	{
	    cmd = "set nohidden3d\n";
	}
	if (style.contains("2d", -1))
	    style = style.before("2d");
	
	cmd += "set data style " + style;

	send_and_replot(plot, cmd);
    }
}

static void SetContourCB(Widget w, XtPointer client_data, XtPointer)
{
    PlotWindowInfo *plot = (PlotWindowInfo *)client_data;

    Widget base    = XtNameToWidget(XtParent(w), "base");
    Widget surface = XtNameToWidget(XtParent(w), "surface");

    assert (base != 0 && surface != 0);

    bool base_set    = XmToggleButtonGetState(base);
    bool surface_set = XmToggleButtonGetState(surface);

    string cmd;
    if (base_set && surface_set)
	cmd = "set contour both";
    else if (base_set && !surface_set)
	cmd = "set contour base";
    else if (!base_set && surface_set)
	cmd = "set contour surface";
    else
	cmd = "set nocontour";

    send_and_replot(plot, cmd);
}

static void SetViewCB(Widget, XtPointer client_data, XtPointer)
{
    PlotWindowInfo *plot = (PlotWindowInfo *)client_data;

    int rot_x = 60;
    int rot_z = 30;

    XtVaGetValues(plot->vsb, XmNvalue, &rot_x, NULL);
    XtVaGetValues(plot->hsb, XmNvalue, &rot_z, NULL);

    string cmd = 
	"set view " + itostring(rot_x) + ", " + itostring(rot_z);

    send_and_replot(plot, cmd);
}

//-------------------------------------------------------------------------
// Status line
//-------------------------------------------------------------------------

// Forward Gnuplot error messages to DDD status line
static void SetStatusHP(Agent *, void *client_data, void *call_data)
{
    PlotWindowInfo *plot = (PlotWindowInfo *)client_data;
    DataLength* dl = (DataLength *) call_data;
    string s(dl->data, dl->length);

    (void) plot;		// Use it
#if 0
    if (!plot->active)
    {
	// Probably an invocation problem
	post_gdb_message(s);
	return;
    }
#endif

    if (plot->command != 0)
    {
	string msg = s;
	strip_space(msg);
	MString xmsg = tb(msg);
	XmCommandError(plot->command, xmsg.xmstring());
    }

    while (s != "")
    {
	string line;
	if (s.contains('\n'))
	    line = s.before('\n');
	else
	    line = s;
	s = s.after('\n');
	strip_space(line);

	if (line != "")
	    set_status(line);
    }
}

//-------------------------------------------------------------------------
// Trace communication
//-------------------------------------------------------------------------

static void trace(char *prefix, void *call_data)
{
    DataLength* dl = (DataLength *) call_data;
    string s(dl->data, dl->length);

    bool s_ends_with_nl = false;
    if (s.length() > 0 && s[s.length() - 1] == '\n')
    {
	s_ends_with_nl = true;
	s = s.before(int(s.length() - 1));
    }

    s = quote(s);
    string nl = string("\\n\"\n") + replicate(' ', strlen(prefix)) + "\"";
    s.gsub("\\n", nl);

    if (s_ends_with_nl)
	s(s.length() - 1, 0) = "\\n";

    dddlog << prefix << s << '\n';
    dddlog.flush();
}

static void TraceInputHP(Agent *, void *, void *call_data)
{
    trace("<< ", call_data);
}

static void TraceOutputHP(Agent *, void *, void *call_data)
{
    trace(">> ", call_data);
}

static void TraceErrorHP(Agent *, void *, void *call_data)
{
    trace("<= ", call_data);
}
