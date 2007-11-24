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

#include <Xmmm/Dialog.h>
#include <Xm/DialogS.h>
#include <Xm/AtomMgr.h>
#include <Xm/Protocols.h>

using namespace Xmmm;

static void
close_shell(::Widget w, XtPointer client_data, XtPointer call_data)
{
    XtPopdown(w);
}

void
Dialog::init_signals(void)
{
    static Atom WM_DELETE_WINDOW =
	XmInternAtom(XtDisplay(dlg_), "WM_DELETE_WINDOW", False);

    // We don't invoke XmAddWMProtocols directly, because this LessTif macro
    // causes a compilation warning with `gcc -Wwrite-strings'.

    static Atom WM_PROTOCOLS = 
	XmInternAtom(XtDisplay(dlg_), "WM_PROTOCOLS", FALSE);
    
    XmAddProtocolCallback(dlg_, WM_PROTOCOLS, WM_DELETE_WINDOW, 
			  (XtCallbackProc)Dialog::delete_callback,
			  XtPointer(this));

    XtAddCallback(dlg_, XmNunmapCallback,
		  (XtCallbackProc)Dialog::unmap_callback,
		  XtPointer(0));
}

Dialog::Dialog(::Widget parent, const Xmmm::String &name)
{
    Arg args[10];
    int nargs;

    // FIXME: Allow override.
    nargs = 0;
    XtSetArg(args[nargs], XtNallowShellResize, True); nargs++;
     // Standard Motif maps/unmaps the child to popup the shell.
    // We want to take control instead.
    XtSetArg(args[nargs], XmNmappedWhenManaged, False); nargs++;
    dlg_ = XmCreateDialogShell(parent, (char *)name.c(), args, nargs);
    WM_set_close_callback(dlg_, close_shell, NULL);

    box0_ = new VBox(dlg_, name+Xmmm::String("_x"));
    box0_->show();
    box1_ = new VBox(*box0_, name+Xmmm::String("_vbox"));
    box1_->show();
    box2_ = new HBox(*box0_, name+Xmmm::String("_hbox"));
    box2_->show();

    init_signals();
    postinit();
}

Dialog::~Dialog(void)
{
    delete box1_;
    XtDestroyWidget(dlg_);
}

::Widget Dialog::internal(void)
{
    return dlg_;
}

::Widget Dialog::xt_container(void)
{
    return box1_->internal();
}

sigc::signal<bool,XEvent*> &
Dialog::signal_delete_event(void)
{
    return signal_delete_;
}

sigc::signal<void> &
Dialog::signal_unmap(void)
{
    return signal_unmap_;
}

void
Dialog::delete_callback(::Widget widget, XtPointer data)
{
    ((Dialog *)data)->signal_delete_((XEvent*)0);
}

void
Dialog::unmap_callback(::Widget widget, XtPointer data)
{
    ((Dialog *)data)->signal_unmap_();
}

Button *
Dialog::add_button(const Xmmm::String& button_text)
{
    return new Button(*box2_, button_text);
}

