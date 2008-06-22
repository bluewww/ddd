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

#include <GtkX/RadioButton.h>
#include <GtkX/Container.h>
#include <gtk/gtkradiobutton.h>

using namespace GtkX;

RadioButton::RadioButton(GtkX::Container &parent, PackOptions po,
			 const GtkX::String &name, const GtkX::String &label)
{
    radio_ = new Gtk::RadioButton(mklabel(name, label).s());
    set_name(name.s());
    parent.add_child(*this, po, 0);
    postinit();
}

RadioButton::~RadioButton(void)
{
    delete radio_;
}

Gtk::Widget *
RadioButton::internal(void)
{
    return radio_;
}

const Gtk::Widget *
RadioButton::internal(void) const
{
    return radio_;
}

void
RadioButton::init_signals(void)
{
    radio_->signal_toggled().connect(sigc::mem_fun(*this, &Bipolar::toggled_callback));
}

bool
RadioButton::get_active()
{
    return radio_->get_active();
}

void
RadioButton::set_active(bool new_state, bool notify)
{
    if (radio_->get_active() != new_state)
    {
	if (notify) {
	    set_active(new_state);
	}
	else {
	    GtkRadioButton *rb_obj = radio_->gobj();
	    rb_obj->check_button.toggle_button.active = !rb_obj->check_button.toggle_button.active;
	    gtk_widget_queue_draw(GTK_WIDGET(rb_obj));
	}
    }
}

