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

#ifndef GTKX_CHECKBUTTON_H
#define GTKX_CHECKBUTTON_H

#include <GtkX/Bipolar.h>
#include <gtkmm/checkbutton.h>

// Template for a widget taking two string constructor arguments.

namespace GtkX {

    class CheckButton: public Bipolar, public Gtk::CheckButton {
    public:
	CheckButton(GtkX::Container &parent, const GtkX::String &name="",
		    const GtkX::String &label="");
	~CheckButton(void);
	Gtk::Widget *internal(void);
	const Gtk::Widget *internal(void) const;
	// FIXME: Disambiguate inheritance from GtkX::Widget and Gtk class.
	bool get_active();
	void set_active(bool new_state, bool notify=false);
#include <GtkX/redirect.h>
    };

}

#endif // GTKX_CHECKBUTTON_H
