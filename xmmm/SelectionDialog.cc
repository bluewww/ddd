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

#include <Xmmm/SelectionDialog.h>
#include <Xm/DialogS.h>
#include <Xm/RowColumn.h>
#include <Xm/MwmUtil.h>

using namespace Xmmm;

static void
close_shell(::Widget w, XtPointer client_data, XtPointer call_data)
{
    XtPopdown(w);
}

SelectionDialog::SelectionDialog(::Widget parent, const Xmmm::String &name,
				 const std::vector<String> &headers)
{
    Arg args[10];
    int nargs;
    std::vector<Arg> vargs;

    int decorations = 
	MWM_DECOR_BORDER | MWM_DECOR_RESIZEH | 
	MWM_DECOR_TITLE | MWM_DECOR_MENU | 
	MWM_DECOR_MINIMIZE;
    int functions = 
	MWM_FUNC_RESIZE | MWM_FUNC_MOVE | MWM_FUNC_MINIMIZE | MWM_FUNC_CLOSE;

    nargs = 0;
    XtSetArg(args[nargs], XmNdialogType, XmDIALOG_SELECTION); nargs++; // ??
    XtSetArg(args[nargs], XmNallowShellResize, True); nargs++;
    XtSetArg(args[nargs], XmNmwmDecorations, decorations); nargs++;
    XtSetArg(args[nargs], XmNmwmFunctions, functions); nargs++;
    XtSetArg(args[nargs], XmNautoUnmanage, False); nargs++;
    // Suppress the normal response to the WM delete button.
    XtSetArg(args[nargs], XmNdeleteResponse, XmDO_NOTHING); nargs++;
    // Standard Motif maps/unmaps the child to popup the shell.
    // We want to take control instead.
    XtSetArg(args[nargs], XmNmappedWhenManaged, False); nargs++;

    dlg_ = XmCreateDialogShell(parent, (char *)name.c(), args, nargs);
    WM_set_close_callback(dlg_, close_shell, NULL);
#if 0
    box_ = new VBox(dlg_, name+Xmmm::String("_vbox"));
    box_->show();
#else
    box1_ = new Box1(dlg_, name+Xmmm::String("_vbox"));
    box1_->show();
#endif
    vargs.resize(6);
    XtSetArg(vargs[0], XmNscrollBarDisplayPolicy, XmAS_NEEDED);
    // FIXME: Lesstif does not apparently allow separate setting of
    // XmNvisualPolicy: it is always set according to the value of
    // XmNscrollingPolicy.  G*d, I hate Motif...
    //XtSetArg(vargs[1], XmNscrollingPolicy, XmAUTOMATIC);
    //XtSetArg(vargs[2], XmNvisualPolicy, XmCONSTANT);
    XtSetArg(vargs[1], XmNscrollingPolicy, XmAPPLICATION_DEFINED);
    XtSetArg(vargs[2], XmNvisualPolicy, XmVARIABLE);
    XtSetArg(vargs[3], XmNscrollBarDisplayPolicy, XmSTATIC);
    XtSetArg(vargs[4], XmNshadowThickness, 0);
    XtSetArg(vargs[5], "pack_options", 2);
    sw_ = new ScrolledWindow(*box1_, name+Xmmm::String("_sw"), vargs);
    list_ = new ListView(*sw_, name+Xmmm::String("_list"), headers);
    // FIXME: If the ScrolledWindow is realized before the list is
    // managed, it will resize the list (so that the visibleItemCount
    // resource is ignored).  This will not matter when we have proper
    // geometry management for the Box1 widget.
    list_->show();
    sw_->show();
    postinit();
}

SelectionDialog::~SelectionDialog(void)
{
    if (box_) delete box_;
    if (box1_) delete box1_;
    delete list_;
    XtDestroyWidget(dlg_);
}

::Widget SelectionDialog::internal(void)
{
    return dlg_;
}

::Widget SelectionDialog::xt_container(void)
{
    return box1_->internal();
}

ListView *
SelectionDialog::list(void) const
{
    return list_;
}

Button *
SelectionDialog::add_button(const String &name)
{
    return new Button(*this, name);
}

