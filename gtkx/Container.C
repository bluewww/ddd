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

#include <GtkX/Container.h>
#include <gtkmm/box.h>

using namespace GtkX;

Container::Container(void)
{
}

Container::~Container(void)
{
}

// This may be overridden.  The container to which we will add stuff
// may not necessarily be the directly corresponding GTK widget.  For
// example, in the case of Dialogs, we almost always want to add to
// the VBox returned by get_vbox().
Gtk::Container *
Container::gtk_container(void)
{
    Gtk::Widget *widget = internal();
    if (!widget) return NULL;
    return static_cast<Gtk::Container *>(widget);
}

// If the container is an instance of Gtk::Box, pack_start() is used,
// and the options will be honoured.
// Otherwise, the default add() method is invoked.

void
Container::add_child(Widget &child, PackOptions options, int padding)
{
    Gtk::Container *cont = gtk_container();
    Gtk::Box *box = dynamic_cast<Gtk::Box *>(cont);
    Gtk::PackOptions tr[] = {Gtk::PACK_SHRINK,
			     Gtk::PACK_EXPAND_PADDING,
			     Gtk::PACK_EXPAND_WIDGET};
    if (box)
	box->pack_start(*child.internal(), tr[options], padding);
    else
	cont->add(*child.internal());
}

ChildList
Container::get_children(void)
{
    Gtk::Container *cont = gtk_container();
    return ChildList(cont->get_children());
}

void
Container::remove(Widget &w)
{
    // We cannot reassign w to a new parent, therefore
    // it must be finished with.
    delete &w;
}

Gtk::RadioButtonGroup &
Container::button_group(void)
{
    return grp;
}

