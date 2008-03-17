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

#include <Xmmm/RadioButton.h>

using namespace Xmmm;

void
RadioButton::init_signals(void)
{
    XtAddCallback(button_, XmNvalueChangedCallback, 
		  (XtCallbackProc)RadioButton::toggled_callback,
		  XtPointer(this));
}

RadioButton::RadioButton(::Widget parent, const Xmmm::String &name,
			 const Xmmm::String &label)
{
    button_ = XmCreateToggleButton(parent, (char *)name.c(), NULL, 0);
    init_signals();
    postinit();
}

RadioButton::RadioButton(Xmmm::Container &parent, const Xmmm::String &name,
			 const Xmmm::String &label)
{
    button_ = XmCreateToggleButton(parent.xt_container(), (char *)name.c(), NULL, 0);
    init_signals();
    postinit();
}

RadioButton::~RadioButton(void)
{
    XtDestroyWidget(button_);
}

::Widget RadioButton::internal(void)
{
    return button_;
}

void
RadioButton::set_active(bool on, bool notify)
{
    if (notify)
	XmToggleButtonSetState(button_, on, True);
    else
	XtVaSetValues(button_, XmNset, on, 0);
}

bool
RadioButton::get_active(void)
{
    Boolean on;
    XtVaGetValues(button_, XmNset, &on, 0);
    return on;
}

sigc::signal<void> &
RadioButton::signal_toggled(void)
{
    return signal_toggled_;
}

void
RadioButton::toggled_callback(::Widget widget, XtPointer data)
{
    ((RadioButton *)data)->signal_toggled_();
}

