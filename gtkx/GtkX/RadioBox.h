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

#include <gtkmm/container.h>
#include <gtkmm/box.h>
#include <gtkmm/radiobuttongroup.h>

namespace GtkX {

    // Compatibility with Gtkmm:
    enum PackOptions
    {
	PACK_SHRINK, /**< Space is contracted to the child widget size. */
	PACK_EXPAND_PADDING, /**< Space is expanded, with extra space filled with padding. */
	PACK_EXPAND_WIDGET /**< Space is expanded, with extra space filled by increasing the child widget size. */
    };
    enum Orientation
    {
	ORIENTATION_HORIZONTAL,
	ORIENTATION_VERTICAL
    };


    // FIXME: Tried to derive from Gtk::Bin, but as Bin is an
    // "abstract" base class we would need to implement more method.
    class RadioBox: public Gtk::VBox {
	Gtk::Box *box_;
	Gtk::RadioButtonGroup group_;
    public:
	void init(Gtk::Container &parent, const Glib::ustring &name,
		  GtkX::Orientation orientation=GtkX::ORIENTATION_VERTICAL);
	RadioBox(Gtk::Container &parent, const Glib::ustring &name,
		 GtkX::Orientation orientation=GtkX::ORIENTATION_VERTICAL);
	RadioBox(Gtk::Container &parent, const char *name,
		 GtkX::Orientation orientation=GtkX::ORIENTATION_VERTICAL);
	~RadioBox(void);
	void pack_start(Gtk::Widget &child, PackOptions options = PACK_EXPAND_WIDGET, int padding = 0);
    };

}
