// -*- C++ -*-

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
// A brief look at QT indicates that this will be required there as
// well.

#ifndef GTKX_RADIOBOX_H
#define GTKX_RADIOBOX_H

#include <gtkmm/container.h>
#include <gtkmm/box.h>
#include <gtkmm/radiobuttongroup.h>

#include <GtkX/Container.h>

namespace GtkX {

    // FIXME: Tried to derive from Gtk::Bin, but as Bin is an
    // "abstract" base class we would need to implement more method.
    class RadioBox: public Gtk::VBox, public Container {
	Gtk::Box *box_;
	Gtk::RadioButtonGroup group_;
    public:
	void create_box(Orientation orientation=ORIENTATION_VERTICAL);
	RadioBox(GtkX::Container &parent, const String &name="",
		 Orientation orientation=ORIENTATION_VERTICAL,
		 PackOptions options=PACK_SHRINK,
		 int padding=0);
	~RadioBox(void);
	Gtk::Widget *internal(void);
	const Gtk::Widget *internal(void) const;
	Gtk::Container *gtk_container(void);
	void add_child(GtkX::Widget &child,
		       PackOptions options=PACK_SHRINK,
		       int padding=0);
	void on_add(Gtk::Widget *child);
	void pack_start(Gtk::Widget &child,
			PackOptions options=PACK_SHRINK,
			int padding=0);
	// FIXME: Disambiguate inheritance from GtkX::Widget and Gtk class.
#include <GtkX/redirect.h>
    };

}

#endif // GTKX_RADIOBOX_H
