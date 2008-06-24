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

// ***************************************************************************

// Primitive widget creation using constructors with no arguments.

#include <GtkX/Widget.h>
#include <GtkX/OptionMenu.h>

using namespace GtkX;

OptionMenu::OptionMenu(GtkX::Container &parent, PackOptions po, const GtkX::String &name)
{
    om_ = new Gtk::OptionMenu();
    set_name(name.s());
    parent.add_child(*this, po, 0);
    postinit();
}

OptionMenu::~OptionMenu(void)
{
    delete om_;
}

Gtk::Widget *
OptionMenu::internal(void)
{
    return om_;
}

const Gtk::Widget *
OptionMenu::internal(void) const
{
    return om_;
}

void
OptionMenu::set_menu(GtkX::Menu &menu)
{
    Gtk::Menu *gmenu = dynamic_cast<Gtk::Menu *>(menu.internal());
    om_->set_menu(*gmenu);
}

GtkX::Menu *
OptionMenu::get_menu(void)
{
    Gtk::Menu *gtk_menu = om_->get_menu();
    if (!gtk_menu) return NULL;
    GtkX::Widget *w = get_wrapper(gtk_menu);
    if (!w) return NULL;
    return dynamic_cast<GtkX::Menu *>(w);
}

void
OptionMenu::set_history(int index)
{
    om_->set_history(index);
}

