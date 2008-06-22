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

#include <GtkX/CheckButton.h>
#include <GtkX/Container.h>

#include <gtk/gtkcheckbutton.h>

using namespace GtkX;

CheckButton::CheckButton(GtkX::Container &parent, PackOptions po,
			 const GtkX::String &name, const GtkX::String &label)
{
    check_ = new Gtk::CheckButton(mklabel(name, label).s());
    set_name(name.s());
    parent.add_child(*this, po, 0);
    postinit();
}

CheckButton::~CheckButton(void)
{
    delete check_;
}

Gtk::Widget *
CheckButton::internal(void)
{
    return check_;
}

const Gtk::Widget *
CheckButton::internal(void) const
{
    return check_;
}

void
CheckButton::init_signals(void)
{
    check_->signal_toggled().connect(sigc::mem_fun(*this, &Bipolar::toggled_callback));
}

bool
CheckButton::get_active()
{
    return check_->get_active();
}

void
CheckButton::set_active(bool new_state, bool notify)
{
    if (check_->get_active() != new_state)
    {
	if (notify) {
	    check_->set_active(new_state);
	}
	else {
	    GtkCheckButton *cb_obj = check_->gobj();
	    cb_obj->toggle_button.active = !cb_obj->toggle_button.active;
	    gtk_widget_queue_draw(GTK_WIDGET(cb_obj));
	}
    }
}

