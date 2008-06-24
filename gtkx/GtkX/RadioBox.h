// -*- C++ -*-

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
// A brief look at QT indicates that this will be required there as
// well.

#ifndef GTKX_RADIOBOX_H
#define GTKX_RADIOBOX_H

#include <gtkmm/container.h>
#include <gtkmm/box.h>
#include <gtkmm/radiobuttongroup.h>

#include <GtkX/Container.h>

namespace GtkX {

    class RadioBox: public Container {
	Gtk::VBox *vbox_;
	Gtk::Box *box_;
	Gtk::RadioButtonGroup group_;
    public:
	void create_box(Orientation orientation=ORIENTATION_VERTICAL);
	RadioBox(GtkX::Container &parent, PackOptions po=PACK_SHRINK,
		 const String &name="",
		 Orientation orientation=ORIENTATION_VERTICAL);
	~RadioBox(void);
	Gtk::Widget *internal(void);
	const Gtk::Widget *internal(void) const;
	Gtk::Container *gtk_container(void);
	void add_child(GtkX::Widget &child,
		       PackOptions options=PACK_SHRINK,
		       int padding=0);
    };

}

#endif // GTKX_RADIOBOX_H
