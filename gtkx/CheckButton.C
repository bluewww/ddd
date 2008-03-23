// High-level GUI wrapper for Gtkmm.

// Copyright (C) 2007 Peter Wainwright <prw@ceiriog.eclipse.co.uk>
// 
// This file is part of GtkX.
// 
// GtkX is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// GtkX is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with GtkX -- see the file COPYING.
// If not, write to the Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

// High level object-oriented wrapper must support Gtk and Motif.
// Unfortunately Motif widgets require parent and name arguments to
// the constructor, unlike the Gtk ones.  Motif (Xt) widgets cannot be
// reparented.  Therefore we need a constructor with extra arguments.

#include <GtkX/CheckButton.h>
#include <GtkX/Container.h>

#include <gtk/gtkcheckbutton.h>

using namespace GtkX;

static const GtkX::String mklabel(const GtkX::String &name,
				  const GtkX::String &label)
{
    if (label.s().length() > 0)
	return label;
    return name;
}

CheckButton::CheckButton(GtkX::Container &parent, const GtkX::String &name,
			 const GtkX::String &label):
    Gtk::CheckButton(mklabel(name, label).s())
{
    set_name(name.s());
    parent.add_child(*this);
    postinit();
}

CheckButton::~CheckButton(void)
{
}

Gtk::Widget *
CheckButton::internal(void)
{
    return this;
}

const Gtk::Widget *
CheckButton::internal(void) const
{
    return this;
}


bool
CheckButton::get_active()
{
    return Gtk::CheckButton::get_active();
}

void
CheckButton::set_active(bool new_state, bool notify)
{
    if (Gtk::CheckButton::get_active() != new_state)
    {
	if (notify) {
	    set_active(new_state);
	}
	else {
	    GtkCheckButton *cb_obj = gobj();
	    cb_obj->toggle_button.active = !cb_obj->toggle_button.active;
	    gtk_widget_queue_draw(GTK_WIDGET(cb_obj));
	}
    }
}

