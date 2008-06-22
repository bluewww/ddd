// High-level GUI wrapper for Gtkmm.

// Copyright (C) 2007 Peter Wainwright <prw@ceiriog.eclipse.co.uk>
// 
// This file is part of GtkX.
// 
// GtkX is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
// 
// GtkX is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with GtkX -- see the file COPYING.
// If not, see <http://www.gnu.org/licenses/>.

// High level object-oriented wrapper must support Gtk and Motif.
// Unfortunately Motif widgets require parent and name arguments to
// the constructor, unlike the Gtk ones.  Motif (Xt) widgets cannot be
// reparented.  Therefore we need a constructor with extra arguments.

#include <iostream>

#include <GtkX/RadioMenuItem.h>
#include <GtkX/Container.h>

#include <gtk/gtkradiomenuitem.h>

using namespace GtkX;

RadioMenuItem::RadioMenuItem(GtkX::Container &parent, PackOptions po,
			     const GtkX::String &name, const GtkX::String &label)
{
    radio_ = new Gtk::RadioMenuItem(parent.button_group(), mklabel(name, label).s());
    set_name(name.s());
    parent.add_child(*this, po, 0);
    postinit();
}

RadioMenuItem::~RadioMenuItem(void)
{
    delete radio_;
}

Gtk::Widget *
RadioMenuItem::internal(void)
{
    return radio_;
}

const Gtk::Widget *
RadioMenuItem::internal(void) const
{
    return radio_;
}

void
RadioMenuItem::init_signals(void)
{
    radio_->signal_toggled().connect(sigc::mem_fun(*this, &Bipolar::toggled_callback));
}

bool
RadioMenuItem::get_active()
{
    return radio_->get_active();
}

void
RadioMenuItem::set_active(bool new_state, bool notify)
{
    if (radio_->get_active() != new_state)
    {
	if (notify) {
	    set_active(new_state);
	}
	else {
	    std::cerr << "ERROR: Setting radio menu item active!\n";
#if 0
	    GtkRadioMenuItem *rmi_obj = gobj();
	    rmi_obj->active = !rmi_obj->active;
	    gtk_widget_queue_draw(GTK_WIDGET(rmi_obj));
#endif
	}
    }
}

