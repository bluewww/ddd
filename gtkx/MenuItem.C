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

#include <GtkX/MenuItem.h>
#include <GtkX/Menu.h>
#include <gtkmm/menuitem.h>

using namespace GtkX;

MenuItem::MenuItem(GtkX::Container &parent, PackOptions po,
		   const GtkX::String &name, const GtkX::String &label)
{
    item_ = new Gtk::MenuItem(mklabel(name, label).s());
    set_name(name.s());
    parent.add_child(*this, po, 0);
    postinit();
}

MenuItem::~MenuItem(void)
{
    delete item_;
}

void
MenuItem::init_signals(void)
{
    item_->signal_activate().connect(sigc::mem_fun(*this, &MenuItem::activate_callback));
}

void
MenuItem::postinit(void)
{
    Widget::postinit();
    init_signals();
}

Gtk::Widget *
MenuItem::internal(void)
{
    return item_;
}

const Gtk::Widget *
MenuItem::internal(void) const
{
    return item_;
}

void
MenuItem::set_label(const String &label)
{
    remove();
    add_label(label.c_str(), false, 0.0, 0.5);
}

void
MenuItem::remove(void)
{
    item_->remove();
}

void
MenuItem::add_label(const String &s, bool mnemonic,
		    double x_align, double y_align)
{
    item_->add_label(s.s(), mnemonic, x_align, y_align);
}

void
MenuItem::set_submenu(GtkX::Menu &sub)
{
    Gtk::Menu *gsub = dynamic_cast<Gtk::Menu *>(sub.internal());
    item_->set_submenu(*gsub);
}

void
MenuItem::activate_callback()
{
    signal_activate_();
}

sigc::signal<void> &
MenuItem::signal_activate()
{
    return signal_activate_;
}

