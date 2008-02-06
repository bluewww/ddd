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

#include <GtkX/Widget.h>
#include <GtkX/RadioButton.h>

using namespace GtkX;

static const GtkX::String mklabel(const GtkX::String &name,
				  const GtkX::String &label)
{
    if (label.s().length() > 0)
	return label;
    return name;
}

RadioButton::RadioButton(GtkX::Container &parent, const GtkX::String &name,
			 const GtkX::String &label):
    Gtk::RadioButton(label.s())
{
    Gtk::RadioButton::set_name(name.s());
    // We cannot use this:
    // parent.gtk_container()->add(*this);
    // If we always had parent.gtk_container() == &parent we could just
    // override the on_add() method to do what we want.  However,
    // sometimes parent.gtk_container() is a standard Gtk widget.
    // In such a case (e.g. RadioBox) we need to override add_child()
    // instead.
    parent.add_child(*this);
    postinit();
    // FIXME: Radio behaviour already supported for children of a RadioBox.
    // Changing the group here causes a crash (invalidates a node in the
    // GList).
    // set_group(parent.grp);
}

// TEMPORARY
RadioButton::RadioButton(Gtk::Container *parent, const GtkX::String &name,
			 const GtkX::String &label):
    Gtk::RadioButton(label.s())
{
    Gtk::RadioButton::set_name(name.s());
    parent->add(*internal());
    postinit();
    // FIXME: No group
}

RadioButton::~RadioButton(void)
{
}

Gtk::Widget *
RadioButton::internal(void)
{
    return this;
}

