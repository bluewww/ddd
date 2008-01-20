// High-level GUI wrapper for Motif.

// Copyright (C) 2007 Peter Wainwright <prw@ceiriog.eclipse.co.uk>
// 
// This file is part of Xmmm.
// 
// Xmmm is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// Xmmm is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with Xmmm -- see the file COPYING.
// If not, write to the Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

// High level object-oriented wrapper must support Gtk and Motif.
// Unfortunately Motif widgets require parent and name arguments to
// the constructor, unlike the Gtk ones.  Motif (Xt) widgets cannot be
// reparented.  Therefore we need a constructor with extra arguments.

#include <string>
#include <iostream>

#include <Xmmm/Window.h>
#include <Xm/DialogS.h>
#include <Xm/RowColumn.h>
#include <Xm/MwmUtil.h>
#include <Xm/AtomMgr.h>
#include <Xm/Protocols.h>

using namespace Xmmm;

void
Xmmm::Window::init_signals(void)
{
    static Atom WM_DELETE_WINDOW =
	XmInternAtom(XtDisplay(win_), "WM_DELETE_WINDOW", False);

    // We don't invoke XmAddWMProtocols directly, because this LessTif macro
    // causes a compilation warning with `gcc -Wwrite-strings'.

    static Atom WM_PROTOCOLS = 
	XmInternAtom(XtDisplay(win_), "WM_PROTOCOLS", FALSE);
    
    XmAddProtocolCallback(win_, WM_PROTOCOLS, WM_DELETE_WINDOW, 
			  (XtCallbackProc)Window::delete_callback,
			  XtPointer(this));
}

void
Xmmm::Window::init(::Widget parent, const Xmmm::String &name,
		   int argc, char **argv)
{
    Arg args[10];
    int nargs;

    nargs = 0;
    XtSetArg(args[nargs], XmNdeleteResponse, XmDO_NOTHING); nargs++;
    XtSetArg(args[nargs], XmNargc,           argc); nargs++;
    XtSetArg(args[nargs], XmNargv,           argv); nargs++;

    // Forward toplevel geometry (as given by `-geometry') to command shell
    ::String toplevel_geometry = 0;
    XtVaGetValues(parent, XmNgeometry, &toplevel_geometry, NULL);
    if (toplevel_geometry != 0)
    {
	XtSetArg(args[nargs], XmNgeometry, toplevel_geometry); nargs++;
    }

    // Make command shell a popup shell.  
    // The toplevel window is never realized.
    win_ = XtCreatePopupShell(name.c(),
			      applicationShellWidgetClass,
			      parent, args, nargs);

    init_signals();
    postinit();
}

Xmmm::Window::Window(::Widget parent, const Xmmm::String &name,
		     int argc, char **argv)
{
    init(parent, name, argc, argv);
}

Xmmm::Window::Window(Xmmm::Widget &parent, const Xmmm::String &name,
		     int argc, char **argv)
{
    init(parent.internal(), name, argc, argv);
}

Xmmm::Window::~Window(void)
{
    XtDestroyWidget(win_);
}

::Widget Xmmm::Window::internal(void)
{
    return win_;
}

sigc::signal<bool,XEvent*> &
Xmmm::Window::signal_delete_event(void)
{
    return signal_delete_;
}

void
Xmmm::Window::delete_callback(::Widget widget, XtPointer data)
{
    ((Window *)data)->signal_delete_((XEvent*)0);
}

