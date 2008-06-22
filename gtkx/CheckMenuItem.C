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

#include <GtkX/CheckMenuItem.h>
#include <GtkX/Container.h>

#include <gtk/gtkcheckmenuitem.h>

using namespace GtkX;

CheckMenuItem::CheckMenuItem(GtkX::Container &parent, PackOptions po,
			     const GtkX::String &name, const GtkX::String &label)
{
    check_ = new Gtk::CheckMenuItem(mklabel(name, label).s());
    set_name(name.s());
    parent.add_child(*this, po, 0);
    postinit();
}

CheckMenuItem::~CheckMenuItem(void)
{
    delete check_;
}

Gtk::Widget *
CheckMenuItem::internal(void)
{
    return check_;
}

const Gtk::Widget *
CheckMenuItem::internal(void) const
{
    return check_;
}

void
CheckMenuItem::init_signals(void)
{
    check_->signal_toggled().connect(sigc::mem_fun(*this, &Bipolar::toggled_callback));
}

bool
CheckMenuItem::get_active()
{
    return check_->get_active();
}

void
CheckMenuItem::set_active(bool new_state, bool notify)
{
    if (check_->get_active() != new_state)
    {
	if (notify) {
	    set_active(new_state);
	}
	else {
	    GtkCheckMenuItem *cmi_obj = check_->gobj();
	    cmi_obj->active = !cmi_obj->active;
	    gtk_widget_queue_draw(GTK_WIDGET(cmi_obj));
	}
    }
}

