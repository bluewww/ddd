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

#ifndef GTKX_BIPOLAR_H
#define GTKX_BIPOLAR_H

#include <GtkX/Widget.h>

namespace GtkX {

    class Bipolar: public Widget {
    private:
	sigc::signal<void> signal_toggled_;
    public:
	// FIXME: The subclasses cannot connect the signal unless this
	// is public.  Alternatively, we could declare a virtual to
	// return the Gtk signal, and connect it in this class.
	void toggled_callback(void);
    public:
	Bipolar();
	~Bipolar();
	virtual void init_signals() = 0;
	void postinit(void);
	virtual bool get_active() = 0;
	virtual void set_active(bool new_state, bool notify=false) = 0;
	sigc::signal<void> &signal_toggled();
    };

}

#endif // GTKX_BIPOLAR_H
