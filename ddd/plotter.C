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
#include "filetype.h"
#include "fonts.h"
#include "print.h"
#include "simpleMenu.h"
#include "verify.h"
#include "status.h"
#include "strclass.h"
#include "string-fun.h"
#include "version.h"
#include "wm.h"
#include "AppData.h"
#include "Command.h"
#include "Delay.h"
#include "HelpCB.h"
#include "MakeMenu.h"
#include "PlotArea.h"
#include "Swallower.h"

#include <stdio.h>
#include <fstream.h>

#include <Xm/MainW.h>
#include <Xm/MessageB.h>
#include <Xm/AtomMgr.h>
#include <Xm/Protocols.h>
#include <Xm/DrawingA.h>
#include <Xm/ScrolledW.h>
#include <Xm/ScrollBar.h>
#include <Xm/ToggleB.h>

#define PLOT_CLASS_NAME "Gnuplot"

static void TraceInputHP (Agent *source, void *, void *call_data);
static void TraceOutputHP(Agent *source, void *, void *call_data);
static void TraceErrorHP (Agent *source, void *, void *call_data);
static void SetStatusHP  (Agent *source, void *, void *call_data);

static void CancelPlotCB(Widget, XtPointer, XtPointer);
static void ExposePlotAreaCB(Widget, XtPointer, XtPointer);
static void ResizePlotAreaCB(Widget, XtPointer, XtPointer);

static void ToggleOptionCB(Widget, XtPointer, XtPointer);
static void ToggleLogscaleCB(Widget, XtPointer, XtPointer);
static void SetStyleCB(Widget, XtPointer, XtPointer);
static void SetContourCB(Widget, XtPointer, XtPointer);
static void SetViewCB(Widget, XtPointer, XtPointer);


struct PlotWindowInfo {
    PlotAgent *plotter;		// The current Gnuplot instance
    PlotArea *area;		// The area we're drawing in
    Widget shell;		// The shell we're in
    Widget dialog;		// The dialog
    Widget swallower;		// The Gnuplot window
    Widget vsb;			// Vertical scroll bar
    Widget hsb;			// Horizontal scroll bar
    bool active;		// True if popped up

    // Constructor - just initialize
    PlotWindowInfo()
	: plotter(0), area(0), shell(0), dialog(0), swallower(0),
	  vsb(0), hsb(0), active(false)
    {}
};


//-------------------------------------------------------------------------
// Menus
//-------------------------------------------------------------------------

static MMDesc file_menu[] = 
{
    { "print", MMPush, { PrintPlotCB, 0 }, 0, 0, 0, 0 },
    MMSep,
    { "close", MMPush, { CancelPlotCB, 0 }, 0, 0, 0, 0 },
    { "exit",  MMPush, { DDDExitCB, XtPointer(EXIT_SUCCESS) }, 0, 0, 0, 0},
    MMEnd
};

static MMDesc view_menu[] = 
{
    { "border",    MMToggle, { ToggleOptionCB, 0 }, 0, 0, 0, 0 },
    { "grid",      MMToggle, { ToggleOptionCB, 0 }, 0, 0, 0, 0 },
    { "time",      MMToggle, { ToggleOptionCB, 0 }, 0, 0, 0, 0 },
    MMSep,
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
    plot->plotter->write(cmd.chars(), cmd.length());

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
}




//-------------------------------------------------------------------------
// Decoration stuff
//-------------------------------------------------------------------------

