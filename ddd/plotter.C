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

#include "charsets.h"
#include "cook.h"
#include "ddd.h"
#include "exit.h"
#include "findParent.h"
#include "findWindow.h"
#include "fonts.h"
#include "simpleMenu.h"
#include "verify.h"
#include "strclass.h"
#include "version.h"
#include "wm.h"
#include "AppData.h"
#include "Command.h"
#include "Delay.h"
#include "DestroyCB.h"
#include "HelpCB.h"
#include "MakeMenu.h"
#include "Swallower.h"

#include <Xm/MainW.h>
#include <Xm/MessageB.h>

#define PLOT_CLASS_NAME "Gnuplot"

static void TraceInputHP (Agent *source, void *, void *call_data);
static void TraceOutputHP(Agent *source, void *, void *call_data);
static void TraceErrorHP (Agent *source, void *, void *call_data);

static void CloseCB(Widget, XtPointer, XtPointer);

static MMDesc file_menu[] = 
{
    { "close", MMPush, { CloseCB } },
    { "exit",  MMPush, { DDDExitCB, XtPointer(EXIT_SUCCESS) }},
    MMEnd
};

static MMDesc menubar[] = 
{
    { "file",     MMMenu, MMNoCB, file_menu },
    { "edit",     MMMenu, MMNoCB, simple_edit_menu },
    { "help",     MMMenu | MMHelp, MMNoCB, simple_help_menu },
    MMEnd
};

struct PlotWindowInfo {
    Widget shell;
    Widget dialog;
};

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
	XtUnmanageChild(plot->dialog);

	XtVaSetValues(swallower, XtNwindow, window, NULL);
	XtManageChild(plot->shell);
	XtRealizeWidget(plot->shell);
	XtRemoveCallback(swallower, XtNwindowCreatedCallback, 
			 SwallowCB, client_data);

	delete plot;
    }
}

// GNUPLOT subwindow has gone - kill shell, too
static void GoneCB(Widget, XtPointer client_data, XtPointer)
{
    Widget shell = Widget(client_data);
    DestroyWhenIdle(shell);
}

// Swallower is being destroyed - kill agent, too
static void KillAgentCB(Widget /* swallower */, 
			XtPointer client_data, XtPointer)
{
    PlotAgent *plotter = (PlotAgent *)client_data;
    delete plotter;
}

// Close action from menu
static void CloseCB(Widget w, XtPointer, XtPointer)
{
    Widget shell = findTopLevelShellParent(w);
    DestroyWhenIdle(shell);
}

// Create a new plot window
PlotAgent *new_plotter(string name)
{
    Arg args[10];
    Cardinal arg;

    string cmd = app_data.plot_command;
    cmd.gsub("@FONT@", make_font(app_data, FixedWidthDDDFont));

    // Pop up a working dialog
    static Widget dialog = 0;
    if (dialog == 0)
    {
	arg = 0;
	dialog = verify(XmCreateWorkingDialog(find_shell(),
					      "launch_plot_dialog", 
					      args, arg));
	XtUnmanageChild(XmMessageBoxGetChild(dialog,
					     XmDIALOG_OK_BUTTON));
	XtUnmanageChild(XmMessageBoxGetChild(dialog,
					     XmDIALOG_HELP_BUTTON));
    }

    string base = cmd;
    if (base.contains(' '))
	base = cmd.before(' ');
    MString msg = rm("Starting ") + tt(base) + rm("...");
    XtVaSetValues(dialog, XmNmessageString, msg.xmstring(), NULL);
    manage_and_raise(dialog);


    // Create control window
    string title = DDD_NAME ": " + name;
    arg = 0;
    XtSetArg(args[arg], XmNtitle,    title.chars());   arg++;
    XtSetArg(args[arg], XmNiconName, title.chars());   arg++;
    XtSetArg(args[arg], XmNallowShellResize, True);    arg++;
    XtSetArg(args[arg], XmNdeleteResponse, XmDESTROY); arg++;
    Widget shell = verify(XtCreateWidget("plot",
					 topLevelShellWidgetClass,
					 find_shell(), args, arg));

    arg = 0;
    Widget main_window = XmCreateMainWindow(shell, "main_window", args, arg);
    XtManageChild(main_window);

    /* Widget menubar_w = */ MMcreateMenuBar(main_window, "menubar", menubar);
    MMaddCallbacks(menubar, XtPointer(shell));
    MMaddHelpCallback(menubar, ImmediateHelpCB);

    arg = 0;
    Widget swallower = 
	XtCreateManagedWidget("plot", swallowerWidgetClass, 
			      main_window, args, arg);

    PlotWindowInfo *plot = new PlotWindowInfo;
    plot->shell  = shell;
    plot->dialog = dialog;

    XtAddCallback(swallower, XtNwindowCreatedCallback, 
		  SwallowCB, XtPointer(plot));
    XtAddCallback(swallower, XtNwindowGoneCallback, 
		  GoneCB, XtPointer(shell));
    Delay::register_shell(shell);

    XtRemoveAllCallbacks(dialog, XmNcancelCallback);
    XtAddCallback(dialog, XmNcancelCallback, DestroyThisCB, XtPointer(shell));

    // Invoke plot process
    PlotAgent *plotter = 
	new PlotAgent(XtWidgetToApplicationContext(shell), cmd);

    XtAddCallback(swallower, XtNdestroyCallback, 
		  KillAgentCB, XtPointer(plotter));

    string init = app_data.plot_init_commands;
    if (init != "" && !init.contains('\n', -1))
	init += '\n';

    // Add trace handlers
    plotter->addHandler(Input,  TraceInputHP);     // Gnuplot => DDD
    plotter->addHandler(Output, TraceOutputHP);    // DDD => Gnuplot
    plotter->addHandler(Error,  TraceErrorHP);     // Gnuplot Errors => DDD

    plotter->start(init);

    return plotter;
}


// Trace communication
static void trace(char *prefix, void *call_data)
{
    DataLength* dl    = (DataLength *) call_data;
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
