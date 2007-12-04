// High-level GUI wrapper for Xmmm.

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

#include <Xm/ToggleB.h>

#include <Xmmm/CheckMenuItem.h>

using namespace Xmmm;

void
CheckMenuItem::init_signals(void)
{
    XtAddCallback(checkmenuitem_, XmNactivateCallback, 
		  (XtCallbackProc)CheckMenuItem::activate_callback,
		  XtPointer(this));
}

CheckMenuItem::CheckMenuItem(::Widget parent, const Xmmm::String &name,
			     const Xmmm::String &label)
{
    checkmenuitem_ = XmCreateToggleButton(parent, (char *)name.c(), NULL, 0);
    init_signals();
    postinit();
}

CheckMenuItem::CheckMenuItem(Xmmm::Container &parent, const Xmmm::String &name,
			     const Xmmm::String &label)
{
    checkmenuitem_ = XmCreateToggleButton(parent.xt_container(), (char *)name.c(), NULL, 0);
    init_signals();
    postinit();
}

CheckMenuItem::~CheckMenuItem(void)
{
    XtDestroyWidget(checkmenuitem_);
}

::Widget CheckMenuItem::internal(void)
{
    return checkmenuitem_;
}

sigc::signal<void> &
CheckMenuItem::signal_clicked(void)
{
    return signal_clicked_;
}

void
CheckMenuItem::activate_callback(::Widget widget, XtPointer data)
{
    ((CheckMenuItem *)data)->signal_clicked_();
}