// Swallow new GNUPLOT window
static void SwallowCB(Widget swallower, XtPointer client_data, 
		      XtPointer call_data)
{
    PlotWindowInfo *plot = (PlotWindowInfo *)client_data;
    SwallowerInfo *info = (SwallowerInfo *)call_data;
    Window window = 
	findWindow(XtDisplay(swallower), info->window, PLOT_CLASS_NAME);

    if (window != None)
    {
	// We have the window
	if (plot->dialog != 0)
	{
	    XtUnmanageChild(plot->dialog);
	    XtPopdown(XtParent(plot->dialog));
	}

	XtVaSetValues(swallower, XtNwindow, window, NULL);

	if (!plot->active && plot->plotter != 0)
	{
	    configure_plot(plot);

	    XtPopup(plot->shell, XtGrabNone);
	    plot->active = true;
	}

	XtRemoveCallback(swallower, XtNwindowCreatedCallback, 
			 SwallowCB, client_data);
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

    if (plot->shell == 0)
	return;

    entered = true;

    // Pop down shell
    XtPopdown(plot->shell);
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

    PlotWindowInfo *plot = (PlotWindowInfo *)client_data;
    assert(plot->plotter == plotter);
    plot->plotter = 0;
    popdown_plot_shell(plot);
}

static void GetPlotHP(Agent *, void *client_data, void *call_data)
{
    PlotWindowInfo *plot = (PlotWindowInfo *)client_data;

    // We got the plot commands
    XtUnmanageChild(plot->dialog);
    XtPopdown(XtParent(plot->dialog));

    if (!plot->active && plot->plotter != 0)
    {
	// Setup menu
	configure_plot(plot);

	XtPopup(plot->shell, XtGrabNone);
	plot->active = true;
    }

    // Pass the received commands to the plot area
    DataLength *dl = (DataLength *)call_data;
    plot->area->plot(string(dl->data, dl->length));
}


#define SWALLOWER_NAME "swallower"
#define PLOT_AREA_NAME "area"

static PlotWindowInfo *new_decoration(const string& name)
{
    static VoidArray infos;

    PlotWindowInfo *plot = 0;

    // Check whether we can reuse an existing decoration
    for (int i = 0; i < infos.size(); i++)
    {
	PlotWindowInfo *info = (PlotWindowInfo *)infos[i];
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
	if (app_data.builtin_plot)
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

	infos += plot;
    }

    string title = DDD_NAME ": " + name;
    XtVaSetValues(plot->shell,
		  XmNtitle, title.chars(),
		  XmNiconName, title.chars(),
		  NULL);

    if (app_data.builtin_plot)
    {
	// Nothing to do
    }
    else
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

// Create a new plot window
PlotAgent *new_plotter(string name)
{
    string cmd = app_data.plot_command;
    cmd.gsub("@FONT@", make_font(app_data, FixedWidthDDDFont));

    // Create shell
    PlotWindowInfo *plot = new_decoration(name);
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
    plot->dialog = dialog;

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
    plotter->addHandler(Error,  SetStatusHP);      // Gnuplot Errors => status
    plotter->addHandler(Died,   DeletePlotterHP, (void *)plot);  // Free memory

    if (plot->area != 0)
	plotter->addHandler(Plot, GetPlotHP, (void *)plot);

    plotter->start(init);
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
	cmd = string("set ") + XtName(w) + "\n";
    else
	cmd = string("set no") + XtName(w) + "\n";
    cmd += "replot\n";

    plot->plotter->write(cmd.chars(), cmd.length());
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
	cmd += "z\n";
    else
	cmd += "y\n";

    plot->plotter->write(cmd.chars(), cmd.length());

    cmd = "replot\n";
    plot->plotter->write(cmd.chars(), cmd.length());
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
	
	cmd += "set data style " + style + "\n";
	cmd += "replot\n";

	plot->plotter->write(cmd.chars(), cmd.length());
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
	cmd = "set contour both\n";
    else if (base_set && !surface_set)
	cmd = "set contour base\n";
    else if (!base_set && surface_set)
	cmd = "set contour surface\n";
    else
	cmd = "set nocontour\n";
    cmd += "replot\n";

    plot->plotter->write(cmd.chars(), cmd.length());
}

static void SetViewCB(Widget, XtPointer client_data, XtPointer)
{
    PlotWindowInfo *plot = (PlotWindowInfo *)client_data;

    int rot_x = 60;
    int rot_z = 30;

    XtVaGetValues(plot->vsb, XmNvalue, &rot_x, NULL);
    XtVaGetValues(plot->hsb, XmNvalue, &rot_z, NULL);

    string cmd = 
	"set view " + itostring(rot_x) + ", " + itostring(rot_z) + "\n";
    cmd += "replot\n";

    plot->plotter->write(cmd.chars(), cmd.length());
}

//-------------------------------------------------------------------------
// Status line
//-------------------------------------------------------------------------

// Forward Gnuplot error messages to DDD status line
static void SetStatusHP(Agent *, void *, void *call_data)
{
    DataLength* dl = (DataLength *) call_data;
    string s(dl->data, dl->length);

    while (s != "")
    {
	string line;
	if (s.contains('\n'))
	    line = s.before('\n');
	else
	    line = s;
	s = s.after('\n');

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
