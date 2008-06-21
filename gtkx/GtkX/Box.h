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

#ifndef GTKX_BOX_H
#define GTKX_BOX_H

#include <gtkmm/container.h>
#include <gtkmm/box.h>
#include <GtkX/Container.h>

namespace GtkX {

    class Box: public Container {
	void add_child(Widget &child, PackOptions options=PACK_EXPAND_WIDGET,
		       int padding=0);
    };

    class Dialog;

    class VBox: public Box {
	Gtk::VBox *vbox_;
    public:
	VBox(GtkX::Container &parent, PackOptions po=PACK_SHRINK, const String &name="",
	    const String &label="");
	~VBox();
	Gtk::Widget *internal(void);
	const Gtk::Widget *internal(void) const;
	// FIXME: Disambiguate inheritance from GtkX::Widget and Gtk class.
	// #include <GtkX/redirect.h>
    private:
	VBox(const String &name="");
	friend class Dialog;
    };

    class HBox: public Box {
	Gtk::HBox *hbox_;
    public:
	HBox(GtkX::Container &parent, PackOptions po=PACK_SHRINK, const String &name="",
	    const String &label="");
	~HBox();
	Gtk::Widget *internal(void);
	const Gtk::Widget *internal(void) const;
	// FIXME: Disambiguate inheritance from GtkX::Widget and Gtk class.
	// #include <GtkX/redirect.h>
    private:
  	HBox(const String &name="");
	friend class Dialog;
  };

}

#endif // GTKX_BOX_H
