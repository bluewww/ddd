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

#include <GtkX/RadioBox.h>

using namespace GtkX;

void
RadioBox::init(Gtk::Container &parent, const Glib::ustring &name,
	       GtkX::Orientation orientation)
{
    switch(orientation) {
    case Gtk::ORIENTATION_VERTICAL:
	box_ = new Gtk::VBox();
	break;
    case Gtk::ORIENTATION_HORIZONTAL:
	box_ = new Gtk::HBox();
	break;
    }
    box_->show();
    add(*box_);
    set_name(name);
    parent.add(*this);
}

RadioBox::RadioBox(Gtk::Container &parent, const Glib::ustring &name,
		   GtkX::Orientation orientation)
{
    init(parent, name, orientation);
}

RadioBox::RadioBox(Gtk::Container &parent, const char *name,
		   GtkX::Orientation orientation)
{
    init(parent, Glib::ustring(name), orientation);
}

RadioBox::~RadioBox(void)
{
    delete box_;
}

void
RadioBox::pack_start(Gtk::Widget& child, PackOptions options, int padding)
{
    box_->pack_start(child, (Gtk::PackOptions)(int)options, (guint)padding);
}

