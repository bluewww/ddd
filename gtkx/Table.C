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

#include <GtkX/Notebook.h>
#include <GtkX/Table.h>

using namespace GtkX;

// Private.  API does not allow construction of unparented widgets.
Table::Table(const GtkX::String &name)
{
    x = y = 0;
    w = h = 1;
    set_name(name.s());
    postinit();
}

Table::Table(GtkX::Container &parent, GtkX::PackOptions po,
	   const GtkX::String &name, const GtkX::String &label)
{
    x = y = 0;
    w = h = 1;
    set_name(name.s());
    parent.add_child(*this, po, 0);
    postinit();
}

Gtk::Widget *
Table::internal(void)
{
    return this;
}

const Gtk::Widget *
Table::internal(void) const
{
    return this;
}

void
Table::set_position(int x0, int y0, int w0, int h0)
{
    x = x0;
    y = y0;
    w = w0;
    h = h0;
}

void
Table::set_cell_size(int w0, int h0)
{
    w = w0;
    h = h0;
}

void
Table::add_child(GtkX::Widget &child,
		 PackOptions options,
		 int padding)
{
    attach(*child.internal(), x, x+w, y, y+h);
    x = x+w;
}

void
Table::cr(int h)
{
    x = 0;
    y = y+h;
}

