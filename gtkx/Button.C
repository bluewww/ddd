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

#include <GtkX/Widget.h>
#include <GtkX/Button.h>

using namespace GtkX;

Button::Button(GtkX::Container &parent, PackOptions po,
	       const GtkX::String &name, const GtkX::String &label):
    Gtk::Button(mklabel(name, label).s())
{
    set_name(name.s());
    // We cannot use this:
    // parent.gtk_container()->add(*this);
    // If we always had parent.gtk_container() == &parent we could just
    // override the on_add() method to do what we want.  However,
    // sometimes parent.gtk_container() is a standard Gtk widget.
    // In such a case (e.g. RadioBox) we need to override add_child()
    // instead.
    parent.add_child(*this, po, 0);
    postinit();
}

Button::~Button(void)
{
}

Gtk::Widget *
Button::internal(void)
{
    return this;
}

const Gtk::Widget *
Button::internal(void) const
{
    return this;
}

void
Button::set_alignment(float xalign, float yalign)
{
    Gtk::Button::set_alignment(xalign, yalign);
}

void
Button::set_label(const String &s)
{
    Gtk::Button::set_label(s.s());
}

void
Button::remove(void)
{
    Gtk::Button::remove();
}

void
Button::add_label(const String &s)
{
    Gtk::Button::add_label(s.s());
}

