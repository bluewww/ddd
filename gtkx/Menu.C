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

// ***************************************************************************

// Primitive widget creation using constructors with no arguments.

#include <GtkX/Menu.h>

using namespace GtkX;

Menu::Menu(GtkX::Widget &parent, const GtkX::String &name)
{
    set_name(name.s());
    // parent.add_child(*this);
    postinit();
}

// TEMPORARY
Menu::Menu(Gtk::Widget *parent, const GtkX::String &name)
{
    set_name(name.s());
    // parent->add(*internal());
    postinit();
}

Menu::~Menu(void)
{
}

Gtk::Widget *
Menu::internal(void)
{
    return this;
}

const Gtk::Widget *
Menu::internal(void) const
{
    return this;
}

