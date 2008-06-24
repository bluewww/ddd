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

#include <GtkX/ScrolledWindow.h>
#include <GtkX/Box.h>

using namespace GtkX;

ScrolledWindow::ScrolledWindow(GtkX::Container &parent, PackOptions po,
			       const String &name)
{
    sw_ = new Gtk::ScrolledWindow();
    set_name(name.s());
    parent.add_child(*this, po, 0);
    postinit();
}

ScrolledWindow::~ScrolledWindow(void)
{
    delete sw_;
}

Gtk::Widget *
ScrolledWindow::internal(void)
{
    return sw_;
}

const Gtk::Widget *
ScrolledWindow::internal(void) const
{
    return sw_;
}

